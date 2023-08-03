//! The Tilemap module provides a way to render tilemaps to the screen.

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_uint, c_float};


use crate::graphics::{Texture, CGL_texture};

/// The internal handle used by CGL
#[repr(C)]
pub(crate) struct CGL_tilemap {
    _private: c_void
}

extern {
    fn CGL_tilemap_create(tile_count_x: c_uint, tile_count_y: c_uint, tile_size_x: c_uint, tile_size_y: c_uint, ssbo_binding: c_uint) -> *mut CGL_tilemap;
    fn CGL_tilemap_destroy(tilemap: *mut CGL_tilemap) -> c_void;
    fn CGL_tilemap_set_auto_upload(tilemap: *mut CGL_tilemap, value: c_int) -> c_void;
    fn CGL_tilemap_get_auto_upload(tilemap: *mut CGL_tilemap) -> c_int;
    fn CGL_tilemap_upload(tilemap: *mut CGL_tilemap) -> c_int;
    fn CGL_tilemap_set_tile_color(tilemap: *mut CGL_tilemap, tile_x: c_uint, tile_y: c_uint, r: c_float, g: c_float, b: c_float) -> c_void;
    fn CGL_tilemap_set_tile_texture_from_array(tilemap: *mut CGL_tilemap, tile_x: c_uint, tile_y: c_uint, texture_index: c_uint) -> c_void;
    fn CGL_tilemap_set_tile_texture_from_tileset(tilemap: *mut CGL_tilemap, tile_x: c_uint, tile_y: c_uint, texture_x_min: c_float, texture_y_min: c_float, texture_x_max: c_float, texture_y_max: c_float) -> c_void;
    fn CGL_tilemap_set_all_tile_color(tilemap: *mut CGL_tilemap, r: c_float, g: c_float, b: c_float) -> c_void;
    fn CGL_tilemap_set_all_tile_texture_from_array(tilemap: *mut CGL_tilemap, texture_index: c_uint) -> c_void;
    fn CGL_tilemap_set_all_tile_texture_from_tileset(tilemap: *mut CGL_tilemap, texture_x_min: c_float, texture_y_min: c_float, texture_x_max: c_float, texture_y_max: c_float) -> c_void;
    fn CGL_tilemap_clear_tile(tilemap: *mut CGL_tilemap, tile_x: c_uint, tile_y: c_uint) -> c_void;
    fn CGL_tilemap_clear_all_tile(tilemap: *mut CGL_tilemap) -> c_void;
    fn CGL_tilemap_render(tilemap: *mut CGL_tilemap, scale_x: c_float, scale_y: c_float, offset_x: c_float, offset_y: c_float, texture: *mut CGL_texture) -> c_void;
    fn CGL_tilemap_reset(tilemap: *mut CGL_tilemap) -> c_void;
}

/// The Tilemap Object
#[repr(C)]
pub struct Tilemap {
    pub(crate) handle: *mut CGL_tilemap,
    pub(crate) has_been_destroyed: bool
}

impl Tilemap {


    /// Creates a new Tilemap object with the specified dimensions and SSBO binding.
    ///
    /// # Arguments
    ///
    /// * `tile_count_x` - The number of tiles in the x direction.
    /// * `tile_count_y` - The number of tiles in the y direction.
    /// * `tile_size_x` - The width of each tile in pixels.
    /// * `tile_size_y` - The height of each tile in pixels.
    /// * `ssbo_binding` - The SSBO binding index to use for the tilemap.
    ///
    /// # Returns
    ///
    /// A `Result` containing the new `Tilemap` object if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///     // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn new(tile_count_x: u32, tile_count_y: u32, tile_size_x: u32, tile_size_y: u32, ssbo_binding: u32) -> Result<Tilemap, &'static str> {
        let handle = unsafe {
            CGL_tilemap_create(tile_count_x, tile_count_y, tile_size_x, tile_size_y, ssbo_binding)
        };
        if handle.is_null() {
            Err("Failed to create tilemap")
        } else {
            Ok(Tilemap {
                handle,
                has_been_destroyed: false
            })
        }
    }

    /// Sets whether the tilemap should automatically upload changes to the GPU.
    ///
    /// # Arguments
    ///
    /// * `value` - A boolean value indicating whether the tilemap should automatically upload changes to the GPU.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_auto_upload(false);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_auto_upload(&self, value: bool) {
        unsafe {
            CGL_tilemap_set_auto_upload(self.handle, value as c_int);
        }
    }

    /// Returns whether the tilemap is set to automatically upload changes to the GPU.
    ///
    /// # Returns
    ///
    /// A boolean value indicating whether the tilemap is set to automatically upload changes to the GPU.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    let auto_upload = tilemap.get_auto_upload();
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_auto_upload(&self) -> bool {
        unsafe {
            CGL_tilemap_get_auto_upload(self.handle) != 0
        }
    }

    /// Uploads any changes made to the tilemap to the GPU. 
    /// This can be slow, so it is recommended to set auto upload to false
    /// and manually call this function when needed for large tilemaps.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_auto_upload(false);
    ///    tilemap.set_tile_color(0, 0, 1.0, 0.0, 0.0, 1.0);
    ///    tilemap.upload();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn upload(&self) {
        unsafe {
            CGL_tilemap_upload(self.handle);
        }
    }

    /// Sets the color of a specific tile in the tilemap.
    ///
    /// # Arguments
    ///
    /// * `tile_x` - The x-coordinate of the tile to set the color of.
    /// * `tile_y` - The y-coordinate of the tile to set the color of.
    /// * `r` - The red component of the color to set.
    /// * `g` - The green component of the color to set.
    /// * `b` - The blue component of the color to set.
    /// * `_a` - The alpha component of the color to set. This is currently unused.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_tile_color(0, 0, 1.0, 0.0, 0.0, 1.0);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_tile_color(&self, tile_x: u32, tile_y: u32, r: f32, g: f32, b: f32, _a: f32) {
        unsafe {
            CGL_tilemap_set_tile_color(self.handle, tile_x, tile_y, r, g, b);
        }
    }

    /// Sets the texture of a specific tile in the tilemap using an array of textures.
    ///
    /// # Arguments
    ///
    /// * `tile_x` - The x-coordinate of the tile to set the texture of.
    /// * `tile_y` - The y-coordinate of the tile to set the texture of.
    /// * `texture_index` - The index of the texture to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_tile_texture_from_array(0, 0, 1);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_tile_texture_from_array(&self, tile_x: u32, tile_y: u32, texture_index: u32) {
        unsafe {
            CGL_tilemap_set_tile_texture_from_array(self.handle, tile_x, tile_y, texture_index);
        }
    }

    /// Sets the texture of a specific tile in the tilemap using a tileset.
    ///
    /// # Arguments
    ///
    /// * `tile_x` - The x-coordinate of the tile to set the texture of.
    /// * `tile_y` - The y-coordinate of the tile to set the texture of.
    /// * `texture_x_min` - The minimum x-coordinate of the texture in the tileset.
    /// * `texture_y_min` - The minimum y-coordinate of the texture in the tileset.
    /// * `texture_x_max` - The maximum x-coordinate of the texture in the tileset.
    /// * `texture_y_max` - The maximum y-coordinate of the texture in the tileset.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_tile_texture_from_tileset(0, 0, 0.0, 0.0, 1.0, 1.0);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_tile_texture_from_tileset(&self, tile_x: u32, tile_y: u32, texture_x_min: f32, texture_y_min: f32, texture_x_max: f32, texture_y_max: f32) {
        unsafe {
            CGL_tilemap_set_tile_texture_from_tileset(self.handle, tile_x, tile_y, texture_x_min, texture_y_min, texture_x_max, texture_y_max);
        }
    }

    
    /// Sets the color of all tiles in the tilemap.
    ///
    /// # Arguments
    ///
    /// * `r` - The red component of the color.
    /// * `g` - The green component of the color.
    /// * `b` - The blue component of the color.
    /// * `_a` - The alpha component of the color (currently unused).
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_all_tile_color(1.0, 0.0, 0.0, 1.0);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_all_tile_color(&self, r: f32, g: f32, b: f32, _a: f32) {
        unsafe {
            CGL_tilemap_set_all_tile_color(self.handle, r, g, b);
        }
    }

    /// Sets the texture of all tiles in the tilemap using an array of texture indices.
    ///
    /// # Arguments
    ///
    /// * `texture_index` - The index of the texture to set for all tiles.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_all_tile_texture_from_array(0);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_all_tile_texture_from_array(&self, texture_index: u32) {
        unsafe {
            CGL_tilemap_set_all_tile_texture_from_array(self.handle, texture_index);
        }
    }

    /// Sets the texture of all tiles in the tilemap using a tileset.
    ///
    /// # Arguments
    ///
    /// * `texture_x_min` - The minimum x-coordinate of the texture in the tileset.
    /// * `texture_y_min` - The minimum y-coordinate of the texture in the tileset.
    /// * `texture_x_max` - The maximum x-coordinate of the texture in the tileset.
    /// * `texture_y_max` - The maximum y-coordinate of the texture in the tileset.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.set_all_tile_texture_from_tileset(0.0, 0.0, 1.0, 1.0);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_all_tile_texture_from_tileset(&self, texture_x_min: f32, texture_y_min: f32, texture_x_max: f32, texture_y_max: f32) {
        unsafe {
            CGL_tilemap_set_all_tile_texture_from_tileset(self.handle, texture_x_min, texture_y_min, texture_x_max, texture_y_max);
        }
    }

    /// Clears the tile at the specified position.
    ///
    /// # Arguments
    ///
    /// * `tile_x` - The x-coordinate of the tile to clear.
    /// * `tile_y` - The y-coordinate of the tile to clear.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.clear_tile(0, 0);
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn clear_tile(&self, tile_x: u32, tile_y: u32) {
        unsafe {
            CGL_tilemap_clear_tile(self.handle, tile_x, tile_y);
        }
    }

    /// Clears all tiles in the tilemap.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.clear_all_tile();
    ///    // Do stuff with tilemap
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn clear_all_tile(&self) {
        unsafe {
            CGL_tilemap_clear_all_tile(self.handle);
        }
    }

    /// Renders the tilemap to the screen.
    ///
    /// # Arguments
    ///
    /// * `scale_x` - The x-axis scale factor.
    /// * `scale_y` - The y-axis scale factor.
    /// * `offset_x` - The x-axis offset.
    /// * `offset_y` - The y-axis offset.
    /// * `texture` - The texture to use for rendering (optional).
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    let mut texture = cgl_rs::graphics::Texture::dummy().unwrap();
    ///    tilemap.set_all_tile_texture_from_tileset(0.0, 0.0, 1.0, 1.0);
    ///    tilemap.render(1.0, 1.0, 0.0, 0.0, Some(&mut texture));
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn render(&self, scale_x: f32, scale_y: f32, offset_x: f32, offset_y: f32, texture: Option<&mut Texture>) {
        unsafe {
            let texture_handle = match texture {
                Some(texture) => texture.handle,
                None => std::ptr::null_mut()
            };
            CGL_tilemap_render(self.handle, scale_x, scale_y, offset_x, offset_y, texture_handle);
        }
    }

    /// Destroys the tilemap object.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut tilemap = cgl_rs::graphics::Tilemap::new(10, 10, 32, 32, 0).unwrap();
    ///    tilemap.destroy(); // Or, just let the tilemap go out of scope.
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_tilemap_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

}


impl Drop for Tilemap {
    fn drop(&mut self) {
        self.destroy();
    }
}

impl Clone for Tilemap {
    fn clone(&self) -> Self {
        Tilemap {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}