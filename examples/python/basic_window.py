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

def main():
    # Initialize CGL
    cgl.init()
    
    # Create a window
    window = cgl.window(640, 480, "Basic Window - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL
    cgl.gl_init()
    
    # Create framebuffer from default
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Main loop
    while not window.should_close():
        # Set window size
        window.set_size(640, 480)
        
        # Bind framebuffer and clear
        framebuffer.bind()
        cgl.gl_clear(0.2, 0.3, 0.4, 1.0)  # Clear with a nice blue color
        
        # Poll events and swap buffers
        window.poll_events()
        window.swap_buffers()
        
        # Exit on ESC key
        if window.is_key_pressed(cgl.KEY_ESCAPE):
            break
    
    # Cleanup
    framebuffer.destroy()
    cgl.gl_shutdown()
    window.destroy()
    cgl.shutdown()

if __name__ == "__main__":
    main()