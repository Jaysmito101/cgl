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
import cv2
import mediapipe as mp
import OpenGL.GL as gl
import musicalbeeps



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
    const vec3 lightPos = vec3(5.0, 5.0, 5.0);
    const vec3 lightColor = vec3(0.85, 1.0, 0.6);
    vec3 normal = normalize(Normal.xyz);
    vec3 lightDir = normalize(lightPos - Position.xyz);
    float diff = max(dot(normal, lightDir), 0.0) + 0.2;
    vec3 diffuse = diff * lightColor;
    FragColor = vec4(diffuse, 1.0);
}

"""


class HandDataProcessor:
    def __init__(self):
        self.detector = mp.solutions.hands.Hands()
        self.results = None
        self.landmark_points = []
        self.left_hand_landmarks = []
        self.right_hand_landmarks = []
        self.left_hand_world_landmarks = []
        self.right_hand_world_landmarks = []
        self.both_hands = False
        self.left_hand = False
        self.right_hand = False
    
    def process(self, frame):
        self.results = self.detector.process(frame)
        right_hand_index = left_hand_index = -1

        self.left_hand = self.right_hand = self.both_hands = False
        self.landmark_points = []
        self.left_hand_landmarks = []
        self.right_hand_landmarks = []
        self.left_hand_world_landmarks = []
        self.right_hand_world_landmarks = []

        if self.results.multi_hand_landmarks is None:
            return False

        for clasf in self.results.multi_handedness:
            if clasf.classification[0].label == "Right":
                right_hand_index = clasf.classification[0].index
            elif clasf.classification[0].label == "Left":
                left_hand_index = clasf.classification[0].index

        self.left_hand = left_hand_index != -1
        self.right_hand = right_hand_index != -1
        self.both_hands = self.left_hand and self.right_hand

        if self.left_hand:
            for lm in self.results.multi_hand_landmarks[left_hand_index].landmark:
                self.left_hand_landmarks.append((lm.x, lm.y, lm.z))
                self.landmark_points.append((lm.x, lm.y, lm.z))
            
            for lm in self.results.multi_hand_world_landmarks[left_hand_index].landmark:
                self.left_hand_world_landmarks.append((lm.x, lm.y, lm.z))
        
        if self.right_hand:
            if right_hand_index >= len(self.results.multi_hand_landmarks):
                right_hand_index = 0

            for lm in self.results.multi_hand_landmarks[right_hand_index].landmark:
                self.right_hand_landmarks.append((lm.x, lm.y, lm.z))
                self.landmark_points.append((lm.x, lm.y, lm.z))
            
            for lm in self.results.multi_hand_world_landmarks[right_hand_index].landmark:
                self.right_hand_world_landmarks.append((lm.x, lm.y, lm.z))


        return True

def draw_hand_landmarks(handler, size, px, py, psx, psy):
    # red for left hand
    cgl.widgets.set_fill_colorf(1.0, 0.0, 0.0, 1.0)
    cgl.widgets.add_rect2f(px, py, size, size)
    
    if handler.left_hand:
        for lm in handler.left_hand_landmarks:
            x, y = lm[0], 1.0 - lm[1]
            cgl.widgets.add_rect2f(px + x * psx, py + y * psy, size, size)
    
    # blue for right hand
    cgl.widgets.set_fill_colorf(0.0, 0.0, 1.0, 1.0)
    if handler.right_hand:
        for lm in handler.right_hand_landmarks:
            x, y = lm[0], 1.0 - lm[1]
            cgl.widgets.add_rect2f(px + x * psx, py + y * psy, size, size)

def draw_box(msh, shd, x, y, z, size):
    model = cgl.mat4_translate(x, y, z)
    model = cgl.mat4_mul(model, cgl.mat4_scale(size, size, size))
    shd.set_uniform_mat4("model", model)
    msh.render()

cam = cv2.VideoCapture(0)
cgl.init()
w = cgl.window(750, 750, "ASCII Video  - Jaysmito Mukherjee")
w.make_context_current()

cgl.gl_init()
cgl.widgets.init()
cgl.text_init()

fb = cgl.framebuffer_create_from_default(w)
tex = cgl.texture_create_blank(cam.get(3), cam.get(4), gl.GL_BGR, gl.GL_RGB8, gl.GL_UNSIGNED_BYTE)
dummy_tex = cgl.texture_create_blank(5, 5, gl.GL_RGB, gl.GL_RGB8, gl.GL_UNSIGNED_BYTE)
dummy_tex.set_data(bytes([255 for i in range(0, 10 * 10 * 3)]))
hand_data_processor = HandDataProcessor()
proj = cgl.mat4_perspective(cgl.deg_to_rad(45), 1.0, 0.1, 100.0)
shd = cgl.shader()
shd.load(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)
cube_cpu = cgl.mesh_cpu_cube(False)
cube_cpu.recalculate_normals()
cube_gpu = cgl.mesh_gpu()
cube_gpu.upload_mesh_cpu(cube_cpu)
cube_cpu.destroy()
music_player = musicalbeeps.Player(volume = 1.0, mute_output = False)


frame_count = 0 

while not w.should_close():
    frame_count += 1

    w.set_size(600, 600)
    fb.bind()
    cgl.gl_clear(0.02, 0.02, 0.02, 1.0)

    ret, frame = cam.read()
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame = cv2.flip(frame, 1)
    flippedFrame = cv2.flip(frame, 0)
    tex.set_data(bytes(flippedFrame))

    # process hands data every few frames
    if frame_count % 5 == 0:
        hand_data_processor.process(frame)


    view = cgl.mat4_look_at(cgl.vec3(0, 10, -5.01), cgl.vec3(0, 0, 0), cgl.vec3(0, 1, 0))
    view_proj = cgl.mat4_mul(proj, view)
    model = cgl.mat4_identity()
    gl.glEnable(gl.GL_DEPTH_TEST) 
    gl.glDepthFunc(gl.GL_LESS)
    shd.bind()
    shd.set_uniform_mat4("view_proj", view_proj)

    if hand_data_processor.right_hand:
        lm = hand_data_processor.right_hand_landmarks[0]
        x, z = lm[0] * 2.0 - 1.0, lm[1] * 2.0 - 1.0
        draw_box(cube_gpu, shd, x * 3, 0, z * 3, 0.1)
        
        for lm in hand_data_processor.right_hand_landmarks:
            x, z = lm[0], lm[1]
            x, z = x * 2 - 1, z * 2 - 1
            draw_box(cube_gpu, shd, x * 3, 0, z * 3, 0.1)

    # music_player.play_note("A", 0.1)

    gl.glDisable(gl.GL_DEPTH_TEST) 
    cgl.widgets.begin()
    
    cgl.widgets.set_texture(tex)
    cgl.widgets.add_rect2f(0.6, 0.6, 0.4, 0.4)
    
#    cgl.widgets.flush()
    
    cgl.widgets.set_texture(dummy_tex)
    draw_hand_landmarks(hand_data_processor, 0.01, 0.6, 0.6, 0.4, 0.4)

    cgl.widgets.end()

    w.swap_buffers()
    w.poll_events()

cam.release() 
cube_gpu.destroy()
shd.destroy()
dummy_tex.destroy()  
tex.destroy()
fb.destroy()
cgl.text_shutdown()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()