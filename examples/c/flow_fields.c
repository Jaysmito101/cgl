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
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_SKY_RENDERER
#define CGL_EXCLUDE_PHONG_RENDERER
#define CGL_EXCLUDE_TILEMAP_RENDERER
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define NUM_PARTICLES 8912
#define NUM_STEPS     16
#define SIZE          1024

static const char* VERTEX_SHADER_SOURCE = 
  "#version 430 core\n"
  "layout (location = 0) in vec4 position;\n"
  "layout (location = 1) in vec4 normal;\n"
  "layout (location = 2) in vec4 texcoord;\n"
  "out vec2 v_texcoord;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.xyz, 1.0);\n"
  "  v_texcoord = texcoord.xy;\n"
  "}\n";

static const char* FRAGMENT_SHADER_SOURCE = 
  "#version 430 core\n"
  "in vec2 v_texcoord;\n"
  "out vec4 frag_color;\n"
  "uniform sampler2D u_tex;\n"
  "\n"
  "vec3 aces(vec3 x) {\n"
  "  const float a = 2.51;\n"
  "  const float b = 0.03;\n"
  "  const float c = 2.43;\n"
  "  const float d = 0.59;\n"
  "  const float e = 0.14;\n"
  "  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);\n"
  "}\n"
  "\n"
  "void main() {\n"
  " vec2 uv = vec2(1.0f - v_texcoord.y, 1.0f - v_texcoord.x);\n"
  " vec3 col = texture(u_tex, uv).rgb;\n"
  " col = aces(col);\n"
  " col = pow(col, vec3(1.0f / 2.2f));\n"
  " frag_color = vec4(col, 1.0);\n"
  "}\n";


static const char* COMPUTE_SHADER_SOURCE = 
  "#version 430 core\n"
  "layout (local_size_x = 16, local_size_y = 16) in;\n"
  "\n"
  "#define NUM_PARTICLES 4096\n"
  "\n"
  "struct Particle {\n"
  "  vec4 position;\n"
  "  vec4 velocity;\n"
  "  vec4 color;\n"
  "};\n"
  "\n"
  "layout (std430, binding = 2) buffer particle_ssbo {\n"
  "  Particle particles[];\n"
  "};\n"
  "\n"
  "layout (rgba32f, binding = 0) uniform image2D tex0;\n"
  "layout (rgba32f, binding = 1) uniform image2D tex1;\n"
  "\n"
  "vec3 palette( in float t )\n"
  "{\n"
    "vec3 a = vec3(0.7, 0.7, 0.7); // Light gray\n"
    "vec3 b = vec3(0.7, 0.7, 0.7); // Light gray\n"
    "vec3 c = vec3(0.1, 0.2, 0.3); // Blueish-gray\n"
    "vec3 d = vec3(0.6, 0.5, 0.4); // Brownish-gray\n"

  "    return a + b * cos(6.28318 * (c * t + d));\n"
  "}\n"
  "\n"
  "uniform int u_mode;\n"
  "uniform float u_dt;\n"
  "uniform float u_time;\n"
  "uniform int u_field_blocks;\n"
  "\n"
  "uint get_particle_index() {\n"
  "  return gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x + gl_GlobalInvocationID.x;\n"
  "}\n"
  "\n"
  "float rand(uint index) {\n"
  "  return fract(sin(float(index) * 12.9898f) * 43758.5453f);\n"
  "}\n"
  "\n"
  "vec2 hash( vec2 p ) // replace this by something better\n"
  "{\n"
  "	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );\n"
	" return -1.0 + 2.0*fract(sin(p)*43758.5453123);\n"
  "}\n"

  "float noise2( in vec2 p)\n"
  "{\n"
  "    const float K1 = 0.366025404; // (sqrt(3)-1)/2;\n"
  "    const float K2 = 0.211324865; // (3-sqrt(3))/6;\n"
  "\n"
	"    vec2  i = floor( p + (p.x+p.y)*K1 ); \n"
  "    vec2  a = p - i + (i.x+i.y)*K2;\n"
  "    float m = step(a.y,a.x); \n"
  "    vec2  o = vec2(m,1.0-m);\n"
  "    vec2  b = a - o + K2;\n"
	"    vec2  c = a - 1.0 + 2.0*K2;\n"
  "    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );\n"
	"    vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0))); \n"
  "    return dot( n, vec3(70.0) );\n"
  "}\n"
  "\n"
  "float noise(in vec2 a , in float b) \n"
  "{\n"
  "    vec2 delta = vec2(sin(u_time), cos(u_time));\n"
  "    return noise2( vec2( noise2(vec2(a.x,a.y)), noise2(vec2(a.y,b)) ) + delta );\n"
  "}\n"
  "\n"
  "// field force\n"
  "vec2 f_field(vec2 uv) {\n"
  "  float f_x = floor(uv.x * float(u_field_blocks));\n"
  "  float f_y = floor(uv.y * float(u_field_blocks));\n"
  "\n"
  "  float tm = u_time * 0.1f;\n"
  "  float sc = 1.0 / 8.0f;\n"
  "  float ss = sin(u_time);\n"
  "  float angle = (noise(vec2(f_x, f_y) * 0.1 * sc + sin(tm), ss) * 2.0 - 1.0);\n"
  "  angle += (noise(vec2(f_x, f_y) * 0.2 * sc + cos(tm), ss) * 2.0 - 1.0) * 0.5f;\n"
  "  angle += (noise(vec2(f_x, f_y) * 0.4 * sc + sin(tm * 0.5f), ss) * 2.0 - 1.0) * 0.25f;\n"
  "  vec2 field = vec2(0.0); \n"
  "  field += vec2(cos(angle), sin(angle));\n"
  // add a force towards the center
  "  field += vec2(sin(u_time * 0.5f), cos(u_time * 0.5f)) * 0.3;\n"

  "  return normalize(field) * 0.8f;\n"
  "}\n"
  "\n"
  "// reset the image buffers\n"
  "void mode_0() {\n"
  "  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);\n"
  "  vec2 uv = vec2(pixel_coords) / vec2(imageSize(tex0));\n"
  "  vec4 pixel = vec4(0.0f);\n"
  "  imageStore(tex0, pixel_coords, pixel);\n"
  "  imageStore(tex1, pixel_coords, pixel);\n"
  "}\n"
  "\n"
  "// reset the particles\n"
  "void mode_1() {\n"
  "  uint index = get_particle_index();\n"
  "  if (index >= NUM_PARTICLES) return;\n"
  "  Particle p;\n"
  "  p.position = vec4((index % int(sqrt(float(NUM_PARTICLES)))) / sqrt(float(NUM_PARTICLES)), (index / sqrt(float(NUM_PARTICLES))) / sqrt(float(NUM_PARTICLES)), 0.0f, 1.0f);\n"
  // "  p.velocity = vec4(normalize(p.position.xy - vec2(0.5f)) * 0.3f, 0.0f, 0.0f);\n"
  "  p.velocity = vec4(0.0f);\n"
  "  p.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
  //"  p.color = vec4(rand(index), rand(index + 1), rand(index + 2), 1.0f);\n"
  "  particles[index] = p;\n"
  "}\n"
  "\n"
  "// update the particles\n"
  "void mode_2() {\n"
  "  uint index = get_particle_index();\n"
  "  if (index >= NUM_PARTICLES) return;\n"
  "  Particle p = particles[index];\n"
  "  p.position.xy += p.velocity.xy * u_dt;\n"
  "\n"
  "  if (p.position.x < 0.0f) p.position.x += 1.0f;\n"
  "  if (p.position.x > 1.0f) p.position.x -= 1.0f;\n"
  "  if (p.position.y < 0.0f) p.position.y += 1.0f;\n"
  "  if (p.position.y > 1.0f) p.position.y -= 1.0f;\n"
  "\n"
  "  p.velocity.xy *= 0.997f;\n"
  "  p.velocity.xy += f_field(p.position.xy) * u_dt;\n"
  "  particles[index] = p;\n"
  "\n"
  "  ivec2 pixel_coords = ivec2(p.position.xy * vec2(imageSize(tex0)));\n"
  "  vec4 pixel = vec4(palette(sin(p.velocity.x) * 0.5 - 0.25 * cos(p.velocity.y)), 1.0f);\n"
  "  float r = 2.0f;\n"
  "  int ri = 4;\n"
  "  vec4 prev_pixel, new_pixel;\n"
  "  for (int i = -ri; i <= ri; i++) {\n"
  "    for (int j = -ri; j <= ri; j++) {\n"
  "      prev_pixel = imageLoad(tex0, pixel_coords + ivec2(i, j));\n"
  "      new_pixel = 2.0f * pixel * exp(-float(i * i + j * j) * r) + prev_pixel;\n"
  "      imageStore(tex0, pixel_coords + ivec2(i, j), new_pixel);\n"
  "    }\n"
  "  }\n"
  "}\n"
  "\n"
  "// blur the image\n"
  "void mode_3() {\n"
  "  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);\n"
  "  vec4 pixel = vec4(0.0f);\n"
  "  for (int i = -1; i <= 1; i++) {\n"
  "    for (int j = -1; j <= 1; j++) {\n"
  "      pixel += imageLoad(tex0, pixel_coords + ivec2(i, j));\n"
  "    }\n"
  "  }\n"
  "  pixel /= 9.0f;\n"
  "  pixel = mix(imageLoad(tex0, pixel_coords), pixel, 0.003f);\n"
  "  pixel = pixel * 0.995f;\n"
  "  vec2 uv = vec2(pixel_coords) / vec2(imageSize(tex0));\n"
  "  vec2 field = f_field(uv) * 0.5f + 0.5f;\n"
  "  imageStore(tex1, pixel_coords, vec4(field, 0.0, 1.0));\n"
  "  imageStore(tex0, pixel_coords, pixel);\n"
  "}\n"
  "\n"
  "void main() {\n"
  " if (u_mode == 0) {\n"
  "   mode_0();\n"
  " }\n"
  " else if (u_mode == 1) {\n"
  "   mode_1();\n"
  " }\n"
  " else if (u_mode == 2) {\n"
  "   mode_2();\n"
  " }\n"
  " else if (u_mode == 3) {\n"
  "   mode_3();\n"
  " }\n"
  "}\n";
    

int main() 
{
  // NOTE: I am not checking the return value of various functions here
  //       for brevity.
  CGL_init();
  CGL_window* window = CGL_window_create(SIZE, SIZE, "Flow Fields - Jaysmito Mukherjee");
  CGL_window_make_context_current(window);
  CGL_gl_init();
  CGL_widgets_init();


  CGL_framebuffer* main_framebuffer = CGL_framebuffer_create_from_default(window);
  CGL_shader* present_shader = CGL_shader_create(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, NULL);
  CGL_shader* compute_shader = CGL_shader_compute_create(COMPUTE_SHADER_SOURCE, NULL);
  CGL_ssbo* particle_ssbo = CGL_ssbo_create(2);
  CGL_texture* tex0 = CGL_texture_create_blank(SIZE, SIZE, GL_RGBA, GL_RGBA32F, GL_FLOAT);
  CGL_texture* tex1 = CGL_texture_create_blank(SIZE, SIZE, GL_RGBA, GL_RGBA32F, GL_FLOAT);

  CGL_ssbo_set_data(particle_ssbo, sizeof(float) * 4 * 3 * NUM_PARTICLES, NULL, false);

  CGL_shader_bind(compute_shader);
  CGL_ssbo_bind2(particle_ssbo, 2);
  glBindImageTexture(0, CGL_texture_get_internal_handle(tex0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  glBindImageTexture(1, CGL_texture_get_internal_handle(tex1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_mode"), 0);
  CGL_shader_compute_dispatch(compute_shader, SIZE / 16, SIZE / 16, 1);
  CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_mode"), 1);
  CGL_shader_compute_dispatch(compute_shader, NUM_PARTICLES / (16 * 16), 1, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);


  CGL_float prev_time = CGL_utils_get_time(), curr_time = 0.0f, dt = 0.0f, speed = 1.5f;

  while(!CGL_window_should_close(window)) 
  {
    curr_time = CGL_utils_get_time();
    dt = curr_time - prev_time;
    prev_time = curr_time;
    
    CGL_window_set_size(window, SIZE, SIZE);
    CGL_window_poll_events(window);

    
    CGL_shader_bind(compute_shader);
    CGL_ssbo_bind2(particle_ssbo, 2);
    
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_field_blocks"), 256);

    for (int i = 0 ; i < NUM_STEPS ; i++ )
    {
      glBindImageTexture(0, CGL_texture_get_internal_handle(tex0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
      glBindImageTexture(1, CGL_texture_get_internal_handle(tex1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
      CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_dt"), dt * speed / (float)(NUM_STEPS));
      CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_time"), curr_time);
    
      CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_mode"), 2);
      CGL_shader_compute_dispatch(compute_shader, NUM_PARTICLES / (16 * 16), 1, 1);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    
      CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_mode"), 3);
      CGL_shader_compute_dispatch(compute_shader, SIZE / 16, SIZE / 16, 1);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    }

    CGL_framebuffer_bind(main_framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);

    CGL_shader_bind(present_shader);
    CGL_texture_bind(tex0, 1); CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 1);
    if (CGL_window_is_key_pressed(window, CGL_KEY_F)) 
    {
      CGL_texture_bind(tex1, 2); CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 2);
    }
    CGL_gl_render_screen_quad();

    if (CGL_window_is_key_pressed(window, CGL_KEY_R)) 
    {
      CGL_shader_bind(compute_shader);
      CGL_ssbo_bind2(particle_ssbo, 2);
      glBindImageTexture(0, CGL_texture_get_internal_handle(tex0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
      glBindImageTexture(1, CGL_texture_get_internal_handle(tex1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
      CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_mode"), 0);
      CGL_shader_compute_dispatch(compute_shader, SIZE / 16, SIZE / 16, 1);
      CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "u_mode"), 1);
      CGL_shader_compute_dispatch(compute_shader, NUM_PARTICLES / (16 * 16), 1, 1);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    }

    if (CGL_window_is_key_pressed(window, CGL_KEY_ESCAPE)) 
      break;

    CGL_window_swap_buffers(window);
  }

  CGL_texture_destroy(tex0);
  CGL_texture_destroy(tex1);
  CGL_ssbo_destroy(particle_ssbo);
  CGL_shader_destroy(compute_shader);
  CGL_shader_destroy(present_shader);
  CGL_framebuffer_destroy(main_framebuffer);
  CGL_widgets_shutdown();
  CGL_gl_shutdown();
  CGL_window_destroy(window);
  CGL_shutdown();
}
