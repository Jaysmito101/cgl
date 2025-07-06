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

class Ball:
    def __init__(self, x, y, radius, color):
        self.x = x
        self.y = y
        self.vx = random.uniform(-5, 5)
        self.vy = random.uniform(-5, 5)
        self.radius = radius
        self.color = color
        self.mass = radius * radius * math.pi  # Mass proportional to area
        self.restitution = 0.8  # Bounce damping
        
    def update(self, dt, gravity):
        """Update ball position and velocity"""
        # Apply gravity
        self.vy += gravity * dt
        
        # Update position
        self.x += self.vx * dt
        self.y += self.vy * dt
        
        # Boundary collisions
        if self.x - self.radius <= -1.0:
            self.x = -1.0 + self.radius
            self.vx = -self.vx * self.restitution
        elif self.x + self.radius >= 1.0:
            self.x = 1.0 - self.radius
            self.vx = -self.vx * self.restitution
            
        if self.y - self.radius <= -1.0:
            self.y = -1.0 + self.radius
            self.vy = -self.vy * self.restitution
        elif self.y + self.radius >= 1.0:
            self.y = 1.0 - self.radius
            self.vy = -self.vy * self.restitution
    
    def draw(self):
        """Draw the ball"""
        cgl.widgets.set_fill_color(self.color[0], self.color[1], self.color[2])
        cgl.widgets.add_circle(self.x, self.y, self.radius)
        
        # Draw velocity vector
        cgl.widgets.set_stroke_color(1.0, 1.0, 1.0)
        cgl.widgets.set_stroke_thickness(0.002)
        end_x = self.x + self.vx * 0.1
        end_y = self.y + self.vy * 0.1
        cgl.widgets.add_line(self.x, self.y, end_x, end_y)

class PhysicsSimulation:
    def __init__(self):
        self.balls = []
        self.gravity = -9.8
        self.air_resistance = 0.99
        self.show_vectors = True
        self.show_trails = False
        self.paused = False
        self.collision_detection = True
        
        # Trail system
        self.trails = []
        self.max_trail_length = 50
        
        # Create initial balls
        self.create_random_balls(5)
    
    def create_random_balls(self, count):
        """Create random balls"""
        self.balls.clear()
        self.trails.clear()
        
        colors = [
            (1.0, 0.0, 0.0),  # Red
            (0.0, 1.0, 0.0),  # Green
            (0.0, 0.0, 1.0),  # Blue
            (1.0, 1.0, 0.0),  # Yellow
            (1.0, 0.0, 1.0),  # Magenta
            (0.0, 1.0, 1.0),  # Cyan
            (1.0, 0.5, 0.0),  # Orange
            (0.5, 0.0, 1.0),  # Purple
        ]
        
        for i in range(count):
            x = random.uniform(-0.8, 0.8)
            y = random.uniform(-0.5, 0.8)
            radius = random.uniform(0.02, 0.08)
            color = colors[i % len(colors)]
            
            ball = Ball(x, y, radius, color)
            self.balls.append(ball)
            
            # Initialize trail
            self.trails.append([])
    
    def update(self, dt):
        """Update physics simulation"""
        if self.paused:
            return
            
        # Update balls
        for ball in self.balls:
            ball.update(dt, self.gravity)
            
            # Apply air resistance
            ball.vx *= self.air_resistance
            ball.vy *= self.air_resistance
        
        # Handle ball-to-ball collisions
        if self.collision_detection:
            for i in range(len(self.balls)):
                for j in range(i + 1, len(self.balls)):
                    self.handle_collision(self.balls[i], self.balls[j])
        
        # Update trails
        if self.show_trails:
            for i, ball in enumerate(self.balls):
                trail = self.trails[i]
                trail.append((ball.x, ball.y))
                
                # Limit trail length
                if len(trail) > self.max_trail_length:
                    trail.pop(0)
    
    def handle_collision(self, ball1, ball2):
        """Handle collision between two balls"""
        dx = ball2.x - ball1.x
        dy = ball2.y - ball1.y
        distance = math.sqrt(dx*dx + dy*dy)
        
        min_distance = ball1.radius + ball2.radius
        
        if distance < min_distance:
            # Collision detected
            
            # Normalize collision vector
            if distance > 0:
                dx /= distance
                dy /= distance
            else:
                dx, dy = 1.0, 0.0
            
            # Separate balls
            overlap = min_distance - distance
            separation = overlap / 2
            
            ball1.x -= dx * separation
            ball1.y -= dy * separation
            ball2.x += dx * separation
            ball2.y += dy * separation
            
            # Calculate relative velocity
            dvx = ball2.vx - ball1.vx
            dvy = ball2.vy - ball1.vy
            
            # Relative velocity along collision normal
            dvn = dvx * dx + dvy * dy
            
            # Do not resolve if velocities are separating
            if dvn > 0:
                return
            
            # Collision impulse
            impulse = 2 * dvn / (ball1.mass + ball2.mass)
            
            # Apply impulse
            ball1.vx += impulse * ball2.mass * dx
            ball1.vy += impulse * ball2.mass * dy
            ball2.vx -= impulse * ball1.mass * dx
            ball2.vy -= impulse * ball1.mass * dy
            
            # Apply restitution
            ball1.vx *= ball1.restitution
            ball1.vy *= ball1.restitution
            ball2.vx *= ball2.restitution
            ball2.vy *= ball2.restitution
    
    def draw(self):
        """Draw the simulation"""
        # Draw trails
        if self.show_trails:
            for i, trail in enumerate(self.trails):
                if len(trail) > 1:
                    color = self.balls[i].color
                    cgl.widgets.set_stroke_color(color[0], color[1], color[2])
                    cgl.widgets.set_stroke_thickness(0.001)
                    
                    for j in range(len(trail) - 1):
                        alpha = j / len(trail)  # Fade trail
                        cgl.widgets.add_line(trail[j][0], trail[j][1], 
                                           trail[j+1][0], trail[j+1][1])
        
        # Draw balls
        for ball in self.balls:
            ball.draw()
        
        # Draw boundaries
        cgl.widgets.set_stroke_color(0.5, 0.5, 0.5)
        cgl.widgets.set_stroke_thickness(0.01)
        cgl.widgets.add_rect_outline(-1.0, -1.0, 2.0, 2.0)
    
    def add_ball_at_mouse(self, x, y):
        """Add a ball at mouse position"""
        if len(self.balls) < 20:  # Limit number of balls
            color = (random.random(), random.random(), random.random())
            radius = random.uniform(0.03, 0.06)
            ball = Ball(x, y, radius, color)
            self.balls.append(ball)
            self.trails.append([])
    
    def apply_force_to_balls(self, force_x, force_y):
        """Apply force to all balls"""
        for ball in self.balls:
            ball.vx += force_x
            ball.vy += force_y

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    window = cgl.window(800, 800, "Simple Physics Simulation - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Create physics simulation
    physics = PhysicsSimulation()
    
    # Timing
    prev_time = cgl.utils_get_time()
    
    # Main loop
    while not window.should_close():
        current_time = cgl.utils_get_time()
        dt = min(current_time - prev_time, 0.02)  # Cap delta time
        prev_time = current_time
        
        window.set_size(800, 800)
        framebuffer.bind()
        cgl.gl_clear(0.05, 0.05, 0.05, 1.0)
        
        # Update physics
        physics.update(dt)
        
        # Handle mouse input
        mouse_x, mouse_y = window.get_mouse_position()
        norm_x = (mouse_x / 800.0) * 2.0 - 1.0
        norm_y = 1.0 - (mouse_y / 800.0) * 2.0
        
        if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_LEFT):
            physics.add_ball_at_mouse(norm_x, norm_y)
        
        # Render
        cgl.widgets.begin()
        
        # Draw simulation
        physics.draw()
        
        # Draw UI
        draw_ui(physics, window)
        
        cgl.widgets.end()
        
        # Handle input
        handle_input(physics, window)
        
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

def draw_ui(physics, window):
    """Draw user interface"""
    # Title
    cgl.widgets.set_fill_color(1.0, 1.0, 1.0)
    cgl.widgets.add_string("Simple Physics Simulation", -0.95, 0.9, 1.0, 0.08)
    
    # Status
    status_text = "PAUSED" if physics.paused else "RUNNING"
    cgl.widgets.add_string(f"Status: {status_text}", -0.95, 0.8, 0.5, 0.06)
    
    # Ball count
    count_text = f"Balls: {len(physics.balls)}"
    cgl.widgets.add_string(count_text, -0.95, 0.72, 0.5, 0.06)
    
    # Physics parameters
    gravity_text = f"Gravity: {physics.gravity:.1f}"
    cgl.widgets.add_string(gravity_text, -0.95, 0.64, 0.5, 0.06)
    
    resistance_text = f"Air Resistance: {physics.air_resistance:.3f}"
    cgl.widgets.add_string(resistance_text, -0.95, 0.56, 0.5, 0.06)
    
    # Feature toggles
    features = []
    if physics.show_vectors:
        features.append("Vectors")
    if physics.show_trails:
        features.append("Trails")
    if physics.collision_detection:
        features.append("Collisions")
    
    features_text = "Features: " + ", ".join(features)
    cgl.widgets.add_string(features_text, -0.95, 0.48, 1.0, 0.06)
    
    # Instructions
    instructions = [
        "Click: Add Ball",
        "Space: Pause/Resume",
        "R: Reset",
        "V: Toggle Vectors",
        "T: Toggle Trails",
        "C: Toggle Collisions",
        "G/F: Gravity +/-",
        "WASD: Apply Force",
        "ESC: Exit"
    ]
    
    y_pos = -0.95
    for instruction in instructions:
        cgl.widgets.set_fill_color(0.7, 0.7, 0.7)
        cgl.widgets.add_string(instruction, -0.95, y_pos, 0.8, 0.05)
        y_pos += 0.08

def handle_input(physics, window):
    """Handle user input"""
    # Pause/Resume
    if window.is_key_pressed(cgl.KEY_SPACE):
        physics.paused = not physics.paused
    
    # Reset
    if window.is_key_pressed(cgl.KEY_R):
        physics.create_random_balls(5)
    
    # Toggle features
    if window.is_key_pressed(cgl.KEY_V):
        physics.show_vectors = not physics.show_vectors
    
    if window.is_key_pressed(cgl.KEY_T):
        physics.show_trails = not physics.show_trails
    
    if window.is_key_pressed(cgl.KEY_C):
        physics.collision_detection = not physics.collision_detection
    
    # Gravity control
    if window.is_key_pressed(cgl.KEY_G):
        physics.gravity += 1.0
    
    if window.is_key_pressed(cgl.KEY_F):
        physics.gravity -= 1.0
    
    # Force application
    force_magnitude = 2.0
    if window.is_key_pressed(cgl.KEY_W):
        physics.apply_force_to_balls(0, force_magnitude)
    
    if window.is_key_pressed(cgl.KEY_S):
        physics.apply_force_to_balls(0, -force_magnitude)
    
    if window.is_key_pressed(cgl.KEY_A):
        physics.apply_force_to_balls(-force_magnitude, 0)
    
    if window.is_key_pressed(cgl.KEY_D):
        physics.apply_force_to_balls(force_magnitude, 0)

if __name__ == "__main__":
    main()