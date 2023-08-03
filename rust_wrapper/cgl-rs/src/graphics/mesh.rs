//! The Mesh (CPU & GPU) module for CGL

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, size_t, c_char, c_float};

use crate::math::{IVector4, Vector4, Vector3, Matrix4x4};

/// The internal handle used by CGL
#[repr(C)]
pub(crate) struct CGL_mesh_gpu {
    _private: c_void
}

/// The Mesh Vertex Structure
#[repr(C)] #[derive(Copy, Clone)]
pub struct MeshVertex {
    pub position: Vector4,
    pub normal: Vector4,
    pub texture_coordinates: Vector4,
    pub bone_weights: Vector4,
    pub bone_ids: IVector4
}

/// The Mesh Structure
#[repr(C)] #[derive(Debug)]
pub struct MeshCPU_C {
    pub(crate) index_count: usize,
    pub(crate) index_count_used: usize,
    pub(crate) indices: *mut u32,
    pub(crate) vertex_count: usize,
    pub(crate) vertex_count_used: usize,
    pub(crate) vertices: *mut MeshVertex
}

pub struct MeshCPU {
    pub(crate) handle: *mut MeshCPU_C,
    pub(crate) has_been_destroyed: bool
}

pub struct MeshGPU {
    pub(crate) handle: *mut CGL_mesh_gpu,
    pub(crate) has_been_destroyed: bool
}


extern {
    fn CGL_mesh_cpu_create(vertex_count: size_t, index_count: size_t) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_recalculate_normals(mesh: *mut MeshCPU_C) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_load_obj(path: *const c_char) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_triangle(a: Vector3, b: Vector3, c: Vector3) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_plane(front: Vector3, right: Vector3, resolution: c_int, scale: c_float) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_quad(a: Vector3, b: Vector3, c: Vector3, d: Vector3) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_cube(use_3d_tex_coords: c_int) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_sphere(res_u: c_int, res_v: c_int) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_create_cylinder(start: Vector3, end: Vector3, radius0: c_float, radius1: c_float, resolution: c_int) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_add_mesh(mesh: *mut MeshCPU_C, mesh_other: *mut MeshCPU_C) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_add_cube(mesh: *mut MeshCPU_C, use_3d_tex_coords: c_int) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_add_triangle(mesh: *mut MeshCPU_C, a: Vector3, b: Vector3, c: Vector3) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_add_quad(mesh: *mut MeshCPU_C, a: Vector3, b: Vector3, c: Vector3, d: Vector3) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_add_sphere(mesh: *mut MeshCPU_C, res_u: c_int, res_v: c_int) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_add_cylinder(mesh: *mut MeshCPU_C, start: Vector3, end: Vector3, radius0: f32, radius1: f32, resolution: c_int) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_offset_vertices(mesh: *mut MeshCPU_C, offset: Vector3) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_scale_vertices(mesh: *mut MeshCPU_C, scale: c_float) -> *mut MeshCPU_C;
    // fn CGL_mesh_cpu_rotate_vertices(mesh: *mut MeshCPU_C, CGL_quat rotation) -> *mut MeshCPU_C; // for future
    fn CGL_mesh_cpu_transform_vertices(mesh: *mut MeshCPU_C, transform: &Matrix4x4) -> *mut MeshCPU_C;
    fn CGL_mesh_cpu_destroy(mesh: *mut MeshCPU_C) -> c_void;

    fn CGL_mesh_gpu_create() -> *mut CGL_mesh_gpu; // create mesh (gpu)
    fn CGL_mesh_gpu_destroy(mesh: *mut CGL_mesh_gpu); // destroy mesh (gpu)
    fn CGL_mesh_gpu_render(mesh: *mut CGL_mesh_gpu); // render mesh (gpu)
    fn CGL_mesh_gpu_render_instanced(mesh: *mut CGL_mesh_gpu, count: u32); // render mesh instanced (gpu)
    fn CGL_mesh_gpu_set_user_data(mesh: *mut CGL_mesh_gpu, user_data: *mut std::os::raw::c_void); // set mesh user data
    fn CGL_mesh_gpu_get_user_data(mesh: *mut CGL_mesh_gpu) -> *mut std::os::raw::c_void; // get mesh user data
    fn CGL_mesh_gpu_upload(mesh: *mut CGL_mesh_gpu, mesh_cpu: *mut MeshCPU_C, static_draw: c_int); // upload mesh from (cpu) to (gpu)
}

impl MeshVertex {
    /// Creates a new `MeshVertex` object with default values.
    /// 
    /// # Examples
    /// 
    /// ```
    /// let vertex = cgl_rs::graphics::MeshVertex::new();
    /// ```
    pub fn new() -> MeshVertex {
        MeshVertex {
            position: Vector4::new(0.0, 0.0, 0.0, 0.0),
            normal: Vector4::new(0.0, 0.0, 0.0, 0.0),
            texture_coordinates: Vector4::new(0.0, 0.0, 0.0, 0.0),
            bone_weights: Vector4::new(0.0, 0.0, 0.0, 0.0),
            bone_ids: IVector4::new(0, 0, 0, 0)
        }
    }
}

impl MeshCPU {

    fn from_handle(handle: *mut MeshCPU_C) -> Result<MeshCPU, &'static str> {
        if handle.is_null() {
            Err("Failed to create MeshCPU_C")
        } else {
            Ok(MeshCPU {
                handle: handle,
                has_been_destroyed: false
            })
        }
    }

    /// Create a new MeshCPU object with the given vertex and index count
    /// 
    /// # Arguments
    /// 
    /// * `vertex_count` - The number of vertices in the mesh
    /// * `index_count` - The number of indices in the mesh
    /// 
    /// # Returns
    /// 
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn new(vertex_count: usize, index_count: usize) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_create(vertex_count, index_count) })
    }

    /// Loads a mesh from an OBJ file
    /// 
    /// # Arguments
    /// 
    /// * `path` - The path to the OBJ file
    /// 
    /// # Returns
    /// 
    /// * `Result<MeshCPU, &'static str>` - The loaded MeshCPU object, or an error message
    /// 
    /// # Example
    /// 
    /// ```no_run
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::load_obj("path/to/mesh.obj").unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn load_obj(path: &str) -> Result<MeshCPU, &'static str> {
        let c_path = std::ffi::CString::new(path).unwrap();
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_load_obj(c_path.as_ptr()) })
    }

    /// Creates a new MeshCPU object with a single triangle
    /// 
    /// # Arguments
    /// 
    /// * `a` - The first vertex of the triangle
    /// * `b` - The second vertex of the triangle
    /// * `c` - The third vertex of the triangle
    /// 
    /// # Returns
    /// 
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    /// 
    /// # Example
    /// 
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::triangle(
    ///         Vector3::new(0.0, 0.0, 0.0),
    ///         Vector3::new(1.0, 0.0, 0.0),
    ///         Vector3::new(0.0, 1.0, 0.0)
    ///     ).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn triangle(a: Vector3, b: Vector3, c: Vector3) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_triangle(a, b, c) })
    }

    /// Creates a new MeshCPU object representing a plane with the given front and right vectors, resolution, and scale.
    ///
    /// # Arguments
    ///
    /// * `front` - The front vector of the plane
    /// * `right` - The right vector of the plane
    /// * `resolution` - The number of subdivisions along each axis of the plane
    /// * `scale` - The scale of the plane
    ///
    /// # Returns
    ///
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::plane(
    ///       Vector3::new(0.0, 0.0, 1.0),
    ///       Vector3::new(1.0, 0.0, 0.0),
    ///       10,
    ///       1.0
    ///    ).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn plane(front: Vector3, right: Vector3, resolution: i32, scale: f32) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_plane(front, right, resolution, scale) })
    }

    /// Creates a new MeshCPU object representing a quad with the given vertices.
    ///
    /// # Arguments
    ///
    /// * `a` - The first vertex of the quad
    /// * `b` - The second vertex of the quad
    /// * `c` - The third vertex of the quad
    /// * `d` - The fourth vertex of the quad
    ///
    /// # Returns
    ///
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::quad(
    ///       Vector3::new(-1.0, -1.0, 0.0),
    ///       Vector3::new(1.0, -1.0, 0.0),
    ///       Vector3::new(1.0, 1.0, 0.0),
    ///       Vector3::new(-1.0, 1.0, 0.0)
    ///    ).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn quad(a: Vector3, b: Vector3, c: Vector3, d: Vector3) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_quad(a, b, c, d) })
    }

    /// Creates a new MeshCPU object representing a cube.
    ///
    /// # Arguments
    ///
    /// * `use_3d_tex_coords` - Whether to use 3D texture coordinates
    ///
    /// # Returns
    ///
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::cube(true).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn cube(use_3d_tex_coords: bool) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_cube(use_3d_tex_coords as i32) })
    }

    /// Creates a new MeshCPU object representing a sphere.
    ///
    /// # Arguments
    ///
    /// * `res_u` - The number of subdivisions along the u-axis of the sphere
    /// * `res_v` - The number of subdivisions along the v-axis of the sphere
    ///
    /// # Returns
    ///
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::sphere(10, 10).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn sphere(res_u: i32, res_v: i32) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_sphere(res_u, res_v) })
    }

    /// Creates a new MeshCPU object representing a cylinder.
    ///
    /// # Arguments
    ///
    /// * `start` - The start position of the cylinder
    /// * `end` - The end position of the cylinder
    /// * `radius0` - The radius of the cylinder at the start position
    /// * `radius1` - The radius of the cylinder at the end position
    /// * `resolution` - The number of subdivisions around the cylinder
    ///
    /// # Returns
    ///
    /// * `Result<MeshCPU, &'static str>` - The new MeshCPU object, or an error message
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::cylinder(
    ///       Vector3::new(0.0, 0.0, 0.0),
    ///       Vector3::new(0.0, 1.0, 0.0),
    ///       1.0,
    ///       1.0,
    ///       10
    ///    ).unwrap();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn cylinder(start: Vector3, end: Vector3, radius0: f32, radius1: f32, resolution: i32) -> Result<MeshCPU, &'static str> {
        MeshCPU::from_handle(unsafe { CGL_mesh_cpu_create_cylinder(start, end, radius0, radius1, resolution) })
    }

    /// Adds the vertices and indices of another MeshCPU object to this MeshCPU object.
    ///
    /// # Arguments
    ///
    /// * `other` - The MeshCPU object to add to this MeshCPU object
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mesh1 = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    let mesh2 = cgl_rs::graphics::MeshCPU::sphere(3, 3).unwrap();
    ///    mesh1.add_mesh(&mesh2);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_mesh(&self, other: &MeshCPU) {
        unsafe {
            CGL_mesh_cpu_add_mesh(self.handle, other.handle);
        }
    }

    /// Adds the vertices and indices of a cube to this MeshCPU object.
    ///
    /// # Arguments
    ///
    /// * `use_3d_tex_coords` - Whether or not to use 3D texture coordinates
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.add_cube(true);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_cube(&self, use_3d_tex_coords: bool) {
        unsafe {
            CGL_mesh_cpu_add_cube(self.handle, use_3d_tex_coords as i32);
        }
    }

    /// Adds the vertices and indices of a sphere to this MeshCPU object.
    ///
    /// # Arguments
    ///
    /// * `res_u` - The number of subdivisions along the u-axis of the sphere
    /// * `res_v` - The number of subdivisions along the v-axis of the sphere
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.add_sphere(2, 2);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_sphere(&self, res_u: i32, res_v: i32) {
        unsafe {
            CGL_mesh_cpu_add_sphere(self.handle, res_u, res_v);
        }
    }

    /// Adds a triangle to this MeshCPU object.
    ///
    /// # Arguments
    ///
    /// * `a` - The first vertex of the triangle
    /// * `b` - The second vertex of the triangle
    /// * `c` - The third vertex of the triangle
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.add_triangle(Vector3::new(0.0, 0.0, 0.0), Vector3::new(1.0, 0.0, 0.0), Vector3::new(0.0, 1.0, 0.0));
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_triangle(&self, a: Vector3, b: Vector3, c: Vector3) {
        unsafe {
            CGL_mesh_cpu_add_triangle(self.handle, a, b, c);
        }
    }

    /// Adds a quad to this MeshCPU object.
    ///
    /// # Arguments
    ///
    /// * `a` - The first vertex of the quad
    /// * `b` - The second vertex of the quad
    /// * `c` - The third vertex of the quad
    /// * `d` - The fourth vertex of the quad
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.add_quad(Vector3::new(0.0, 0.0, 0.0), Vector3::new(1.0, 0.0, 0.0), Vector3::new(1.0, 1.0, 0.0), Vector3::new(0.0, 1.0, 0.0));
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_quad(&self, a: Vector3, b: Vector3, c: Vector3, d: Vector3) {
        unsafe {
            CGL_mesh_cpu_add_quad(self.handle, a, b, c, d);
        }
    }

    // Adds a cylinder to this MeshCPU object.
    ///
    /// # Arguments
    ///
    /// * `start` - The starting point of the cylinder
    /// * `end` - The ending point of the cylinder
    /// * `radius0` - The radius of the cylinder at the starting point
    /// * `radius1` - The radius of the cylinder at the ending point
    /// * `resolution` - The number of subdivisions around the circumference of the cylinder
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.add_cylinder(Vector3::new(0.0, 0.0, 0.0), Vector3::new(0.0, 1.0, 0.0), 0.5, 0.5, 10);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_cylinder(&self, start: Vector3, end: Vector3, radius0: f32, radius1: f32, resolution: i32) {
        unsafe {
            CGL_mesh_cpu_add_cylinder(self.handle, start, end, radius0, radius1, resolution);
        }
    }

    // Offsets the vertices of this MeshCPU object by a given vector.
    ///
    /// # Arguments
    ///
    /// * `offset` - The vector by which to offset the vertices
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.offset_vertices(Vector3::new(1.0, 0.0, 0.0));
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn offset_vertices(&self, offset: Vector3) {
        unsafe {
            CGL_mesh_cpu_offset_vertices(self.handle, offset);
        }
    }

    // Scales the vertices of this MeshCPU object by a given factor.
    ///
    /// # Arguments
    ///
    /// * `scale` - The factor by which to scale the vertices
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    mesh.scale_vertices(2.0);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn scale_vertices(&self, scale: f32) {
        unsafe {
            CGL_mesh_cpu_scale_vertices(self.handle, scale);
        }
    }

    // Transforms the vertices of this MeshCPU object by a given transformation matrix.
    ///
    /// # Arguments
    ///
    /// * `transform` - The transformation matrix by which to transform the vertices
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::*;
    /// cgl_rs::init();
    /// {
    ///    let mesh = cgl_rs::graphics::MeshCPU::new(100, 100).unwrap();
    ///    let transform = Matrix4x4::identity();
    ///    mesh.transform_vertices(&transform);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn transform_vertices(&self, transform: &Matrix4x4) {
        unsafe {
            CGL_mesh_cpu_transform_vertices(self.handle, transform);
        }
    }

    /// Destroy the MeshCPU object
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///    let mut mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///    mesh.destroy(); // or just let the mesh go out of scope
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_mesh_cpu_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

    /// Recalculates the normals of the mesh
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///     mesh.recalculate_normals();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn recalculate_normals(&self) {
        unsafe {
            CGL_mesh_cpu_recalculate_normals(self.handle);
        }
    }


    // Get the vertex at the specified index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the vertex to retrieve
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///     let vertex = mesh.get_vertex(0);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_vertex(&self, index: usize) -> &MeshVertex {
        unsafe {
            let vertex_array = self.handle.as_ref().unwrap().vertices;
            vertex_array.offset(index as isize).as_ref().unwrap()
        }
    }

    // Set the vertex at the specified index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the vertex to set
    /// * `vertex` - The vertex to set
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///     let vertex = cgl_rs::graphics::MeshVertex::new();
    ///     mesh.set_vertex(0, &vertex);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_vertex(&self, index: usize, vertex: &MeshVertex) {
        unsafe {
            let vertex_array = self.handle.as_ref().unwrap().vertices;
            let vertex_ptr = vertex_array.offset(index as isize);
            *vertex_ptr = *vertex;
        }
    }

    // Get the index at the specified index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the index to retrieve
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///     let index = mesh.get_index(0);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_index(&self, index: usize) -> u32 {
        unsafe {
            let index_array = self.handle.as_ref().unwrap().indices;
            *index_array.offset(index as isize)
        }
    }

    // Set the index at the specified index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the index to set
    /// * `value` - The value to set
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///     mesh.set_index(0, 1);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_index(&self, index: usize, value: u32) {
        unsafe {
            let index_array = self.handle.as_ref().unwrap().indices;
            let index_ptr = index_array.offset(index as isize);
            *index_ptr = value;
        }
    }


}

impl Drop for MeshCPU {
    fn drop(&mut self) {
        self.destroy();
    }
}

impl Clone for MeshCPU {
    fn clone(&self) -> Self {
        MeshCPU {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}


impl MeshGPU {

    /// Creates a new MeshGPU instance.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing a `MeshGPU` instance if successful, or a `&'static str` error message if unsuccessful.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::window::Window::new("Test Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshGPU::new().unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn new() -> Result<MeshGPU, &'static str> {
        let handle = unsafe {
            CGL_mesh_gpu_create()
        };
        if handle.is_null() {
            Err("Failed to create MeshGPU")
        } else {
            Ok(MeshGPU {
                handle: handle,
                has_been_destroyed: false
            })
        }
    }


    /// Renders the mesh using the GPU.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::window::Window::new("Test Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshGPU::new().unwrap();
    ///     mesh.render();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn render(&self) {
        unsafe {
            CGL_mesh_gpu_render(self.handle);
        }
    }

    /// Renders the mesh using the GPU with instancing.
    ///
    /// * `instance_count` - The number of instances to render.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::window::Window::new("Test Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mesh = cgl_rs::graphics::MeshGPU::new().unwrap();
    ///     mesh.render_instanced(10);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn render_instanced(&self, instance_count: u32) {
        unsafe {
            CGL_mesh_gpu_render_instanced(self.handle, instance_count);
        }
    }

    /// Uploads the mesh data to the GPU.
    ///
    /// * `mesh_cpu` - The mesh data to upload.
    /// * `static_draw` - Whether the mesh data is static or dynamic.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::window::Window::new("Test Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut mesh_cpu = cgl_rs::graphics::MeshCPU::new(3, 3).unwrap();
    ///     let mut mesh_gpu = cgl_rs::graphics::MeshGPU::new().unwrap();
    ///     mesh_gpu.upload(&mesh_cpu, true);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn upload(&mut self, mesh_cpu: &MeshCPU, static_draw: bool) {
        unsafe {
            CGL_mesh_gpu_upload(self.handle, mesh_cpu.handle, static_draw as i32);
        }
    }

    /// Destroys the mesh GPU handle if it has not already been destroyed.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::window::Window::new("Test Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut mesh = cgl_rs::graphics::MeshGPU::new().unwrap();
    ///     mesh.destroy();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_mesh_gpu_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

}

impl Drop for MeshGPU {
    fn drop(&mut self) {
        self.destroy();
    }
}

impl Clone for MeshGPU {
    fn clone(&self) -> Self {
        MeshGPU {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}