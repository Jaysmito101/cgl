/*
MIT License

Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * Cloth Simulation Example for CGL
 * 
 * This example demonstrates a real-time cloth simulation using:
 * - Compute shaders for physics calculations
 * - Verlet integration for stable particle dynamics
 * - Spring constraints for cloth structure
 * - Interactive parameter control
 * 
 * Technical Details:
 * - 32x32 grid of particles (1024 total)
 * - Structural and shear spring constraints
 * - Gravity, wind, and damping forces
 * - Ground collision detection
 * - Real-time normal calculation for lighting
 * 
 * Controls:
 * - SPACE: Pause/Resume simulation
 * - R: Reset cloth to initial state
 * - UP/DOWN: Adjust gravity strength
 * - LEFT/RIGHT: Adjust wind strength
 * 
 * Author: AI Assistant (based on CGL framework by Jaysmito Mukherjee)
 */

#include <stdlib.h>

#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_RAY_CASTER
#define CGL_EXCLUDE_NODE_EDITOR
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#define EM_BOOL int
#endif

// Cloth simulation parameters - adjust these for different cloth behaviors
#define CLOTH_WIDTH 32      // Number of particles horizontally
#define CLOTH_HEIGHT 32     // Number of particles vertically
#define CLOTH_PARTICLES (CLOTH_WIDTH * CLOTH_HEIGHT)  // Total particle count
// Note: Spring count includes structural (horizontal/vertical) and shear (diagonal) springs
#define CLOTH_SPRINGS ((CLOTH_WIDTH - 1) * CLOTH_HEIGHT + CLOTH_WIDTH * (CLOTH_HEIGHT - 1) + (CLOTH_WIDTH - 1) * (CLOTH_HEIGHT - 1) * 2)

// Pass through vertex shader for screen quad rendering
static const char* PASS_THROUGH_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec2 TexCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"}";

// Pass through fragment shader for screen quad rendering
static const char* PASS_THROUGH_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform sampler2D u_tex;\n"
"\n"
"void main()\n"
"{\n"
"	vec3 color = texture(u_tex, TexCoord).rgb;\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";

// Cloth vertex shader - renders particles as points
static const char* CLOTH_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec3 position;\n"
"\n"
"struct Particle\n"
"{\n"
"    vec4 position; // xyz = position, w = mass\n"
"    vec4 prev_position; // xyz = previous position, w = pinned (1.0 = pinned)\n"
"    vec4 velocity; // xyz = velocity, w = unused\n"
"    vec4 normal; // xyz = normal, w = unused\n"
"};\n"
"\n"
"layout(std430, binding = 0) buffer ParticleBuffer\n"
"{\n"
"    Particle particles[];\n"
"};\n"
"\n"
"out vec3 Color;\n"
"out vec3 Normal;\n"
"\n"
"uniform mat4 view_proj;\n"
"uniform vec3 light_pos;\n"
"uniform vec3 cloth_color;\n"
"\n"
"void main()\n"
"{\n"
"	int particle_index = gl_VertexID;\n"
"	vec3 world_pos = particles[particle_index].position.xyz;\n"
"	vec3 normal = particles[particle_index].normal.xyz;\n"
"	\n"
"	gl_Position = view_proj * vec4(world_pos, 1.0);\n"
"	gl_PointSize = 3.0;\n"
"	\n"
"	// Simple lighting calculation\n"
"	vec3 light_dir = normalize(light_pos - world_pos);\n"
"	float diffuse = max(dot(normal, light_dir), 0.0);\n"
"	Color = cloth_color * (0.3 + diffuse * 0.7);\n"
"	Normal = normal;\n"
"}";

// Cloth fragment shader
static const char* CLOTH_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"in vec3 Color;\n"
"in vec3 Normal;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"	// Make points round\n"
"	vec2 center = gl_PointCoord - vec2(0.5);\n"
"	if (dot(center, center) > 0.25)\n"
"		discard;\n"
"	\n"
"	FragColor = vec4(Color, 1.0);\n"
"}";

// Cloth physics compute shader - this runs on the GPU for high performance
static const char* CLOTH_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"// Compute shader work group size - processes 16x16 particles per dispatch\n"
"layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\n"
"\n"
"// Simulation control uniforms\n"
"uniform int mode; // 0: initialize, 1: update physics, 2: calculate normals\n"
"uniform float dt; // delta time for integration\n"
"uniform float time; // current simulation time\n"
"uniform vec3 gravity; // gravity force vector\n"
"uniform vec3 wind; // wind force vector\n"
"uniform float damping; // velocity damping factor (0-1)\n"
"uniform float rest_length; // natural spring length\n"
"uniform float spring_strength; // spring stiffness constant\n"
"uniform int cloth_width;\n"
"uniform int cloth_height;\n"
"uniform float cloth_size; // world-space size of cloth\n"
"\n"
"// Particle data structure - each particle is 4 vec4s (64 bytes)\n"
"struct Particle\n"
"{\n"
"    vec4 position; // xyz = position, w = mass\n"
"    vec4 prev_position; // xyz = previous position, w = pinned flag (1.0 = pinned)\n"
"    vec4 velocity; // xyz = velocity, w = unused\n"
"    vec4 normal; // xyz = surface normal, w = unused\n"
"};\n"
"\n"
"// GPU buffer containing all particle data\n"
"layout(std430, binding = 0) buffer ParticleBuffer\n"
"{\n"
"    Particle particles[];\n"
"};\n"
"\n"
"// Hash function for random number generation\n"
"float hash(vec2 co)\n"
"{\n"
"    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
"}\n"
"\n"
"// Get particle index from 2D coordinates\n"
"int getParticleIndex(int x, int y)\n"
"{\n"
"    if (x < 0 || x >= cloth_width || y < 0 || y >= cloth_height)\n"
"        return -1;\n"
"    return y * cloth_width + x;\n"
"}\n"
"\n"
"// Initialize cloth particles\n"
"void initialize()\n"
"{\n"
"    int x = int(gl_GlobalInvocationID.x);\n"
"    int y = int(gl_GlobalInvocationID.y);\n"
"    \n"
"    if (x >= cloth_width || y >= cloth_height)\n"
"        return;\n"
"    \n"
"    int index = getParticleIndex(x, y);\n"
"    if (index < 0)\n"
"        return;\n"
"    \n"
"    // Initialize particle position\n"
"    float fx = float(x) / float(cloth_width - 1);\n"
"    float fy = float(y) / float(cloth_height - 1);\n"
"    \n"
"    vec3 pos = vec3((fx - 0.5) * cloth_size, 2.0, (fy - 0.5) * cloth_size);\n"
"    \n"
"    particles[index].position = vec4(pos, 1.0); // mass = 1.0\n"
"    particles[index].prev_position = vec4(pos, 0.0); // not pinned by default\n"
"    particles[index].velocity = vec4(0.0, 0.0, 0.0, 0.0);\n"
"    particles[index].normal = vec4(0.0, 1.0, 0.0, 0.0);\n"
"    \n"
"    // Pin top corners\n"
"    if ((x == 0 || x == cloth_width - 1) && y == 0)\n"
"    {\n"
"        particles[index].prev_position.w = 1.0; // pinned\n"
"    }\n"
"}\n"
"\n"
"// Update cloth physics using Verlet integration\n"
"void updatePhysics()\n"
"{\n"
"    int x = int(gl_GlobalInvocationID.x);\n"
"    int y = int(gl_GlobalInvocationID.y);\n"
"    \n"
"    if (x >= cloth_width || y >= cloth_height)\n"
"        return;\n"
"    \n"
"    int index = getParticleIndex(x, y);\n"
"    if (index < 0)\n"
"        return;\n"
"    \n"
"    // Skip if particle is pinned\n"
"    if (particles[index].prev_position.w > 0.5)\n"
"        return;\n"
"    \n"
"    vec3 pos = particles[index].position.xyz;\n"
"    vec3 prev_pos = particles[index].prev_position.xyz;\n"
"    vec3 vel = particles[index].velocity.xyz;\n"
"    float mass = particles[index].position.w;\n"
"    \n"
"    // Calculate forces\n"
"    vec3 force = vec3(0.0);\n"
"    \n"
"    // Gravity\n"
"    force += gravity * mass;\n"
"    \n"
"    // Wind (simple)\n"
"    force += wind * 0.1;\n"
"    \n"
"    // Spring forces\n"
"    vec3 spring_force = vec3(0.0);\n"
"    \n"
"    // Check all 8 neighbors for spring connections\n"
"    for (int dx = -1; dx <= 1; dx++)\n"
"    {\n"
"        for (int dy = -1; dy <= 1; dy++)\n"
"        {\n"
"            if (dx == 0 && dy == 0)\n"
"                continue;\n"
"            \n"
"            int nx = x + dx;\n"
"            int ny = y + dy;\n"
"            int neighbor_index = getParticleIndex(nx, ny);\n"
"            \n"
"            if (neighbor_index < 0)\n"
"                continue;\n"
"            \n"
"            vec3 neighbor_pos = particles[neighbor_index].position.xyz;\n"
"            vec3 diff = neighbor_pos - pos;\n"
"            float distance = length(diff);\n"
"            \n"
"            if (distance > 0.0001)\n"
"            {\n"
"                vec3 direction = diff / distance;\n"
"                float target_length = rest_length;\n"
"                \n"
"                // Diagonal springs are longer\n"
"                if (abs(dx) + abs(dy) == 2)\n"
"                    target_length *= 1.414; // sqrt(2)\n"
"                \n"
"                float spring_force_mag = spring_strength * (distance - target_length);\n"
"                spring_force += direction * spring_force_mag;\n"
"            }\n"
"        }\n"
"    }\n"
"    \n"
"    force += spring_force;\n"
"    \n"
"    // Verlet integration\n"
"    vec3 acceleration = force / mass;\n"
"    vec3 new_pos = pos + (pos - prev_pos) * (1.0 - damping) + acceleration * dt * dt;\n"
"    \n"
"    // Simple ground collision\n"
"    if (new_pos.y < -1.0)\n"
"    {\n"
"        new_pos.y = -1.0;\n"
"        vel.y = 0.0;\n"
"    }\n"
"    \n"
"    // Update particle\n"
"    particles[index].prev_position.xyz = pos;\n"
"    particles[index].position.xyz = new_pos;\n"
"    particles[index].velocity.xyz = (new_pos - pos) / dt;\n"
"}\n"
"\n"
"// Calculate normals for lighting\n"
"void calculateNormals()\n"
"{\n"
"    int x = int(gl_GlobalInvocationID.x);\n"
"    int y = int(gl_GlobalInvocationID.y);\n"
"    \n"
"    if (x >= cloth_width || y >= cloth_height)\n"
"        return;\n"
"    \n"
"    int index = getParticleIndex(x, y);\n"
"    if (index < 0)\n"
"        return;\n"
"    \n"
"    vec3 normal = vec3(0.0);\n"
"    int count = 0;\n"
"    \n"
"    // Calculate normal by averaging cross products of adjacent triangles\n"
"    for (int dx = -1; dx <= 0; dx++)\n"
"    {\n"
"        for (int dy = -1; dy <= 0; dy++)\n"
"        {\n"
"            int x1 = x + dx;\n"
"            int y1 = y + dy;\n"
"            int x2 = x1 + 1;\n"
"            int y2 = y1 + 1;\n"
"            \n"
"            if (x1 >= 0 && x1 < cloth_width && y1 >= 0 && y1 < cloth_height &&\n"
"                x2 >= 0 && x2 < cloth_width && y2 >= 0 && y2 < cloth_height)\n"
"            {\n"
"                vec3 p1 = particles[getParticleIndex(x1, y1)].position.xyz;\n"
"                vec3 p2 = particles[getParticleIndex(x2, y1)].position.xyz;\n"
"                vec3 p3 = particles[getParticleIndex(x1, y2)].position.xyz;\n"
"                vec3 p4 = particles[getParticleIndex(x2, y2)].position.xyz;\n"
"                \n"
"                // Two triangles per quad\n"
"                vec3 n1 = cross(p2 - p1, p3 - p1);\n"
"                vec3 n2 = cross(p4 - p2, p3 - p2);\n"
"                \n"
"                if (length(n1) > 0.0001)\n"
"                {\n"
"                    normal += normalize(n1);\n"
"                    count++;\n"
"                }\n"
"                if (length(n2) > 0.0001)\n"
"                {\n"
"                    normal += normalize(n2);\n"
"                    count++;\n"
"                }\n"
"            }\n"
"        }\n"
"    }\n"
"    \n"
"    if (count > 0)\n"
"    {\n"
"        normal = normalize(normal / float(count));\n"
"    }\n"
"    else\n"
"    {\n"
"        normal = vec3(0.0, 1.0, 0.0);\n"
"    }\n"
"    \n"
"    particles[index].normal.xyz = normal;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    if (mode == 0)\n"
"    {\n"
"        initialize();\n"
"    }\n"
"    else if (mode == 1)\n"
"    {\n"
"        updatePhysics();\n"
"    }\n"
"    else if (mode == 2)\n"
"    {\n"
"        calculateNormals();\n"
"    }\n"
"}";

// Global state structure
static struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_shader* present_shader;
    CGL_shader* cloth_shader;
    CGL_shader* compute_shader;
    CGL_ssbo* particle_ssbo;
    GLuint dummy_vao;
    
    // Simulation parameters
    CGL_float delta_time;
    CGL_float previous_time;
    CGL_float gravity_strength;
    CGL_float wind_strength;
    CGL_float damping;
    CGL_float rest_length;
    CGL_float spring_strength;
    CGL_float cloth_size;
    CGL_bool simulation_running;
    
    // Camera control
    CGL_vec3 camera_pos;
    CGL_vec3 camera_target;
    CGL_float camera_angle;
    
    // Performance tracking
    CGL_float frame_time;
    CGL_int frames;
    CGL_int fps;
} g_State;

// Function declarations
CGL_bool init();
void initialize_cloth();
void update_cloth_physics();
void render_cloth();
void cleanup();
EM_BOOL loop(double time, void* userData);

void initialize_cloth()
{
    // Create particle SSBO
    g_State.particle_ssbo = CGL_ssbo_create(0);
    CGL_ssbo_set_data(g_State.particle_ssbo, CLOTH_PARTICLES * 4 * 4 * sizeof(CGL_float), NULL, GL_DYNAMIC_DRAW);
    
    // Create dummy VAO for rendering
    glGenVertexArrays(1, &g_State.dummy_vao);
    
    // Initialize particles
    CGL_shader_bind(g_State.compute_shader);
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "mode"), 0);
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "cloth_width"), CLOTH_WIDTH);
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "cloth_height"), CLOTH_HEIGHT);
    CGL_shader_set_uniform_float(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "cloth_size"), g_State.cloth_size);
    CGL_shader_set_uniform_float(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "rest_length"), g_State.rest_length);
    CGL_shader_compute_dispatch(g_State.compute_shader, (CLOTH_WIDTH + 15) / 16, (CLOTH_HEIGHT + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void update_cloth_physics()
{
    if (!g_State.simulation_running) return;
    
    // Step 1: Update particle physics using Verlet integration
    CGL_shader_bind(g_State.compute_shader);
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "mode"), 1);
    CGL_shader_set_uniform_float(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "dt"), g_State.delta_time);
    CGL_shader_set_uniform_float(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "time"), CGL_utils_get_time());
    CGL_shader_set_uniform_vec3v(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "gravity"), 0.0f, g_State.gravity_strength, 0.0f);
    CGL_shader_set_uniform_vec3v(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "wind"), g_State.wind_strength, 0.0f, 0.0f);
    CGL_shader_set_uniform_float(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "damping"), g_State.damping);
    CGL_shader_set_uniform_float(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "spring_strength"), g_State.spring_strength);
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "cloth_width"), CLOTH_WIDTH);
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "cloth_height"), CLOTH_HEIGHT);
    CGL_shader_compute_dispatch(g_State.compute_shader, (CLOTH_WIDTH + 15) / 16, (CLOTH_HEIGHT + 15) / 16, 1);
    
    // Memory barrier ensures physics update completes before normal calculation
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    // Step 2: Calculate surface normals for lighting (runs after physics update)
    CGL_shader_set_uniform_int(g_State.compute_shader, CGL_shader_get_uniform_location(g_State.compute_shader, "mode"), 2);
    CGL_shader_compute_dispatch(g_State.compute_shader, (CLOTH_WIDTH + 15) / 16, (CLOTH_HEIGHT + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void render_cloth()
{
    // Update camera position
    g_State.camera_angle += g_State.delta_time * 0.2f;
    g_State.camera_pos.x = 5.0f * cosf(g_State.camera_angle);
    g_State.camera_pos.z = 5.0f * sinf(g_State.camera_angle);
    
    CGL_mat4 view = CGL_mat4_look_at(g_State.camera_pos, g_State.camera_target, CGL_vec3_init(0.0f, 1.0f, 0.0f));
    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.1f, 100.0f);
    CGL_mat4 view_proj = CGL_mat4_mul(projection, view);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    CGL_shader_bind(g_State.cloth_shader);
    CGL_shader_set_uniform_mat4(g_State.cloth_shader, CGL_shader_get_uniform_location(g_State.cloth_shader, "view_proj"), &view_proj);
    CGL_shader_set_uniform_vec3v(g_State.cloth_shader, CGL_shader_get_uniform_location(g_State.cloth_shader, "light_pos"), 5.0f, 5.0f, 5.0f);
    CGL_shader_set_uniform_vec3v(g_State.cloth_shader, CGL_shader_get_uniform_location(g_State.cloth_shader, "cloth_color"), 0.8f, 0.2f, 0.2f);
    
    // Render particles as points
    glBindVertexArray(g_State.dummy_vao);
    glDrawArrays(GL_POINTS, 0, CLOTH_PARTICLES);
    glBindVertexArray(0);
    
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_DEPTH_TEST);
}

CGL_bool init()
{
    // Initialize CGL
    if (!CGL_init())
    {
        CGL_error("Failed to initialize CGL");
        return CGL_FALSE;
    }
    
    g_State.window = CGL_window_create(800, 600, "Cloth Simulation - CGL Example");
    if (!g_State.window)
    {
        CGL_error("Failed to create window");
        return CGL_FALSE;
    }
    
    CGL_window_make_context_current(g_State.window);
    if (!CGL_gl_init())
    {
        CGL_error("Failed to initialize OpenGL");
        return CGL_FALSE;
    }
    CGL_widgets_init();
    
    // Create framebuffer
    g_State.default_framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    
    // Create shaders
    g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER, PASS_THROUGH_FRAGMENT_SHADER, NULL);
    g_State.cloth_shader = CGL_shader_create(CLOTH_VERTEX_SHADER, CLOTH_FRAGMENT_SHADER, NULL);
    g_State.compute_shader = CGL_shader_compute_create(CLOTH_COMPUTE_SHADER, NULL);
    
    // Initialize simulation parameters
    g_State.delta_time = 0.0f;
    g_State.gravity_strength = -9.8f;
    g_State.wind_strength = 0.0f;
    g_State.damping = 0.01f;
    g_State.rest_length = 0.1f;
    g_State.spring_strength = 50.0f;
    g_State.cloth_size = 4.0f;
    g_State.simulation_running = CGL_TRUE;
    
    // Initialize camera
    g_State.camera_pos = CGL_vec3_init(0.0f, 2.0f, 5.0f);
    g_State.camera_target = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    g_State.camera_angle = 0.0f;
    
    // Initialize performance tracking
    g_State.previous_time = CGL_utils_get_time();
    g_State.frame_time = 0.0f;
    g_State.frames = 0;
    g_State.fps = 0;
    
    // Initialize cloth simulation
    initialize_cloth();
    
    return CGL_TRUE;
}

EM_BOOL loop(double time, void* userData)
{
    (void)time;
    (void)userData;
    
    // Calculate delta time
    CGL_float current_time = CGL_utils_get_time();
    g_State.delta_time = current_time - g_State.previous_time;
    g_State.delta_time = CGL_utils_clamp(g_State.delta_time, 0.0f, 0.033f); // Cap at 30 FPS
    g_State.previous_time = current_time;
    
    g_State.frame_time += g_State.delta_time;
    g_State.frames++;
    if (g_State.frame_time >= 1.0f)
    {
        g_State.fps = g_State.frames;
        g_State.frames = 0;
        g_State.frame_time = 0.0f;
    }
    
    // Handle input
    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_SPACE))
    {
        g_State.simulation_running = !g_State.simulation_running;
    }
    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_R))
    {
        initialize_cloth();
    }
    
    // Update physics
    update_cloth_physics();
    
    // Render
    CGL_framebuffer_bind(g_State.default_framebuffer);
    CGL_gl_clear(0.1f, 0.1f, 0.1f, 1.0f);
    
    render_cloth();
    
    // UI
    CGL_widgets_begin();
    
    static CGL_byte buffer[512];
    sprintf(buffer, "FPS: %d", g_State.fps);
    CGL_widgets_add_string(buffer, -1.0f, 0.95f, 1.0f, 0.05f);
    
    sprintf(buffer, "Frame Time: %.3f ms", g_State.delta_time * 1000.0f);
    CGL_widgets_add_string(buffer, -1.0f, 0.90f, 1.0f, 0.05f);
    
    sprintf(buffer, "Particles: %d", CLOTH_PARTICLES);
    CGL_widgets_add_string(buffer, -1.0f, 0.85f, 1.0f, 0.05f);
    
    sprintf(buffer, "Gravity: %.1f", g_State.gravity_strength);
    CGL_widgets_add_string(buffer, -1.0f, 0.80f, 1.0f, 0.05f);
    
    sprintf(buffer, "Wind: %.1f", g_State.wind_strength);
    CGL_widgets_add_string(buffer, -1.0f, 0.75f, 1.0f, 0.05f);
    
    CGL_widgets_add_string("Controls:", -1.0f, 0.65f, 1.0f, 0.05f);
    CGL_widgets_add_string("SPACE - Pause/Resume", -1.0f, 0.60f, 1.0f, 0.05f);
    CGL_widgets_add_string("R - Reset", -1.0f, 0.55f, 1.0f, 0.05f);
    CGL_widgets_add_string("UP/DOWN - Adjust Gravity", -1.0f, 0.50f, 1.0f, 0.05f);
    CGL_widgets_add_string("LEFT/RIGHT - Adjust Wind", -1.0f, 0.45f, 1.0f, 0.05f);
    
    // Interactive controls
    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_UP))
    {
        g_State.gravity_strength += 1.0f * g_State.delta_time * 10.0f;
    }
    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_DOWN))
    {
        g_State.gravity_strength -= 1.0f * g_State.delta_time * 10.0f;
    }
    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_LEFT))
    {
        g_State.wind_strength -= 1.0f * g_State.delta_time * 10.0f;
    }
    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_RIGHT))
    {
        g_State.wind_strength += 1.0f * g_State.delta_time * 10.0f;
    }
    
    CGL_widgets_end();
    
    // Swap buffers and poll events
    CGL_window_swap_buffers(g_State.window);
    CGL_window_poll_events(g_State.window);
    
    return !CGL_window_should_close(g_State.window);
}

// NOTE: This will not work with WASM for now as SSBOs are not supported in WebGL
int main()
{
    if (!init()) return EXIT_FAILURE;

#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
	while (!CGL_window_should_close(g_State.window))
	{
        if (!loop(0.0, NULL)) break;		
	}
    cleanup();
#endif
	return EXIT_SUCCESS;
}

void cleanup()
{
    if (g_State.particle_ssbo) CGL_ssbo_destroy(g_State.particle_ssbo);
    if (g_State.dummy_vao) glDeleteVertexArrays(1, &g_State.dummy_vao);
    if (g_State.compute_shader) CGL_shader_destroy(g_State.compute_shader);
    if (g_State.cloth_shader) CGL_shader_destroy(g_State.cloth_shader);
    if (g_State.present_shader) CGL_shader_destroy(g_State.present_shader);
    if (g_State.default_framebuffer) CGL_framebuffer_destroy(g_State.default_framebuffer);
    
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}