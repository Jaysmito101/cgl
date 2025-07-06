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

class MandelbrotViewer:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.pixels = [[0 for _ in range(width)] for _ in range(height)]
        
        # View parameters
        self.center_x = -0.5
        self.center_y = 0.0
        self.zoom = 1.0
        self.max_iterations = 100
        
        # Colors
        self.colors = self.generate_color_palette()
        
    def generate_color_palette(self):
        """Generate a color palette for the fractal"""
        colors = []
        num_colors = 256
        
        for i in range(num_colors):
            t = i / num_colors
            
            # Create a smooth color transition
            r = int(255 * (0.5 + 0.5 * math.sin(t * 2 * math.pi + 0)))
            g = int(255 * (0.5 + 0.5 * math.sin(t * 2 * math.pi + 2)))
            b = int(255 * (0.5 + 0.5 * math.sin(t * 2 * math.pi + 4)))
            
            colors.append((r / 255.0, g / 255.0, b / 255.0))
        
        return colors
        
    def mandelbrot_iteration(self, cx, cy):
        """Calculate Mandelbrot iteration count for a point"""
        x, y = 0.0, 0.0
        
        for i in range(self.max_iterations):
            x2, y2 = x * x, y * y
            
            if x2 + y2 > 4.0:
                return i
                
            # z = z^2 + c
            x, y = x2 - y2 + cx, 2 * x * y + cy
            
        return self.max_iterations
        
    def calculate_fractal(self):
        """Calculate the Mandelbrot set for current view"""
        # Calculate view bounds
        aspect_ratio = self.width / self.height
        view_width = 4.0 / self.zoom
        view_height = view_width / aspect_ratio
        
        min_x = self.center_x - view_width / 2
        max_x = self.center_x + view_width / 2
        min_y = self.center_y - view_height / 2
        max_y = self.center_y + view_height / 2
        
        # Calculate each pixel
        for y in range(self.height):
            for x in range(self.width):
                # Map pixel to complex plane
                cx = min_x + (max_x - min_x) * x / self.width
                cy = min_y + (max_y - min_y) * y / self.height
                
                # Calculate iteration count
                iterations = self.mandelbrot_iteration(cx, cy)
                
                # Store result
                self.pixels[y][x] = iterations
                
    def get_color(self, iterations):
        """Get color for iteration count"""
        if iterations >= self.max_iterations:
            return (0.0, 0.0, 0.0)  # Black for points in the set
        
        # Use smooth coloring
        color_index = int(iterations * len(self.colors) / self.max_iterations)
        color_index = min(color_index, len(self.colors) - 1)
        
        return self.colors[color_index]
        
    def zoom_in(self, x, y, factor=2.0):
        """Zoom in at a specific point"""
        # Convert screen coordinates to complex plane
        aspect_ratio = self.width / self.height
        view_width = 4.0 / self.zoom
        view_height = view_width / aspect_ratio
        
        min_x = self.center_x - view_width / 2
        max_x = self.center_x + view_width / 2
        min_y = self.center_y - view_height / 2
        max_y = self.center_y + view_height / 2
        
        # Calculate new center
        self.center_x = min_x + (max_x - min_x) * x / self.width
        self.center_y = min_y + (max_y - min_y) * y / self.height
        
        # Apply zoom
        self.zoom *= factor
        
    def zoom_out(self, factor=2.0):
        """Zoom out"""
        self.zoom /= factor
        
    def reset_view(self):
        """Reset to default view"""
        self.center_x = -0.5
        self.center_y = 0.0
        self.zoom = 1.0

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    WINDOW_WIDTH = 800
    WINDOW_HEIGHT = 600
    window = cgl.window(WINDOW_WIDTH, WINDOW_HEIGHT, "Mandelbrot Set Viewer - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Create Mandelbrot viewer
    FRACTAL_WIDTH = 200
    FRACTAL_HEIGHT = 150
    mandelbrot = MandelbrotViewer(FRACTAL_WIDTH, FRACTAL_HEIGHT)
    
    # Calculate initial fractal
    mandelbrot.calculate_fractal()
    
    # Timing for recalculation
    need_recalc = False
    last_calc_time = cgl.utils_get_time()
    
    # Main loop
    while not window.should_close():
        current_time = cgl.utils_get_time()
        
        window.set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
        framebuffer.bind()
        cgl.gl_clear(0.0, 0.0, 0.0, 1.0)
        
        # Recalculate if needed (but not too frequently)
        if need_recalc and (current_time - last_calc_time) > 0.1:
            mandelbrot.calculate_fractal()
            need_recalc = False
            last_calc_time = current_time
        
        # Handle mouse input for zooming
        mouse_x, mouse_y = window.get_mouse_position()
        
        # Convert mouse position to fractal coordinates
        fractal_x = int(mouse_x / WINDOW_WIDTH * FRACTAL_WIDTH)
        fractal_y = int((WINDOW_HEIGHT - mouse_y) / WINDOW_HEIGHT * FRACTAL_HEIGHT)
        
        if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_LEFT):
            mandelbrot.zoom_in(fractal_x, fractal_y, 1.5)
            need_recalc = True
            
        if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_RIGHT):
            mandelbrot.zoom_out(1.5)
            need_recalc = True
        
        # Render
        cgl.widgets.begin()
        
        # Draw fractal
        pixel_width = 2.0 / FRACTAL_WIDTH
        pixel_height = 2.0 / FRACTAL_HEIGHT
        
        for y in range(FRACTAL_HEIGHT):
            for x in range(FRACTAL_WIDTH):
                pixel_x = -1.0 + x * pixel_width
                pixel_y = -1.0 + y * pixel_height
                
                iterations = mandelbrot.pixels[y][x]
                color = mandelbrot.get_color(iterations)
                
                cgl.widgets.set_fill_color(color[0], color[1], color[2])
                cgl.widgets.add_rect2f(pixel_x, pixel_y, pixel_width, pixel_height)
        
        # Draw UI
        ui_y = 0.85
        cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
        
        # Display information
        zoom_text = f"Zoom: {mandelbrot.zoom:.2f}"
        cgl.widgets.add_string(zoom_text, -0.95, ui_y, 0.3, 0.08)
        
        center_text = f"Center: ({mandelbrot.center_x:.6f}, {mandelbrot.center_y:.6f})"
        cgl.widgets.add_string(center_text, -0.6, ui_y, 0.8, 0.08)
        
        iter_text = f"Max Iterations: {mandelbrot.max_iterations}"
        cgl.widgets.add_string(iter_text, -0.95, ui_y - 0.1, 0.4, 0.08)
        
        # Instructions
        instructions = [
            "Left Click: Zoom In",
            "Right Click: Zoom Out",
            "R: Reset View",
            "=/-: Increase/Decrease Iterations",
            "ESC: Exit"
        ]
        
        for i, instruction in enumerate(instructions):
            cgl.widgets.add_string(instruction, -0.95, ui_y - 0.25 - i * 0.08, 0.6, 0.06)
        
        # Draw crosshair at mouse position
        if 0 <= mouse_x <= WINDOW_WIDTH and 0 <= mouse_y <= WINDOW_HEIGHT:
            mouse_norm_x = (mouse_x / WINDOW_WIDTH) * 2.0 - 1.0
            mouse_norm_y = 1.0 - (mouse_y / WINDOW_HEIGHT) * 2.0
            
            cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
            cgl.widgets.set_stroke_thickness(0.005)
            cgl.widgets.add_line(mouse_norm_x - 0.05, mouse_norm_y, mouse_norm_x + 0.05, mouse_norm_y)
            cgl.widgets.add_line(mouse_norm_x, mouse_norm_y - 0.05, mouse_norm_x, mouse_norm_y + 0.05)
        
        cgl.widgets.end()
        
        # Handle keyboard input
        if window.is_key_pressed(cgl.KEY_R):
            mandelbrot.reset_view()
            need_recalc = True
            
        if window.is_key_pressed(cgl.KEY_EQUAL):  # Plus key
            mandelbrot.max_iterations = min(1000, mandelbrot.max_iterations + 10)
            need_recalc = True
            
        if window.is_key_pressed(cgl.KEY_MINUS):
            mandelbrot.max_iterations = max(10, mandelbrot.max_iterations - 10)
            need_recalc = True
            
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

if __name__ == "__main__":
    main()