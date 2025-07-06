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

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    window = cgl.window(800, 600, "Basic Widgets - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Widget demo state
    demo_mode = 1
    animation_time = 0.0
    
    # Main loop
    while not window.should_close():
        current_time = cgl.utils_get_time()
        animation_time += 0.016  # Approximate 60 FPS
        
        window.set_size(800, 600)
        framebuffer.bind()
        cgl.gl_clear(0.1, 0.1, 0.1, 1.0)
        
        cgl.widgets.begin()
        
        # Draw different widget demos based on mode
        if demo_mode == 1:
            draw_basic_shapes()
        elif demo_mode == 2:
            draw_animated_shapes(animation_time)
        elif demo_mode == 3:
            draw_text_demo()
        elif demo_mode == 4:
            draw_interactive_demo(window)
        
        # Draw mode selector
        draw_mode_selector(demo_mode)
        
        cgl.widgets.end()
        
        # Handle input
        if window.is_key_pressed(cgl.KEY_1):
            demo_mode = 1
        elif window.is_key_pressed(cgl.KEY_2):
            demo_mode = 2
        elif window.is_key_pressed(cgl.KEY_3):
            demo_mode = 3
        elif window.is_key_pressed(cgl.KEY_4):
            demo_mode = 4
        elif window.is_key_pressed(cgl.KEY_ESCAPE):
            break
        
        window.swap_buffers()
        window.poll_events()
    
    # Cleanup
    cgl.widgets.shutdown()
    framebuffer.destroy()
    cgl.gl_shutdown()
    window.destroy()
    cgl.shutdown()

def draw_basic_shapes():
    """Draw basic shape widgets"""
    # Title
    cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
    cgl.widgets.add_string("Basic Shapes", -0.95, 0.85, 0.6, 0.1)
    
    # Rectangles
    cgl.widgets.set_fill_color(1.0, 0.0, 0.0)
    cgl.widgets.add_rect2f(-0.8, 0.5, 0.3, 0.2)
    
    cgl.widgets.set_fill_color(0.0, 1.0, 0.0)
    cgl.widgets.add_rect2f(-0.4, 0.5, 0.3, 0.2)
    
    cgl.widgets.set_fill_color(0.0, 0.0, 1.0)
    cgl.widgets.add_rect2f(0.0, 0.5, 0.3, 0.2)
    
    # Circles
    cgl.widgets.set_fill_color(1.0, 1.0, 0.0)
    cgl.widgets.add_circle(-0.65, 0.15, 0.1)
    
    cgl.widgets.set_fill_color(1.0, 0.0, 1.0)
    cgl.widgets.add_circle(-0.25, 0.15, 0.1)
    
    cgl.widgets.set_fill_color(0.0, 1.0, 1.0)
    cgl.widgets.add_circle(0.15, 0.15, 0.1)
    
    # Triangles
    cgl.widgets.set_fill_color(0.8, 0.4, 0.0)
    cgl.widgets.add_triangle(-0.7, -0.2, -0.6, 0.0, -0.5, -0.2)
    
    cgl.widgets.set_fill_color(0.4, 0.8, 0.0)
    cgl.widgets.add_triangle(-0.3, -0.2, -0.2, 0.0, -0.1, -0.2)
    
    cgl.widgets.set_fill_color(0.0, 0.4, 0.8)
    cgl.widgets.add_triangle(0.1, -0.2, 0.2, 0.0, 0.3, -0.2)
    
    # Lines
    cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
    cgl.widgets.set_stroke_thickness(0.01)
    cgl.widgets.add_line(-0.8, -0.5, -0.2, -0.5)
    
    cgl.widgets.set_stroke_color(1.0, 0.5, 0.0)
    cgl.widgets.set_stroke_thickness(0.02)
    cgl.widgets.add_line(-0.8, -0.6, -0.2, -0.6)
    
    # Outlined shapes
    cgl.widgets.set_stroke_color(1.0, 0.0, 0.0)
    cgl.widgets.set_stroke_thickness(0.015)
    cgl.widgets.add_rect_outline(0.4, 0.4, 0.4, 0.3)
    
    cgl.widgets.set_stroke_color(0.0, 1.0, 0.0)
    cgl.widgets.set_stroke_thickness(0.01)
    cgl.widgets.add_circle_outline(0.6, 0.0, 0.15)
    
    # Labels
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    cgl.widgets.add_string("Rectangles", -0.8, 0.3, 0.6, 0.05)
    cgl.widgets.add_string("Circles", -0.8, -0.05, 0.6, 0.05)
    cgl.widgets.add_string("Triangles", -0.8, -0.35, 0.6, 0.05)
    cgl.widgets.add_string("Lines", -0.8, -0.65, 0.6, 0.05)
    cgl.widgets.add_string("Outlines", 0.4, 0.2, 0.4, 0.05)

def draw_animated_shapes(time):
    """Draw animated shapes"""
    # Title
    cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
    cgl.widgets.add_string("Animated Shapes", -0.95, 0.85, 0.6, 0.1)
    
    # Rotating rectangles
    center_x, center_y = -0.5, 0.5
    for i in range(8):
        angle = time * 0.5 + i * math.pi / 4
        radius = 0.3
        x = center_x + radius * math.cos(angle)
        y = center_y + radius * math.sin(angle)
        
        # Color based on angle
        r = (math.sin(angle) + 1) / 2
        g = (math.cos(angle) + 1) / 2
        b = (math.sin(angle + math.pi/2) + 1) / 2
        
        cgl.widgets.set_fill_color(r, g, b)
        cgl.widgets.add_rect2f(x - 0.05, y - 0.05, 0.1, 0.1)
    
    # Pulsing circles
    for i in range(3):
        x = 0.2 + i * 0.3
        y = 0.5
        radius = 0.05 + 0.03 * math.sin(time * 2 + i * math.pi / 3)
        
        # Color cycle
        hue = (time * 0.5 + i * 0.33) % 1.0
        r = abs(math.sin(hue * math.pi * 2))
        g = abs(math.sin(hue * math.pi * 2 + math.pi * 2 / 3))
        b = abs(math.sin(hue * math.pi * 2 + math.pi * 4 / 3))
        
        cgl.widgets.set_fill_color(r, g, b)
        cgl.widgets.add_circle(x, y, radius)
    
    # Sine wave
    cgl.widgets.set_stroke_color(0.0, 1.0, 1.0)
    cgl.widgets.set_stroke_thickness(0.01)
    
    num_points = 100
    for i in range(num_points - 1):
        x1 = -0.8 + 1.6 * i / num_points
        x2 = -0.8 + 1.6 * (i + 1) / num_points
        y1 = 0.1 * math.sin(x1 * 5 + time * 3)
        y2 = 0.1 * math.sin(x2 * 5 + time * 3)
        
        cgl.widgets.add_line(x1, y1, x2, y2)
    
    # Bouncing ball
    ball_x = 0.7 * math.sin(time * 2)
    ball_y = -0.3 + 0.2 * abs(math.sin(time * 4))
    
    cgl.widgets.set_fill_color(1.0, 1.0, 0.0)
    cgl.widgets.add_circle(ball_x, ball_y, 0.05)
    
    # Rotating triangle
    angle = time
    size = 0.1
    triangle_x, triangle_y = 0.0, -0.6
    
    # Calculate triangle points
    points = []
    for i in range(3):
        a = angle + i * 2 * math.pi / 3
        px = triangle_x + size * math.cos(a)
        py = triangle_y + size * math.sin(a)
        points.append((px, py))
    
    cgl.widgets.set_fill_color(1.0, 0.5, 0.0)
    cgl.widgets.add_triangle(points[0][0], points[0][1], 
                           points[1][0], points[1][1], 
                           points[2][0], points[2][1])

def draw_text_demo():
    """Draw text demonstration"""
    # Title
    cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
    cgl.widgets.add_string("Text Demo", -0.95, 0.85, 0.6, 0.1)
    
    # Different sizes
    texts = [
        ("Large Text", 0.15),
        ("Medium Text", 0.1),
        ("Small Text", 0.06),
        ("Tiny Text", 0.04)
    ]
    
    y_pos = 0.6
    for text, size in texts:
        cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
        cgl.widgets.add_string(text, -0.8, y_pos, 1.0, size)
        y_pos -= size + 0.05
    
    # Colored text
    colors = [
        (1.0, 0.0, 0.0, "Red Text"),
        (0.0, 1.0, 0.0, "Green Text"),
        (0.0, 0.0, 1.0, "Blue Text"),
        (1.0, 1.0, 0.0, "Yellow Text"),
        (1.0, 0.0, 1.0, "Magenta Text"),
        (0.0, 1.0, 1.0, "Cyan Text")
    ]
    
    y_pos = 0.1
    for r, g, b, text in colors:
        cgl.widgets.set_fill_color(r, g, b)
        cgl.widgets.add_string(text, -0.8, y_pos, 1.0, 0.08)
        y_pos -= 0.1
    
    # Centered text
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    cgl.widgets.add_string("Centered Text", -0.3, -0.7, 0.6, 0.1)

def draw_interactive_demo(window):
    """Draw interactive elements"""
    # Title
    cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
    cgl.widgets.add_string("Interactive Demo", -0.95, 0.85, 0.6, 0.1)
    
    # Get mouse position
    mouse_x, mouse_y = window.get_mouse_position()
    
    # Convert to normalized coordinates
    norm_x = (mouse_x / 800.0) * 2.0 - 1.0
    norm_y = 1.0 - (mouse_y / 600.0) * 2.0
    
    # Draw mouse cursor
    cgl.widgets.set_fill_color(1.0, 0.0, 0.0)
    cgl.widgets.add_circle(norm_x, norm_y, 0.03)
    
    # Draw crosshair
    cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
    cgl.widgets.set_stroke_thickness(0.005)
    cgl.widgets.add_line(norm_x - 0.1, norm_y, norm_x + 0.1, norm_y)
    cgl.widgets.add_line(norm_x, norm_y - 0.1, norm_x, norm_y + 0.1)
    
    # Mouse position display
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    pos_text = f"Mouse: ({norm_x:.2f}, {norm_y:.2f})"
    cgl.widgets.add_string(pos_text, -0.8, 0.6, 0.8, 0.08)
    
    # Interactive buttons (simple rectangles that change color on hover)
    button_rects = [
        (-0.8, 0.3, 0.3, 0.15, "Button 1"),
        (-0.4, 0.3, 0.3, 0.15, "Button 2"),
        (0.0, 0.3, 0.3, 0.15, "Button 3"),
        (0.4, 0.3, 0.3, 0.15, "Button 4")
    ]
    
    for x, y, w, h, label in button_rects:
        # Check if mouse is over button
        if (x <= norm_x <= x + w and y <= norm_y <= y + h):
            # Hover state
            cgl.widgets.set_fill_color(0.3, 0.3, 0.8)
            
            # Check for click
            if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_LEFT):
                cgl.widgets.set_fill_color(0.8, 0.3, 0.3)
        else:
            # Normal state
            cgl.widgets.set_fill_color(0.2, 0.2, 0.2)
        
        # Draw button
        cgl.widgets.add_rect2f(x, y, w, h)
        
        # Draw button outline
        cgl.widgets.set_stroke_color(0.6, 0.6, 0.6)
        cgl.widgets.set_stroke_thickness(0.01)
        cgl.widgets.add_rect_outline(x, y, w, h)
        
        # Draw label
        cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
        cgl.widgets.add_string(label, x + 0.05, y + 0.05, w - 0.1, h - 0.1)
    
    # Draw keyboard state
    key_states = [
        (cgl.KEY_W, "W"),
        (cgl.KEY_A, "A"),
        (cgl.KEY_S, "S"),
        (cgl.KEY_D, "D"),
        (cgl.KEY_SPACE, "SPACE")
    ]
    
    y_pos = 0.0
    for key, label in key_states:
        if window.is_key_pressed(key):
            cgl.widgets.set_fill_color(0.0, 1.0, 0.0)
            status = "PRESSED"
        else:
            cgl.widgets.set_fill_color(0.5, 0.5, 0.5)
            status = "RELEASED"
        
        text = f"{label}: {status}"
        cgl.widgets.add_string(text, -0.8, y_pos, 0.8, 0.06)
        y_pos -= 0.08
    
    # Instructions
    cgl.widgets.set_fill_color(0.7, 0.7, 0.7)
    cgl.widgets.add_string("Move mouse around, click buttons, press WASD and SPACE", -0.8, -0.7, 1.6, 0.06)

def draw_mode_selector(current_mode):
    """Draw the mode selector"""
    modes = [
        "1: Basic Shapes",
        "2: Animated Shapes", 
        "3: Text Demo",
        "4: Interactive Demo"
    ]
    
    cgl.widgets.set_fill_color(0.0, 0.0, 0.0)
    cgl.widgets.add_rect2f(-0.95, -0.95, 1.9, 0.15)
    
    for i, mode_text in enumerate(modes):
        if i + 1 == current_mode:
            cgl.widgets.set_fill_color(1.0, 1.0, 0.0)  # Highlight current mode
        else:
            cgl.widgets.set_fill_color(0.7, 0.7, 0.7)
        
        x = -0.9 + i * 0.45
        cgl.widgets.add_string(mode_text, x, -0.92, 0.4, 0.08)

if __name__ == "__main__":
    main()