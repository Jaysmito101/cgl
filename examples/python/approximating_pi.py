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
import random
import math

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    window = cgl.window(700, 700, "Approximating Pi - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Monte Carlo variables
    MAX_POINTS = 10000
    points = []
    points_in_circle = 0.0
    points_in_square = 0.0
    pi_estimate = 0.0
    point_size = 0.01
    
    # Timing variables
    prev_time = cgl.utils_get_time()
    frame_count = 0
    frame_rate = 0
    frame_time = 0.0
    
    # Main loop
    while not window.should_close():
        # Update timing
        curr_time = cgl.utils_get_time()
        delta_time = curr_time - prev_time
        prev_time = curr_time
        frame_time += delta_time
        frame_count += 1
        
        if frame_time >= 1.0:
            frame_rate = frame_count
            frame_time = 0.0
            frame_count = 0
        
        # Generate random point
        x = random.uniform(-1.0, 1.0)
        y = random.uniform(-1.0, 1.0)
        
        # Add point to list (limit size)
        if len(points) < MAX_POINTS:
            points.append((x, y))
        else:
            # Remove oldest point and add new one
            points.pop(0)
            points.append((x, y))
        
        # Calculate pi estimate
        points_in_square += 1.0
        distance = math.sqrt(x * x + y * y)
        if distance <= 1.0:
            points_in_circle += 1.0
        
        if points_in_square > 0:
            pi_estimate = 4.0 * (points_in_circle / points_in_square)
        
        # Render
        window.set_size(700, 700)
        framebuffer.bind()
        cgl.gl_clear(0.0, 0.0, 0.0, 1.0)
        
        # Begin widget rendering
        cgl.widgets.begin()
        
        # Draw circle outline
        cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
        cgl.widgets.set_stroke_thickness(0.01)
        cgl.widgets.add_circle_outline(0.0, 0.0, 1.0)
        
        # Draw square outline
        cgl.widgets.add_rect_outline(-1.0, -1.0, 2.0, 2.0)
        
        # Draw all points
        for px, py in points:
            distance = math.sqrt(px * px + py * py)
            if distance <= 1.0:
                cgl.widgets.set_fill_color(0.0, 0.8, 0.0)  # Green for inside circle
            else:
                cgl.widgets.set_fill_color(0.8, 0.0, 0.0)  # Red for outside circle
            cgl.widgets.add_rect2f(px, py, point_size, point_size)
        
        # Display pi estimate
        cgl.widgets.set_fill_color(0.0, 0.0, 0.0)
        cgl.widgets.add_rect2f(-0.4, -0.15, 0.8, 0.3)
        
        cgl.widgets.set_fill_color(0.7, 0.7, 0.7)
        pi_text = f"Pi ≈ {pi_estimate:.6f}"
        cgl.widgets.add_string(pi_text, -0.4, -0.05, 0.8, 0.1)
        
        # Display point count
        count_text = f"Points: {int(points_in_square)}"
        cgl.widgets.add_string(count_text, -0.4, -0.15, 0.8, 0.1)
        
        # Display instructions
        cgl.widgets.set_fill_color(0.5, 0.5, 0.5)
        cgl.widgets.add_string("Press SPACE to reset", -0.4, 0.8, 0.8, 0.1)
        
        cgl.widgets.end()
        
        # Handle input
        if window.is_key_pressed(cgl.KEY_SPACE):
            # Reset simulation
            points.clear()
            points_in_circle = 0.0
            points_in_square = 0.0
            pi_estimate = 0.0
        
        if window.is_key_pressed(cgl.KEY_ESCAPE):
            break
        
        # Update window title with FPS
        window.set_title(f"Approximating Pi - CGL Python Example | FPS: {frame_rate}")
        
        window.swap_buffers()
        window.poll_events()
    
    # Cleanup
    cgl.widgets.shutdown()
    framebuffer.destroy()
    cgl.gl_shutdown()
    window.destroy()
    cgl.shutdown()

if __name__ == "__main__":
    main()