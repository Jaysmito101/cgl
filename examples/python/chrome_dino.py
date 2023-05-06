import cgl
import random
cgl.init()
w = cgl.window(512, 512, "Dino--  - Jaysmito Mukherjee")
w.make_context_current()
cgl.gl_init()
cgl.widgets.init()
fb = cgl.framebuffer_create_from_default(w)
FRONT_COLOR, BACK_COLOR = cgl.vec4(0, 0, 0, 1), cgl.vec4(0.9, 0.9, 0.9, 1)
player_height, player_pos, player_vel, enemies, co, game_over = 0.2, 0, 0, [], 0, False
while not w.should_close():
    w.set_size(512, 512)
    fb.bind()
    cgl.gl_clear(0.2, 0.2, 0.2, 1.0)
    cgl.widgets.begin()
    cgl.widgets.set_fill_color(BACK_COLOR)
    cgl.widgets.add_rect2f(-1, -1, 2, 2)
    cgl.widgets.set_fill_color(FRONT_COLOR)
    cgl.widgets.add_rect2f(-1, -0.5, 2, 0.02)
    cgl.widgets.add_rect2f(-0.9, -0.5 + player_pos, 0.05, player_height)
    for enemy in enemies:
        enemy.x = enemy.x - (0.001 if not game_over else 0)
        cgl.widgets.add_rect2f(enemy.x, enemy.y, enemy.z, enemy.w)
        if cgl.aabb_intersects_aabb(cgl.vec2(-0.9, -0.5 + player_pos), cgl.vec2(-0.85, -0.5 + player_pos + player_height), cgl.vec2(enemy.x, enemy.y), cgl.vec2(enemy.x + enemy.z, enemy.y + enemy.w)):
            game_over = True
    cgl.widgets.add_string(str(co // 100), -0.2, 0.5, 0.4, 0.3)
    cgl.widgets.end()
    if player_pos > 0:
        player_pos = player_pos + player_vel * 0.001
        player_pos = 0 if player_pos <= 0 else player_pos
        player_vel -= 0.98 * 0.02
    if w.is_key_pressed(cgl.KEY_SPACE) and player_pos <= 0 and not game_over:
        player_pos = 0.01
        player_vel = 4.2
    if w.is_key_pressed(cgl.KEY_ENTER) and game_over:
        game_over = False
        enemies = []
        co = 0
    player_height = 0.1 if w.is_key_pressed(cgl.KEY_DOWN) and not player_pos > 0 else 0.2
    if not game_over:
        co += 1
    if co % 1700 == 0:
        en_id = random.randint(0, 4)
        if en_id == 0:
            enemies.append(cgl.vec4(1, -0.5, 0.05, 0.15))
            enemies.append(cgl.vec4(1.06, -0.5, 0.05, 0.15))
            enemies.append(cgl.vec4(1.12, -0.5, 0.05, 0.15))
        elif en_id == 1:
            enemies.append(cgl.vec4(1, -0.5, 0.05, 0.15))
            enemies.append(cgl.vec4(1.06, -0.5, 0.05, 0.15))
        elif en_id == 2:
            enemies.append(cgl.vec4(1, -0.5, 0.05, 0.18))
        elif en_id == 3:
            enemies.append(cgl.vec4(1, -0.35, 0.05, 0.05))    
    w.swap_buffers()
    w.poll_events()
fb.destroy()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()

import cgl
import random
cgl.init()
w = cgl.window(512, 512, "Dino--  - Jaysmito Mukherjee")
w.make_context_current()
cgl.gl_init()
cgl.widgets.init()
fb = cgl.framebuffer_create_from_default(w)
FRONT_COLOR, BACK_COLOR = cgl.vec4(0, 0, 0, 1), cgl.vec4(0.9, 0.9, 0.9, 1)
player_height, player_pos, player_vel, enemies, co, game_over = 0.2, 0, 0, [], 0, False
while not w.should_close():
    w.set_size(512, 512)
    fb.bind()
    cgl.gl_clear(0.2, 0.2, 0.2, 1.0)
    cgl.widgets.begin()
    cgl.widgets.set_fill_color(BACK_COLOR)
    cgl.widgets.add_rect2f(-1, -1, 2, 2)
    cgl.widgets.set_fill_color(FRONT_COLOR)
    cgl.widgets.add_rect2f(-1, -0.5, 2, 0.02)
    cgl.widgets.add_rect2f(-0.9, -0.5 + player_pos, 0.05, player_height)
    for enemy in enemies:
        enemy.x = enemy.x - (0.001 if not game_over else 0)
        cgl.widgets.add_rect2f(enemy.x, enemy.y, enemy.z, enemy.w)
        if cgl.aabb_intersects_aabb(cgl.vec2(-0.9, -0.5 + player_pos), cgl.vec2(-0.85, -0.5 + player_pos + player_height), cgl.vec2(enemy.x, enemy.y), cgl.vec2(enemy.x + enemy.z, enemy.y + enemy.w)):
            game_over = True
    cgl.widgets.add_string(str(co // 100), -0.2, 0.5, 0.4, 0.3)
    cgl.widgets.end()
    if player_pos > 0:
        player_pos = player_pos + player_vel * 0.001
        player_pos = 0 if player_pos <= 0 else player_pos
        player_vel -= 0.98 * 0.02
    if w.is_key_pressed(cgl.KEY_SPACE) and player_pos <= 0 and not game_over:
        player_pos = 0.01
        player_vel = 4.2
    if w.is_key_pressed(cgl.KEY_ENTER) and game_over:
        game_over = False
        enemies = []
        co = 0
    player_height = 0.1 if w.is_key_pressed(cgl.KEY_DOWN) and not player_pos > 0 else 0.2
    if not game_over:
        co += 1
    if co % 1700 == 0:
        en_id = random.randint(0, 4)
        if en_id == 0:
            enemies.append(cgl.vec4(1, -0.5, 0.05, 0.15))
            enemies.append(cgl.vec4(1.06, -0.5, 0.05, 0.15))
            enemies.append(cgl.vec4(1.12, -0.5, 0.05, 0.15))
        elif en_id == 1:
            enemies.append(cgl.vec4(1, -0.5, 0.05, 0.15))
            enemies.append(cgl.vec4(1.06, -0.5, 0.05, 0.15))
        elif en_id == 2:
            enemies.append(cgl.vec4(1, -0.5, 0.05, 0.18))
        elif en_id == 3:
            enemies.append(cgl.vec4(1, -0.35, 0.05, 0.05))    
    w.swap_buffers()
    w.poll_events()
fb.destroy()
cgl.widgets.shutdown()
cgl.gl_shutdown()
w.destroy()
cgl.shutdown()
