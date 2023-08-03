//! The Shader module for CGL

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_char, c_float, c_double};

/// The internal window handle used by CGL
#[repr(C)]
pub(crate) struct CGL_shader {
    _private: c_void
}

extern {
   fn CGL_shader_create(vertex_shader_source: *const c_char, fragment_shader_source: *const c_char, error: *mut *mut c_char) -> *mut CGL_shader;
   fn CGL_shader_compute_create(compute_shader_source: *const c_char, error: *mut *mut c_char) -> *mut CGL_shader;
   fn CGL_shader_create_from_files(vertex_shader_file: *const c_char, fragment_shader_file: *const c_char, error: *mut *mut c_char) -> *mut CGL_shader;
   fn CGL_shader_compute_create_from_files(compute_shader_file: *const c_char, error: *mut *mut c_char) -> *mut CGL_shader;
   fn CGL_shader_destroy(shader: *mut CGL_shader) -> ();
   fn CGL_shader_bind(shader: *mut CGL_shader) -> ();
   fn CGL_shader_get_uniform_location(shader: *mut CGL_shader, name: *const c_char) -> c_int;
   fn CGL_shader_set_uniform_mat4(shader: *mut CGL_shader, location: c_int, matrix: *const crate::math::Matrix4x4) -> ();
   fn CGL_shader_set_uniform_vec4(shader: *mut CGL_shader, location: c_int, vector: *const crate::math::Vector4) -> ();
   fn CGL_shader_set_uniform_vec3(shader: *mut CGL_shader, location: c_int, vector: *const crate::math::Vector3) -> ();
   fn CGL_shader_set_uniform_vec2(shader: *mut CGL_shader, location: c_int, vector: *const crate::math::Vector2) -> ();
   fn CGL_shader_set_uniform_int(shader: *mut CGL_shader, location: c_int, value: c_int) -> ();
   fn CGL_shader_set_uniform_bool(shader: *mut CGL_shader, location: c_int, value: c_int) -> ();
   fn CGL_shader_set_uniform_float(shader: *mut CGL_shader, location: c_int, value: c_float) -> ();
   fn CGL_shader_set_uniform_double(shader: *mut CGL_shader, location: c_int, value: c_double) -> ();
   fn CGL_shader_set_uniform_vec2v(shader: *mut CGL_shader, location: c_int, x: c_float, y: c_float) -> ();
   fn CGL_shader_set_uniform_vec3v(shader: *mut CGL_shader, location: c_int, x: c_float, y: c_float, z: c_float) -> ();
   fn CGL_shader_set_uniform_vec4v(shader: *mut CGL_shader, location: c_int, x: c_float, y: c_float, z: c_float, w: c_float) -> ();
   fn CGL_shader_set_uniform_ivec2v(shader: *mut CGL_shader, location: c_int, x: c_int, y: c_int) -> ();
   fn CGL_shader_set_uniform_ivec3v(shader: *mut CGL_shader, location: c_int, x: c_int, y: c_int, z: c_int) -> ();
   fn CGL_shader_set_uniform_ivec4v(shader: *mut CGL_shader, location: c_int, x: c_int, y: c_int, z: c_int, w: c_int) -> ();
   fn CGL_shader_set_user_data(shader: *mut CGL_shader, user_data: *mut c_void) -> ();
   fn CGL_shader_get_user_data(shader: *mut CGL_shader) -> *mut c_void;
   fn CGL_shader_compute_dispatch(shader: *mut CGL_shader, x: c_int, y: c_int, z: c_int) -> ();
}

/// A shader object
#[derive(Debug)]
pub struct Shader {
    handle: *mut CGL_shader,
    has_been_destroyed: bool,
    uniform_locations: std::collections::HashMap<String, i32>
}


const BASIC_VERTEX_SHADER: &str = r#"#version 430 core

layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
"#;

const BASIC_FRAGMENT_SHADER: &str = r#"#version 430 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
"#;

const BASIC_COMPUTE_SHADER: &str = r#"#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
    // Do nothing
}
"#;

impl Shader {

    /// Creates a new shader object from the provided vertex and fragment shader source code.
    ///
    /// # Arguments
    ///
    /// * `vertex_shader_source` - The source code for the vertex shader.
    /// * `fragment_shader_source` - The source code for the fragment shader.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Shader` object if successful, or a `String` containing an error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```no_run
    /// let vertex_shader_source = r#"#version 430 core
    ///
    ///     layout (location = 0) in vec3 aPos;
    ///
    ///     void main()
    ///     {
    ///         gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ///     }
    /// "#;
    ///
    /// let fragment_shader_source = r#"#version 430 core
    ///
    ///     out vec4 FragColor;
    ///
    ///     void main()
    ///     {
    ///         FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    ///     }
    /// "#;
    ///
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let shader = cgl_rs::graphics::Shader::from_source(vertex_shader_source, fragment_shader_source).unwrap();
    ///    // Do stuff with the shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn from_source(vertex_shader_source: &str, fragment_shader_source: &str) -> Result<Shader, String> {
        let mut error_string_ptr: *mut c_char = std::ptr::null_mut();
        let error_string_ptr_ptr: *mut *mut c_char = &mut error_string_ptr;
        let handle = unsafe {
            let vertex_shader_source_c = std::ffi::CString::new(vertex_shader_source).unwrap();
            let fragment_shader_source_c = std::ffi::CString::new(fragment_shader_source).unwrap();
            CGL_shader_create(vertex_shader_source_c.as_ptr(), fragment_shader_source_c.as_ptr(), error_string_ptr_ptr)
        };
        if handle.is_null() {
            let error_string = unsafe { std::ffi::CStr::from_ptr(*error_string_ptr_ptr) };
            let error_string = error_string.to_str().unwrap();
            unsafe { libc::free(error_string_ptr as *mut c_void) };
            return Err(error_string.to_string());
        }
        Ok(Shader {
            handle,
            has_been_destroyed: false,
            uniform_locations: std::collections::HashMap::new()
        })
    }

    /// Creates a new shader object from the provided vertex and fragment shader source files.
    ///
    /// # Arguments
    ///
    /// * `vertex_shader_path` - The path to the file containing the source code for the vertex shader.
    /// * `fragment_shader_path` - The path to the file containing the source code for the fragment shader.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Shader` object if successful, or a `String` containing an error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let shader = cgl_rs::graphics::Shader::from_files("path/to/vertex_shader.glsl", "path/to/fragment_shader.glsl").unwrap();
    ///    // Do stuff with the shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn from_files(vertex_shader_path: &str, fragment_shader_path: &str) -> Result<Shader, String> {
        let mut error_string_ptr: *mut c_char = std::ptr::null_mut();
        let error_string_ptr_ptr: *mut *mut c_char = &mut error_string_ptr;
        let handle = unsafe {
            let vertex_shader_path_c = std::ffi::CString::new(vertex_shader_path).unwrap();
            let fragment_shader_path_c = std::ffi::CString::new(fragment_shader_path).unwrap();
            CGL_shader_create_from_files(vertex_shader_path_c.as_ptr(), fragment_shader_path_c.as_ptr(), error_string_ptr_ptr)
        };
        if handle.is_null() {
            let error_string = unsafe { std::ffi::CStr::from_ptr(*error_string_ptr_ptr) };
            let error_string = error_string.to_str().unwrap();
            unsafe { libc::free(error_string_ptr as *mut c_void) };
            return Err(error_string.to_string());
        }
        Ok(Shader {
            handle,
            has_been_destroyed: false,
            uniform_locations: std::collections::HashMap::new()
        })
    }

    /// Creates a new compute shader object from the provided compute shader source code.
    ///
    /// # Arguments
    ///
    /// * `compute_shader_source` - The source code for the compute shader.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Shader` object if successful, or a `String` containing an error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```no_run
    /// let compute_shader_source = r#"#version 430 core
    /// 
    ///    layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    /// 
    ///    void main()
    ///    {
    ///       // Do stuff
    ///    }
    /// "#;
    /// 
    /// 
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let shader = cgl_rs::graphics::Shader::compute_from_source("path/to/compute_shader.glsl").unwrap();
    ///    // Do stuff with the shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn compute_from_source(compute_shader_source: &str) -> Result<Shader, String> {
        let mut error_string_ptr: *mut c_char = std::ptr::null_mut();
        let error_string_ptr_ptr: *mut *mut c_char = &mut error_string_ptr;
        let handle = unsafe {
            let compute_shader_source_c = std::ffi::CString::new(compute_shader_source).unwrap();
            CGL_shader_compute_create(compute_shader_source_c.as_ptr(), error_string_ptr_ptr)
        };
        if handle.is_null() {
            let error_string = unsafe { std::ffi::CStr::from_ptr(*error_string_ptr_ptr) };
            let error_string = error_string.to_str().unwrap();
            unsafe { libc::free(error_string_ptr as *mut c_void) };
            return Err(error_string.to_string());
        }
        Ok(Shader {
            handle,
            has_been_destroyed: false,
            uniform_locations: std::collections::HashMap::new()
        })
    }

    /// Creates a new compute shader object from the provided compute shader file.
    ///
    /// # Arguments
    ///
    /// * `compute_shader_path` - The path to the compute shader file.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Shader` object if successful, or a `String` containing an error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let shader = cgl_rs::graphics::Shader::compute_from_files("path/to/compute_shader.glsl").unwrap();
    ///    // Do stuff with the shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn compute_from_files(compute_shader_path: &str) -> Result<Shader, String> {
        let mut error_string_ptr: *mut c_char = std::ptr::null_mut();
        let error_string_ptr_ptr: *mut *mut c_char = &mut error_string_ptr;
        let handle = unsafe {
            let compute_shader_path_c = std::ffi::CString::new(compute_shader_path).unwrap();
            CGL_shader_compute_create_from_files(compute_shader_path_c.as_ptr(), error_string_ptr_ptr)
        };
        if handle.is_null() {
            let error_string = unsafe { std::ffi::CStr::from_ptr(*error_string_ptr_ptr) };
            let error_string = error_string.to_str().unwrap();
            unsafe { libc::free(error_string_ptr as *mut c_void) };
            return Err(error_string.to_string());
        }
        Ok(Shader {
            handle,
            has_been_destroyed: false,
            uniform_locations: std::collections::HashMap::new()
        })
    }

    /// Creates a basic compute shader object with a simple compute shader.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Shader` object if successful, or a `String` containing an error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let shader = cgl_rs::graphics::Shader::create_dummy_compute().unwrap();
    ///    // Do stuff with the shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn create_dummy_compute() -> Result<Shader, String> {
        Shader::compute_from_source(BASIC_COMPUTE_SHADER)
    }

    /// Creates a basic shader object with a simple vertex and fragment shader.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Shader` object if successful, or a `String` containing an error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///    // Do stuff with the shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn create_dummy() -> Result<Shader, String> {
        Shader::from_source(BASIC_VERTEX_SHADER, BASIC_FRAGMENT_SHADER)
    }

    /// Destroys the shader object.
    ///
    /// # Remarks
    ///
    /// This function destroys the shader object and frees any resources associated with it. If the shader object has already been destroyed, this function does nothing.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///    // Do stuff with the shader
    ///    shader.destroy();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_shader_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }


    /// Retrieves the location of0 a uniform variable in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    ///
    /// # Returns
    ///
    /// Returns an `i32` representing the location of the uniform variable in the shader program.
    ///
    /// # Remarks
    ///
    /// This function caches the uniform location for future use, so it is more efficient to call this function once and store the result than to call it repeatedly.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     let uniform_location = shader.get_uniform_location("uniform_name");
    ///     // Do stuff with the uniform location
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_uniform_location(&mut self, uniform_name: &str) -> i32 {
        if self.uniform_locations.contains_key(uniform_name) {
            return self.uniform_locations[uniform_name];
        }
        let uniform_name_c = std::ffi::CString::new(uniform_name).unwrap();
        let location = unsafe {
            CGL_shader_get_uniform_location(self.handle, uniform_name_c.as_ptr()) as i32
        };
        self.uniform_locations.insert(uniform_name.to_string(), location);
        location
    }


    
    /// Sets the value of a uniform variable of type Vector2 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A reference to a Vector2 containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     let vec2 = cgl_rs::math::Vector2::new(1.0, 2.0);
    ///     shader.set_vec2("uniform_name", &vec2);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vec2(&mut self, uniform_name: &str, value: &crate::math::Vector2) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_vec2(self.handle, location, value);
        }
    }
    
    /// Sets the value of a uniform variable of type Vector3 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A reference to a Vector3 containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     let vec3 = cgl_rs::math::Vector3::new(1.0, 2.0, 3.0);
    ///     shader.set_vec3("uniform_name", &vec3);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vec3(&mut self, uniform_name: &str, value: &crate::math::Vector3) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_vec3(self.handle, location, value);
        }
    }
    
    /// Sets the value of a uniform variable of type Vector4 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A reference to a Vector4 containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     let vec4 = cgl_rs::math::Vector4::new(1.0, 2.0, 3.0, 4.0);
    ///     shader.set_vec4("uniform_name", &vec4);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vec4(&mut self, uniform_name: &str, value: &crate::math::Vector4) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_vec4(self.handle, location, value);
        }
    }
    
    /// Sets the value of a uniform variable of type Matrix4x4 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A reference to a Matrix4x4 containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     let mat4 = cgl_rs::math::Matrix4x4::identity();
    ///     shader.set_mat4("uniform_name", &mat4);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_mat4(&mut self, uniform_name: &str, value: &crate::math::Matrix4x4) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_mat4(self.handle, location, value);
        }
    }
    
    /// Sets the value of a uniform variable of type i32 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - An i32 value containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_int("uniform_name", 42);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_int(&mut self, uniform_name: &str, value: i32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_int(self.handle, location, value);
        }
    }

    /// Sets the value of a uniform variable of type bool in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A boolean value containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_bool("uniform_name", true);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_bool(&mut self, uniform_name: &str, value: bool) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_bool(self.handle, location, value as i32);
        }
    }

    /// Sets the value of a uniform variable of type float in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A f32 value containing the value to set.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_float("uniform_name", 3.14);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_float(&mut self, uniform_name: &str, value: f32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_float(self.handle, location, value);
        }
    }

    /// Sets the value of a uniform variable of type double in the shader program.
    /// 
    /// # Arguments
    /// 
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `value` - A f64 value containing the value to set.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///    shader.set_double("uniform_name", 3.14);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_double(&mut self, uniform_name: &str, value: f64) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_double(self.handle, location, value);
        }
    }


    /// Sets the value of a uniform variable of type vec2 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `x` - A f32 value containing the x component of the vector.
    /// * `y` - A f32 value containing the y component of the vector.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_vec2v("uniform_name", 1.0, 2.0);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vec2v(&mut self, uniform_name: &str, x: f32, y: f32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_vec2v(self.handle, location, x, y);
        }
    }

    /// Sets the value of a uniform variable of type vec3 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `x` - A f32 value containing the x component of the vector.
    /// * `y` - A f32 value containing the y component of the vector.
    /// * `z` - A f32 value containing the z component of the vector.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_vec3v("uniform_name", 1.0, 2.0, 3.0);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vec3v(&mut self, uniform_name: &str, x: f32, y: f32, z: f32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_vec3v(self.handle, location, x, y, z);
        }
    }

    /// Sets the value of a uniform variable of type vec4 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `x` - A f32 value containing the x component of the vector.
    /// * `y` - A f32 value containing the y component of the vector.
    /// * `z` - A f32 value containing the z component of the vector.
    /// * `w` - A f32 value containing the w component of the vector.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_vec4v("uniform_name", 1.0, 2.0, 3.0, 4.0);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vec4v(&mut self, uniform_name: &str, x: f32, y: f32, z: f32, w: f32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_vec4v(self.handle, location, x, y, z, w);
        }
    }

    /// Sets the value of a uniform variable of type ivec2 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `x` - An i32 value containing the x component of the vector.
    /// * `y` - An i32 value containing the y component of the vector.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_ivec2v("uniform_name", 1, 2);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_ivec2v(&mut self, uniform_name: &str, x: i32, y: i32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_ivec2v(self.handle, location, x, y);
        }
    }

    /// Sets the value of a uniform variable of type ivec3 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `x` - An i32 value containing the x component of the vector.
    /// * `y` - An i32 value containing the y component of the vector.
    /// * `z` - An i32 value containing the z component of the vector.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_ivec3v("uniform_name", 1, 2, 3);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_ivec3v(&mut self, uniform_name: &str, x: i32, y: i32, z: i32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_ivec3v(self.handle, location, x, y, z);
        }
    }

    /// Sets the value of a uniform variable of type ivec4 in the shader program.
    ///
    /// # Arguments
    ///
    /// * `uniform_name` - A string slice containing the name of the uniform variable.
    /// * `x` - An i32 value containing the x component of the vector.
    /// * `y` - An i32 value containing the y component of the vector.
    /// * `z` - An i32 value containing the z component of the vector.
    /// * `w` - An i32 value containing the w component of the vector.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     shader.set_ivec4v("uniform_name", 1, 2, 3, 4);
    ///     // Do stuff with the uniform variable
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_ivec4v(&mut self, uniform_name: &str, x: i32, y: i32, z: i32, w: i32) {
        let location = self.get_uniform_location(uniform_name);
        unsafe {
            CGL_shader_set_uniform_ivec4v(self.handle, location, x, y, z, w);
        }
    }

    /// Dispatches a compute shader with the specified number of work groups.
    ///
    /// # Arguments
    ///
    /// * `x` - An i32 value containing the number of work groups in the x dimension.
    /// * `y` - An i32 value containing the number of work groups in the y dimension.
    /// * `z` - An i32 value containing the number of work groups in the z dimension.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello World", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let shader = cgl_rs::graphics::Shader::create_dummy_compute().unwrap();
    ///     shader.dispatch(1, 1, 1);
    ///     // Do stuff with the compute shader
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn dispatch(&self, x: i32, y: i32, z: i32) {
        unsafe {
            CGL_shader_compute_dispatch(self.handle, x, y, z);
        }
    }
}

impl Drop for Shader {
    fn drop(&mut self) {
        self.destroy();
    }
}


impl Clone for Shader {
    /// Clones the shader object.
    ///
    /// NOTE: The new instance will have the same handle, `has_been_destroyed` flag.
    ///       This means that the new instance will not be able to receive events nor will the internal window handle be
    ///       destroyed when the new instance is dropped. The internal window handle will be destroyed when the original
    ///       instance is dropped.
    /// 
    /// 
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let shader = cgl_rs::graphics::Shader::create_dummy().unwrap();
    ///     let shader2 = shader.clone();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    fn clone(&self) -> Self {
        Shader {
            handle: self.handle.clone(),
            has_been_destroyed: true,
            uniform_locations: self.uniform_locations.clone()
        }
    }
}