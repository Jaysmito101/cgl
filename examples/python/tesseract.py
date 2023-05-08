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

DIFF_VERT = """
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 texcoord;

out vec4 Position;
out vec4 Normal;
out vec4 Texcoord;

uniform bool is_present;
uniform bool render_face;
uniform vec3 fc[4];
uniform mat4 model;
uniform mat4 view_proj;

void main()
{
    mat4 mvp = view_proj * model;
    Normal = normal;
    Texcoord = texcoord;
    if (render_face)
    {
        if (gl_VertexID < 3)       gl_Position = view_proj * vec4(fc[gl_VertexID], 1.0);
        else if (gl_VertexID == 3) gl_Position = view_proj * vec4(fc[0], 1.0);
        else if (gl_VertexID == 4) gl_Position = view_proj * vec4(fc[2], 1.0);
        else if (gl_VertexID == 5) gl_Position = view_proj * vec4(fc[3], 1.0);
    }
    else
        gl_Position = mvp * position;
    Position =  gl_Position;
}
"""

DIFF_FRAG = """
#version 430 core

in vec4 Position;
in vec4 Normal;
in vec4 Texcoord;

uniform bool render_face;
uniform bool is_present;
uniform bool outer_face;
uniform float time;
uniform sampler2D tex;
out vec4 FragColor;

vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}

void main()
{
    if (render_face)
    {
        float ns =  ( noise(Position.xy + time * 0.3) +  noise(Position.yz + time * 0.3) +  noise(Position.zx + time * 0.3) );
        ns = pow(ns, 4) + 0.8;
        if (outer_face) { FragColor = vec4(0.4 * ns, 0.6 * ns, 0.8 * ns, 0.5); }
        else            { FragColor = vec4(0.4 * ns, 0.8 * ns, 0.6 * ns, 0.2); }
    }
    else FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

"""

cgl.init()
w = cgl.window(700, 700, "Tesseract - Jaysmito Mukherjee")
w.make_context_current()
cgl.gl_init()
fb = cgl.framebuffer_create_from_default(w)
fb2 = cgl.framebuffer_create(700, 700)

point_shd = cgl.shader()
point_shd.load(DIFF_VERT, DIFF_FRAG)

proj = cgl.mat4_perspective(math.pi / 4.0, 1.0, 0.1, 100.0)
view = cgl.mat4_look_at(cgl.vec3(0.0, 8.0, 8.0), cgl.vec3(0.0, 0.0, 0.0), cgl.vec3(0.0, 1.0, 0.0))
view_proj = cgl.mat4_mul(proj, view)

sph_cp = cgl.mesh_cpu_sphere(8, 8)
sph = cgl.mesh_gpu()
sph.upload_mesh_cpu(sph_cp)
sph_cp.destroy()

d = 3.0 
c = 0.5

# 16 vertices of a hypercube
points = [
    cgl.vec4(-1.0, -1.0, -1.0, 1.0),
    cgl.vec4(-1.0, -1.0,  1.0, 1.0),
    cgl.vec4(-1.0,  1.0, -1.0, 1.0),
    cgl.vec4(-1.0,  1.0,  1.0, 1.0),
    cgl.vec4( 1.0, -1.0, -1.0, 1.0),
    cgl.vec4( 1.0, -1.0,  1.0, 1.0),
    cgl.vec4( 1.0,  1.0, -1.0, 1.0),
    cgl.vec4( 1.0,  1.0,  1.0, 1.0),
    cgl.vec4(-1.0, -1.0, -1.0, -1.0),
    cgl.vec4(-1.0, -1.0,  1.0, -1.0),
    cgl.vec4(-1.0,  1.0, -1.0, -1.0),
    cgl.vec4(-1.0,  1.0,  1.0, -1.0),
    cgl.vec4( 1.0, -1.0, -1.0, -1.0),
    cgl.vec4( 1.0, -1.0,  1.0, -1.0),
    cgl.vec4( 1.0,  1.0, -1.0, -1.0),
    cgl.vec4( 1.0,  1.0,  1.0, -1.0),
]

projected_points = [cgl.vec3() for i in range(0, len(points))]

def render_point(px, py, pz, sc = 0.1):
    model = cgl.mat4_translate(px, py, pz)
    model = cgl.mat4_mul(model, cgl.mat4_scale(sc, sc, sc))
    point_shd.bind()
    point_shd.set_uniform_bool("render_face", False)
    point_shd.set_uniform_mat4("model", model)
    point_shd.set_uniform_mat4("view_proj", view_proj)
    sph.render()

def get_4d_rot_mat(theta, a, b):
    mat = cgl.matrix(4, 4)
    mat.load_identity()
    mat[a, a] = mat[b, b] = math.cos(theta)
    mat[a, b] = math.sin(theta)
    mat[b, a] = -math.sin(theta)
    return mat

# this function has not yet been ported (as of 25-04-2023)
def mul_mat4_vec4(m4, v4):
    v = cgl.vec4()
    for i in range(0, 4):
        for j in range(0, 4):
            v[i] += m4[i, j] * v4[j]
    return v

def update_points():
    for i in range(0, len(points)):
        pc = points[i]
        mat = get_4d_rot_mat(0.01, 0, 3)
        points[i] = mul_mat4_vec4(mat, pc)       

def render_face(pa: cgl.vec3, pb: cgl.vec3, pc: cgl.vec3, pd: cgl.vec3):
    point_shd.bind()
    point_shd.set_uniform_mat4("view_proj", view_proj)
    point_shd.set_uniform_bool("render_face", True)
    point_shd.set_uniform_vec3("fc[0]", pa)
    point_shd.set_uniform_vec3("fc[1]", pb)
    point_shd.set_uniform_vec3("fc[2]", pc)
    point_shd.set_uniform_vec3("fc[3]", pd)
    cgl.gl_render_screen_quad()   

while not w.should_close():
    w.set_size(700, 700)
    update_points()
    fb.bind()
    cgl.gl_clear(0.06, 0.06, 0.06, 1.0)
    view = cgl.mat4_look_at(cgl.vec3(0.0, 8.0, 8.0), cgl.vec3(0.0, 0.0, 0.0), cgl.vec3(0.0, 1.0, 0.0))
    view_proj = cgl.mat4_mul(proj, view)
    gl.glEnable(gl.GL_DEPTH_TEST)
    gl.glEnable(gl.GL_BLEND)
    gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)
    view = cgl.mat4_look_at(cgl.vec3(math.sin(cgl.utils_get_time() * 0.3) * 8.0, 8.0, math.cos(cgl.utils_get_time() * 0.3) *  8.0), cgl.vec3(0.0, 0.0, 0.0), cgl.vec3(0.0, 1.0, 0.0))
    view_proj = cgl.mat4_mul(proj, view)
    point_shd.bind()
    for i in range(0, len(points)):
        # project the 4D point to 3D
        po = points[i]
        p = cgl.vec4()
        p.x = (d - c) * po.x / (d - po.w)
        p.y = (d - c) * po.y / (d - po.w)
        p.z = (d - c) * po.z / (d - po.w)
        p.w = po.w
        projected_points[i] = cgl.vec3(p.x, p.y, p.z)
        render_point(p.x, p.y, p.z)
    # render the faces
    point_shd.bind()
    point_shd.set_uniform_float("time", cgl.utils_get_time())
    point_shd.set_uniform_bool("outer_face", True)
    render_face(projected_points[0], projected_points[1], projected_points[3], projected_points[2])
    render_face(projected_points[0], projected_points[1], projected_points[5], projected_points[4])
    render_face(projected_points[0], projected_points[2], projected_points[6], projected_points[4])
    render_face(projected_points[1], projected_points[3], projected_points[7], projected_points[5])
    render_face(projected_points[2], projected_points[3], projected_points[7], projected_points[6])
    render_face(projected_points[4], projected_points[5], projected_points[7], projected_points[6])
    point_shd.bind()
    point_shd.set_uniform_bool("outer_face", False)
    render_face(projected_points[8], projected_points[9], projected_points[11], projected_points[10])
    render_face(projected_points[8], projected_points[9], projected_points[13], projected_points[12])
    render_face(projected_points[8], projected_points[10], projected_points[14], projected_points[12])
    render_face(projected_points[9], projected_points[11], projected_points[15], projected_points[13])
    render_face(projected_points[10], projected_points[11], projected_points[15], projected_points[14])
    render_face(projected_points[12], projected_points[13], projected_points[15], projected_points[14])
    if w.is_key_pressed(cgl.KEY_ESCAPE):
        break
    w.swap_buffers()
    w.poll_events()
point_shd.destroy()
sph.destroy()
fb2.destroy()
fb.destroy()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()
