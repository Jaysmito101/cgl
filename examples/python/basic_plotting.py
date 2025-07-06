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
import random

def sine_wave(x):
    """Simple sine wave function"""
    return math.sin(x)

def complex_wave(x):
    """Complex wave with multiple frequencies"""
    time = cgl.utils_get_time()
    return (math.sin(x + time) + 
            math.sin(x * 2 + time) * 0.5 + 
            math.sin(x * 4 + time) * 0.25 + 
            math.sin(x * 8 + time) * 0.125)

def quadratic_function(x):
    """Quadratic function"""
    return x * x * 0.1

def oscillating_function(x):
    """Oscillating function with time"""
    time = cgl.utils_get_time()
    return (math.sin(x + time) + math.cos(x * 3.0 + time)) / 2.0

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    window = cgl.window(800, 600, "Basic Plotting - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Plot modes
    plot_mode = 1
    
    # Main loop
    while not window.should_close():
        window.set_size(800, 600)
        framebuffer.bind()
        cgl.gl_clear(0.1, 0.1, 0.1, 1.0)
        
        cgl.widgets.begin()
        
        # Draw different plots based on mode
        if plot_mode == 1:
            # Function plots in four quadrants
            draw_function_plots()
        elif plot_mode == 2:
            # Scatter plot
            draw_scatter_plot()
        elif plot_mode == 3:
            # Bar chart
            draw_bar_chart()
        elif plot_mode == 4:
            # Pie chart
            draw_pie_chart()
        
        # Draw mode indicator
        cgl.widgets.set_fill_color(0.7, 0.7, 0.7)
        mode_text = f"Mode: {plot_mode} (Press 1-4 to change)"
        cgl.widgets.add_string(mode_text, -0.95, 0.85, 1.9, 0.1)
        
        cgl.widgets.end()
        
        # Handle input
        if window.is_key_pressed(cgl.KEY_1):
            plot_mode = 1
        elif window.is_key_pressed(cgl.KEY_2):
            plot_mode = 2
        elif window.is_key_pressed(cgl.KEY_3):
            plot_mode = 3
        elif window.is_key_pressed(cgl.KEY_4):
            plot_mode = 4
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

def draw_function_plots():
    """Draw function plots in four quadrants"""
    # Top-left: Sine wave
    plot_function(sine_wave, -0.95, 0.05, 0.9, 0.8, 
                 -math.pi * 2, math.pi * 2, -1.5, 1.5,
                 (1.0, 0.0, 0.0), "sin(x)")
    
    # Top-right: Complex wave
    plot_function(complex_wave, 0.05, 0.05, 0.9, 0.8,
                 -math.pi, math.pi, -2.0, 2.0,
                 (0.0, 1.0, 0.0), "Complex Wave")
    
    # Bottom-left: Quadratic
    plot_function(quadratic_function, -0.95, -0.85, 0.9, 0.8,
                 -5.0, 5.0, -0.5, 2.5,
                 (0.0, 0.0, 1.0), "x²")
    
    # Bottom-right: Oscillating
    plot_function(oscillating_function, 0.05, -0.85, 0.9, 0.8,
                 -math.pi, math.pi, -1.5, 1.5,
                 (1.0, 0.0, 1.0), "Oscillating")

def plot_function(func, x_pos, y_pos, width, height, x_min, x_max, y_min, y_max, color, title):
    """Plot a function in the specified region"""
    # Draw background
    cgl.widgets.set_fill_color(0.05, 0.05, 0.05)
    cgl.widgets.add_rect2f(x_pos, y_pos, width, height)
    
    # Draw axes
    cgl.widgets.set_stroke_color(0.3, 0.3, 0.3)
    cgl.widgets.set_stroke_thickness(0.01)
    
    # X-axis
    y_zero = y_pos + height * (0 - y_min) / (y_max - y_min)
    if y_min <= 0 <= y_max:
        cgl.widgets.add_line(x_pos, y_zero, x_pos + width, y_zero)
    
    # Y-axis
    x_zero = x_pos + width * (0 - x_min) / (x_max - x_min)
    if x_min <= 0 <= x_max:
        cgl.widgets.add_line(x_zero, y_pos, x_zero, y_pos + height)
    
    # Plot function
    cgl.widgets.set_stroke_color(color[0], color[1], color[2])
    cgl.widgets.set_stroke_thickness(0.005)
    
    num_points = 200
    prev_x = None
    prev_y = None
    
    for i in range(num_points):
        x = x_min + (x_max - x_min) * i / (num_points - 1)
        try:
            y = func(x)
        except:
            y = 0
        
        # Convert to screen coordinates
        screen_x = x_pos + width * (x - x_min) / (x_max - x_min)
        screen_y = y_pos + height * (y - y_min) / (y_max - y_min)
        
        # Clamp to plot area
        if screen_y < y_pos:
            screen_y = y_pos
        elif screen_y > y_pos + height:
            screen_y = y_pos + height
        
        if prev_x is not None and prev_y is not None:
            cgl.widgets.add_line(prev_x, prev_y, screen_x, screen_y)
        
        prev_x = screen_x
        prev_y = screen_y
    
    # Draw title
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    cgl.widgets.add_string(title, x_pos, y_pos + height + 0.02, width, 0.08)

def draw_scatter_plot():
    """Draw a scatter plot"""
    # Generate random data points
    random.seed(42)  # For consistent results
    points = []
    for _ in range(50):
        x = random.uniform(-3, 3)
        y = x * x + random.uniform(-2, 2)  # Quadratic with noise
        points.append((x, y))
    
    # Plot setup
    x_pos, y_pos, width, height = -0.8, -0.6, 1.6, 1.2
    x_min, x_max = -4, 4
    y_min, y_max = -3, 10
    
    # Draw background and axes
    cgl.widgets.set_fill_color(0.05, 0.05, 0.05)
    cgl.widgets.add_rect2f(x_pos, y_pos, width, height)
    
    # Draw axes
    cgl.widgets.set_stroke_color(0.3, 0.3, 0.3)
    cgl.widgets.set_stroke_thickness(0.01)
    
    # X-axis
    y_zero = y_pos + height * (0 - y_min) / (y_max - y_min)
    cgl.widgets.add_line(x_pos, y_zero, x_pos + width, y_zero)
    
    # Y-axis
    x_zero = x_pos + width * (0 - x_min) / (x_max - x_min)
    cgl.widgets.add_line(x_zero, y_pos, x_zero, y_pos + height)
    
    # Plot points
    cgl.widgets.set_fill_color(1.0, 0.0, 0.0)
    point_size = 0.02
    
    for x, y in points:
        screen_x = x_pos + width * (x - x_min) / (x_max - x_min)
        screen_y = y_pos + height * (y - y_min) / (y_max - y_min)
        
        if y_pos <= screen_y <= y_pos + height:
            cgl.widgets.add_circle(screen_x, screen_y, point_size)
    
    # Title
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    cgl.widgets.add_string("Scatter Plot", x_pos, y_pos + height + 0.05, width, 0.08)

def draw_bar_chart():
    """Draw a bar chart"""
    # Sample data
    categories = ["A", "B", "C", "D", "E", "F"]
    values = [23, 45, 56, 78, 32, 67]
    colors = [(1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0), 
              (1.0, 1.0, 0.0), (1.0, 0.0, 1.0), (0.0, 1.0, 1.0)]
    
    # Plot setup
    x_pos, y_pos, width, height = -0.8, -0.6, 1.6, 1.2
    max_value = max(values)
    
    # Draw background
    cgl.widgets.set_fill_color(0.05, 0.05, 0.05)
    cgl.widgets.add_rect2f(x_pos, y_pos, width, height)
    
    # Draw bars
    bar_width = width / len(values) * 0.8
    bar_spacing = width / len(values)
    
    for i, (value, color) in enumerate(zip(values, colors)):
        bar_height = height * value / max_value
        bar_x = x_pos + i * bar_spacing + bar_spacing * 0.1
        bar_y = y_pos
        
        cgl.widgets.set_fill_color(color[0], color[1], color[2])
        cgl.widgets.add_rect2f(bar_x, bar_y, bar_width, bar_height)
        
        # Draw category label
        cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
        cgl.widgets.add_string(categories[i], bar_x, bar_y - 0.1, bar_width, 0.05)
    
    # Title
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    cgl.widgets.add_string("Bar Chart", x_pos, y_pos + height + 0.05, width, 0.08)

def draw_pie_chart():
    """Draw a pie chart"""
    # Sample data
    values = [25, 35, 15, 20, 5]
    colors = [(1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0), 
              (1.0, 1.0, 0.0), (1.0, 0.0, 1.0)]
    labels = ["Red", "Green", "Blue", "Yellow", "Magenta"]
    
    # Calculate angles
    total = sum(values)
    angles = [2 * math.pi * value / total for value in values]
    
    # Draw pie slices
    center_x, center_y = 0.0, 0.0
    radius = 0.4
    current_angle = 0
    
    for i, (angle, color, label) in enumerate(zip(angles, colors, labels)):
        # Draw slice (simplified as triangle fan)
        cgl.widgets.set_fill_color(color[0], color[1], color[2])
        
        # Create slice as multiple triangles
        num_segments = max(3, int(angle / (2 * math.pi) * 32))
        for j in range(num_segments):
            angle1 = current_angle + j * angle / num_segments
            angle2 = current_angle + (j + 1) * angle / num_segments
            
            x1 = center_x + radius * math.cos(angle1)
            y1 = center_y + radius * math.sin(angle1)
            x2 = center_x + radius * math.cos(angle2)
            y2 = center_y + radius * math.sin(angle2)
            
            # Draw triangle (center, point1, point2)
            cgl.widgets.add_triangle(center_x, center_y, x1, y1, x2, y2)
        
        current_angle += angle
    
    # Draw legend
    legend_x = 0.6
    legend_y = 0.4
    for i, (color, label, value) in enumerate(zip(colors, labels, values)):
        y = legend_y - i * 0.15
        cgl.widgets.set_fill_color(color[0], color[1], color[2])
        cgl.widgets.add_rect2f(legend_x, y, 0.05, 0.05)
        
        cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
        text = f"{label}: {value}"
        cgl.widgets.add_string(text, legend_x + 0.08, y, 0.3, 0.05)
    
    # Title
    cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
    cgl.widgets.add_string("Pie Chart", -0.2, 0.7, 0.4, 0.08)

if __name__ == "__main__":
    main()