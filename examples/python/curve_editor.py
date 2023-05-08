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

cgl.init()
w = cgl.window(512, 512, "Curve Editor")
w.make_context_current()
cgl.gl_init()
cgl.widgets.init()
fb = cgl.framebuffer_create_from_default(w)

selected_point = 0
line_resolution = 10
frame_number = 0
points = [
    cgl.vec2(-0.8, 0.0),
    cgl.vec2(0.8, 0.0),
    cgl.vec2(0.0, 0.5),
    cgl.vec2(0.0, -0.5)
]

while not w.should_close():
    frame_number += 1
    w.set_size(512, 512)
    fb.bind()
    cgl.gl_clear(0.2, 0.2, 0.2, 1.0)


    cgl.widgets.begin()

    for i in range(0, 4):
        point = points[i]
        cgl.widgets.set_fill_colorf(0.9, 0.8, 0.2)
        if i == selected_point:
            cgl.widgets.add_circle2f(point.x, point.y, 0.06)
        if i < 2:
            cgl.widgets.set_fill_colorf(0.2, 0.8, 0.2)
        else:
            cgl.widgets.set_fill_colorf(0.8, 0.2, 0.2)
        cgl.widgets.add_circle2f(point.x, point.y, 0.05)
    
    cgl.widgets.set_stroke_colorf(0.8, 0.7, 0.3)
    cgl.widgets.set_stroke_thickness(0.01)
    cgl.widgets.add_cubic_bazier2v(points[0], points[1], points[2], points[3], line_resolution)
    cgl.widgets.add_cubic_bazier_points2v(points[0], points[1], points[2], points[3], line_resolution)

    cgl.widgets.set_fill_colorf(0.8, 0.8, 0.8)
    cgl.widgets.add_string(f"Line Resolution: {line_resolution}", -1.0, 0.9, 1.0, 0.1)
    
    if w.is_key_pressed(cgl.KEY_UP):
        line_resolution += 1
        cgl.utils_sleep(0.5)
    
    if w.is_key_pressed(cgl.KEY_DOWN):
        line_resolution -= 1
        cgl.utils_sleep(0.5)    

    mpx, mpy = w.get_cursor_pos()
    mpx = mpx / 512.0 * 2.0 - 1.0
    mpy = 2.0 - mpy / 512.0 * 2.0 - 1.0
    selected_point = -1
    for i in range(0, 4):
        point = points[i]
        if math.sqrt((point.x - mpx) ** 2 + (point.y - mpy) ** 2) < 0.1:
            selected_point = i
            break
    
    if selected_point != -1:
        if w.get_mouse_button(cgl.MOUSE_BUTTON_LEFT) == cgl.PRESS:
            points[selected_point] = cgl.vec2(mpx, mpy)
    
    cgl.widgets.end()

    w.swap_buffers()
    w.poll_events()

fb.destroy()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()
