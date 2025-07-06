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
 * - CPU-based physics calculations with Verlet integration
 * - Triangular mesh rendering with Phong shading
 * - Spring constraints for cloth structure
 * - Interactive parameter control
 * 
 * Technical Details:
 * - 32x32 grid of particles (1024 total)
 * - Structural and shear spring constraints
 * - Gravity, wind, and damping forces
 * - Ground collision detection
 * - Triangular mesh with proper lighting
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
#include <math.h>

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
#define CLOTH_TRIANGLES ((CLOTH_WIDTH - 1) * (CLOTH_HEIGHT - 1) * 2)  // Two triangles per quad
#define CLOTH_INDICES (CLOTH_TRIANGLES * 3)  // Three indices per triangle

// Particle structure for CPU-based simulation
typedef struct {
    CGL_vec3 position;     // Current position
    CGL_vec3 prev_position; // Previous position for Verlet integration
    CGL_vec3 velocity;     // Current velocity
    CGL_vec3 normal;       // Surface normal for lighting
    CGL_float mass;        // Particle mass
    CGL_bool pinned;       // Whether particle is fixed in place
} Particle;

// Spring constraint structure
typedef struct {
    CGL_int p1, p2;        // Particle indices
    CGL_float rest_length; // Natural length of spring
    CGL_float stiffness;   // Spring stiffness
} Spring;

// Global state structure
static struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_phong_pipeline* phong_pipeline;
    CGL_phong_mat* cloth_material;
    CGL_camera* camera;
    CGL_mesh* cloth_mesh;
    
    // Cloth simulation data
    Particle* particles;
    Spring* springs;
    CGL_int spring_count;
    CGL_float* vertices;     // Vertex data for mesh
    CGL_float* normals;      // Normal data for mesh
    CGL_uint* indices;       // Index data for triangulation
    
    // Simulation parameters
    CGL_float delta_time;
    CGL_float previous_time;
    CGL_float gravity_strength;
    CGL_float wind_strength;
    CGL_float damping;
    CGL_float spring_stiffness;
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
    
    // Window dimensions for UI scaling
    CGL_int window_width;
    CGL_int window_height;
} g_State;

// Function declarations
CGL_bool init();
void initialize_cloth();
void create_cloth_mesh();
void update_cloth_physics();
void update_cloth_mesh();
void calculate_cloth_normals();
void render_cloth();
void cleanup();
EM_BOOL loop(double time, void* userData);
CGL_int get_particle_index(CGL_int x, CGL_int y);

// Helper function to get particle index from 2D coordinates
CGL_int get_particle_index(CGL_int x, CGL_int y)
{
    if (x < 0 || x >= CLOTH_WIDTH || y < 0 || y >= CLOTH_HEIGHT)
        return -1;
    return y * CLOTH_WIDTH + x;
}

void initialize_cloth()
{
    // Allocate memory for particles
    g_State.particles = (Particle*)malloc(CLOTH_PARTICLES * sizeof(Particle));
    
    // Initialize particle positions and properties
    for (CGL_int y = 0; y < CLOTH_HEIGHT; y++)
    {
        for (CGL_int x = 0; x < CLOTH_WIDTH; x++)
        {
            CGL_int index = get_particle_index(x, y);
            if (index < 0) continue;
            
            // Calculate normalized position
            CGL_float fx = (CGL_float)x / (CGL_float)(CLOTH_WIDTH - 1);
            CGL_float fy = (CGL_float)y / (CGL_float)(CLOTH_HEIGHT - 1);
            
            // Set initial position
            CGL_vec3 pos = CGL_vec3_init(
                (fx - 0.5f) * g_State.cloth_size,
                2.0f,
                (fy - 0.5f) * g_State.cloth_size
            );
            
            g_State.particles[index].position = pos;
            g_State.particles[index].prev_position = pos;
            g_State.particles[index].velocity = CGL_vec3_init(0.0f, 0.0f, 0.0f);
            g_State.particles[index].normal = CGL_vec3_init(0.0f, 1.0f, 0.0f);
            g_State.particles[index].mass = 1.0f;
            g_State.particles[index].pinned = CGL_FALSE;
            
            // Pin top corners
            if ((x == 0 || x == CLOTH_WIDTH - 1) && y == 0)
            {
                g_State.particles[index].pinned = CGL_TRUE;
            }
        }
    }
    
    // Create spring constraints
    g_State.spring_count = 0;
    CGL_int max_springs = (CLOTH_WIDTH - 1) * CLOTH_HEIGHT + CLOTH_WIDTH * (CLOTH_HEIGHT - 1) + 
                          (CLOTH_WIDTH - 1) * (CLOTH_HEIGHT - 1) * 2;
    g_State.springs = (Spring*)malloc(max_springs * sizeof(Spring));
    
    // Create structural springs (horizontal and vertical)
    for (CGL_int y = 0; y < CLOTH_HEIGHT; y++)
    {
        for (CGL_int x = 0; x < CLOTH_WIDTH; x++)
        {
            CGL_int index = get_particle_index(x, y);
            if (index < 0) continue;
            
            // Horizontal springs
            if (x < CLOTH_WIDTH - 1)
            {
                CGL_int neighbor = get_particle_index(x + 1, y);
                if (neighbor >= 0)
                {
                    g_State.springs[g_State.spring_count].p1 = index;
                    g_State.springs[g_State.spring_count].p2 = neighbor;
                    g_State.springs[g_State.spring_count].rest_length = g_State.cloth_size / (CGL_float)(CLOTH_WIDTH - 1);
                    g_State.springs[g_State.spring_count].stiffness = g_State.spring_stiffness;
                    g_State.spring_count++;
                }
            }
            
            // Vertical springs
            if (y < CLOTH_HEIGHT - 1)
            {
                CGL_int neighbor = get_particle_index(x, y + 1);
                if (neighbor >= 0)
                {
                    g_State.springs[g_State.spring_count].p1 = index;
                    g_State.springs[g_State.spring_count].p2 = neighbor;
                    g_State.springs[g_State.spring_count].rest_length = g_State.cloth_size / (CGL_float)(CLOTH_HEIGHT - 1);
                    g_State.springs[g_State.spring_count].stiffness = g_State.spring_stiffness;
                    g_State.spring_count++;
                }
            }
            
            // Diagonal springs (shear)
            if (x < CLOTH_WIDTH - 1 && y < CLOTH_HEIGHT - 1)
            {
                CGL_int neighbor1 = get_particle_index(x + 1, y + 1);
                CGL_int neighbor2 = get_particle_index(x + 1, y - 1);
                
                if (neighbor1 >= 0)
                {
                    g_State.springs[g_State.spring_count].p1 = index;
                    g_State.springs[g_State.spring_count].p2 = neighbor1;
                    g_State.springs[g_State.spring_count].rest_length = 
                        sqrtf(2.0f) * g_State.cloth_size / (CGL_float)(CLOTH_WIDTH - 1);
                    g_State.springs[g_State.spring_count].stiffness = g_State.spring_stiffness * 0.5f;
                    g_State.spring_count++;
                }
                
                if (neighbor2 >= 0 && y > 0)
                {
                    g_State.springs[g_State.spring_count].p1 = index;
                    g_State.springs[g_State.spring_count].p2 = neighbor2;
                    g_State.springs[g_State.spring_count].rest_length = 
                        sqrtf(2.0f) * g_State.cloth_size / (CGL_float)(CLOTH_WIDTH - 1);
                    g_State.springs[g_State.spring_count].stiffness = g_State.spring_stiffness * 0.5f;
                    g_State.spring_count++;
                }
            }
        }
    }
    
    // Create mesh data
    create_cloth_mesh();
}

void create_cloth_mesh()
{
    // Allocate memory for mesh data
    g_State.vertices = (CGL_float*)malloc(CLOTH_PARTICLES * 3 * sizeof(CGL_float));
    g_State.normals = (CGL_float*)malloc(CLOTH_PARTICLES * 3 * sizeof(CGL_float));
    g_State.indices = (CGL_uint*)malloc(CLOTH_INDICES * sizeof(CGL_uint));
    
    // Create triangle indices for cloth mesh
    CGL_uint index = 0;
    for (CGL_int y = 0; y < CLOTH_HEIGHT - 1; y++)
    {
        for (CGL_int x = 0; x < CLOTH_WIDTH - 1; x++)
        {
            CGL_uint p1 = get_particle_index(x, y);
            CGL_uint p2 = get_particle_index(x + 1, y);
            CGL_uint p3 = get_particle_index(x, y + 1);
            CGL_uint p4 = get_particle_index(x + 1, y + 1);
            
            // First triangle (p1, p2, p3)
            g_State.indices[index++] = p1;
            g_State.indices[index++] = p2;
            g_State.indices[index++] = p3;
            
            // Second triangle (p2, p4, p3)
            g_State.indices[index++] = p2;
            g_State.indices[index++] = p4;
            g_State.indices[index++] = p3;
        }
    }
    
    // Create mesh
    g_State.cloth_mesh = CGL_mesh_create();
    
    // Update mesh data
    update_cloth_mesh();
}

void update_cloth_physics()
{
    if (!g_State.simulation_running) return;
    
    // Apply forces and update positions using Verlet integration
    for (CGL_int i = 0; i < CLOTH_PARTICLES; i++)
    {
        if (g_State.particles[i].pinned) continue;
        
        CGL_vec3 pos = g_State.particles[i].position;
        CGL_vec3 prev_pos = g_State.particles[i].prev_position;
        CGL_float mass = g_State.particles[i].mass;
        
        // Calculate forces
        CGL_vec3 force = CGL_vec3_init(0.0f, 0.0f, 0.0f);
        
        // Gravity
        force.y += g_State.gravity_strength * mass;
        
        // Wind
        force.x += g_State.wind_strength * 0.1f;
        
        // Verlet integration
        CGL_vec3 acceleration = CGL_vec3_scale(force, 1.0f / mass);
        CGL_vec3 velocity = CGL_vec3_scale(CGL_vec3_sub(pos, prev_pos), 1.0f - g_State.damping);
        CGL_vec3 new_pos = CGL_vec3_add(pos, CGL_vec3_add(velocity, 
            CGL_vec3_scale(acceleration, g_State.delta_time * g_State.delta_time)));
        
        // Ground collision
        if (new_pos.y < -1.0f)
        {
            new_pos.y = -1.0f;
        }
        
        // Update particle
        g_State.particles[i].prev_position = pos;
        g_State.particles[i].position = new_pos;
        g_State.particles[i].velocity = CGL_vec3_scale(CGL_vec3_sub(new_pos, pos), 1.0f / g_State.delta_time);
    }
    
    // Apply spring constraints
    for (CGL_int iteration = 0; iteration < 3; iteration++)  // Multiple iterations for stability
    {
        for (CGL_int i = 0; i < g_State.spring_count; i++)
        {
            Spring* spring = &g_State.springs[i];
            Particle* p1 = &g_State.particles[spring->p1];
            Particle* p2 = &g_State.particles[spring->p2];
            
            CGL_vec3 diff = CGL_vec3_sub(p2->position, p1->position);
            CGL_float distance = CGL_vec3_length(diff);
            
            if (distance > 0.0001f)
            {
                CGL_vec3 correction = CGL_vec3_scale(diff, 
                    (spring->rest_length - distance) / distance * 0.5f);
                
                if (!p1->pinned)
                {
                    p1->position = CGL_vec3_sub(p1->position, correction);
                }
                if (!p2->pinned)
                {
                    p2->position = CGL_vec3_add(p2->position, correction);
                }
            }
        }
    }
    
    // Calculate normals for lighting
    calculate_cloth_normals();
    
    // Update mesh data
    update_cloth_mesh();
}

void calculate_cloth_normals()
{
    // Reset normals
    for (CGL_int i = 0; i < CLOTH_PARTICLES; i++)
    {
        g_State.particles[i].normal = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    }
    
    // Calculate normals from triangles
    for (CGL_int i = 0; i < CLOTH_INDICES; i += 3)
    {
        CGL_uint i1 = g_State.indices[i];
        CGL_uint i2 = g_State.indices[i + 1];
        CGL_uint i3 = g_State.indices[i + 2];
        
        CGL_vec3 p1 = g_State.particles[i1].position;
        CGL_vec3 p2 = g_State.particles[i2].position;
        CGL_vec3 p3 = g_State.particles[i3].position;
        
        CGL_vec3 normal = CGL_vec3_cross(CGL_vec3_sub(p2, p1), CGL_vec3_sub(p3, p1));
        normal = CGL_vec3_normalize(normal);
        
        // Add to vertex normals
        g_State.particles[i1].normal = CGL_vec3_add(g_State.particles[i1].normal, normal);
        g_State.particles[i2].normal = CGL_vec3_add(g_State.particles[i2].normal, normal);
        g_State.particles[i3].normal = CGL_vec3_add(g_State.particles[i3].normal, normal);
    }
    
    // Normalize vertex normals
    for (CGL_int i = 0; i < CLOTH_PARTICLES; i++)
    {
        g_State.particles[i].normal = CGL_vec3_normalize(g_State.particles[i].normal);
    }
}

void update_cloth_mesh()
{
    // Update vertex positions
    for (CGL_int i = 0; i < CLOTH_PARTICLES; i++)
    {
        g_State.vertices[i * 3] = g_State.particles[i].position.x;
        g_State.vertices[i * 3 + 1] = g_State.particles[i].position.y;
        g_State.vertices[i * 3 + 2] = g_State.particles[i].position.z;
        
        g_State.normals[i * 3] = g_State.particles[i].normal.x;
        g_State.normals[i * 3 + 1] = g_State.particles[i].normal.y;
        g_State.normals[i * 3 + 2] = g_State.particles[i].normal.z;
    }
    
    // Update mesh
    CGL_mesh_destroy(g_State.cloth_mesh);
    g_State.cloth_mesh = CGL_mesh_create();
    CGL_mesh_add_vertex_f3(g_State.cloth_mesh, g_State.vertices, CLOTH_PARTICLES);
    CGL_mesh_add_normal_f3(g_State.cloth_mesh, g_State.normals, CLOTH_PARTICLES);
    CGL_mesh_add_index_i(g_State.cloth_mesh, g_State.indices, CLOTH_INDICES);
    CGL_mesh_upload(g_State.cloth_mesh, true);
}

void render_cloth()
{
    // Update camera position
    g_State.camera_angle += g_State.delta_time * 0.2f;
    g_State.camera_pos.x = 5.0f * cosf(g_State.camera_angle);
    g_State.camera_pos.z = 5.0f * sinf(g_State.camera_angle);
    
    CGL_camera_set_position(g_State.camera, g_State.camera_pos);
    CGL_camera_set_target(g_State.camera, g_State.camera_target);
    CGL_camera_recalculate_mat(g_State.camera);
    
    // Render cloth using Phong shading
    CGL_mat4 model_matrix = CGL_mat4_identity();
    CGL_phong_render_begin(g_State.phong_pipeline, g_State.camera);
    CGL_phong_render(g_State.cloth_mesh, &model_matrix, g_State.cloth_material, g_State.phong_pipeline, g_State.camera);
    CGL_phong_render_end(g_State.phong_pipeline, g_State.camera);
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
    
    // Create Phong pipeline and material
    g_State.phong_pipeline = CGL_phong_pipeline_create();
    CGL_phong_pipeline_add_light(g_State.phong_pipeline, 
        CGL_phong_light_directional(CGL_vec3_init(0.5f, -1.0f, 0.3f), CGL_vec3_init(0.8f, 0.8f, 0.8f), 1.0f));
    CGL_phong_pipeline_add_light(g_State.phong_pipeline, 
        CGL_phong_light_directional(CGL_vec3_init(-0.5f, -1.0f, -0.3f), CGL_vec3_init(0.4f, 0.4f, 0.4f), 0.5f));
    
    g_State.cloth_material = CGL_phong_mat_create();
    CGL_phong_mat_set_diffuse_color(g_State.cloth_material, CGL_vec3_init(0.8f, 0.2f, 0.2f));
    CGL_phong_mat_set_specular_color(g_State.cloth_material, CGL_vec3_init(0.3f, 0.3f, 0.3f));
    CGL_phong_mat_set_ambient_color(g_State.cloth_material, CGL_vec3_init(0.2f, 0.05f, 0.05f));
    CGL_phong_mat_set_shininess(g_State.cloth_material, 32.0f);
    
    // Create camera
    g_State.camera = CGL_camera_create();
    CGL_camera_set_aspect_ratio(g_State.camera, 800.0f / 600.0f);
    
    // Initialize simulation parameters
    g_State.delta_time = 0.0f;
    g_State.gravity_strength = -9.8f;
    g_State.wind_strength = 0.0f;
    g_State.damping = 0.01f;
    g_State.spring_stiffness = 50.0f;
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
    
    // Initialize window dimensions
    g_State.window_width = 800;
    g_State.window_height = 600;
    
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
    
    // Update window dimensions
    CGL_window_get_size(g_State.window, &g_State.window_width, &g_State.window_height);
    CGL_camera_set_aspect_ratio(g_State.camera, (CGL_float)g_State.window_width / (CGL_float)g_State.window_height);
    
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
    
    // UI with responsive scaling
    CGL_widgets_begin();
    
    // Calculate responsive UI positions and sizes
    CGL_float ui_scale = CGL_utils_min((CGL_float)g_State.window_width / 800.0f, (CGL_float)g_State.window_height / 600.0f);
    CGL_float ui_text_height = 0.05f / ui_scale;
    CGL_float ui_margin = 0.02f / ui_scale;
    CGL_float ui_x = -1.0f + ui_margin;
    CGL_float ui_y = 1.0f - ui_margin - ui_text_height;
    
    static CGL_byte buffer[512];
    
    // Performance information
    sprintf(buffer, "FPS: %d", g_State.fps);
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    sprintf(buffer, "Frame Time: %.3f ms", g_State.delta_time * 1000.0f);
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    sprintf(buffer, "Particles: %d", CLOTH_PARTICLES);
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    sprintf(buffer, "Triangles: %d", CLOTH_TRIANGLES);
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    // Simulation parameters
    ui_y -= ui_margin;
    sprintf(buffer, "Gravity: %.1f", g_State.gravity_strength);
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    sprintf(buffer, "Wind: %.1f", g_State.wind_strength);
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    sprintf(buffer, "Status: %s", g_State.simulation_running ? "Running" : "Paused");
    CGL_widgets_add_string(buffer, ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    // Controls
    ui_y -= ui_margin;
    CGL_widgets_add_string("Controls:", ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    CGL_widgets_add_string("SPACE - Pause/Resume", ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    CGL_widgets_add_string("R - Reset", ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    CGL_widgets_add_string("UP/DOWN - Adjust Gravity", ui_x, ui_y, 0.3f, ui_text_height);
    ui_y -= ui_text_height + ui_margin * 0.5f;
    
    CGL_widgets_add_string("LEFT/RIGHT - Adjust Wind", ui_x, ui_y, 0.3f, ui_text_height);
    
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

// NOTE: This version uses CPU-based physics simulation and works with both native and WASM builds
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
    // Free memory
    if (g_State.particles) free(g_State.particles);
    if (g_State.springs) free(g_State.springs);
    if (g_State.vertices) free(g_State.vertices);
    if (g_State.normals) free(g_State.normals);
    if (g_State.indices) free(g_State.indices);
    
    // Destroy CGL objects
    if (g_State.cloth_mesh) CGL_mesh_destroy(g_State.cloth_mesh);
    if (g_State.cloth_material) CGL_phong_mat_destroy(g_State.cloth_material);
    if (g_State.phong_pipeline) CGL_phong_pipeline_destroy(g_State.phong_pipeline);
    if (g_State.camera) CGL_camera_destroy(g_State.camera);
    if (g_State.default_framebuffer) CGL_framebuffer_destroy(g_State.default_framebuffer);
    
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}