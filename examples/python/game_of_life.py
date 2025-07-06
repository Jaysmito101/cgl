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

class GameOfLife:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.grid = [[False for _ in range(width)] for _ in range(height)]
        self.next_grid = [[False for _ in range(width)] for _ in range(height)]
        self.generation = 0
        self.running = False
        
    def randomize(self, density=0.3):
        """Randomize the grid with given density"""
        for y in range(self.height):
            for x in range(self.width):
                self.grid[y][x] = random.random() < density
        self.generation = 0
        
    def clear(self):
        """Clear the grid"""
        for y in range(self.height):
            for x in range(self.width):
                self.grid[y][x] = False
        self.generation = 0
        
    def set_cell(self, x, y, state):
        """Set a cell state"""
        if 0 <= x < self.width and 0 <= y < self.height:
            self.grid[y][x] = state
            
    def get_cell(self, x, y):
        """Get a cell state"""
        if 0 <= x < self.width and 0 <= y < self.height:
            return self.grid[y][x]
        return False
        
    def count_neighbors(self, x, y):
        """Count live neighbors around a cell"""
        count = 0
        for dy in [-1, 0, 1]:
            for dx in [-1, 0, 1]:
                if dx == 0 and dy == 0:
                    continue
                nx, ny = x + dx, y + dy
                if 0 <= nx < self.width and 0 <= ny < self.height:
                    if self.grid[ny][nx]:
                        count += 1
        return count
        
    def update(self):
        """Update the game state by one generation"""
        # Calculate next generation
        for y in range(self.height):
            for x in range(self.width):
                neighbors = self.count_neighbors(x, y)
                current_state = self.grid[y][x]
                
                # Conway's Game of Life rules
                if current_state:
                    # Live cell survives with 2 or 3 neighbors
                    self.next_grid[y][x] = neighbors == 2 or neighbors == 3
                else:
                    # Dead cell becomes alive with exactly 3 neighbors
                    self.next_grid[y][x] = neighbors == 3
        
        # Swap grids
        self.grid, self.next_grid = self.next_grid, self.grid
        self.generation += 1
        
    def add_glider(self, x, y):
        """Add a glider pattern at position (x, y)"""
        pattern = [
            [False, True, False],
            [False, False, True],
            [True, True, True]
        ]
        for dy in range(3):
            for dx in range(3):
                self.set_cell(x + dx, y + dy, pattern[dy][dx])
                
    def add_oscillator(self, x, y):
        """Add a blinker oscillator at position (x, y)"""
        self.set_cell(x, y, True)
        self.set_cell(x + 1, y, True)
        self.set_cell(x + 2, y, True)

def main():
    # Initialize CGL
    cgl.init()
    
    # Create window
    window = cgl.window(800, 600, "Game of Life - CGL Python Example")
    window.make_context_current()
    
    # Initialize OpenGL and widgets
    cgl.gl_init()
    cgl.widgets.init()
    
    # Create framebuffer
    framebuffer = cgl.framebuffer_create_from_default(window)
    
    # Create Game of Life instance
    GRID_WIDTH = 80
    GRID_HEIGHT = 60
    game = GameOfLife(GRID_WIDTH, GRID_HEIGHT)
    
    # Initial random pattern
    game.randomize(0.3)
    
    # Timing variables
    update_interval = 0.1  # 10 FPS for game updates
    last_update = cgl.utils_get_time()
    
    # UI state
    mouse_drawing = False
    mouse_erasing = False
    
    # Main loop
    while not window.should_close():
        current_time = cgl.utils_get_time()
        
        window.set_size(800, 600)
        framebuffer.bind()
        cgl.gl_clear(0.1, 0.1, 0.1, 1.0)
        
        # Update game logic
        if game.running and (current_time - last_update) >= update_interval:
            game.update()
            last_update = current_time
        
        # Handle mouse input for drawing
        mouse_x, mouse_y = window.get_mouse_position()
        
        # Convert mouse position to grid coordinates
        # Assuming the grid is rendered in normalized coordinates from -1 to 1
        grid_x = int((mouse_x / 800.0) * GRID_WIDTH)
        grid_y = int(((600 - mouse_y) / 600.0) * GRID_HEIGHT)
        
        if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_LEFT):
            if not mouse_drawing:
                mouse_drawing = True
                game.set_cell(grid_x, grid_y, True)
        else:
            mouse_drawing = False
            
        if window.is_mouse_button_pressed(cgl.MOUSE_BUTTON_RIGHT):
            if not mouse_erasing:
                mouse_erasing = True
                game.set_cell(grid_x, grid_y, False)
        else:
            mouse_erasing = False
        
        # Continue drawing while mouse is held
        if mouse_drawing:
            game.set_cell(grid_x, grid_y, True)
        elif mouse_erasing:
            game.set_cell(grid_x, grid_y, False)
        
        # Render
        cgl.widgets.begin()
        
        # Draw grid
        cell_width = 2.0 / GRID_WIDTH
        cell_height = 2.0 / GRID_HEIGHT
        
        for y in range(GRID_HEIGHT):
            for x in range(GRID_WIDTH):
                cell_x = -1.0 + x * cell_width
                cell_y = -1.0 + y * cell_height
                
                if game.get_cell(x, y):
                    # Live cell - bright green
                    cgl.widgets.set_fill_color(0.0, 1.0, 0.0)
                else:
                    # Dead cell - dark gray
                    cgl.widgets.set_fill_color(0.05, 0.05, 0.05)
                
                cgl.widgets.add_rect2f(cell_x, cell_y, cell_width * 0.9, cell_height * 0.9)
        
        # Draw UI
        ui_y = 0.85
        cgl.widgets.set_fill_color(0.8, 0.8, 0.8)
        
        # Generation counter
        gen_text = f"Generation: {game.generation}"
        cgl.widgets.add_string(gen_text, -0.95, ui_y, 0.4, 0.1)
        
        # Status
        status_text = "Running" if game.running else "Paused"
        cgl.widgets.add_string(status_text, -0.5, ui_y, 0.3, 0.1)
        
        # Instructions
        instructions = [
            "SPACE: Play/Pause",
            "R: Randomize",
            "C: Clear",
            "G: Add Glider",
            "O: Add Oscillator",
            "Mouse: Draw/Erase"
        ]
        
        for i, instruction in enumerate(instructions):
            cgl.widgets.add_string(instruction, -0.95, ui_y - 0.15 - i * 0.08, 0.6, 0.06)
        
        cgl.widgets.end()
        
        # Handle keyboard input
        if window.is_key_pressed(cgl.KEY_SPACE):
            game.running = not game.running
            
        if window.is_key_pressed(cgl.KEY_R):
            game.randomize(0.3)
            game.running = False
            
        if window.is_key_pressed(cgl.KEY_C):
            game.clear()
            game.running = False
            
        if window.is_key_pressed(cgl.KEY_G):
            # Add glider at random position
            x = random.randint(0, GRID_WIDTH - 5)
            y = random.randint(0, GRID_HEIGHT - 5)
            game.add_glider(x, y)
            
        if window.is_key_pressed(cgl.KEY_O):
            # Add oscillator at random position
            x = random.randint(0, GRID_WIDTH - 5)
            y = random.randint(0, GRID_HEIGHT - 5)
            game.add_oscillator(x, y)
            
        # Speed controls
        if window.is_key_pressed(cgl.KEY_EQUAL):  # Plus key
            update_interval = max(0.01, update_interval - 0.01)
            
        if window.is_key_pressed(cgl.KEY_MINUS):
            update_interval = min(1.0, update_interval + 0.01)
            
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