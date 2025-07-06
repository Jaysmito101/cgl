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

#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_RAY_CASTER
#define CGL_EXCLUDE_NODE_EDITOR
#include "cgl.h"

// Cloth simulation parameters
#define CLOTH_WIDTH 32
#define CLOTH_HEIGHT 32
#define CLOTH_PARTICLES (CLOTH_WIDTH * CLOTH_HEIGHT)
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

// Cloth vertex shader
static const char* CLOTH_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 normal;\n"
"layout (location = 2) in vec2 texcoord;\n"
"\n"
"out vec3 WorldPos;\n"
"out vec3 Normal;\n"
"out vec2 TexCoord;\n"
"\n"
"uniform mat4 view_proj;\n"
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"	WorldPos = (model * vec4(position, 1.0)).xyz;\n"
"	Normal = normalize((model * vec4(normal, 0.0)).xyz);\n"
"	TexCoord = texcoord;\n"
"	gl_Position = view_proj * vec4(WorldPos, 1.0);\n"
"}";

// Cloth fragment shader
static const char* CLOTH_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"in vec3 WorldPos;\n"
"in vec3 Normal;\n"
"in vec2 TexCoord;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform vec3 light_pos;\n"
"uniform vec3 view_pos;\n"
"uniform vec3 cloth_color;\n"
"\n"
"void main()\n"
"{\n"
"	// Simple Phong lighting\n"
"	vec3 light_dir = normalize(light_pos - WorldPos);\n"
"	vec3 view_dir = normalize(view_pos - WorldPos);\n"
"	vec3 reflect_dir = reflect(-light_dir, Normal);\n"
"	\n"
"	float ambient = 0.2;\n"
"	float diffuse = max(dot(Normal, light_dir), 0.0);\n"
"	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);\n"
"	\n"
"	vec3 color = cloth_color * (ambient + diffuse * 0.6 + specular * 0.2);\n"
"	FragColor = vec4(color, 1.0);\n"
"}";

// Cloth physics compute shader
static const char* CLOTH_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\n"
"\n"
"// Uniforms\n"
"uniform int mode; // 0: initialize, 1: update physics\n"
"uniform float dt; // delta time\n"
"uniform float time; // current time\n"
"uniform vec3 gravity; // gravity vector\n"
"uniform vec3 wind; // wind force\n"
"uniform float damping; // velocity damping\n"
"uniform float rest_length; // rest length of springs\n"
"uniform float spring_strength; // spring strength\n"
"uniform int cloth_width;\n"
"uniform int cloth_height;\n"
"uniform float cloth_size;\n"
"\n"
"// Particle structure\n"
"struct Particle\n"
"{\n"
"    vec4 position; // xyz = position, w = mass\n"
"    vec4 prev_position; // xyz = previous position, w = pinned (1.0 = pinned)\n"
"    vec4 velocity; // xyz = velocity, w = unused\n"
"    vec4 normal; // xyz = normal, w = unused\n"
"};\n"
"\n"
"// Particle buffer\n"
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

// Global variables
static CGL_window* window = NULL;
static CGL_framebuffer* default_framebuffer = NULL;
static CGL_shader* present_shader = NULL;
static CGL_shader* cloth_shader = NULL;
static CGL_shader* compute_shader = NULL;
static CGL_ssbo* particle_ssbo = NULL;
static CGL_mesh* cloth_mesh = NULL;

// Simulation parameters
static float delta_time = 0.0f;
static float gravity_strength = -9.8f;
static float wind_strength = 0.0f;
static float damping = 0.01f;
static float rest_length = 0.1f;
static float spring_strength = 50.0f;
static float cloth_size = 4.0f;
static bool simulation_running = true;

// Camera
static CGL_vec3 camera_pos = {0.0f, 2.0f, 5.0f};
static CGL_vec3 camera_target = {0.0f, 0.0f, 0.0f};
static float camera_angle = 0.0f;

// Function declarations
void initialize_cloth();
void update_cloth_physics();
void render_cloth();
void cleanup();

// Create cloth mesh for rendering
void create_cloth_mesh()
{
    // Create vertices and indices for cloth mesh
    CGL_float* vertices = (CGL_float*)malloc(CLOTH_PARTICLES * 6 * sizeof(CGL_float)); // pos(3) + texcoord(2) + normal(3) = 8, but we'll use 6 for simplicity
    CGL_uint* indices = (CGL_uint*)malloc((CLOTH_WIDTH - 1) * (CLOTH_HEIGHT - 1) * 6 * sizeof(CGL_uint));
    
    // Generate vertices
    for (int y = 0; y < CLOTH_HEIGHT; y++)
    {
        for (int x = 0; x < CLOTH_WIDTH; x++)
        {
            int index = y * CLOTH_WIDTH + x;
            
            // Position (will be updated from particle buffer)
            vertices[index * 6 + 0] = 0.0f;
            vertices[index * 6 + 1] = 0.0f;
            vertices[index * 6 + 2] = 0.0f;
            
            // Texture coordinates
            vertices[index * 6 + 3] = (float)x / (float)(CLOTH_WIDTH - 1);
            vertices[index * 6 + 4] = (float)y / (float)(CLOTH_HEIGHT - 1);
            
            // Normal (will be updated from particle buffer)
            vertices[index * 6 + 5] = 0.0f;
        }
    }
    
    // Generate indices for triangles
    int index_count = 0;
    for (int y = 0; y < CLOTH_HEIGHT - 1; y++)
    {
        for (int x = 0; x < CLOTH_WIDTH - 1; x++)
        {
            int top_left = y * CLOTH_WIDTH + x;
            int top_right = y * CLOTH_WIDTH + (x + 1);
            int bottom_left = (y + 1) * CLOTH_WIDTH + x;
            int bottom_right = (y + 1) * CLOTH_WIDTH + (x + 1);
            
            // First triangle
            indices[index_count++] = top_left;
            indices[index_count++] = bottom_left;
            indices[index_count++] = top_right;
            
            // Second triangle
            indices[index_count++] = top_right;
            indices[index_count++] = bottom_left;
            indices[index_count++] = bottom_right;
        }
    }
    
    cloth_mesh = CGL_mesh_create();
    CGL_mesh_add_vertex_buffer(cloth_mesh, vertices, CLOTH_PARTICLES * 6 * sizeof(CGL_float), 0, 3, CGL_FLOAT, false, 6 * sizeof(CGL_float), 0);
    CGL_mesh_add_vertex_buffer(cloth_mesh, vertices + 3, CLOTH_PARTICLES * 6 * sizeof(CGL_float), 1, 2, CGL_FLOAT, false, 6 * sizeof(CGL_float), 3 * sizeof(CGL_float));
    CGL_mesh_add_vertex_buffer(cloth_mesh, vertices + 5, CLOTH_PARTICLES * 6 * sizeof(CGL_float), 2, 1, CGL_FLOAT, false, 6 * sizeof(CGL_float), 5 * sizeof(CGL_float));
    CGL_mesh_set_indices(cloth_mesh, indices, index_count);
    
    free(vertices);
    free(indices);
}

void initialize_cloth()
{
    // Create particle SSBO
    particle_ssbo = CGL_ssbo_create(0);
    CGL_ssbo_set_data(particle_ssbo, CLOTH_PARTICLES * 4 * 4 * sizeof(CGL_float), NULL, GL_DYNAMIC_DRAW);
    
    // Initialize particles
    CGL_shader_bind(compute_shader);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "mode"), 0);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "cloth_width"), CLOTH_WIDTH);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "cloth_height"), CLOTH_HEIGHT);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "cloth_size"), cloth_size);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "rest_length"), rest_length);
    CGL_shader_compute_dispatch(compute_shader, (CLOTH_WIDTH + 15) / 16, (CLOTH_HEIGHT + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void update_cloth_physics()
{
    if (!simulation_running) return;
    
    CGL_shader_bind(compute_shader);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "mode"), 1);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "dt"), delta_time);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "time"), CGL_utils_get_time());
    CGL_shader_set_uniform_vec3v(compute_shader, CGL_shader_get_uniform_location(compute_shader, "gravity"), 0.0f, gravity_strength, 0.0f);
    CGL_shader_set_uniform_vec3v(compute_shader, CGL_shader_get_uniform_location(compute_shader, "wind"), wind_strength, 0.0f, 0.0f);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "damping"), damping);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "spring_strength"), spring_strength);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "cloth_width"), CLOTH_WIDTH);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "cloth_height"), CLOTH_HEIGHT);
    CGL_shader_compute_dispatch(compute_shader, (CLOTH_WIDTH + 15) / 16, (CLOTH_HEIGHT + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    // Calculate normals
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "mode"), 2);
    CGL_shader_compute_dispatch(compute_shader, (CLOTH_WIDTH + 15) / 16, (CLOTH_HEIGHT + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void render_cloth()
{
    // Update camera position
    camera_angle += delta_time * 0.2f;
    camera_pos.x = 5.0f * cosf(camera_angle);
    camera_pos.z = 5.0f * sinf(camera_angle);
    
    CGL_mat4 view = CGL_mat4_look_at(camera_pos, camera_target, CGL_vec3_init(0.0f, 1.0f, 0.0f));
    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.1f, 100.0f);
    CGL_mat4 view_proj = CGL_mat4_mul(projection, view);
    CGL_mat4 model = CGL_mat4_identity();
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    CGL_shader_bind(cloth_shader);
    CGL_shader_set_uniform_mat4(cloth_shader, CGL_shader_get_uniform_location(cloth_shader, "view_proj"), &view_proj);
    CGL_shader_set_uniform_mat4(cloth_shader, CGL_shader_get_uniform_location(cloth_shader, "model"), &model);
    CGL_shader_set_uniform_vec3(cloth_shader, CGL_shader_get_uniform_location(cloth_shader, "light_pos"), CGL_vec3_init(5.0f, 5.0f, 5.0f));
    CGL_shader_set_uniform_vec3(cloth_shader, CGL_shader_get_uniform_location(cloth_shader, "view_pos"), camera_pos);
    CGL_shader_set_uniform_vec3(cloth_shader, CGL_shader_get_uniform_location(cloth_shader, "cloth_color"), CGL_vec3_init(0.8f, 0.2f, 0.2f));
    
    // TODO: Update mesh vertices with particle positions
    // For now, we'll just render a simple representation
    CGL_mesh_render(cloth_mesh);
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

int main()
{
    // Initialize CGL
    CGL_init();
    window = CGL_window_create(800, 600, "Cloth Simulation - CGL Example");
    if (!window)
    {
        CGL_error("Failed to create window");
        return 1;
    }
    
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();
    
    // Create framebuffer
    default_framebuffer = CGL_framebuffer_create_from_default(window);
    
    // Create shaders
    present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER, PASS_THROUGH_FRAGMENT_SHADER, NULL);
    cloth_shader = CGL_shader_create(CLOTH_VERTEX_SHADER, CLOTH_FRAGMENT_SHADER, NULL);
    compute_shader = CGL_shader_compute_create(CLOTH_COMPUTE_SHADER, NULL);
    
    // Create cloth mesh
    create_cloth_mesh();
    
    // Initialize cloth simulation
    initialize_cloth();
    
    // Main loop variables
    CGL_float current_time = CGL_utils_get_time();
    CGL_float previous_time = current_time;
    CGL_float frame_time = 0.0f;
    CGL_int frames = 0, fps = 0;
    
    // Main loop
    while (!CGL_window_should_close(window))
    {
        // Calculate delta time
        current_time = CGL_utils_get_time();
        delta_time = current_time - previous_time;
        delta_time = CGL_utils_clamp(delta_time, 0.0f, 0.033f); // Cap at 30 FPS
        previous_time = current_time;
        
        frame_time += delta_time;
        frames++;
        if (frame_time >= 1.0f)
        {
            fps = frames;
            frames = 0;
            frame_time = 0.0f;
        }
        
        // Handle input
        if (CGL_window_is_key_pressed(window, CGL_KEY_SPACE))
        {
            simulation_running = !simulation_running;
        }
        if (CGL_window_is_key_pressed(window, CGL_KEY_R))
        {
            initialize_cloth();
        }
        
        // Update physics
        update_cloth_physics();
        
        // Render
        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.1f, 0.1f, 0.1f, 1.0f);
        
        render_cloth();
        
        // UI
        CGL_widgets_begin();
        
        static CGL_byte buffer[512];
        sprintf(buffer, "FPS: %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.95f, 1.0f, 0.05f);
        
        sprintf(buffer, "Frame Time: %.3f ms", delta_time * 1000.0f);
        CGL_widgets_add_string(buffer, -1.0f, 0.90f, 1.0f, 0.05f);
        
        sprintf(buffer, "Particles: %d", CLOTH_PARTICLES);
        CGL_widgets_add_string(buffer, -1.0f, 0.85f, 1.0f, 0.05f);
        
        sprintf(buffer, "Gravity: %.1f", gravity_strength);
        CGL_widgets_add_string(buffer, -1.0f, 0.80f, 1.0f, 0.05f);
        
        sprintf(buffer, "Wind: %.1f", wind_strength);
        CGL_widgets_add_string(buffer, -1.0f, 0.75f, 1.0f, 0.05f);
        
        CGL_widgets_add_string("Controls:", -1.0f, 0.65f, 1.0f, 0.05f);
        CGL_widgets_add_string("SPACE - Pause/Resume", -1.0f, 0.60f, 1.0f, 0.05f);
        CGL_widgets_add_string("R - Reset", -1.0f, 0.55f, 1.0f, 0.05f);
        CGL_widgets_add_string("UP/DOWN - Adjust Gravity", -1.0f, 0.50f, 1.0f, 0.05f);
        CGL_widgets_add_string("LEFT/RIGHT - Adjust Wind", -1.0f, 0.45f, 1.0f, 0.05f);
        
        // Interactive controls
        if (CGL_window_is_key_pressed(window, CGL_KEY_UP))
        {
            gravity_strength += 1.0f * delta_time * 10.0f;
        }
        if (CGL_window_is_key_pressed(window, CGL_KEY_DOWN))
        {
            gravity_strength -= 1.0f * delta_time * 10.0f;
        }
        if (CGL_window_is_key_pressed(window, CGL_KEY_LEFT))
        {
            wind_strength -= 1.0f * delta_time * 10.0f;
        }
        if (CGL_window_is_key_pressed(window, CGL_KEY_RIGHT))
        {
            wind_strength += 1.0f * delta_time * 10.0f;
        }
        
        CGL_widgets_end();
        
        // Swap buffers and poll events
        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }
    
    // Cleanup
    cleanup();
    
    return 0;
}

void cleanup()
{
    if (particle_ssbo) CGL_ssbo_destroy(particle_ssbo);
    if (cloth_mesh) CGL_mesh_destroy(cloth_mesh);
    if (compute_shader) CGL_shader_destroy(compute_shader);
    if (cloth_shader) CGL_shader_destroy(cloth_shader);
    if (present_shader) CGL_shader_destroy(present_shader);
    if (default_framebuffer) CGL_framebuffer_destroy(default_framebuffer);
    
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
}