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
import cv2
import OpenGL.GL as gl
cam = cv2.VideoCapture(0)
# ascii_density = "N@#W$9876543210?!abc;:+=-,._ "
ascii_density = "N@#W$9876543210?!abc;:+=-,._      "
ascii_resolution = 64
cgl.init()
w = cgl.window(750, 750, "ASCII Video  - Jaysmito Mukherjee")
w.make_context_current()
cgl.gl_init()
cgl.widgets.init()
cgl.text_init()
fb = cgl.framebuffer_create_from_default(w)
tex = cgl.texture_create_blank(ascii_resolution, ascii_resolution, gl.GL_BGR, gl.GL_RGB, gl.GL_UNSIGNED_BYTE)
# source: https://www.fontsquirrel.com/fonts/download/camingocode
fnt = cgl.font()
fnt.load("font.ttf")
fnt.build_atlas(1024, 1024, 48)
while not w.should_close():
    w.set_size(750, 750)
    fb.bind()
    cgl.gl_clear(0.02, 0.02, 0.02, 1.0)
    cgl.widgets.begin()
    ret, frame = cam.read()
    frame = cv2.resize(frame, (ascii_resolution, ascii_resolution))
    tex.set_data(bytes(frame))

    cgl.widgets.set_texture(tex)
    # cgl.widgets.add_rect2f(-0.5, -0.5, 1.0, 1.0)
    for py_i in range(ascii_resolution):
        for px_i in range(ascii_resolution):
            rgb = cgl.vec3(frame[py_i, px_i, 0], frame[py_i, px_i, 1], frame[py_i, px_i, 2])
            rgb = rgb / cgl.vec3(256, 256, 256)
            pixel_lum = 0.2126 * rgb.x + 0.7152 * rgb.y + 0.0722 * rgb.z
            pixel_lum = rgb.dot(cgl.vec3(0, 1, 0))
            pixel_lum = int(pixel_lum*len(ascii_density))
            pos_x = px_i / ascii_resolution * 2 - 1
            pos_y = (1.0 - py_i / ascii_resolution) * 2 - 1
            size_x = 1.0 / ascii_resolution
            # i did this to filter out the background in my case
            if rgb.z < 0.3:
                pixel_lum = 0
            cgl.widgets.set_fill_colorf(rgb.x, rgb.y, rgb.z, 1.0)
            cgl.widgets.add_string_with_font(ascii_density[len(ascii_density) - 1 - pixel_lum], fnt, pos_x, pos_y, size_x, 1.0)
            
    cgl.widgets.end()
    w.swap_buffers()
    w.poll_events()
cam.release()   
fnt.destroy()
tex.destroy()
fb.destroy()
cgl.text_shutdown()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()
