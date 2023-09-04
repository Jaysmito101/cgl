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

def upload_tilemap(tilemap, grid):
    for i in range(0, 32):
        for j in range(0, 32):
            (r, g, b) = (0, 0, 0)
            if grid[i][j] == 0:
                (r, g, b) = (0.2, 0.2, 0.2)
            elif grid[i][j] == 1:
                (r, g, b) = (0.1, 0.1, 0.8)
            elif grid[i][j] == 2:
                (r, g, b) = (0.8, 0.1, 0.1)
            tilemap.set_tile_color(i, j, r, g, b)
    tilemap.upload()

class Snake:
    def __init__(self, position, direction):
        self.direction = direction
        self.pieces = [position]
    
    def move(self, fruit_pos = (-1, -1)):
        head = self.pieces[0]
        new_head = (head[0] + self.direction[0], head[1] + self.direction[1])
        if new_head in self.pieces:
            return 2
        last_piece = self.pieces[-1]
        for i in range(len(self.pieces) - 1, 0, -1):
            self.pieces[i] = self.pieces[i - 1]
        self.pieces[0] = new_head
        if new_head == fruit_pos:
            self.pieces.append(last_piece)
            return 1
        return 0
    
    def update_grid(self, grid):
        board_dim = len(grid)
        for piece in self.pieces:
            if piece[0] < 0 or piece[0] >= board_dim or piece[1] < 0 or piece[1] >= board_dim:
                return False
            grid[piece[0]][piece[1]] = 1
        return True
        
    def set_direction(self, direction):
        self.direction = direction

cgl.init()
w = cgl.window(512, 512, "Dino--  - Jaysmito Mukherjee")
w.make_context_current()
cgl.gl_init()
cgl.widgets.init()
fb = cgl.framebuffer_create_from_default(w)

tilemap = cgl.tilemap(32, 32, 16, 16, 0)
tilemap.set_auto_upload(False)
snake = Snake((16, 16), (1, 0))

prev_time, curr_time = cgl.utils_get_time(), 0

snake_move_time, snake_move_time_default = 0.1, 0.1

fruit_pos = (random.randint(0, 31), random.randint(0, 31))
game_runnning = True
while not w.should_close():
    curr_time = cgl.utils_get_time()
    delta_time = curr_time - prev_time
    prev_time = curr_time
    snake_move_time -= delta_time
    w.set_size(512, 512)
    fb.bind()
    cgl.gl_clear(0.2, 0.2, 0.2, 1.0)
    grid = [[0 for i in range(32)] for j in range(32)]


    if snake_move_time <= 0 and game_runnning:
        result = snake.move(fruit_pos)
        if result == 1:
            fruit_pos = (random.randint(0, 31), random.randint(0, 31))
            snake_move_time_default -= 0.005
            if snake_move_time_default <= 0.05:
                snake_move_time_default = 0.05
        elif result == 2:
            game_runnning = False
        snake_move_time = snake_move_time_default
    
    game_runnning = game_runnning and snake.update_grid(grid)
    grid[fruit_pos[0]][fruit_pos[1]] = 2
    
    if w.is_key_pressed(cgl.KEY_UP):
        snake.set_direction((0, 1))
    elif w.is_key_pressed(cgl.KEY_DOWN):
        snake.set_direction((0, -1))
    elif w.is_key_pressed(cgl.KEY_LEFT):
        snake.set_direction((-1, 0))
    elif w.is_key_pressed(cgl.KEY_RIGHT):
        snake.set_direction((1, 0))    

    upload_tilemap(tilemap, grid)

    tilemap.render(1.0, 1.0, 0, 0)


    cgl.widgets.begin()

    if not game_runnning:
        cgl.widgets.add_string("Game Over", -0.7, -0.2, 1.4, 0.4)
    # cgl.widgets.add_rect2f(0, 0, 1, 1)

    cgl.widgets.end()
    w.swap_buffers()
    w.poll_events()


fb.destroy()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()
