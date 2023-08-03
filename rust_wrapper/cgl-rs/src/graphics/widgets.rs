//! The CGL Widgets Module. This is powered by th epowerful cgl widgets batch renderer.

#![allow(non_camel_case_types)]


use std::ffi::{c_void, c_float, c_int, c_char};


use crate::math::{Vector2, Vector3, Vector4, Matrix4x4};
use crate::graphics::{Texture, CGL_texture, MeshCPU_C, text::CGL_font, text::Font};

/*

/// This functionality is not yet available from Rust side.
/// You can still create the defualt context using CGL_init and use all widgets functionality.

#[repr(C)]
pub(crate) struct CGL_widgets_context {
    _private: c_void
}

/// The CGL Widgets Context.
#[repr(C)]
pub struct WidgetsContext {
    handle: *mut CGL_widgets_context,
    has_been_destroyed: bool
}

*/

extern {
    /*
    fn CGL_widgets_context_create(max_vertices: size_t, max_indices: size_t) -> *mut CGL_widgets_context;
    fn CGL_widgets_context_destroy(context: *mut CGL_widgets_context) -> c_void;
    fn CGL_window_get_current_context() -> *mut CGL_widgets_context; // This is a typo from the original CGL code. It should be CGL_widgets_context_get_current_context.
    fn CGL_window_set_current_context(context: *mut WidgetsContext) -> c_void; // This is a typo from the original CGL code. It should be CGL_widgets_context_set_current_context.
    */

    fn CGL_widgets_init() -> c_int;
    fn CGL_widgets_shutdown() -> c_void;
    fn CGL_widgets_begin() -> c_int;
    fn CGL_widgets_begin_int(scale_x: c_float, scale_y: c_float, offset_x: c_float, offset_y: c_float) -> c_int;
    fn CGL_widgets_end() -> c_int;
    fn CGL_widgets_flush() -> c_int;
    fn CGL_widgets_flush_if_required() -> c_int;
    fn CGL_widgets_add_mesh(mesh: *mut MeshCPU_C) -> c_void;

    fn CGL_widgets_add_vertex_p(position: Vector3) -> c_void;
    fn CGL_widgets_add_vertex_p3f(pos_x: c_float, pos_y: c_float, pos_z: c_float) -> c_void;
    fn CGL_widgets_add_vertex_pt(position: Vector3, tex_coord: Vector2) -> c_void;
    fn CGL_widgets_add_vertex_p3ft(pos_x: c_float, pos_y: c_float, pos_z: c_float, tex_coord: Vector2) -> c_void;
    fn CGL_widgets_add_vertex_pt2f(position: Vector3, tex_x: c_float, tex_y: c_float) -> c_void;
    fn CGL_widgets_add_vertex_p3ft2f(pos_x: c_float, pos_y: c_float, pos_z: c_float, tex_x: c_float, tex_y: c_float) -> c_void;
    
    fn CGL_widgets_set_stroke_color(color: Vector4) -> c_void;
    fn CGL_widgets_set_stroke_colorf(r: c_float, g: c_float, b: c_float, a: c_float) -> c_void;
    fn CGL_widgets_set_mask(mask: Vector4) -> c_void;
    fn CGL_widgets_set_maskf(min_x: c_float, min_y: c_float, max_x: c_float, max_y: c_float) -> c_void;
    fn CGL_widgets_set_stroke_thicnkess(thickness: c_float) -> c_void;
    fn CGL_widgets_set_fill_color(color: Vector4) -> c_void;
    fn CGL_widgets_set_fill_colorf(r: c_float, g: c_float, b: c_float, a: c_float) -> c_void;
    fn CGL_widgets_set_fill_mode(is_enabled: c_int) -> c_void;
    
    fn CGL_widgets_enable_diffuse_shading(light_position: Vector3, light_color: Vector3) -> c_void;
    fn CGL_widgets_disable_diffuse_shading() -> c_void;
    
    fn CGL_widgets_set_projection_matrix(matrix: *mut Matrix4x4) -> c_void;
    fn CGL_widgets_set_view_matrix(matrix: *mut Matrix4x4) -> c_void;
    fn CGL_widgets_set_model_matrix(matrix: *mut Matrix4x4) -> c_void;

    fn CGL_widgets_set_texture(texture: *mut CGL_texture) -> c_void;
    fn CGL_widgets_set_font_texture(texture: *mut CGL_texture) -> c_void;
    fn CGL_widgets_set_texture_coordinate_so(scale_x: c_float, scale_y: c_float, offset_x: c_float, offset_y: c_float) -> c_void;
    
    fn CGL_widgets_apply_transformations_on_cpu() -> c_void;
    fn CGL_widgets_apply_transformations_on_gpu() -> c_void;

    fn CGL_widgets_add_triangle(a: Vector3, b: Vector3, c: Vector3) -> c_void;
    fn CGL_widgets_add_quad(a: Vector3, b: Vector3, c: Vector3, d: Vector3) -> c_void;
    fn CGL_widgets_add_quad_8f(ax: c_float, ay: c_float, bx: c_float, by: c_float, cx: c_float, cy: c_float, dx: c_float, dy: c_float) -> c_void;
    fn CGL_widgets_add_line(start: Vector3, end: Vector3) -> c_void;
    fn CGL_widgets_add_line2f(start_x: c_float, start_y: c_float, end_x: c_float, end_y: c_float) -> c_void;
    fn CGL_widgets_add_rect(start: Vector3, size: Vector2) -> c_void;
    fn CGL_widgets_add_rect2f(start_x: c_float, start_y: c_float, size_x: c_float, size_y: c_float) -> c_void;
    fn CGL_widgets_add_rounded_rect2f(start_x: c_float, start_y: c_float, size_x: c_float, size_y: c_float, radius: c_float, res: c_int) -> c_void;
    fn CGL_widgets_add_circle(position: Vector3, radius: c_float) -> c_void;
    fn CGL_widgets_add_circle2f(pos_x: c_float, pos_y: c_float, radius: c_float) -> c_void;
    fn CGL_widgets_add_circler(position: Vector3, radius: c_float, res: c_int) -> c_void;
    fn CGL_widgets_add_circle2fr(pos_x: c_float, pos_y: c_float, radius: c_float, res: c_int) -> c_void;
    fn CGL_widgets_add_oval(position: Vector3, radius: Vector2) -> c_void;
    fn CGL_widgets_add_oval2f(pos_x: c_float, pos_y: c_float, radius_x: c_float, radius_y: c_float) -> c_void;
    fn CGL_widgets_add_oval2fr(pos_x: c_float, pos_y: c_float, radiusx: c_float, radiusy: c_float, resolution: c_int) -> c_void;
    fn CGL_widgets_add_arc2f(pos_x: c_float, pos_y: c_float, radius: c_float, start_angle: c_float, end_angle: c_float, resolution: c_int) -> c_void;
    fn CGL_widgets_add_character(c: c_char, x: c_float, y: c_float, sx: c_float, sy: c_float) -> c_int;
    fn CGL_widgets_add_string(str: *const c_char, x: c_float, y: c_float, sx: c_float, sy: c_float) -> c_int;
    fn CGL_widgets_add_string_with_font(str: *const c_char, font: *mut CGL_font, x: c_float, y: c_float, sx: c_float, scale_y: c_float) -> c_float;
    fn CGL_widgets_add_cubic_bazier(start: Vector3, end: Vector3, control_1: Vector3, control_2: Vector3, resolution: c_int) -> c_void;
    fn CGL_widgets_add_cubic_bazier2v(start: Vector2, end: Vector2, control_1: Vector2, control_2: Vector2, resolution: c_int) -> c_void;
    fn CGL_widgets_add_cubic_bazier2f(start_x: c_float, start_y: c_float, end_x: c_float, end_y: c_float, control_1_x: c_float, control_1_y: c_float, control_2_x: c_float, control_2_y: c_float, resolution: c_int) -> c_void;
    fn CGL_widgets_add_cubic_bazier_points(start: Vector3, end: Vector3, control_1: Vector3, control_2: Vector3, resolution: c_int) -> c_void;
    
    fn CGL_widgets_add_cubic_bazier_points2v(start: Vector2, end: Vector2, control_1: Vector2, control_2: Vector2, resolution: c_int) -> c_void;
    fn CGL_widgets_add_cubic_bazier_points2f(start_x: c_float, start_y: c_float, end_x: c_float, end_y: c_float, control_1_x: c_float, control_1_y: c_float, control_2_x: c_float, control_2_y: c_float, resolution: c_int) -> c_void;
    

    // For future ...
    // fn CGL_widgets_add_plot_array(start_x: c_float, start_y: c_float, size_x: c_float, size_y: c_float, values: *mut Vector2, count: c_int, marker_size: c_float, marker_color: Vector3, draw_axes: c_int, axes_thickness: c_float, axes_color: Vector3) -> c_void;
    // fn CGL_widgets_add_plot_pie_chart(start_x: c_float, start_y: c_float, radius: c_float, values: *mut c_float, colors: *mut Vector3, count: c_int, resolution: c_int) -> c_void;
    // fn CGL_widgets_add_bar_graph(start_x: c_float, start_y: c_float, size_x: c_float, size_y: c_float, values: *mut c_float, colors: *mut Vector3, count: c_int, draw_axes: c_int, axes_thickness: c_float, axes_color: Vector3, vertical: c_int) -> c_void;
    // fn CGL_widgets_add_plot_function(start_x: c_float, start_y: c_float, size_x: c_float, size_y: c_float, func_to_plot: fn(CGL_float) -> CGL_float, num_samples: c_int, x_min: c_float, x_max: c_float, y_min: c_float, y_max: c_float, plot_thickness: c_float, plot_color: Vector3, draw_axes: c_int, axes_thickness: c_float, axes_color: Vector3) -> c_void;
    // fn CGL_widgets_add_shape_out_line(shape: *mut CGL_shape) -> c_void; // TODO: Implement shape 

}    

/// Initializes the CGL widgets
/// 
/// # Returns
/// 
/// Returns `Ok(())` if the initialization was successful, otherwise returns `Err("Failed to initialize CGL widgets")`
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// // do stuff
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn init() -> Result<(), &'static str> {
    unsafe {
        if CGL_widgets_init() == 0 {
            return Err("Failed to initialize CGL widgets");
        }
    }
    Ok(())
}


/// Shuts down the CGL widgets
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// // do stuff
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn shutdown() {
    unsafe {
        CGL_widgets_shutdown();
    }
}



/// Begins a new CGL frame
/// 
/// # Returns
/// 
/// Returns `true` if the frame was successfully created, otherwise returns `false`
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// 
/// while !window.should_close() {
///     // bind framebuffers, etc.
///     cgl_rs::graphics::widgets::begin(); // you could also check if the function returns true
///         // draw widgets here
///     cgl_rs::graphics::widgets::end();
///     // update window, swap buffers, etc.
///     break; // for testing purposes
/// }
/// 
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn begin() -> bool {
    unsafe {
        CGL_widgets_begin() == 1
    }
}


/// Begins a new CGL frame with a global scale and offset.
/// 
/// # Arguments
/// 
/// * `scale_x` - The horizontal scaling factor
/// * `scale_y` - The vertical scaling factor
/// * `offset_x` - The horizontal offset
/// * `offset_y` - The vertical offset
/// 
/// # Returns
/// 
/// Returns `true` if the frame was successfully created, otherwise returns `false`
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// 
/// while !window.should_close() {
///     // bind framebuffers, etc.
///     cgl_rs::graphics::widgets::begin_int(2.0, 2.0, 0.0, 0.0); // you could also check if the function returns true
///         // draw widgets here
///     cgl_rs::graphics::widgets::end();
///     // update window, swap buffers, etc.
///     break; // for testing purposes
/// }
/// 
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn begin_int(scale_x: f32, scale_y: f32, offset_x: f32, offset_y: f32) -> bool {
    unsafe {
        CGL_widgets_begin_int(scale_x, scale_y, offset_x, offset_y) == 1
    }    
}

/// Ends the current CGL frame
/// 
/// # Returns
/// 
/// Returns `true` if the frame was successfully ended, otherwise returns `false`
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// 
/// while !window.should_close() {
///     // bind framebuffers, etc.
///     cgl_rs::graphics::widgets::begin();
///         // draw widgets here
///     cgl_rs::graphics::widgets::end(); // you could also check if the function returns true
///     // update window, swap buffers, etc.
///     break; // for testing purposes
/// }
/// 
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn end() -> bool {
    unsafe {
        CGL_widgets_end() == 1
    }
}


/// Flushes all the vertices added to the current batch.
/// 
/// # Returns
/// 
/// Returns `true` if the vertices were successfully flushed, otherwise returns `false`
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// 
/// while !window.should_close() {
///     // bind framebuffers, etc.
///     cgl_rs::graphics::widgets::begin();
///         // draw widgets here
///     cgl_rs::graphics::widgets::flush(); // you could also check if the function returns true
///     cgl_rs::graphics::widgets::end();
///     // update window, swap buffers, etc.
///     break; // for testing purposes
/// }
/// 
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn flush() -> bool {
    unsafe {
        CGL_widgets_flush() == 1
    }
}



/// Flushes all the vertices added to the current batch if required.
/// 
/// # Returns
/// 
/// Returns `true` if the vertices were successfully flushed, otherwise returns `false`
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::widgets::init();
/// 
/// while !window.should_close() {
///     // bind framebuffers, etc.
///     cgl_rs::graphics::widgets::begin();
///         // draw widgets here
///     cgl_rs::graphics::widgets::flush_if_required(); // you could also check if the function returns true
///     cgl_rs::graphics::widgets::end();
///     // update window, swap buffers, etc.
///     break; // for testing purposes
/// }
/// 
/// cgl_rs::graphics::widgets::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn flush_if_required() -> bool {
    unsafe {
        CGL_widgets_flush_if_required() == 1
    }
}

/// Adds a vertex with the given position to the current batch.
/// 
/// # Arguments
/// 
/// * `position` - A `Vector3` representing the position of the vertex.
pub fn add_vertex_p(position: Vector3) {
    unsafe {
        CGL_widgets_add_vertex_p(position);
    }
}

/// Adds a vertex with the given position to the current batch.
/// 
/// # Arguments
/// 
/// * `x` - A `f32` representing the x-coordinate of the position of the vertex.
/// * `y` - A `f32` representing the y-coordinate of the position of the vertex.
/// * `z` - A `f32` representing the z-coordinate of the position of the vertex.
pub fn add_vertex_p3f(x: f32, y: f32, z: f32) {
    unsafe {
        CGL_widgets_add_vertex_p3f(x, y, z);
    }
}

/// Adds a vertex with the given position and texture coordinate to the current batch.
/// 
/// # Arguments
/// 
/// * `position` - A `Vector3` representing the position of the vertex.
/// * `tex_coord` - A `Vector2` representing the texture coordinate of the vertex.
pub fn add_vertex_pt(position: Vector3, tex_coord: Vector2) {
    unsafe {
        CGL_widgets_add_vertex_pt(position, tex_coord);
    }
}

/// Adds a vertex with the given position and texture coordinate to the current batch.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the position of the vertex.
/// * `pos_y` - A `f32` representing the y-coordinate of the position of the vertex.
/// * `pos_z` - A `f32` representing the z-coordinate of the position of the vertex.
/// * `tex_coord` - A `Vector2` representing the texture coordinate of the vertex.
pub fn  add_vertex_p3ft(pos_x: f32, pos_y: f32, pos_z: f32, tex_coord: Vector2) {
    unsafe {
        CGL_widgets_add_vertex_p3ft(pos_x, pos_y, pos_z, tex_coord);
    }
}

/// Adds a vertex with the given position and texture coordinate to the current batch.
/// 
/// # Arguments
/// 
/// * `position` - A `Vector3` representing the position of the vertex.
/// * `tex_coord_x` - A `f32` representing the x-coordinate of the texture coordinate of the vertex.
/// * `tex_coord_y` - A `f32` representing the y-coordinate of the texture coordinate of the vertex.
pub fn add_vertex_pt2f(position: Vector3, tex_coord_x: f32, tex_coord_y: f32) {
    unsafe {
        CGL_widgets_add_vertex_pt2f(position, tex_coord_x, tex_coord_y);
    }
}

/// Adds a vertex with the given position and texture coordinate to the current batch.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the position of the vertex.
/// * `pos_y` - A `f32` representing the y-coordinate of the position of the vertex.
/// * `pos_z` - A `f32` representing the z-coordinate of the position of the vertex.
/// * `tex_coord_x` - A `f32` representing the x-coordinate of the texture coordinate of the vertex.
/// * `tex_coord_y` - A `f32` representing the y-coordinate of the texture coordinate of the vertex.
pub fn add_vertex_p3ft2f(pos_x: f32, pos_y: f32, pos_z: f32, tex_coord_x: f32, tex_coord_y: f32) {
    unsafe {
        CGL_widgets_add_vertex_p3ft2f(pos_x, pos_y, pos_z, tex_coord_x, tex_coord_y);
    }
}


/// Sets the stroke color for the current frame.
/// 
/// # Arguments
/// 
/// * `color` - A `Vector4` representing the color of the stroke.
pub fn set_stroke_color(color: Vector4) {
    unsafe {
        CGL_widgets_set_stroke_color(color);
    }
}

/// Sets the stroke color for the current frame.
/// 
/// # Arguments
/// 
/// * `r` - A `f32` representing the red component of the stroke color.
/// * `g` - A `f32` representing the green component of the stroke color.
/// * `b` - A `f32` representing the blue component of the stroke color.
/// * `a` - A `f32` representing the alpha component of the stroke color.
pub fn set_stroke_colorf(r: f32, g: f32, b: f32, a: f32) {
    unsafe {
        CGL_widgets_set_stroke_colorf(r, g, b, a);
    }
}

/// Sets the fill color for the current frame.
/// 
/// # Arguments
/// 
/// * `color` - A `Vector4` representing the color of the fill.
pub fn set_fill_color(color: Vector4) {
    unsafe {
        CGL_widgets_set_fill_color(color);
    }
}

/// Sets the fill color for the current frame.
/// 
/// # Arguments
/// 
/// * `r` - A `f32` representing the red component of the fill color.
/// * `g` - A `f32` representing the green component of the fill color.
/// * `b` - A `f32` representing the blue component of the fill color.
/// * `a` - A `f32` representing the alpha component of the fill color.
pub fn set_fill_colorf(r: f32, g: f32, b: f32, a: f32) {
    unsafe {
        CGL_widgets_set_fill_colorf(r, g, b, a);
    }
}

/// Sets the mask rendering mask for the current frame.
/// 
/// 
/// # Arguments
/// 
/// * `mask` - A `Vector4` representing the color of the mask. 
///            The mask is in the format (start_x, start_y, end_x, end_y)
pub fn set_mask(mask: Vector4) {
    unsafe {
        CGL_widgets_set_mask(mask);
    }
}

/// Sets the mask rendering mask for the current frame.
/// 
/// # Arguments
/// 
/// * `r` - A `f32` representing the red component of the mask color.
/// * `g` - A `f32` representing the green component of the mask color.
/// * `b` - A `f32` representing the blue component of the mask color.
/// * `a` - A `f32` representing the alpha component of the mask color.
pub fn set_maskf(r: f32, g: f32, b: f32, a: f32) {
    unsafe {
        CGL_widgets_set_maskf(r, g, b, a);
    }
}

/// Sets the stroke thickness for the current frame.
/// 
/// # Arguments
/// 
/// * `thickness` - A `f32` representing the thickness of the stroke.
pub fn set_stroke_thickness(thickness: f32) {
    unsafe {
        CGL_widgets_set_stroke_thicnkess(thickness);
    }
}

/// Sets the fill mode for the current frame.
/// 
/// # Arguments
/// 
/// * `mode` - A `bool` representing the fill mode. `true` for fill, `false` for wireframe.
pub fn set_fill_mode(mode: bool) {
    unsafe {
        CGL_widgets_set_fill_mode(mode as i32);
    }
}

/// Enables diffuse shading for the current frame.
/// 
/// # Arguments
/// 
/// * `light_pos` - A `Vector3` representing the position of the light source.
/// * `light_color` - A `Vector3` representing the color of the light source.
pub fn enable_diffuse_shading(light_pos: Vector3, light_color: Vector3) {
    unsafe {
        CGL_widgets_enable_diffuse_shading(light_pos, light_color);
    }
}

/// Disables diffuse shading for the current frame.
pub fn disable_diffuse_shading() {
    unsafe {
        CGL_widgets_disable_diffuse_shading();
    }
}

/// Sets the projection matrix for the current frame.
/// 
/// # Arguments
/// 
/// * `matrix` - A reference to a `Matrix4x4` representing the projection matrix.
pub fn set_projection_matrix(matrix: &Matrix4x4) {
    unsafe {
        CGL_widgets_set_projection_matrix(matrix as *const Matrix4x4 as *mut Matrix4x4);
    }
}

/// Sets the view matrix for the current frame.
/// 
/// # Arguments
/// 
/// * `matrix` - A reference to a `Matrix4x4` representing the view matrix.
pub fn set_view_matrix(matrix: &Matrix4x4) {
    unsafe {
        CGL_widgets_set_view_matrix(matrix as *const Matrix4x4 as *mut Matrix4x4);
    }
}

/// Sets the model matrix for the current frame.
/// 
/// # Arguments
/// 
/// * `matrix` - A reference to a `Matrix4x4` representing the model matrix.
pub fn set_model_matrix(matrix: &Matrix4x4) {
    unsafe {
        CGL_widgets_set_model_matrix(matrix as *const Matrix4x4 as *mut Matrix4x4);
    }
}

/// Sets the texture for the current frame.
/// 
/// # Arguments
/// 
/// * `texture` - A reference to a `Texture` representing the texture to be set.
pub fn set_texture(texture: &Texture) {
    unsafe {
        CGL_widgets_set_texture(texture.handle);
    }
}

/// Sets the font texture for the current frame.
/// 
/// # Arguments
/// 
/// * `texture` - A reference to a `Texture` representing the font texture to be set.
pub fn set_font_texture(texture: &Texture) {
    unsafe {
        CGL_widgets_set_font_texture(texture.handle);
    }
}

/// Sets the texture coordinate scale and offset for the current frame.
/// 
/// # Arguments
/// 
/// * `scale_x` - A `f32` representing the scale factor for the x-coordinate.
/// * `scale_y` - A `f32` representing the scale factor for the y-coordinate.
/// * `offset_x` - A `f32` representing the offset for the x-coordinate.
/// * `offset_y` - A `f32` representing the offset for the y-coordinate.
pub fn set_texture_coordinate_so(scale_x: f32, scale_y: f32, offset_x: f32, offset_y: f32) {
    unsafe {
        CGL_widgets_set_texture_coordinate_so(scale_x, scale_y, offset_x, offset_y);
    }
}



/// This enables the transformation application on the CPU.
/// 
/// Enabling this will slow down the rendering process.
/// 
/// Also, this is currently a bit glitchy.
pub fn apply_transformations_on_cpu() {
    unsafe {
        CGL_widgets_apply_transformations_on_cpu();
    }
}

/// This enables the transformation application on the GPU.
/// 
/// This is the default setting.
pub fn apply_transformations_on_gpu() {
    unsafe {
        CGL_widgets_apply_transformations_on_gpu();
    }
}

/// Adds a triangle to the current frame.
/// 
/// # Arguments
/// 
/// * `a` - A `Vector3` representing the first vertex of the triangle.
/// * `b` - A `Vector3` representing the second vertex of the triangle.
/// * `c` - A `Vector3` representing the third vertex of the triangle.
pub fn add_triangle(a: Vector3, b: Vector3, c: Vector3) {
    unsafe {
        CGL_widgets_add_triangle(a, b, c);
    }
}

/// Adds a quad to the current frame.
/// 
/// # Arguments
/// 
/// * `a` - A `Vector3` representing the first vertex of the quad.
/// * `b` - A `Vector3` representing the second vertex of the quad.
/// * `c` - A `Vector3` representing the third vertex of the quad.
/// * `d` - A `Vector3` representing the fourth vertex of the quad.
pub fn add_quad(a: Vector3, b: Vector3, c: Vector3, d: Vector3) {
    unsafe {
        CGL_widgets_add_quad(a, b, c, d);
    }
}

/// Adds a quad to the current frame.
/// 
/// # Arguments
/// 
/// * `ax` - A `f32` representing the x-coordinate of the first vertex of the quad.
/// * `ay` - A `f32` representing the y-coordinate of the first vertex of the quad.
/// * `bx` - A `f32` representing the x-coordinate of the second vertex of the quad.
/// * `by` - A `f32` representing the y-coordinate of the second vertex of the quad.
/// * `cx` - A `f32` representing the x-coordinate of the third vertex of the quad.
/// * `cy` - A `f32` representing the y-coordinate of the third vertex of the quad.
/// * `dx` - A `f32` representing the x-coordinate of the fourth vertex of the quad.
/// * `dy` - A `f32` representing the y-coordinate of the fourth vertex of the quad.
pub fn add_quad_8f(ax: f32, ay: f32, bx: f32, by: f32, cx: f32, cy: f32, dx: f32, dy: f32) {
    unsafe {
        CGL_widgets_add_quad_8f(ax, ay, bx, by, cx, cy, dx, dy);
    }
}

/// Adds a line to the current frame.
/// 
/// # Arguments
/// 
/// * `start` - A `Vector3` representing the starting point of the line.
/// * `end` - A `Vector3` representing the ending point of the line.
pub fn add_line(start: Vector3, end: Vector3) {
    unsafe {
        CGL_widgets_add_line(start, end);
    }
}

/// Adds a line to the current frame.
/// 
/// # Arguments
/// 
/// * `start_x` - A `f32` representing the x-coordinate of the starting point of the line.
/// * `start_y` - A `f32` representing the y-coordinate of the starting point of the line.
/// * `end_x` - A `f32` representing the x-coordinate of the ending point of the line.
/// * `end_y` - A `f32` representing the y-coordinate of the ending point of the line.
pub fn add_line2f(start_x: f32, start_y: f32, end_x: f32, end_y: f32) {
    unsafe {
        CGL_widgets_add_line2f(start_x, start_y, end_x, end_y);
    }
}

/// Adds a rectangle to the current frame.
/// 
/// # Arguments
/// 
/// * `start` - A `Vector3` representing the starting point of the rectangle.
/// * `size` - A `Vector2` representing the size of the rectangle.
pub fn add_rect(start: Vector3, size: Vector2) {
    unsafe {
        CGL_widgets_add_rect(start, size);
    }
}

/// Adds a rectangle to the current frame.
/// 
/// # Arguments
/// 
/// * `start_x` - A `f32` representing the x-coordinate of the starting point of the rectangle.
/// * `start_y` - A `f32` representing the y-coordinate of the starting point of the rectangle.
/// * `size_x` - A `f32` representing the width of the rectangle.
/// * `size_y` - A `f32` representing the height of the rectangle.
pub fn add_rect2f(start_x: f32, start_y: f32, size_x: f32, size_y: f32) {
    unsafe {
        CGL_widgets_add_rect2f(start_x, start_y, size_x, size_y);
    }
}

/// Adds a rounded rectangle to the current frame.
/// 
/// # Arguments
/// 
/// * `start_x` - A `f32` representing the x-coordinate of the starting point of the rectangle.
/// * `start_y` - A `f32` representing the y-coordinate of the starting point of the rectangle.
/// * `size_x` - A `f32` representing the width of the rectangle.
/// * `size_y` - A `f32` representing the height of the rectangle.
/// * `radius` - A `f32` representing the radius of the corners of the rectangle.
/// * `res` - An `i32` representing the number of segments used to approximate the corners of the rectangle.
pub fn add_rounded_rect2f(start_x: f32, start_y: f32, size_x: f32, size_y: f32, radius: f32, res: i32) {
    unsafe {
        CGL_widgets_add_rounded_rect2f(start_x, start_y, size_x, size_y, radius, res);
    }
}

/// Adds a circle to the current frame.
/// 
/// # Arguments
/// 
/// * `position` - A `Vector3` representing the position of the center of the circle.
/// * `radius` - A `f32` representing the radius of the circle.
pub fn add_circle(position: Vector3, radius: f32) {
    unsafe {
        CGL_widgets_add_circle(position, radius);
    }
}

/// Adds a circle to the current frame.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the center of the circle.
/// * `pos_y` - A `f32` representing the y-coordinate of the center of the circle.
/// * `radius` - A `f32` representing the radius of the circle.
pub fn add_circle2f(pos_x: f32, pos_y: f32, radius: f32) {
    unsafe {
        CGL_widgets_add_circle2f(pos_x, pos_y, radius);
    }
}

/// Adds a circle to the current frame.
/// 
/// # Arguments
/// 
/// * `position` - A `Vector3` representing the position of the center of the circle.
/// * `radius` - A `f32` representing the radius of the circle.
/// * `res` - An `i32` representing the number of segments used to approximate the circle.
pub fn add_circler(position: Vector3, radius: f32, res: i32) {
    unsafe {
        CGL_widgets_add_circler(position, radius, res);
    }
}

/// Adds a circle to the current frame.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the center of the circle.
/// * `pos_y` - A `f32` representing the y-coordinate of the center of the circle.
/// * `radius` - A `f32` representing the radius of the circle.
/// * `res` - An `i32` representing the number of segments used to approximate the circle.
pub fn add_circle2fr(pos_x: f32, pos_y: f32, radius: f32, res: i32) {
    unsafe {
        CGL_widgets_add_circle2fr(pos_x, pos_y, radius, res);
    }
}

/// Adds an oval to the current frame.
/// 
/// # Arguments
/// 
/// * `position` - A `Vector3` representing the position of the center of the oval.
/// * `radius` - A `Vector2` representing the radii of the oval.
pub fn add_oval(position: Vector3, radius: Vector2) {
    unsafe {
        CGL_widgets_add_oval(position, radius);
    }
}

/// Adds an oval to the current frame.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the center of the oval.
/// * `pos_y` - A `f32` representing the y-coordinate of the center of the oval.
/// * `radius_x` - A `f32` representing the x-radius of the oval.
/// * `radius_y` - A `f32` representing the y-radius of the oval.
pub fn add_oval2f(pos_x: f32, pos_y: f32, radius_x: f32, radius_y: f32) {
    unsafe {
        CGL_widgets_add_oval2f(pos_x, pos_y, radius_x, radius_y);
    }
}


/// Adds an oval to the current frame.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the center of the oval.
/// * `pos_y` - A `f32` representing the y-coordinate of the center of the oval.
/// * `radiusx` - A `f32` representing the x-radius of the oval.
/// * `radiusy` - A `f32` representing the y-radius of the oval.
/// * `resolution` - An `i32` representing the number of segments used to approximate the oval.
pub fn add_oval2fr(pos_x: f32, pos_y: f32, radiusx: f32, radiusy: f32, resolution: i32) {
    unsafe {
        CGL_widgets_add_oval2fr(pos_x, pos_y, radiusx, radiusy, resolution);
    }
}

/// Adds an arc to the current frame.
/// 
/// # Arguments
/// 
/// * `pos_x` - A `f32` representing the x-coordinate of the center of the arc.
/// * `pos_y` - A `f32` representing the y-coordinate of the center of the arc.
/// * `radius` - A `f32` representing the radius of the arc.
/// * `start_angle` - A `f32` representing the starting angle of the arc in radians.
/// * `end_angle` - A `f32` representing the ending angle of the arc in radians.
/// * `resolution` - An `i32` representing the number of segments used to approximate the arc.
pub fn add_arc2f(pos_x: f32, pos_y: f32, radius: f32, start_angle: f32, end_angle: f32, resolution: i32) {
    unsafe {
        CGL_widgets_add_arc2f(pos_x, pos_y, radius, start_angle, end_angle, resolution);
    }
}

/// Adds a character to the current frame.
/// 
/// # Arguments
/// 
/// * `c` - A `char` representing the character to add.
/// * `x` - A `f32` representing the x-coordinate of the starting point of the character.
/// * `y` - A `f32` representing the y-coordinate of the starting point of the character.
/// * `sx` - A `f32` representing the x-scale of the character.
/// * `sy` - A `f32` representing the y-scale of the character.
/// 
/// # Returns
/// 
/// An `i32` representing the width of the character.
pub fn add_character(c: char, x: f32, y: f32, sx: f32, sy: f32) -> i32 {
    unsafe {
        CGL_widgets_add_character(c as c_char, x, y, sx, sy)
    }
}

/// Adds a string to the current frame.
/// 
/// # Arguments
/// 
/// * `str` - A `&str` representing the string to add.
/// * `x` - A `f32` representing the x-coordinate of the starting point of the string.
/// * `y` - A `f32` representing the y-coordinate of the starting point of the string.
/// * `sx` - A `f32` representing the x-scale of the string.
/// * `sy` - A `f32` representing the y-scale of the string.
/// 
/// # Returns
/// 
/// An `i32` representing the width of the string.
pub fn add_string(str_value: &str, x: f32, y: f32, sx: f32, sy: f32) -> i32 {
    unsafe {
        let string_c =  std::ffi::CString::new(str_value).unwrap();
        CGL_widgets_add_string( string_c.as_ptr() as *const c_char, x, y, sx, sy)
    }
}

/// Adds a string to the current frame with a specified font.
/// 
/// # Arguments
/// 
/// * `str` - A `&str` representing the string to add.
/// * `font` - A `*mut CGL_font` representing the font to use.
/// * `x` - A `f32` representing the x-coordinate of the starting point of the string.
/// * `y` - A `f32` representing the y-coordinate of the starting point of the string.
/// * `sx` - A `f32` representing the x-scale of the string.
/// * `scale_y` - A `f32` representing the y-scale of the string.
/// 
/// # Returns
/// 
/// A `f32` representing the width of the string.
pub fn add_string_with_font(str_value: &str, font: &Font, x: f32, y: f32, sx: f32, scale_y: f32) -> f32 {
    unsafe {
        let string_c =  std::ffi::CString::new(str_value).unwrap();
        CGL_widgets_add_string_with_font(string_c.as_ptr() as *const c_char, font.handle, x, y, sx, scale_y)
    }
}

/// Adds a cubic bezier curve to the current frame.
/// 
/// # Arguments
/// 
/// * `start` - A `Vector3` representing the starting point of the curve.
/// * `end` - A `Vector3` representing the ending point of the curve.
/// * `control_1` - A `Vector3` representing the first control point of the curve.
/// * `control_2` - A `Vector3` representing the second control point of the curve.
/// * `resolution` - An `i32` representing the number of segments used to approximate the curve.
pub fn add_cubic_bazier(start: Vector3, end: Vector3, control_1: Vector3, control_2: Vector3, resolution: i32) {
    unsafe {
        CGL_widgets_add_cubic_bazier(start, end, control_1, control_2, resolution);
    }
}

/// Adds a cubic bezier curve to the current frame.
/// 
/// # Arguments
/// 
/// * `start` - A `Vector2` representing the starting point of the curve.
/// * `end` - A `Vector2` representing the ending point of the curve.
/// * `control_1` - A `Vector2` representing the first control point of the curve.
/// * `control_2` - A `Vector2` representing the second control point of the curve.
/// * `resolution` - An `i32` representing the number of segments used to approximate the curve.
pub fn add_cubic_bazier2v(start: Vector2, end: Vector2, control_1: Vector2, control_2: Vector2, resolution: i32) {
    unsafe {
        CGL_widgets_add_cubic_bazier2v(start, end, control_1, control_2, resolution);
    }
}

/// Adds a cubic bezier curve to the current frame.
/// 
/// # Arguments
/// 
/// * `start_x` - A `f32` representing the x-coordinate of the starting point of the curve.
/// * `start_y` - A `f32` representing the y-coordinate of the starting point of the curve.
/// * `end_x` - A `f32` representing the x-coordinate of the ending point of the curve.
/// * `end_y` - A `f32` representing the y-coordinate of the ending point of the curve.
/// * `control_1_x` - A `f32` representing the x-coordinate of the first control point of the curve.
/// * `control_1_y` - A `f32` representing the y-coordinate of the first control point of the curve.
/// * `control_2_x` - A `f32` representing the x-coordinate of the second control point of the curve.
/// * `control_2_y` - A `f32` representing the y-coordinate of the second control point of the curve.
/// * `resolution` - An `i32` representing the number of segments used to approximate the curve.
pub fn add_cubic_bazier2f(start_x: f32, start_y: f32, end_x: f32, end_y: f32, control_1_x: f32, control_1_y: f32, control_2_x: f32, control_2_y: f32, resolution: i32) {
    unsafe {
        CGL_widgets_add_cubic_bazier2f(start_x, start_y, end_x, end_y, control_1_x, control_1_y, control_2_x, control_2_y, resolution);
    }
}

/// Adds a cubic bezier curve to the current frame using points.
/// 
/// # Arguments
/// 
/// * `start` - A `Vector3` representing the starting point of the curve.
/// * `end` - A `Vector3` representing the ending point of the curve.
/// * `control_1` - A `Vector3` representing the first control point of the curve.
/// * `control_2` - A `Vector3` representing the second control point of the curve.
/// * `resolution` - An `i32` representing the number of segments used to approximate the curve.
pub fn add_cubic_bazier_points(start: Vector3, end: Vector3, control_1: Vector3, control_2: Vector3, resolution: i32) {
    unsafe {
        CGL_widgets_add_cubic_bazier_points(start, end, control_1, control_2, resolution);
    }
}


// Adds a cubic bezier curve to the current frame using 2D vectors as points.
/// 
/// # Arguments
/// 
/// * `start` - A `Vector2` representing the starting point of the curve.
/// * `end` - A `Vector2` representing the ending point of the curve.
/// * `control_1` - A `Vector2` representing the first control point of the curve.
/// * `control_2` - A `Vector2` representing the second control point of the curve.
/// * `resolution` - An `i32` representing the number of segments used to approximate the curve.
pub fn add_cubic_bazier_points2v(start: Vector2, end: Vector2, control_1: Vector2, control_2: Vector2, resolution: i32) {
    unsafe {
        CGL_widgets_add_cubic_bazier_points2v(start, end, control_1, control_2, resolution);
    }
}
