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

class SierpinskiTriangle:
    def __init__(self):
        # Triangle vertices
        self.vertices = [
            (0.0, 0.8),     # Top
            (-0.8, -0.8),   # Bottom left
            (0.8, -0.8)     # Bottom right
        ]
        
        # Current point and points history
        self.current_point = (0.0, 0.0)
        self.points = []
        self.max_points = 50000
        
        # Visualization settings
        self.point_size = 0.003
        self.chaos_game = True
        self.recursive_mode = False
        self.max_depth = 6
        
        # Animation
        self.animation_speed = 100  # Points per frame
        self.auto_animate = True
        
    def chaos_game_step(self):
        """Perform one step of the chaos game"""
        # Choose random vertex
        vertex = random.choice(self.vertices)
        
        # Move halfway toward chosen vertex
        new_x = (self.current_point[0] + vertex[0]) / 2
        new_y = (self.current_point[1] + vertex[1]) / 2
        
        self.current_point = (new_x, new_y)
        
        # Add to points list
        if len(self.points) < self.max_points:
            self.points.append(self.current_point)
        else:
            # Remove oldest point and add new one
            self.points.pop(0)
            self.points.append(self.current_point)
    
    def draw_recursive_triangle(self, p1, p2, p3, depth):
        """Draw Sierpinski triangle recursively"""
        if depth <= 0:
            return
        
        # Calculate midpoints
        m1 = ((p1[0] + p2[0]) / 2, (p1[1] + p2[1]) / 2)
        m2 = ((p2[0] + p3[0]) / 2, (p2[1] + p3[1]) / 2)
        m3 = ((p3[0] + p1[0]) / 2, (p3[1] + p1[1]) / 2)
        
        # Draw inner triangle (the hole)
        cgl.widgets.set_fill_color(0.1, 0.1, 0.1)  # Background color
        cgl.widgets.add_triangle(m1[0], m1[1], m2[0], m2[1], m3[0], m3[1])
        
        # Recursively draw the three smaller triangles
        self.draw_recursive_triangle(p1, m1, m3, depth - 1)
        self.draw_recursive_triangle(m1, p2, m2, depth - 1)
        self.draw_recursive_triangle(m3, m2, p3, depth - 1)
    
    def reset(self):
        """Reset the fractal"""
        self.points.clear()
        self.current_point = (0.0, 0.0)
    
    def set_random_start(self):
        """Set a random starting point"""
        self.current_point = (
            random.uniform(-0.5, 0.5),
            random.uniform(-0.5, 0.5)
        )

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    window = cgl.window(800, 800, "Sierpinski Triangle Fractal - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Create Sierpinski triangle instance
    sierpinski = SierpinskiTriangle()
    
    # Set initial random point
    sierpinski.set_random_start()
    
    # Main loop
    while not window.should_close():
        window.set_size(800, 800)
        framebuffer.bind()
        cgl.gl_clear(0.1, 0.1, 0.1, 1.0)
        
        # Update fractal
        if sierpinski.auto_animate and sierpinski.chaos_game:
            for _ in range(sierpinski.animation_speed):
                sierpinski.chaos_game_step()
        
        # Render
        cgl.widgets.begin()
        
        if sierpinski.recursive_mode:
            draw_recursive_mode(sierpinski)
        else:
            draw_chaos_game_mode(sierpinski)
        
        # Draw UI
        draw_ui(sierpinski, window)
        
        cgl.widgets.end()
        
        # Handle input
        handle_input(sierpinski, window)
        
        if window.is_key_pressed(cgl.KEY_ESCAPE):
            break
        
        window.swap_buffers()
        window.poll_events()
    
    # Cleanup
    cgl.widgets.shutdown()
    framebuffer.destroy()
    cgl.gl_shutdown()
    window.destroy()
    cgl.shutdown()

def draw_chaos_game_mode(sierpinski):
    """Draw the chaos game mode"""
    # Draw triangle outline
    cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
    cgl.widgets.set_stroke_thickness(0.01)
    vertices = sierpinski.vertices
    cgl.widgets.add_triangle_outline(vertices[0][0], vertices[0][1],
                                   vertices[1][0], vertices[1][1],
                                   vertices[2][0], vertices[2][1])
    
    # Draw vertex points
    cgl.widgets.set_fill_color(1.0, 0.0, 0.0)
    for vertex in vertices:
        cgl.widgets.add_circle(vertex[0], vertex[1], 0.02)
    
    # Draw fractal points with color based on age
    total_points = len(sierpinski.points)
    for i, point in enumerate(sierpinski.points):
        # Color gradient from old (dark) to new (bright)
        intensity = i / max(total_points, 1)
        
        # Use different colors based on position for visual appeal
        distance_from_center = math.sqrt(point[0]**2 + point[1]**2)
        hue = distance_from_center * 2
        
        r = 0.5 + 0.5 * intensity * abs(math.sin(hue))
        g = 0.5 + 0.5 * intensity * abs(math.sin(hue + 2))
        b = 0.5 + 0.5 * intensity * abs(math.sin(hue + 4))
        
        cgl.widgets.set_fill_color(r, g, b)
        cgl.widgets.add_circle(point[0], point[1], sierpinski.point_size)
    
    # Draw current point
    cgl.widgets.set_fill_color(1.0, 1.0, 0.0)
    cgl.widgets.add_circle(sierpinski.current_point[0], sierpinski.current_point[1], 0.01)

def draw_recursive_mode(sierpinski):
    """Draw the recursive mode"""
    # Draw filled triangle first
    vertices = sierpinski.vertices
    cgl.widgets.set_fill_color(0.0, 0.5, 1.0)
    cgl.widgets.add_triangle(vertices[0][0], vertices[0][1],
                           vertices[1][0], vertices[1][1],
                           vertices[2][0], vertices[2][1])
    
    # Draw recursive triangles
    sierpinski.draw_recursive_triangle(vertices[0], vertices[1], vertices[2], sierpinski.max_depth)
    
    # Draw triangle outline
    cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
    cgl.widgets.set_stroke_thickness(0.01)
    cgl.widgets.add_triangle_outline(vertices[0][0], vertices[0][1],
                                   vertices[1][0], vertices[1][1],
                                   vertices[2][0], vertices[2][1])

def draw_ui(sierpinski, window):
    """Draw the user interface"""
    # Title
    cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
    mode_text = "Recursive Mode" if sierpinski.recursive_mode else "Chaos Game Mode"
    cgl.widgets.add_string(f"Sierpinski Triangle - {mode_text}", -0.95, 0.9, 1.9, 0.08)
    
    # Statistics
    if not sierpinski.recursive_mode:
        stats_text = f"Points: {len(sierpinski.points)}/{sierpinski.max_points}"
        cgl.widgets.add_string(stats_text, -0.95, 0.8, 0.8, 0.06)
        
        speed_text = f"Speed: {sierpinski.animation_speed} pts/frame"
        cgl.widgets.add_string(speed_text, -0.95, 0.72, 0.8, 0.06)
        
        auto_text = "Auto: ON" if sierpinski.auto_animate else "Auto: OFF"
        cgl.widgets.add_string(auto_text, -0.95, 0.64, 0.8, 0.06)
    else:
        depth_text = f"Recursion Depth: {sierpinski.max_depth}"
        cgl.widgets.add_string(depth_text, -0.95, 0.8, 0.8, 0.06)
    
    # Instructions
    instructions = [
        "M: Toggle Mode",
        "R: Reset",
        "S: Random Start",
        "A: Toggle Auto (Chaos Game)",
        "Space: Single Step (Chaos Game)",
        "+/-: Speed/Depth Control",
        "ESC: Exit"
    ]
    
    y_pos = -0.9
    for instruction in instructions:
        cgl.widgets.set_fill_color(0.7, 0.7, 0.7)
        cgl.widgets.add_string(instruction, -0.95, y_pos, 0.8, 0.05)
        y_pos += 0.08
    
    # Show current point coordinates in chaos game mode
    if not sierpinski.recursive_mode:
        coord_text = f"Current: ({sierpinski.current_point[0]:.3f}, {sierpinski.current_point[1]:.3f})"
        cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
        cgl.widgets.add_string(coord_text, 0.1, 0.8, 0.8, 0.06)

def handle_input(sierpinski, window):
    """Handle user input"""
    # Mode toggle
    if window.is_key_pressed(cgl.KEY_M):
        sierpinski.recursive_mode = not sierpinski.recursive_mode
    
    # Reset
    if window.is_key_pressed(cgl.KEY_R):
        sierpinski.reset()
    
    # Random start
    if window.is_key_pressed(cgl.KEY_S):
        sierpinski.set_random_start()
    
    # Auto animate toggle (chaos game only)
    if window.is_key_pressed(cgl.KEY_A) and not sierpinski.recursive_mode:
        sierpinski.auto_animate = not sierpinski.auto_animate
    
    # Single step (chaos game only)
    if window.is_key_pressed(cgl.KEY_SPACE) and not sierpinski.recursive_mode:
        sierpinski.chaos_game_step()
    
    # Speed/depth control
    if window.is_key_pressed(cgl.KEY_EQUAL):  # Plus key
        if sierpinski.recursive_mode:
            sierpinski.max_depth = min(8, sierpinski.max_depth + 1)
        else:
            sierpinski.animation_speed = min(1000, sierpinski.animation_speed + 10)
    
    if window.is_key_pressed(cgl.KEY_MINUS):
        if sierpinski.recursive_mode:
            sierpinski.max_depth = max(1, sierpinski.max_depth - 1)
        else:
            sierpinski.animation_speed = max(1, sierpinski.animation_speed - 10)
    
    # Mouse interaction - click to set current point
    if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_LEFT) and not sierpinski.recursive_mode:
        mouse_x, mouse_y = window.get_mouse_position()
        
        # Convert to normalized coordinates
        norm_x = (mouse_x / 800.0) * 2.0 - 1.0
        norm_y = 1.0 - (mouse_y / 800.0) * 2.0
        
        sierpinski.current_point = (norm_x, norm_y)

if __name__ == "__main__":
    main()