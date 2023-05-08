"""
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
"""
import cgl
import math
import OpenGL.GL as gl

VERTEX_SHADER_SOURCE = """
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 texcoord;

out vec4 Position;
out vec4 Normal;

uniform mat4 model;
uniform mat4 view_proj;

void main()
{
    mat4 mvp = view_proj * model;
    Normal = normal;
    Position = model * position;
    gl_Position = mvp * position;
}
"""

FRAGMENT_SHADER_SOURCE = """
#version 430 core

in vec4 Position;
in vec4 Normal;

out vec4 FragColor;

void main()
{
    const vec3 lightPos = vec3(30.0, 20.0, -10.0);
    const vec3 lightColor = vec3(0.85, 1.0, 0.6);
    vec3 normal = normalize(Normal.xyz);
    vec3 lightDir = normalize(lightPos - Position.xyz);
    float diff = max(dot(normal, lightDir), 0.0) + 0.2;
    vec3 diffuse = diff * lightColor;
    FragColor = vec4(diffuse, 1.0);
}

"""

cgl.init()
w = cgl.window(750, 750, "Cube Waves  - Jaysmito Mukherjee")
w.make_context_current()
cgl.gl_init()
cgl.widgets.init()
fb = cgl.framebuffer_create_from_default(w)

proj = cgl.mat4_perspective(cgl.deg_to_rad(45), 1.0, 0.1, 100.0)

shd = cgl.shader()
shd.load(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)

cube_cpu = cgl.mesh_cpu_cube(False)
cube_cpu.recalculate_normals()
cube_gpu = cgl.mesh_gpu()
cube_gpu.upload_mesh_cpu(cube_cpu)
cube_cpu.destroy()

RESOLUTION = 17

while not w.should_close():
    w.set_size(750, 750)
    fb.bind()
    
    # cgl.gl_clear(0.02, 0.02, 0.02, 1.0)
    cgl.gl_clear(1.02, 1.02, 1.02, 1.0)

    view = cgl.mat4_look_at(cgl.vec3(25 * math.cos(cgl.utils_get_time() * 0.1), 25, 25 * math.sin(cgl.utils_get_time() * 0.1)), cgl.vec3(0, 0, 0), cgl.vec3(0, 1, 0))
    view_proj = cgl.mat4_mul(proj, view)
    model = cgl.mat4_identity()

    gl.glEnable(gl.GL_DEPTH_TEST) 
    gl.glDepthFunc(gl.GL_LESS)
    shd.bind()
    shd.set_uniform_mat4("view_proj", view_proj)
    for px in range(RESOLUTION):
        for pz in range(RESOLUTION):
            pos_x = px - RESOLUTION / 2 * 1.0
            pos_z = pz - RESOLUTION / 2 * 1.0
            rad = math.sqrt(pos_x * pos_x + pos_z * pos_z) 
            model = cgl.mat4_translate(pos_x, 0, pos_z)
            scale_y = math.sin(cgl.utils_get_time() * 1.5 + rad * 0.3) ** 2 * 4 + 1
            model = cgl.mat4_mul(model, cgl.mat4_scale(0.5, scale_y, 0.5))
            shd.set_uniform_mat4("model", model)
            cube_gpu.render()

    w.swap_buffers()
    w.poll_events()

cube_gpu.destroy()
shd.destroy()
fb.destroy()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()
