// Copyright Jaysmito Mukherjee, 2023
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.
// email : jaysmito101@gmail.com


#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define MAX_BRANCHES 100000
#define MAX_GENERATE_TREE_RECURSIVE_CALLS 1000000
#define ADD_INPUT(var_name, mask_key, step_val, min_val, max_val) \
    if(CGL_window_is_key_pressed(g_context.window, mask_key)) { \
        if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_UP)) var_name += step_val; \
        if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_DOWN)) var_name -= step_val; \
        var_name = CGL_utils_clamp(var_name, min_val, max_val); \
    } \

typedef struct
{
    CGL_vec2 start;
    CGL_float angle;
    CGL_float length;
    CGL_float thickness;
    CGL_vec4 color;
} tree_branch;

static struct
{
    CGL_window* window; // the main window
    CGL_int window_height;
    CGL_int window_width;
    CGL_framebuffer* framebuffer;
} g_context;

static struct
{
    tree_branch branches[MAX_BRANCHES];
    CGL_int branch_count;
    CGL_int seed;
    CGL_float angle_right;
    CGL_float angle_left;
    CGL_float thickness_factor;
    CGL_float length_factor;
    CGL_float branch_chance;
    CGL_float min_branch_length;
    CGL_float min_branch_thickness;
} g_tree;

static void draw_tree()
{
    CGL_int branch_count = g_tree.branch_count;
    for(CGL_int i = 0; i < branch_count; i++)
    {
        tree_branch* branch = &g_tree.branches[i];
        CGL_vec2 end = CGL_vec2_add_(branch->start, CGL_vec2_scale_(CGL_vec2_from_angle(branch->angle + CGL_PI_2), branch->length));
        CGL_widgets_set_stroke_color(branch->color);
        CGL_widgets_set_stroke_thicnkess(branch->thickness);
        CGL_widgets_add_line2f(branch->start.x, branch->start.y, end.x, end.y);
    }
}

static tree_branch create_branch(CGL_vec2 start, CGL_float angle, CGL_float length, CGL_float thickness)
{
    tree_branch branch;
    branch.start = start;
    branch.angle = angle;
    branch.length = length;
    branch.thickness = thickness;
    branch.color =CGL_vec4_scale_(CGL_vec4_init(0.179f, 0.097f, 0.066f, 1.0f), 0.5f);
    return branch;
}

static void add_branch(tree_branch branch)
{
    if(g_tree.branch_count >= MAX_BRANCHES)
    {
        CGL_warn("Max branches reached!");
        return;
    }
    g_tree.branches[g_tree.branch_count++] = branch;
}

static int generate_tree_recursive_calls = 0;

static CGL_float generate_random_float(CGL_float center, CGL_float percent)
{
    CGL_float range = center * percent;
    CGL_float min = center - range;
    CGL_float max = center + range;
    return CGL_utils_random_float() * (max - min) + min;
}

static void generate_tree_recursive(CGL_int prev_branch)
{
    generate_tree_recursive_calls++;
    if(generate_tree_recursive_calls > MAX_GENERATE_TREE_RECURSIVE_CALLS)
    {
        CGL_warn("Max generate tree recursive calls reached!");
        return;
    }

    tree_branch* p_branch = &g_tree.branches[prev_branch];

    CGL_float c_length_factor[2] = {g_tree.length_factor, g_tree.length_factor};
    CGL_float c_branch_chance[2] = {g_tree.branch_chance, g_tree.branch_chance};
    CGL_float c_angle_nudge[2] = {g_tree.angle_left, g_tree.angle_right};
    CGL_float c_thickness_factor = g_tree.thickness_factor;
    
    CGL_float l_length = p_branch->length * generate_random_float(c_length_factor[0], 0.1f);
    CGL_float l_thickness = p_branch->thickness * generate_random_float(c_thickness_factor, 0.1f);
    CGL_float l_angle_nudge = generate_random_float(c_angle_nudge[0], 0.1f) + p_branch->angle;
    CGL_bool l_branch = CGL_utils_random_float() < c_branch_chance[0];
    if(l_branch)
    {
        CGL_vec2 start = CGL_vec2_add_(p_branch->start, CGL_vec2_scale_(CGL_vec2_from_angle(p_branch->angle + CGL_PI_2), p_branch->length*0.98f));
        tree_branch branch = create_branch(start, l_angle_nudge, l_length, l_thickness);
        add_branch(branch);
        if(l_length > g_tree.min_branch_length && l_thickness > g_tree.min_branch_thickness) generate_tree_recursive(g_tree.branch_count - 1);
    }
    
    CGL_float r_length = p_branch->length * generate_random_float(c_length_factor[1], 0.1f);
    CGL_float r_thickness = p_branch->thickness * generate_random_float(c_thickness_factor, 0.1f);
    CGL_float r_angle_nudge = generate_random_float(-c_angle_nudge[1], 0.1f) + p_branch->angle;
    CGL_bool r_branch = CGL_utils_random_float() < c_branch_chance[1];
    if(r_branch)
    {
        CGL_vec2 start = CGL_vec2_add_(p_branch->start, CGL_vec2_scale_(CGL_vec2_from_angle(p_branch->angle + CGL_PI_2), p_branch->length*0.98f));
        tree_branch branch = create_branch(start, r_angle_nudge, r_length, r_thickness);
        add_branch(branch);
        if(r_length > g_tree.min_branch_length && r_thickness > g_tree.min_branch_thickness) generate_tree_recursive(g_tree.branch_count - 1);
    }

    
}

static void generate_tree()
{
    generate_tree_recursive_calls = 0;
    g_tree.branch_count = 0;
    g_tree.branches[g_tree.branch_count++] = create_branch(CGL_vec2_init(0.0f, -1.0f), 0.0f, 0.5f, 0.1f);
    srand((CGL_uint)g_tree.seed);
    generate_tree_recursive(0);
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window_height = g_context.window_width = 700;
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Procedural Trees - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(!CGL_widgets_init()) return EXIT_FAILURE; 

    CGL_float prev_time = CGL_utils_get_time(), curr_time = 0.0f, delta_time = 0.0f, frame_time = 0.0f;
    CGL_int frame_count = 0, frame_rate = 60;

    g_tree.angle_left = CGL_deg_to_rad(30.0f);
    g_tree.angle_right = CGL_deg_to_rad(30.0f);
    g_tree.thickness_factor = 0.7f;
    g_tree.length_factor = 0.7f;
    g_tree.branch_chance = 1.0f;
    g_tree.min_branch_thickness = 0.0001f;
    g_tree.min_branch_length = 0.01f;

    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time; prev_time = curr_time; frame_time += delta_time; frame_count++;
        if(frame_time >= 1.0f) { frame_rate = frame_count; frame_time = 0.0f; frame_count = 0; }

        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set the window size
        // rendering
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color

        generate_tree();

        CGL_widgets_begin();
        draw_tree();
        CGL_widgets_set_fill_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        static CGL_byte text_buffer[256];
        sprintf(text_buffer, "Delta Time: %f", delta_time);
        CGL_widgets_add_string(text_buffer, -1.0f, 0.95f, 0.8f, 0.05f);
        sprintf(text_buffer, "Frame Rate: %d", frame_rate);
        CGL_widgets_add_string(text_buffer, 0.0f, 0.95f, 0.8f, 0.05f);
        sprintf(text_buffer, "Branch Count: %d", g_tree.branch_count);
        CGL_widgets_add_string(text_buffer, -1.0f, 0.90f, 0.8f, 0.05f);
        sprintf(text_buffer, "Angles: (%d, %d)", (CGL_int)CGL_rad_to_deg(g_tree.angle_left), (CGL_int)CGL_rad_to_deg(g_tree.angle_right));
        CGL_widgets_add_string(text_buffer, 0.0f, 0.90f, 0.8f, 0.05f);
        sprintf(text_buffer, "Length Factor: %f", g_tree.length_factor);
        CGL_widgets_add_string(text_buffer, -1.0f, 0.85f, 0.8f, 0.05f);
        sprintf(text_buffer, "Thickness Factor: %f", g_tree.thickness_factor);
        CGL_widgets_add_string(text_buffer, 0.0f, 0.85f, 0.8f, 0.05f);
        sprintf(text_buffer, "Branch Chance: %f", g_tree.branch_chance);
        CGL_widgets_add_string(text_buffer, -1.0f, 0.80f, 0.8f, 0.05f);
        sprintf(text_buffer, "Recursive Calls: %d", generate_tree_recursive_calls);
        CGL_widgets_add_string(text_buffer, 0.0f, 0.80f, 0.8f, 0.05f);
        CGL_widgets_end();
        

        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)

        ADD_INPUT(g_tree.angle_left, CGL_KEY_A, 0.005f, 0.0f, CGL_PI);
        ADD_INPUT(g_tree.angle_right, CGL_KEY_D, 0.005f, 0.0f, CGL_PI);
        ADD_INPUT(g_tree.length_factor, CGL_KEY_W, 0.001f, 0.0f, 0.8f);
        ADD_INPUT(g_tree.thickness_factor, CGL_KEY_S, 0.001f, 0.0f, 1.0f);
        ADD_INPUT(g_tree.branch_chance, CGL_KEY_Q, 0.001f, 0.0f, 1.0f);

        if(CGL_window_get_key(g_context.window, CGL_KEY_R) == CGL_PRESS) g_tree.seed = (CGL_int)(CGL_utils_get_time() * 10000.0f);

        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
    }

    // cleanup
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}
