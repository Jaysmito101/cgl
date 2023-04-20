from libcpp cimport bool
from libc.stdint cimport uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, uintptr_t, intptr_t
cimport libc.stdlib as stdlib
cimport libc.math as math
import copy
from typing import Callable
from PIL import Image
import PIL
import os
import random

# CGL typedefs of standard types

cdef extern from "cgl.h":
    ctypedef unsigned char CGL_ubyte;
    ctypedef unsigned short CGL_ushort;
    ctypedef unsigned int CGL_uint;
    ctypedef unsigned long CGL_ulong;
    ctypedef unsigned long long CGL_ulonglong;
    ctypedef char CGL_byte;
    ctypedef short CGL_short;
    ctypedef int CGL_int;
    ctypedef long CGL_long;
    ctypedef long long CGL_longlong;
    ctypedef float CGL_float;
    ctypedef double CGL_double;
    ctypedef long double CGL_longdouble;
    ctypedef uint64_t CGL_sizei;
    ctypedef bool CGL_bool;
    ctypedef void CGL_void;
    ctypedef CGL_int GLint;
    ctypedef CGL_uint GLuint;


    CGL_bool CGL_init()
    void CGL_shutdown()

    void CGL_utils_sleep(const CGL_sizei milliseconds)
    CGL_byte* CGL_utils_read_file(const char* path, CGL_sizei* size)
    CGL_sizei CGL_utils_get_file_size(const CGL_byte* path)
    CGL_bool CGL_utils_append_file(const CGL_byte* path, const CGL_byte* data, size_t size)
    CGL_bool CGL_utils_write_file(const CGL_byte* path, const CGL_byte* data, size_t size)
    CGL_float CGL_utils_get_time()
    void CGL_utils_get_timestamp(char* buffer)
    CGL_bool CGL_utils_is_little_endian()
    CGL_sizei CGL_utils_get_random_with_probability(CGL_float* probabilities, CGL_sizei count)
    void CGL_utils_reverse_bytes(void* data, size_t size)
    void CGL_utils_little_endian_to_current(void* data, size_t size)
    void CGL_utils_big_endian_to_current(void* data, size_t size)
    void CGL_utils_fast_srand(CGL_int seed)
    CGL_int CGL_utils_fast_rand()
    CGL_ulong CGL_utils_xorshf96()
    void CGL_utils_srand31(CGL_uint seed)
    CGL_uint CGL_utils_rand31()

    CGL_float CGL_utils_sigmoid(CGL_float x)
    CGL_float CGL_utils_sigmoid_derivative(CGL_float x)
    CGL_float CGL_utils_relu(CGL_float x)
    CGL_float CGL_utils_relu_derivative(CGL_float x)
    CGL_float CGL_utils_tanh(CGL_float x)
    CGL_float CGL_utils_tanh_derivative(CGL_float x)
    CGL_float CGL_utils_step(CGL_float x)
    CGL_float CGL_utils_step_derivative(CGL_float x)
    CGL_float CGL_utils_relu_leaky(CGL_float x)
    CGL_float CGL_utils_relu_leaky_derivative(CGL_float x)
    CGL_float CGL_utils_relu_smooth(CGL_float x)
    CGL_float CGL_utils_relu_smooth_derivative(CGL_float x)

    void CGL_console_set_color(uint8_t color)
    void CGL_printf_red(const char* format, ...)
    void CGL_printf_green(const char* format, ...)
    void CGL_printf_gray(const char* format, ...)
    void CGL_printf_blue(const char* format, ...)
    void CGL_console_progress_bar(CGL_float progress, CGL_int width, CGL_byte* prefix, CGL_byte* suffix, CGL_byte complete_char, CGL_byte incomplete_char)

    cdef struct CGL_logger_context:
        pass

    void CGL_logger_init(bool enable_console_logging);
    void CGL_logger_shutdown();
    bool CGL_logger_attach_log_file(const char* path);
    bool CGL_logger_detach_log_file(const char* path);
    void CGL_logger_flush();
    void CGL_logger_disable_console_logging();
    void CGL_logger_enable_console_logging();
    void CGL_logger_log(CGL_int level, const char* log_format, ...);

    uint32_t CGL_utils_crc32(const void* data, size_t size)
    uint64_t CGL_utils_crc64(const void* data, size_t size)
    void CGL_utils_rot13(const char* data_in, char* data_out)
    uint32_t CGL_utils_super_fast_hash(const void* data, size_t size)

    cdef struct CGL_vec2:
        CGL_float x
        CGL_float y

    cdef struct CGL_vec3:
        CGL_float x
        CGL_float y
        CGL_float z

    cdef struct CGL_vec4:
        CGL_float x
        CGL_float y
        CGL_float z
        CGL_float w
    ctypedef CGL_vec4 CGL_color

    cdef struct CGL_ivec4:
        CGL_int x
        CGL_int y
        CGL_int z
        CGL_int w

    cdef struct CGL_mat3:
        CGL_float m[9]

    cdef struct CGL_mat4:
        CGL_float m[16]

    cdef struct CGL_quat:
        CGL_float x
        CGL_float y
        CGL_float z
        CGL_float w

    cdef CGL_float CGL_float_quadratic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float t)
    cdef CGL_float CGL_float_cubic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float d, CGL_float t)

    # cdef enum GLenum:
    #     pass
    ctypedef CGL_uint GLenum

    CGL_vec2 CGL_vec2_triple_product(CGL_vec2 a, CGL_vec2 b, CGL_vec2 c)
    CGL_vec3 CGL_vec3_reflect(CGL_vec3 a, CGL_vec3 n)
    CGL_vec3 CGL_vec3_rotate_about_axis(CGL_vec3 v, CGL_vec3 axis, CGL_float theta)
    CGL_vec3 CGL_vec3_triple_product(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
    void CGL_vec3_calculate_orthonormal_basis_from_one_vector(CGL_vec3 a, CGL_vec3* pb, CGL_vec3* pc)
    CGL_vec4 CGL_vec4_triple_product(CGL_vec4 a, CGL_vec4 b, CGL_vec4 c)

    CGL_mat4 CGL_mat4_mul(CGL_mat4 a, CGL_mat4 b)
    CGL_float CGL_mat4_det(CGL_mat4 m)
    CGL_float CGL_mat4_det_by_lu(CGL_mat4 m)
    CGL_float CGL_mat4_det_by_gauss(CGL_mat4 m)
    CGL_vec4 CGL_mat4_mul_vec4(CGL_mat4 m, CGL_vec4 v)
    CGL_mat4 CGL_mat4_inverse(CGL_mat4 m)
    CGL_mat4 CGL_mat4_transpose(CGL_mat4 m)
    CGL_mat4 CGL_mat4_adjoint(CGL_mat4 m)
    CGL_mat4 CGL_mat4_gauss_elim(CGL_mat4 m)
    CGL_int CGL_mat4_rank(CGL_mat4 m)
    CGL_float CGL_mat4_trace(CGL_mat4 m)
    CGL_mat3 CGL_mat4_to_mat3(CGL_mat4 m)
    CGL_mat4 CGL_mat4_from_mat3(CGL_mat3 m)
    CGL_mat4 CGL_mat4_rotate_about_axis(CGL_vec3 axis, CGL_float angle)
    CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up)
    CGL_mat4 CGL_mat4_lerp(CGL_mat4 a, CGL_mat4 b, CGL_float t)
    void CGL_mat4_decompose_lu(CGL_mat4 m, CGL_mat4* l, CGL_mat4* u)
    
    CGL_mat4 CGL_mat4_perspective(CGL_float fov, CGL_float aspect, CGL_float near, CGL_float far)
    CGL_mat4 CGL_mat4_orthographic(CGL_float left, CGL_float right, CGL_float bottom, CGL_float top, CGL_float near, CGL_float far)
    CGL_mat4 CGL_mat4_zero()
    CGL_mat4 CGL_mat4_identity()
    CGL_mat4 CGL_mat4_scale(CGL_float x, CGL_float y, CGL_float z)
    CGL_mat4 CGL_mat4_translate(CGL_float x, CGL_float y, CGL_float z)
    CGL_mat4 CGL_mat4_rotate_x(CGL_float angle)
    CGL_mat4 CGL_mat4_rotate_y(CGL_float angle)
    CGL_mat4 CGL_mat4_rotate_z(CGL_float angle)

    cdef struct CGL_shape:
        CGL_vec3* vertices
        CGL_vec3 position
        CGL_vec3 rotation
        CGL_vec3 scale
        size_t vertices_count

    void CGL_shape_init(CGL_shape* shape, size_t vertices_count)
    void CGL_shape_destroy(CGL_shape* shape)

    cdef struct CGL_transform:
        CGL_vec4 position
        CGL_vec4 rotation
        CGL_vec4 scale
        CGL_mat4 matrix
        CGL_transform* parent
    
    
    bool CGL_sat_collision_overlap_on_axis(CGL_shape* a, CGL_shape* b, CGL_vec2 axis, float* overlap_amount)
    bool CGL_sat_collision_detect(CGL_shape* a, CGL_shape* b, CGL_vec2* n_vector)
    void CGL_sat_collision_calculate_axes(CGL_shape* shape, CGL_vec2* axes, CGL_int* axes_count)
    CGL_bool CGL_utils_is_point_in_triangle(CGL_vec2 p, CGL_vec2 a, CGL_vec2 b, CGL_vec2 c)
    CGL_bool CGL_utils_is_point_in_circle(CGL_vec2 p, CGL_float r)
    CGL_bool CGL_utils_calculate_circumcircle(CGL_vec2 a, CGL_vec2 b, CGL_vec2 c, CGL_vec2* center, CGL_float* radius)
    CGL_bool CGL_utils_calculate_super_triangle(CGL_vec2* points, CGL_int points_count, CGL_vec2* a, CGL_vec2* b, CGL_vec2* c, CGL_float padding)
    CGL_bool CGL_utils_calculate_bounding_square(CGL_vec2* points, CGL_int points_count, CGL_vec2* a, CGL_vec2* b, CGL_vec2* c, CGL_vec2* d, CGL_float padding)
    CGL_bool CGL_utils_calculate_bounding_box(CGL_vec2* points, CGL_int points_count, CGL_vec2* a, CGL_vec2* b, CGL_vec2* c, CGL_vec2* d, CGL_float padding)
    CGL_float CGL_utils_random_gaussian(CGL_float mean, CGL_float std_dev)
    CGL_vec3 CGL_gjk_shape_default_support(CGL_shape* a, CGL_vec3 d)
    CGL_vec3 CGL_gjk_default_support(CGL_shape* a, CGL_shape* b, CGL_vec3 d)
    CGL_bool CGL_gjk_check_collision_2d(CGL_shape* a, CGL_shape* b, CGL_vec3* simplex_out)
    CGL_vec3 CGL_gjk_epa_2d(CGL_shape* a, CGL_shape* b, CGL_vec3* simplex)
    CGL_bool CGL_aabb_contains_point(CGL_vec2 aabb_min, CGL_vec2 aabb_max, CGL_vec2 point)
    CGL_bool CGL_aabb_intersects_aabb(CGL_vec2 aabb_min, CGL_vec2 aabb_max, CGL_vec2 aabb_min2, CGL_vec2 aabb_max2)
    CGL_bool CGL_triangulate_points_incremental(CGL_vec2* points, CGL_int points_count, CGL_int* triangles_out, CGL_int* triangles_count_out)


    CGL_transform CGL_transform_create_empty()
    CGL_transform CGL_transform_create(CGL_vec3 position, CGL_vec3 rotation, CGL_vec3 scale)
    CGL_transform CGL_transform_create_from_matrix(CGL_mat4 matrix)
    CGL_transform* CGL_transform_set_position(CGL_transform* transform, CGL_vec3 position)
    CGL_transform* CGL_transform_set_rotation(CGL_transform* transform, CGL_vec3 rotation)
    CGL_transform* CGL_transform_set_scale(CGL_transform* transform, CGL_vec3 scale)
    CGL_transform* CGL_transform_set_parent(CGL_transform* transform, CGL_transform* parent)
    CGL_transform* CGL_transform_update(CGL_transform* transform)
    CGL_transform* CGL_transform_update_matrix_local(CGL_transform* transform)
    CGL_mat4 CGL_transform_get_matrix(CGL_transform* transform)
    CGL_mat4* CGL_transform_get_matrix_ptr(CGL_transform* transform)

    cdef struct CGL_window:
        pass
    
    cdef struct GLFWwindow:
        pass
    
    ctypedef void(*CGL_window_key_callback)(CGL_window* window, CGL_int key, CGL_int scancode, CGL_int action, CGL_int mods)
    ctypedef void(*CGL_window_mouse_button_callback)(CGL_window* window, CGL_int button, CGL_int action, CGL_int mods)
    ctypedef void(*CGL_window_mouse_position_callback)(CGL_window* window, double xpos, double ypos)
    ctypedef void(*CGL_window_mouse_scroll_callback)(CGL_window* window, double xoffset, double yoffset)
    ctypedef void(*CGL_window_framebuffer_size_callback)(CGL_window* window, CGL_int width, CGL_int height)
    ctypedef void(*CGL_window_close_callback)(CGL_window* window)
    ctypedef void(*CGL_window_drag_n_drop_callback)(CGL_window* window, const CGL_byte** paths, CGL_int count)

    CGL_window* CGL_window_create(CGL_int width, CGL_int height, const char* title)
    CGL_window* CGL_window_create_undecorated(CGL_int width, CGL_int height, const char* title)
    void CGL_window_destroy(CGL_window* window)
    void CGL_window_poll_events(CGL_window* window)
    void CGL_window_swap_buffers(CGL_window* window)
    bool CGL_window_should_close(CGL_window* window)
    void CGL_window_set_title(CGL_window* window, const char* title)
    void CGL_window_set_size(CGL_window* window, CGL_int width, CGL_int height)
    void CGL_window_set_position(CGL_window* window, CGL_int x, CGL_int y)
    void CGL_window_set_hidden(CGL_window* window, bool hidden)
    void CGL_window_set_user_data(CGL_window* window, void* user_data)
    void* CGL_window_get_user_data(CGL_window* window)
    void CGL_window_get_size(CGL_window* window, CGL_int* width, CGL_int* height)
    void CGL_window_get_position(CGL_window* window, CGL_int* x, CGL_int* y)
    void CGL_window_get_framebuffer_size(CGL_window* window, CGL_int* width, CGL_int* height)
    void CGL_window_set_key_callback(CGL_window* window, CGL_window_key_callback callback)
    void CGL_window_set_mouse_button_callback(CGL_window* window, CGL_window_mouse_button_callback callback)
    void CGL_window_set_mouse_position_callback(CGL_window* window, CGL_window_mouse_position_callback callback)
    void CGL_window_set_mouse_scroll_callback(CGL_window* window, CGL_window_mouse_scroll_callback callback)
    void CGL_window_set_framebuffer_size_callback(CGL_window* window, CGL_window_framebuffer_size_callback callback)
    void CGL_window_set_close_callback(CGL_window* window, CGL_window_close_callback callback)
    void CGL_window_set_drag_n_drop_callback(CGL_window* window, CGL_window_drag_n_drop_callback callback)
    void CGL_window_resecure_callbacks(CGL_window* window)
    void CGL_window_make_context_current(CGL_window* window)
    GLFWwindow* CGL_window_get_glfw_handle(CGL_window* window)
    CGL_int CGL_window_get_key(CGL_window* window, CGL_int key)
    CGL_bool CGL_window_is_key_pressed(CGL_window* window, CGL_int key)
    CGL_int CGL_window_get_mouse_button(CGL_window* window, CGL_int button)
    void CGL_window_get_mouse_position(CGL_window* window, double* xpos, double* ypos)

    cdef struct CGL_image:
        void* data
        CGL_int height
        CGL_int width
        CGL_int bytes_per_channel
        CGL_int channels

    cdef struct CGL_texture:
        pass
    
    cdef struct CGL_framebuffer:
        pass
    
    cdef struct CGL_shader:
        pass
    
    cdef struct CGL_mesh_gpu:
        pass

    cdef struct CGL_mesh_vertex:
        CGL_vec4 position
        CGL_vec4 normal
        CGL_vec4 texture_coordinates
        CGL_vec4 bone_wieghts
        CGL_ivec4 bone_ids
    
    cdef struct CGL_mesh_cpu:
        CGL_sizei index_count
        CGL_sizei index_count_used
        CGL_uint* indices
        CGL_sizei vertex_count
        CGL_sizei vertex_count_used
        CGL_mesh_vertex* vertices
        
    
    cdef struct CGL_ssbo:
        pass
    
    cdef struct CGL_ubo:
        pass
    
    CGL_mesh_cpu* CGL_mesh_cpu_create(size_t vertex_count, size_t index_count)
    CGL_mesh_cpu* CGL_mesh_cpu_recalculate_normals(CGL_mesh_cpu* mesh)
    CGL_mesh_cpu* CGL_mesh_cpu_load_obj(const char* path)
    CGL_mesh_cpu* CGL_mesh_cpu_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
    CGL_mesh_cpu* CGL_mesh_cpu_plane(CGL_vec3 front, CGL_vec3 right, CGL_int resolution, CGL_float scale)
    CGL_mesh_cpu* CGL_mesh_cpu_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
    CGL_mesh_cpu* CGL_mesh_cpu_cube(CGL_bool use_3d_tex_coords)
    CGL_mesh_cpu* CGL_mesh_cpu_sphere(CGL_int res_u, CGL_int res_v)
    CGL_mesh_cpu* CGL_mesh_cpu_create_cylinder(CGL_vec3 start, CGL_vec3 end, CGL_float radius0, CGL_float radius1, CGL_int resolution)
    CGL_mesh_cpu* CGL_mesh_cpu_add_mesh(CGL_mesh_cpu* mesh, CGL_mesh_cpu* mesh_other)
    CGL_mesh_cpu* CGL_mesh_cpu_add_cube(CGL_mesh_cpu* mesh, CGL_bool use_3d_tex_coords)
    CGL_mesh_cpu* CGL_mesh_cpu_add_triangle(CGL_mesh_cpu* mesh, CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
    CGL_mesh_cpu* CGL_mesh_cpu_add_quad(CGL_mesh_cpu* mesh, CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
    CGL_mesh_cpu* CGL_mesh_cpu_add_sphere(CGL_mesh_cpu* mesh, CGL_int res_u, CGL_int res_v)
    CGL_mesh_cpu* CGL_mesh_cpu_add_cylinder(CGL_mesh_cpu* mesh, CGL_vec3 start, CGL_vec3 end, CGL_float radius0, CGL_float radius1, CGL_int resolution)
    CGL_mesh_cpu* CGL_mesh_cpu_offset_vertices(CGL_mesh_cpu* mesh, CGL_vec3 offset)
    CGL_mesh_cpu* CGL_mesh_cpu_scale_vertices(CGL_mesh_cpu* mesh, CGL_float scale)
    CGL_mesh_cpu* CGL_mesh_cpu_rotate_vertices(CGL_mesh_cpu* mesh, CGL_quat rotation)
    CGL_mesh_cpu* CGL_mesh_cpu_transform_vertices(CGL_mesh_cpu* mesh, CGL_mat4 transform)
    void CGL_mesh_cpu_generate_c_initialization_code(CGL_mesh_cpu* mesh, char* buffer, const char* function_name)
    void CGL_mesh_cpu_destroy(CGL_mesh_cpu* mesh)

    CGL_texture* CGL_texture_create(CGL_image* image)
    CGL_texture* CGL_texture_create_blank(CGL_int width, CGL_int height, GLenum format, GLenum internal_format, GLenum type)
    CGL_texture* CGL_texture_create_array(CGL_int width, CGL_int height, CGL_int layers, GLenum format, GLenum internal_format, GLenum type)
    CGL_texture* CGL_texture_create_3d(CGL_int width, CGL_int height, CGL_int depth, GLenum format, GLenum internal_format, GLenum type)
    CGL_texture* CGL_texture_create_cubemap()
    void CGL_texture_cubemap_set_face(CGL_texture* texture, CGL_int face, CGL_image* image)
    void CGL_texture_array_set_layer_data(CGL_texture* texture, CGL_int layer, void* data)
    void CGL_texture_destroy(CGL_texture* texture)
    void CGL_texture_bind(CGL_texture* texture, CGL_int unit)
    void CGL_texture_set_data(CGL_texture* texture, void* data)
    void CGL_texture_set_sub_data(CGL_texture* texture, size_t offset_x, size_t offset_y, size_t size_x, size_t size_y,  void* data)
    void CGL_texture_set_pixel_data(CGL_texture* texture, int x, int y, void* data)
    void CGL_texture_set_user_data(CGL_texture* texture, void* user_data)
    void* CGL_texture_get_user_data(CGL_texture* texture)
    CGL_uint CGL_texture_get_internal_handle(CGL_texture* texture)
    void CGL_texture_get_size(CGL_texture* texture, CGL_int* width, CGL_int* height)
    void CGL_texture_set_scaling_method(CGL_texture* texture, GLint method)
    void CGL_texture_set_wrapping_method(CGL_texture* texture, GLint method)

    CGL_framebuffer* CGL_framebuffer_create_from_default(CGL_window* window)
    CGL_framebuffer* CGL_framebuffer_create(CGL_int width, CGL_int height)
    CGL_framebuffer* CGL_framebuffer_create_basic(CGL_int width, CGL_int height)
    void CGL_framebuffer_add_color_attachment(CGL_framebuffer* framebuffer, CGL_texture* texture)
    void CGL_framebuffer_destroy(CGL_framebuffer* framebuffer)
    CGL_texture* CGL_framebuffer_get_color_attachment(CGL_framebuffer* framebuffer, CGL_int index)
    void CGL_framebuffer_bind(CGL_framebuffer* framebuffer)
    void CGL_framebuffer_get_size(CGL_framebuffer* framebuffer, CGL_int* width, CGL_int* height)
    void CGL_framebuffer_set_user_data(CGL_framebuffer* framebuffer, void* user_data)
    void* CGL_framebuffer_get_user_data(CGL_framebuffer* framebuffer)
    void CGL_framebuffer_read_pixels(CGL_framebuffer* framebuffer, CGL_int x, CGL_int y, CGL_int width, CGL_int height, void* pixels)
    CGL_int CGL_framebuffer_get_mouse_pick_id(CGL_framebuffer* framebuffer, CGL_int x, CGL_int y, CGL_int index)
    CGL_texture* CGL_framebuffer_get_color_texture(CGL_framebuffer* framebuffer)
    CGL_texture* CGL_framebuffer_get_depth_texture(CGL_framebuffer* framebuffer)


    void CGL_gl_clear(CGL_float r, CGL_float g, CGL_float b, CGL_float a)
    bool CGL_gl_init()
    void CGL_gl_shutdown()
    void CGL_gl_render_screen_quad()

    
    CGL_mesh_gpu* CGL_mesh_gpu_create()
    void CGL_mesh_gpu_destroy(CGL_mesh_gpu* mesh)
    void CGL_mesh_gpu_render(CGL_mesh_gpu* mesh)
    void CGL_mesh_gpu_render_instanced(CGL_mesh_gpu* mesh, uint32_t count)
    void CGL_mesh_gpu_set_user_data(CGL_mesh_gpu* mesh, void* user_data) 
    void* CGL_mesh_gpu_get_user_data(CGL_mesh_gpu* mesh)
    void CGL_mesh_gpu_upload(CGL_mesh_gpu* mesh, CGL_mesh_cpu* mesh_cpu, bool static_draw)

    CGL_shader* CGL_shader_create(const char* vertex_shader_source, const char* fragment_shader_source, char** error)
    CGL_shader* CGL_shader_compute_create(const char* compute_shader_source, char** error)
    CGL_shader* CGL_shader_create_from_files(const char* vertex_shader_file, const char* fragment_shader_file, char** error)
    CGL_shader* CGL_shader_compute_create_from_files(const char* compute_shader_file, char** error)
    void CGL_shader_destroy(CGL_shader* shader)
    void CGL_shader_bind(CGL_shader* shader)
    int CGL_shader_get_uniform_location(CGL_shader* shader, const char* name)
    void CGL_shader_set_uniform_mat4(CGL_shader* shader, CGL_int location, CGL_mat4* matrix)
    void CGL_shader_set_uniform_vec4(CGL_shader* shader, CGL_int location, CGL_vec4* vector)
    void CGL_shader_set_uniform_vec3(CGL_shader* shader, CGL_int location, CGL_vec3* vector)
    void CGL_shader_set_uniform_vec2(CGL_shader* shader, CGL_int location, CGL_vec2* vector)
    void CGL_shader_set_uniform_int(CGL_shader* shader, CGL_int location, CGL_int value)
    void CGL_shader_set_uniform_bool(CGL_shader* shader, CGL_int location, bool value)
    void CGL_shader_set_uniform_float(CGL_shader* shader, CGL_int location, CGL_float value)
    void CGL_shader_set_uniform_double(CGL_shader* shader, CGL_int location, CGL_double value)
    void CGL_shader_set_uniform_vec2v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y)
    void CGL_shader_set_uniform_vec3v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y, CGL_float z)
    void CGL_shader_set_uniform_vec4v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y, CGL_float z, CGL_float w)
    void CGL_shader_set_uniform_ivec2v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y)
    void CGL_shader_set_uniform_ivec3v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y, CGL_int z)
    void CGL_shader_set_uniform_ivec4v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y, CGL_int z, CGL_int w)
    void CGL_shader_set_user_data(CGL_shader* shader, void* user_data)
    void* CGL_shader_get_user_data(CGL_shader* shader)
    void CGL_shader_compute_dispatch(CGL_shader* shader, CGL_int x, CGL_int y, CGL_int z)

    CGL_ssbo* CGL_ssbo_create(uint32_t binding)
    void CGL_ssbo_destroy(CGL_ssbo* ssbo)
    void CGL_ssbo_bind(CGL_ssbo* ssbo)
    void CGL_ssbo_bind2(CGL_ssbo* ssbo, uint32_t binding)
    void CGL_ssbo_set_data(CGL_ssbo* ssbo, size_t size, void* data, bool static_draw)
    void CGL_ssbo_set_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data, bool static_draw)
    void CGL_ssbo_get_data(CGL_ssbo* ssbo, size_t* size, void* data)
    void CGL_ssbo_get_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data)
    void CGL_ssbo_set_user_data(CGL_ssbo* ssbo, void* user_data)
    void* CGL_ssbo_get_user_data(CGL_ssbo* ssbo)
    size_t CGL_ssbo_get_size(CGL_ssbo* ssbo)
    void CGL_ssbo_copy(CGL_ssbo* dst, CGL_ssbo* src, size_t src_offset, size_t dst_offset, size_t size)

    CGL_ubo* CGL_ubo_create()
    void CGL_ubo_destroy(CGL_ubo* ubo)
    void CGL_ubo_bind(CGL_ubo* ubo, CGL_shader* shader, const CGL_byte* name, uint32_t binding)
    void CGL_ubo_set_data(CGL_ubo* ubo, size_t size, void* data, bool static_draw)
    void CGL_ubo_set_sub_data(CGL_ubo* ubo, size_t offset, size_t size, void* data, bool static_draw)
    void CGL_ubo_get_data(CGL_ubo* ubo, size_t* size, void* data)
    void CGL_ubo_get_sub_data(CGL_ubo* ubo, size_t offset, size_t size, void* data)
    void CGL_ubo_set_user_data(CGL_ubo* ubo, void* user_data)
    void* CGL_ubo_get_user_data(CGL_ubo* ubo)
    size_t CGL_ubo_get_size(CGL_ubo* ubo)

    cdef struct CGL_tilemap:
        pass
    
    cdef struct CGL_tile:
        pass
    
    CGL_tilemap* CGL_tilemap_create(uint32_t tile_count_x, uint32_t tile_count_y, uint32_t tile_size_x, uint32_t tile_size_y, uint32_t ssbo_binding)
    void CGL_tilemap_destroy(CGL_tilemap* tilemap)
    void CGL_tilemap_set_auto_upload(CGL_tilemap* tilemap, bool value)
    bool CGL_tilemap_get_auto_upload(CGL_tilemap* tilemap)
    bool CGL_tilemap_upload(CGL_tilemap* tilemap)
    void CGL_tilemap_set_tile_color(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, CGL_float r, CGL_float g, CGL_float b)
    void CGL_tilemap_set_tile_texture_from_array(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, uint32_t texture_index)
    void CGL_tilemap_set_tile_texture_from_tileset(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, CGL_float texture_x_min, CGL_float texture_y_min, CGL_float texture_x_max, CGL_float texture_y_max)
    void CGL_tilemap_set_all_tile_color(CGL_tilemap* tilemap, CGL_float r, CGL_float g, CGL_float b)
    void CGL_tilemap_set_all_tile_texture_from_array(CGL_tilemap* tilemap, uint32_t texture_index)
    void CGL_tilemap_set_all_tile_texture_from_tileset(CGL_tilemap* tilemap, CGL_float texture_x_min, CGL_float texture_y_min, CGL_float texture_x_max, CGL_float texture_y_max)
    void CGL_tilemap_clear_tile(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y)
    void CGL_tilemap_clear_all_tile(CGL_tilemap* tilemap)
    void CGL_tilemap_render(CGL_tilemap* tilemap, CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y, CGL_texture* texture)
    void CGL_tilemap_reset(CGL_tilemap* tilemap)

    cdef struct CGL_font_character:
        CGL_vec2 size
        CGL_vec2 normalized_size
        CGL_vec2 offset
        CGL_vec2 normalized_offset
        CGL_vec2 bearing
        CGL_vec2 bearing_normalized
        CGL_vec2 advance
        CGL_vec2 advance_normalized
        unsigned char* bitmap
        char ch
    
    cdef struct CGL_font:
        pass
    
    CGL_bool CGL_text_init()
    CGL_void CGL_text_shutdown()
    CGL_font* CGL_font_load(const char* path)
    CGL_font* CGL_font_load_from_memory(const char* data, CGL_sizei size)
    CGL_void CGL_font_destory(CGL_font* font)
    CGL_texture* CGL_font_get_atlas(CGL_font* font)
    CGL_bool CGL_font_build_atlas(CGL_font* font, size_t width, size_t height, size_t font_size)
    CGL_font_character* CGL_font_get_characters(CGL_font* font)
    CGL_texture* CGL_text_bake_to_texture(const char* string, size_t string_length, CGL_font* font, size_t* width, size_t* height)

    cdef struct CGL_widgets_context:
        pass    
    
    CGL_widgets_context* CGL_widgets_context_create(size_t max_vertices, size_t max_indices)
    void CGL_widgets_context_destory(CGL_widgets_context* context)
    CGL_widgets_context* CGL_window_get_current_context()
    CGL_bool CGL_widgets_init()
    void CGL_widgets_shutdown()
    void CGL_window_set_current_context(CGL_widgets_context* context)
    CGL_bool CGL_widgets_begin()
    CGL_bool CGL_widgets_begin_int(CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y)
    CGL_bool CGL_widgets_end()
    CGL_bool CGL_widgets_flush()
    CGL_bool CGL_widgets_flush_if_required()
    void CGL_widgets_add_vertex(CGL_mesh_vertex* vertex)
    void CGL_widgets_add_mesh(CGL_mesh_cpu* mesh)
    void CGL_widgets_add_vertex_p(CGL_vec3 position)
    void CGL_widgets_add_vertex_p3f(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z)
    void CGL_widgets_add_vertex_pt(CGL_vec3 position, CGL_vec2 tex_coord)
    void CGL_widgets_add_vertex_p3ft(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z, CGL_vec2 tex_coord)
    void CGL_widgets_add_vertex_pt2f(CGL_vec3 position, CGL_float tex_x, CGL_float tex_y)
    void CGL_widgets_add_vertex_p3ft2f(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z, CGL_float tex_x, CGL_float tex_y)
    void CGL_widgets_set_stroke_color(CGL_color color)
    void CGL_widgets_set_stroke_colorf(CGL_float r, CGL_float g, CGL_float b, CGL_float a)
    void CGL_widgets_set_stroke_thicnkess(CGL_float thickness)
    void CGL_widgets_set_fill_color(CGL_color color)
    void CGL_widgets_set_fill_colorf(CGL_float r, CGL_float g, CGL_float b, CGL_float a)
    void CGL_widgets_set_fill_mode(CGL_bool is_enabled)
    void CGL_widgets_set_projection_matrix(CGL_mat4* matrix)
    void CGL_widgets_enable_diffuse_shading(CGL_vec3 light_position, CGL_vec3 light_color)
    void CGL_widgets_disable_diffuse_shading()
    void CGL_widgets_set_view_matrix(CGL_mat4* matrix)
    void CGL_widgets_set_model_matrix(CGL_mat4* matrix)
    void CGL_widgets_set_texture(CGL_texture* texture)
    void CGL_widgets_set_font_texture(CGL_texture* texture)
    void CGL_widgets_set_texture_coordinate_so(CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y)
    void CGL_widgets_apply_transformations_on_cpu()
    void CGL_widgets_apply_transformations_on_gpu()
    void CGL_widgets_add_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
    void CGL_widgets_add_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
    void CGL_widgets_add_quad_8f(CGL_float ax, CGL_float ay, CGL_float bx, CGL_float by, CGL_float cx, CGL_float cy, CGL_float dx, CGL_float dy)
    void CGL_widgets_add_line(CGL_vec3 start, CGL_vec3 end)
    void CGL_widgets_add_line2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y)
    void CGL_widgets_add_rect(CGL_vec3 start, CGL_vec2 size)
    void CGL_widgets_add_rect2f(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y)
    void CGL_widgets_add_circle(CGL_vec3 position, CGL_float radius)
    void CGL_widgets_add_circle2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius)
    void CGL_widgets_add_circler(CGL_vec3 position, CGL_float radius, CGL_int res)
    void CGL_widgets_add_circle2fr(CGL_float pos_x, CGL_float pos_y, CGL_float radius, CGL_int res)
    void CGL_widgets_add_oval(CGL_vec3 position, CGL_vec2 radius)
    void CGL_widgets_add_oval2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius_x, CGL_float radius_y)
    void CGL_widgets_add_arc2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius, CGL_float start_angle, CGL_float end_angle, CGL_int resolution)
    CGL_bool CGL_widgets_add_character(char c, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy)
    CGL_bool CGL_widgets_add_string(const char* str, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy)
    void CGL_widgets_add_cubic_bazier(CGL_vec3 start, CGL_vec3 end, CGL_vec3 control_1, CGL_vec3 control_2, CGL_int resolution)
    void CGL_widgets_add_cubic_bazier2v(CGL_vec2 start, CGL_vec2 end, CGL_vec2 control_1, CGL_vec2 control_2, CGL_int resolution)
    void CGL_widgets_add_cubic_bazier2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float control_1_x, CGL_float control_1_y, CGL_float control_2_x, CGL_float control_2_y, CGL_int resolution)
    void CGL_widgets_add_cubic_bazier_points(CGL_vec3 start, CGL_vec3 end, CGL_vec3 control_1, CGL_vec3 control_2, CGL_int resolution)
    void CGL_widgets_add_cubic_bazier_points2v(CGL_vec2 start, CGL_vec2 end, CGL_vec2 control_1, CGL_vec2 control_2, CGL_int resolution)
    void CGL_widgets_add_cubic_bazier_points2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float control_1_x, CGL_float control_1_y, CGL_float control_2_x, CGL_float control_2_y, CGL_int resolution)
    void CGL_widgets_add_plot_function(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_float(*func_to_plot)(CGL_float), CGL_int num_samples, CGL_float x_min, CGL_float x_max, CGL_float y_min, CGL_float y_max, CGL_float plot_thickness, CGL_vec3 plot_color, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color)
    void CGL_widgets_add_plot_array(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_vec2* values, CGL_sizei count, CGL_float marker_size, CGL_vec3 marker_color, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color)
    void CGL_widgets_add_plot_pie_chart(CGL_float start_x, CGL_float start_y, CGL_float radius, CGL_float* values, CGL_vec3* colors, CGL_sizei count, CGL_int resolution)
    void CGL_widgets_add_bar_graph(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_float* values, CGL_vec3* colors, CGL_sizei count, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color, CGL_bool vertical)

    
    void CGL_widgets_add_shape_out_line(CGL_shape* shape)
    CGL_float CGL_widgets_add_string_with_font(const char* str, CGL_font* font, CGL_float x, CGL_float y, CGL_float sx, CGL_float scale_y)

    cdef struct CGL_matrix:
        pass
    
    # we use this as this is also in the build lib
    ctypedef CGL_float CGL_MATRIX_DATA_TYPE
    
    CGL_matrix* CGL_matrix_create(CGL_int m, CGL_int n)
    CGL_void CGL_matrix_destroy(CGL_matrix* m)
    CGL_matrix* CGL_matrix_create_from_array(CGL_MATRIX_DATA_TYPE* array, CGL_int m, CGL_int n)
    CGL_matrix* CGL_matrix_add_to(CGL_matrix* a, CGL_matrix* b, CGL_matrix* out)
    CGL_matrix* CGL_matrix_add(CGL_matrix* a, CGL_matrix* b)
    CGL_matrix* CGL_matrix_sub_to(CGL_matrix* a, CGL_matrix* b, CGL_matrix* out)
    CGL_matrix* CGL_matrix_sub(CGL_matrix* a, CGL_matrix* b)
    CGL_matrix* CGL_matrix_elem_mul_to(CGL_matrix* a, CGL_matrix* b, CGL_matrix* out)
    CGL_matrix* CGL_matrix_elem_mul(CGL_matrix* a, CGL_matrix* b)
    CGL_matrix* CGL_matrix_mul_to(CGL_matrix* a, CGL_matrix* b, CGL_matrix* out)
    CGL_matrix* CGL_matrix_mul(CGL_matrix* a, CGL_matrix* b)
    CGL_matrix* CGL_matrix_transpose_to(CGL_matrix* m, CGL_matrix* out)
    CGL_matrix* CGL_matrix_transpose(CGL_matrix* m)
    CGL_matrix* CGL_matrix_identity_to(CGL_int m, CGL_int n, CGL_matrix* out)
    CGL_matrix* CGL_matrix_identity(CGL_int m, CGL_int n)
    CGL_matrix* CGL_matrix_scale_to(CGL_matrix* m, CGL_MATRIX_DATA_TYPE s)
    CGL_matrix* CGL_matrix_scale(CGL_matrix* m, CGL_MATRIX_DATA_TYPE s)
    CGL_MATRIX_DATA_TYPE CGL_matrix_get_elem(CGL_matrix* m, CGL_int i, CGL_int j)
    CGL_bool CGL_matrix_set_elem(CGL_matrix* m, CGL_int i, CGL_int j, CGL_MATRIX_DATA_TYPE value)
    CGL_bool CGL_matrix_set_row(CGL_matrix* m, CGL_int i, CGL_MATRIX_DATA_TYPE* row)
    CGL_bool CGL_matrix_set_col(CGL_matrix* m, CGL_int j, CGL_MATRIX_DATA_TYPE* col)
    CGL_bool CGL_matrix_get_row(CGL_matrix* m, CGL_int i, CGL_MATRIX_DATA_TYPE* out)
    CGL_bool CGL_matrix_get_col(CGL_matrix* m, CGL_int j, CGL_MATRIX_DATA_TYPE* out)
    CGL_matrix* CGL_matrix_row_to_matrix_to(CGL_MATRIX_DATA_TYPE* row, CGL_int n, CGL_matrix* out)
    CGL_matrix* CGL_matrix_row_to_matrix(CGL_MATRIX_DATA_TYPE* row, CGL_int n)
    CGL_matrix* CGL_matrix_col_to_matrix_to(CGL_MATRIX_DATA_TYPE* col, CGL_int n, CGL_matrix* out)
    CGL_matrix* CGL_matrix_col_to_matrix(CGL_MATRIX_DATA_TYPE* col, CGL_int n)
    CGL_bool CGL_matrix_is_square(CGL_matrix* m)
    CGL_bool CGL_matrix_is_symmetric(CGL_matrix* m)
    CGL_bool CGL_matrix_is_orthogonal(CGL_matrix* m)
    CGL_bool CGL_matrix_is_diagonal(CGL_matrix* m)
    CGL_bool CGL_matrix_is_identity(CGL_matrix* m)
    CGL_bool CGL_matrix_is_zero(CGL_matrix* m)
    CGL_bool CGL_matrix_is_equal(CGL_matrix* a, CGL_matrix* b)
    CGL_MATRIX_DATA_TYPE CGL_matrix_trace(CGL_matrix* m)
    CGL_MATRIX_DATA_TYPE CGL_matrix_determinant(CGL_matrix* m)
    CGL_matrix* CGL_matrix_inverse_to(CGL_matrix* m, CGL_matrix* out)
    CGL_matrix* CGL_matrix_inverse(CGL_matrix* m)
    CGL_matrix* CGL_matrix_copy_to(CGL_matrix* m, CGL_matrix* out)
    CGL_matrix* CGL_matrix_copy(CGL_matrix* m)
    CGL_bool CGL_matrix_print(CGL_matrix* m)
    CGL_matrix* CGL_matrix_gauss_jordan_to(CGL_matrix* m, CGL_matrix* out)
    CGL_matrix* CGL_matrix_gauss_jordan(CGL_matrix* m)
    CGL_matrix* CGL_matrix_submatrix_to(CGL_matrix* mat, CGL_int i, CGL_int j, CGL_int m, CGL_int n, CGL_matrix* out)
    CGL_matrix* CGL_matrix_submatrix(CGL_matrix* mat, CGL_int i, CGL_int j, CGL_int m, CGL_int n)
    CGL_matrix* CGL_matrix_minor_to(CGL_matrix* mat, CGL_int i, CGL_int j, CGL_matrix* out)
    CGL_matrix* CGL_matrix_minor(CGL_matrix* mat, CGL_int i, CGL_int j)
    CGL_matrix* CGL_matrix_adjugate_to(CGL_matrix* m, CGL_matrix* out)
    CGL_matrix* CGL_matrix_adjugate(CGL_matrix* m)
    CGL_matrix* CGL_matrix_transpose_inplace(CGL_matrix* m)
    CGL_float CGL_matrix_sum_of_row(CGL_matrix* m, CGL_int i)
    CGL_float CGL_matrix_sum_of_col(CGL_matrix* m, CGL_int j)
    CGL_float CGL_matrix_product_of_row(CGL_matrix* m, CGL_int i)
    CGL_float CGL_matrix_product_of_col(CGL_matrix* m, CGL_int j)
    CGL_matrix* CGL_matrix_make_zero(CGL_matrix* m)

    cdef struct CGL_bloom:
        pass

    
    CGL_bloom* CGL_bloom_create(CGL_int width, CGL_int height, CGL_int iterations)
    void CGL_bloom_destroy(CGL_bloom* bloom)
    void CGL_bloom_set_threshold(CGL_bloom* bloom, CGL_float val)
    float CGL_bloom_get_threshold(CGL_bloom* bloom)
    void CGL_bloom_set_knee(CGL_bloom* bloom, CGL_float val)
    float CGL_bloom_get_knee(CGL_bloom* bloom)
    void CGL_bloom_set_offset(CGL_bloom* bloom, CGL_float x, CGL_float y)
    void CGL_bloom_apply(CGL_bloom* bloom, CGL_texture* tex)
    void CGL_bloom_apply2(CGL_bloom* bloom, CGL_texture* tex_src, CGL_texture* tex_dst)
    int CGL_bloom_get_iterations(CGL_bloom* bloom)
    CGL_texture* CGL_bloom_get_lod_texture(CGL_bloom* bloom, CGL_int index)
    CGL_texture* CGL_bloom_get_prefiltered_texture(CGL_bloom* bloom)

    ctypedef CGL_float CGL_noise_data_type

    cdef struct CGL_noise_params:
        CGL_int type
        CGL_int fractal_type
        CGL_int octaves
        CGL_noise_data_type frequency
        CGL_noise_data_type lacunarity
        CGL_noise_data_type gain
        CGL_noise_data_type weighted_strength
        CGL_noise_data_type ping_pong_strength

    void CGL_noise_init()
    void CGL_noise_shutdown()
    CGL_noise_data_type CGL_noise_perlin(CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)
    CGL_noise_data_type CGL_noise_opensimplex(CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)
    CGL_noise_data_type CGL_noise_opensimplex2s(CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)
    CGL_noise_data_type CGL_noise_value(CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)
    CGL_noise_data_type CGL_noise_valuecubic(CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)
    CGL_noise_data_type CGL_noise_worley(CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)
    void CGL_noise_params_default(CGL_noise_params* params)
    CGL_noise_data_type CGL_noise_get(CGL_noise_params* params, CGL_noise_data_type x, CGL_noise_data_type y, CGL_noise_data_type z)



CGL_TRUE = 1
CGL_FALSE = 0

def clear_console():
    os.system('cls' if os.name == 'nt' else 'clear')

def free(ptr):
    stdlib.free(<void*>ptr)

def init() -> bool:
    return CGL_init()

def shutdown():
    CGL_shutdown()

def utils_sleep(milliseconds: int):
    CGL_utils_sleep(milliseconds)

def utils_read_file(path: str) -> bytes:
    cdef CGL_sizei size
    cdef CGL_byte* data = CGL_utils_read_file(path.encode(), &size)
    if data is NULL:
        raise MemoryError()
    result = copy.deepcopy(data[:size])
    with nogil:
        stdlib.free(data)
    return result

def utils_get_file_size(path: str) -> int:
    return CGL_utils_get_file_size(path.encode())

def utils_append_file(path: str, data: bytes) -> bool:
    return CGL_utils_append_file(path.encode(), data, len(data))

def utils_write_file(path : str, data : bytes) -> bool:
    return CGL_utils_write_file(path.encode(), data, len(data))

def utils_get_time() -> float:
    return CGL_utils_get_time()

def utils_get_timestamp() -> str:
    cdef CGL_byte buffer[128]
    CGL_utils_get_timestamp(buffer)
    return buffer.decode()

def utils_is_little_endian() -> bool:
    return CGL_utils_is_little_endian()

def utils_is_big_endian() -> bool:
    return not CGL_utils_is_little_endian()

def utils_get_random_with_probability(probabilities) -> int:
    cdef CGL_float* c_probabilities
    c_probabilities = <CGL_float*>stdlib.malloc(len(probabilities) * sizeof(CGL_float))
    if c_probabilities is NULL:
        raise MemoryError()
    for i in range(len(probabilities)):
        c_probabilities[i] = probabilities[i]
    result = CGL_utils_get_random_with_probability(c_probabilities, len(probabilities))
    with nogil:
        stdlib.free(c_probabilities)
    return result

def utils_reverse_bytes(data: bytes):
    CGL_utils_reverse_bytes(<void*>data, len(data))

def utils_little_endian_to_current(data : bytes):
    CGL_utils_little_endian_to_current(<void*>data, len(data))

def utils_big_endian_to_current(data : bytes):
    CGL_utils_big_endian_to_current(<void*>data, len(data))

def utils_fast_srand(seed: int):
    CGL_utils_fast_srand(seed)

def utils_fast_rand() -> int:
    return CGL_utils_fast_rand()

def utils_xorshf96() -> int:
    return CGL_utils_xorshf96()

def utils_srand31(seed: int):
    CGL_utils_srand31(seed)

def utils_rand31() -> int:
    return CGL_utils_rand31()

def utils_sigmoid(x: float) -> float:
    CGL_utils_sigmoid(x)

def utils_sigmoid_derivative(x: float) -> float:
    CGL_utils_sigmoid_derivative(x)

def utils_relu(x: float) -> float:
    CGL_utils_relu(x)

def utils_relu_derivative(x: float) -> float:
    CGL_utils_relu_derivative(x)

def utils_tanh(x: float) -> float:
    CGL_utils_tanh(x)

def utils_tanh_derivative(x: float) -> float:
    CGL_utils_tanh_derivative(x)

def utils_step(x: float) -> float:
    CGL_utils_step(x)

def utils_step_derivative(x: float) -> float:
    CGL_utils_step_derivative(x)

def utils_relu_leaky(x: float) -> float:
    CGL_utils_relu_leaky(x)

def utils_relu_leaky_derivative(x: float) -> float:
    CGL_utils_relu_leaky_derivative(x)

def utils_relu_smooth(x: float) -> float:
    CGL_utils_relu_smooth(x)

def utils_relu_smooth_derivative(x: float) -> float:
    CGL_utils_relu_smooth_derivative(x)


CONSOLE_COLOR_RESET = 0
CONSOLE_COLOR_RED = 1
CONSOLE_COLOR_GREEN = 2
CONSOLE_COLOR_GRAY = 3
CONSOLE_COLOR_BLUE = 4

def console_set_color(color: int):
    if color < 0 or color > 4:
        raise ValueError()
    CGL_console_set_color(color)

def printf_red(format: str):
    CGL_printf_red(format.encode())

def printf_green(format: str):
    CGL_printf_green(format.encode())

def printf_gray(format: str):
    CGL_printf_gray(format.encode())

def printf_blue(format: str):
    CGL_printf_blue(format.encode())

def console_progress_bar(progress: float, width: int, prefix: str, suffix: str, complete_char: str = "#", incomplete_char: str = "_"):
    CGL_console_progress_bar(progress, width, prefix.encode(), suffix.encode(), complete_char[0], incomplete_char[0])

LOG_LEVEL_TRACE        = 0
LOG_LEVEL_INFO         = 1
LOG_LEVEL_WARN         = 2
LOG_LEVEL_ERROR        = 3 
LOG_LEVEL_INTERNAL     = 4 
LOGGER_MAX_LOG_FILES   = 32
LOGGER_LOG_BUFFER_SIZE = (1024 * 4)


def logger_init(enable_console_logging: bool):
    CGL_logger_init(enable_console_logging)

def logger_shutdown():
    CGL_logger_shutdown()

def logger_attach_log_file(path: str) -> bool:
    return CGL_logger_attach_log_file(path.encode())

def logger_detach_log_file(path: str) -> bool:
    return CGL_logger_detach_log_file(path.encode())

def logger_flush():
    CGL_logger_flush()
    
def logger_disable_console_logging():
    CGL_logger_disable_console_logging()

def logger_enable_console_logging():
    CGL_logger_enable_console_logging()

def logger_log(level: int, log_format: str):
    CGL_logger_log(level, log_format.encode())

def log_trace(log_format: str):
    CGL_logger_log(LOG_LEVEL_TRACE, log_format.encode())

def log_info(log_format: str):
    CGL_logger_log(LOG_LEVEL_INFO, log_format.encode())

def log_warn(log_format: str):
    CGL_logger_log(LOG_LEVEL_WARN, log_format.encode())

def log_error(log_format: str):
    CGL_logger_log(LOG_LEVEL_ERROR, log_format.encode())

def log_internal(log_format: str):
    CGL_logger_log(LOG_LEVEL_INTERNAL, log_format.encode())

def utils_crc32(data: bytes) -> int:
    return CGL_utils_crc32(<void*>data, len(data))

def utils_crc64(data: bytes) -> int:
    return CGL_utils_crc64(<void*>data, len(data))

def utils_rot13(data: bytes) -> bytes:
    result = bytes(len(data))
    CGL_utils_rot13(<char*>data, <char*>result)
    return result

def utils_super_fast_hash(data: bytes) -> int:
    return CGL_utils_super_fast_hash(<void*>data, len(data))


PI = (3.14159265358979323846)
TWO_PI = (6.28318530717958647692)
PI_2 = (1.57079632679489661923)
E = (2.71828182845904523536)
SQRT2 = (1.41421356237309504880)
SQRT3 = (1.73205080756887729352)
SQRT5 = (2.23606797749978969640)
SQRT6 = (2.44948974278317809820)
SQRT7 = (2.64575131106459059050)
SQRT8 = (2.82842712474619009760)

def deg_to_rad(deg):
    return ((deg) * (PI / 180.0))

def rad_to_deg(rad):
    return ((rad) * (180.0 / PI))

def float_lerp(a: float, b: float, t: float):
    return ((a) + ((b) - (a)) * (t))

def float_quadratic_lerp(a: float, b: float, c: float, t: float) -> float:
    return CGL_float_quadratic_lerp(a, b, c, t)

def float_cubic_lerp(a: float, b: float, c: float, d: float, t: float) -> float:
    return CGL_float_cubic_lerp(a, b, c, d, t)

cdef class vec2:
    cdef CGL_vec2 c_vec2

    def __init__(self, x: float = 0.0, y: float = 0.0):
        self.c_vec2 = CGL_vec2(x, y)
    
    def __repr__(self):
        return f"vec2({self.x}, {self.y})"
    
    def __str__(self):
        return f"vec2({self.x}, {self.y})"

    def __bytes__(self):
        return bytes(self.c_vec2)
    
    def __len__(self):
        return 2
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_vec2.x
        elif index == 1:
            return self.c_vec2.y
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_vec2.x = value
        elif index == 1:
            self.c_vec2.y = value
        else:
            raise IndexError()
    
    def __eq__(self, other: vec2):
        return self.c_vec2.x == other.c_vec2.x and self.c_vec2.y == other.c_vec2.y
    
    def __ne__(self, other: vec2):
        return self.c_vec2.x != other.c_vec2.x or self.c_vec2.y != other.c_vec2.y
    
    def __hash__(self):
        return hash((self.c_vec2.x, self.c_vec2.y))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_vec2.x
        elif name == "y":
            return self.c_vec2.y
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value):
        if name == "x":
            self.c_vec2.x = value
        elif name == "y":
            self.c_vec2.y = value
        else:
            raise AttributeError()
    
    def __add__(self, other: vec2):
        return vec2(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other: vec2):
        return vec2(self.x - other.x, self.y - other.y)
    
    def __mul__(self, other: vec2):
        return vec2(self.x * other.x, self.y * other.y)
    
    def __truediv__(self, other: vec2):
        return vec2(self.x / other.x, self.y / other.y)
    
    def __floordiv__(self, other: vec2):
        return vec2(self.x // other.x, self.y // other.y)
    
    def dot(self, other: vec2):
        return (self.x * other.x) + (self.y * other.y)
    
    def cross(self, other: vec2):
        return (self.x * other.y) - (self.y * other.x)
    
    def length(self):
        return math.sqrt((self.x * self.x) + (self.y * self.y))
    
    def length_squared(self):
        return (self.x * self.x) + (self.y * self.y)
    
    def normalized(self):
        length = self.length()
        return vec2(self.x / length, self.y / length)
    
def vec2_triple_product(a: vec2, b: vec2, c: vec2) -> vec2:
    cdef CGL_vec2 obj = vec2(CGL_vec2_triple_product(a.c_vec2, b.c_vec2, c.c_vec2))
    return vec2(obj.x, obj.y)


cdef class vec3:
    cdef CGL_vec3 c_vec3

    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0):
        self.c_vec3 = CGL_vec3(x, y, z)
    
    def __repr__(self):
        return f"vec3({self.x}, {self.y}, {self.z})"
    
    def __str__(self):
        return f"vec3({self.x}, {self.y}, {self.z})"

    def __bytes__(self):
        return bytes(self.c_vec3)
    
    def __len__(self):
        return 3
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_vec3.x
        elif index == 1:
            return self.c_vec3.y
        elif index == 2:
            return self.c_vec3.z
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_vec3.x = value
        elif index == 1:
            self.c_vec3.y = value
        elif index == 2:
            self.c_vec3.z = value
        else:
            raise IndexError()
    
    def __eq__(self, other: vec3):
        return self.c_vec3.x == other.c_vec3.x and self.c_vec3.y == other.c_vec3.y and self.c_vec3.z == other.c_vec3.z
    
    def __ne__(self, other: vec3):
        return self.c_vec3.x != other.c_vec3.x or self.c_vec3.y != other.c_vec3.y or self.c_vec3.z != other.c_vec3.z
    
    
    def __hash__(self):
        return hash((self.c_vec3.x, self.c_vec3.y, self.c_vec3.z))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_vec3.x
        elif name == "y":
            return self.c_vec3.y
        elif name == "z":
            return self.c_vec3.z
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value):
        if name == "x":
            self.c_vec3.x = value
        elif name == "y":
            self.c_vec3.y = value
        elif name == "z":
            self.c_vec3.z = value
        else:
            raise AttributeError()
    
    def __add__(self, other: vec3):
        return vec3(self.x + other.x, self.y + other.y, self.z + other.z)
    
    def __sub__(self, other: vec3):
        return vec3(self.x - other.x, self.y - other.y, self.z - other.z)
    
    def __mul__(self, other: vec3):
        return vec3(self.x * other.x, self.y * other.y, self.z * other.z)
    
    def __truediv__(self, other: vec3):
        return vec3(self.x / other.x, self.y / other.y, self.z / other.z)
    
    def __floordiv__(self, other: vec3):
        return vec3(self.x // other.x, self.y // other.y, self.z // other.z)
    
    def dot(self, other: vec3):
        return (self.x * other.x) + (self.y * other.y) + (self.z * other.z)
    
    def cross(self, other: vec3):
        return vec3((self.y * other.z) - (self.z * other.y), (self.z * other.x) - (self.x * other.z), (self.x * other.y) - (self.y * other.x))
    
    def length(self):
        return math.sqrt((self.x * self.x) + (self.y * self.y) + (self.z * self.z))
    
    def length_squared(self):
        return (self.x * self.x) + (self.y * self.y) + (self.z * self.z)
    
    def normalized(self):
        cdef float length = self.length()
        return vec3(self.x / length, self.y / length, self.z / length)
    
def vec3_reflect(a: vec3, n: vec3) -> vec3:
    cdef CGL_vec3 obj = vec3(CGL_vec3_reflect(a.c_vec3, n.c_vec3))
    return vec3(obj.x, obj.y, obj.z)

def vec3_rotate_about_axis(v: vec3, axis: vec3, theta: float) -> vec3:
    cdef CGL_vec3 obj = vec3(CGL_vec3_rotate_about_axis(v.c_vec3, axis.c_vec3, theta))
    return vec3(obj.x, obj.y, obj.z)

def vec3_triple_product(a: vec3, b: vec3, c: vec3) -> vec3:
    cdef CGL_vec3 obj = vec3(CGL_vec3_triple_product(a.c_vec3, b.c_vec3, c.c_vec3))
    return vec3(obj.x, obj.y, obj.z)

cdef class vec4:
    cdef CGL_vec4 c_vec4

    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0, w: float = 0.0):
        self.c_vec4 = CGL_vec4(x, y, z, w)
    
    def __repr__(self):
        return f"vec4({self.x}, {self.y}, {self.z}, {self.w})"
    
    def __str__(self):
        return f"vec4({self.x}, {self.y}, {self.z}, {self.w})"

    def __bytes__(self):
        return bytes(self.c_vec4)
    
    def __len__(self):
        return 4
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_vec4.x
        elif index == 1:
            return self.c_vec4.y
        elif index == 2:
            return self.c_vec4.z
        elif index == 3:
            return self.c_vec4.w
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_vec4.x = value
        elif index == 1:
            self.c_vec4.y = value
        elif index == 2:
            self.c_vec4.z = value
        elif index == 3:
            self.c_vec4.w = value
        else:
            raise IndexError()
    
    def __eq__(self, other: vec4):
        return self.c_vec4.x == other.c_vec4.x and self.c_vec4.y == other.c_vec4.y and self.c_vec4.z == other.c_vec4.z and self.c_vec4.w == other.c_vec4.w
    
    def __ne__(self, other: vec4):
        return self.c_vec4.x != other.c_vec4.x or self.c_vec4.y != other.c_vec4.y or self.c_vec4.z != other.c_vec4.z or self.c_vec4.w != other.c_vec4.w
    
    def __hash__(self):
        return hash((self.c_vec4.x, self.c_vec4.y, self.c_vec4.z, self.c_vec4.w))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_vec4.x
        elif name == "y":
            return self.c_vec4.y
        elif name == "z":
            return self.c_vec4.z
        elif name == "w":
            return self.c_vec4.w
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value):
        if name == "x":
            self.c_vec4.x = value
        elif name == "y":
            self.c_vec4.y = value
        elif name == "z":
            self.c_vec4.z = value
        elif name == "w":
            self.c_vec4.w = value
        else:
            raise AttributeError()
    
    def __add__(self, other: vec4):
        return vec4(self.x + other.x, self.y + other.y, self.z + other.z, self.w + other.w)
    
    def __sub__(self, other: vec4):
        return vec4(self.x - other.x, self.y - other.y, self.z - other.z, self.w - other.w)
    
    def __mul__(self, other: vec4):
        return vec4(self.x * other.x, self.y * other.y, self.z * other.z, self.w * other.w)
    
    def __truediv__(self, other: vec4):
        return vec4(self.x / other.x, self.y / other.y, self.z / other.z, self.w / other.w)
    
    def __floordiv__(self, other: vec3):
        return vec4(self.x // other.x, self.y // other.y, self.z // other.z, self.w // other.w)
    
    def dot(self, other: vec3):
        return (self.x * other.x) + (self.y * other.y) + (self.z * other.z) + (self.w * other.w)
    
    def cross(self, other: vec3):
        return vec4(self.y * other.z - self.z * other.y, self.z * other.x - self.x * other.z, self.x * other.y - self.y * other.x, 0.0)
    
    def length(self):
        return math.sqrt(self.length_squared())
    
    def length_squared(self):
        return (self.x * self.x) + (self.y * self.y) + (self.z * self.z) + (self.w * self.w)

    def normalized(self):
        length = self.length()
        return vec4(self.x / length, self.y / length, self.z / length, self.w / length)

def vec4_triple_product(a: vec4, b: vec4, c: vec4) -> vec4:
    cdef CGL_vec4 obj = vec4(CGL_vec4_triple_product(a.c_vec4, b.c_vec4, c.c_vec4))
    return vec4(obj.x, obj.y, obj.z, obj.w)

cdef class ivec4:
    cdef CGL_ivec4 c_ivec4

    def __init__(self, x: int = 0, y: int = 0, z: int = 0, w: int = 0):
        self.c_ivec4 = CGL_ivec4(x, y, z, w)
    
    def __repr__(self):
        return f"ivec4({self.c_ivec4.x}, {self.c_ivec4.y}, {self.c_ivec4.z}, {self.c_ivec4.w})"

    def __str__(self): 
        return f"ivec4({self.c_ivec4.x}, {self.c_ivec4.y}, {self.c_ivec4.z}, {self.c_ivec4.w})"
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_ivec4.x
        elif index == 1:
            return self.c_ivec4.y
        elif index == 2:
            return self.c_ivec4.z
        elif index == 3:
            return self.c_ivec4.w
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_ivec4.x = value
        elif index == 1:
            self.c_ivec4.y = value
        elif index == 2:
            self.c_ivec4.z = value
        elif index == 3:
            self.c_ivec4.w = value
        else:
            raise IndexError()
    
    def __hash__(self):
        return hash((self.c_ivec4.x, self.c_ivec4.y, self.c_ivec4.z, self.c_ivec4.w))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_ivec4.x
        elif name == "y":
            return self.c_ivec4.y
        elif name == "z":
            return self.c_ivec4.z
        elif name == "w":
            return self.c_ivec4.w
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value: int):   
        if name == "x":
            self.c_ivec4.x = value
        elif name == "y":
            self.c_ivec4.y = value
        elif name == "z":
            self.c_ivec4.z = value
        elif name == "w":
            self.c_ivec4.w = value
        else:
            raise AttributeError()

cdef class mat4:
    cdef CGL_mat4 c_mat4

    def __init__(self):
        self.c_mat4 = CGL_mat4()
    
    def __repr__(self):
        result = "mat4x4(\n"
        for i in range(4):
            result += "    "
            for j in range(4):
                result += f"{self.c_mat4.m[i * 4 + j]}, "
            result += "\n"
        result += ")"
        return result
    
    def __str__(self):
        result = "mat4x4(\n"
        for i in range(4):
            result += "    "
            for j in range(4):
                result += f"{self.c_mat4.m[i * 4 + j]}, "
            result += "\n"
        result += ")"
        return result
    
    cdef set_c_mat4(self, c_mat4: CGL_mat4):
        self.c_mat4 = c_mat4
    
    def __mul__(self, other: mat4):
        return mat4_mul(self, other)
    
    def __hash__(self):
        return hash((self.c_mat4.m[0], self.c_mat4.m[1], self.c_mat4.m[2], self.c_mat4.m[3], self.c_mat4.m[4], self.c_mat4.m[5], self.c_mat4.m[6], self.c_mat4.m[7], self.c_mat4.m[8], self.c_mat4.m[9], self.c_mat4.m[10], self.c_mat4.m[11], self.c_mat4.m[12], self.c_mat4.m[13], self.c_mat4.m[14], self.c_mat4.m[15]))

def mat4_mul(a: mat4, b: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_mul(a.c_mat4, b.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_det(m: mat4) -> float:
    return CGL_mat4_det(m.c_mat4)
    
def mat4_det_by_lu(m: mat4) -> float:
    return CGL_mat4_det_by_lu(m.c_mat4)
    
def mat4_det_by_gauss(m: mat4) -> float:
    return CGL_mat4_det_by_gauss(m.c_mat4)
    
def mat4_mul_vec4(m: mat4, v: vec4) -> vec4:
    cdef CGL_vec4 obj = CGL_mat4_mul_vec4(m.c_mat4, v.c_vec4)
    return vec4(obj.x, obj.y, obj.z, obj.w)
    
def mat4_inverse(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_inverse(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_transpose(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_transpose(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_adjoint(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_adjoint(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_gauss_elim(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_gauss_elim(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_rank(m: mat4) -> int:
    return CGL_mat4_rank(m.c_mat4)

def mat4_trace(m: mat4) -> float:
    return CGL_mat4_trace(m.c_mat4)
    
def mat4_rotate_about_axis(axis: vec3, angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_about_axis(axis.c_vec3, angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_look_at(eye: vec3, target: vec3, up: vec3) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_look_at(eye.c_vec3, target.c_vec3, up.c_vec3)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_lerp(a: mat4, b: mat4, t: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_lerp(a.c_mat4, b.c_mat4, t)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_decompose_lu(m: mat4, l: mat4, u: mat4):
    cdef CGL_mat4 l_obj = CGL_mat4()
    cdef CGL_mat4 u_obj = CGL_mat4()
    CGL_mat4_decompose_lu(m.c_mat4, &l_obj, &u_obj)
    l = mat4()
    u = mat4()
    l.set_c_mat4(l_obj)
    u.set_c_mat4(u_obj)
    return (l, u)

def mat4_perspective(fov: float, aspect: float, near: float, far: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_perspective(fov, aspect, near, far)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_orthographic(left: float, right: float, bottom: float, top: float, near: float, far: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_orthographic(left, right, bottom, top, near, far)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_zero() -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_zero()
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_identity() -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_identity()
    result = mat4()
    result.set_c_mat4(obj)
    return result


def mat4_scale(x: float, y: float, z: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_scale(x, y, z)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_translate(x: float, y: float, z: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_translate(x, y, z)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_rotate_x(angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_x(angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_rotate_y(angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_y(angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_rotate_z(angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_z(angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result


cdef class transform:
    cdef CGL_transform c_transform

    @property
    def position(self) -> vec3:
        return vec3(self.c_transform.position.x, self.c_transform.position.y, self.c_transform.position.z)
    
    @position.setter
    def position(self, value: vec3):
        self.c_transform.position.x = value.x
        self.c_transform.position.y = value.y
        self.c_transform.position.z = value.z
        self.c_transform.position.w = 1.0
    
    @property
    def rotation(self) -> vec3:
        return vec3(self.c_transform.rotation.x, self.c_transform.rotation.y, self.c_transform.rotation.z)
    
    @rotation.setter
    def rotation(self, value: vec3):
        self.c_transform.rotation.x = value.x
        self.c_transform.rotation.y = value.y
        self.c_transform.rotation.z = value.z
        self.c_transform.rotation.w = 0.0
    
    @property
    def scale(self) -> vec3:
        return vec3(self.c_transform.scale.x, self.c_transform.scale.y, self.c_transform.scale.z)
    
    @scale.setter
    def scale(self, value: vec3):
        self.c_transform.scale.x = value.x
        self.c_transform.scale.y = value.y
        self.c_transform.scale.z = value.z
        self.c_transform.scale.w = 1.0
    
    @property
    def matrix(self) -> mat4:
        return mat4(self.c_transform.matrix)
    
    @matrix.setter
    def matrix(self, value: mat4):
        self.c_transform.matrix = value.c_mat4


    def __cinit__(self):
        self.c_transform = CGL_transform_create_empty()

    def __init__(self, position: vec3 = None, rotation: vec3 = None, scale: vec3 = None):
        if position is None:
            position = vec3()
        if rotation is None:
            rotation = vec3()
        if scale is None:
            scale = vec3(1.0, 1.0, 1.0)
        self.c_transform = CGL_transform_create(position.c_vec3, rotation.c_vec3, scale.c_vec3)

    def __hash__(self):
        return hash((self.position, self.rotation, self.scale))
    
    def __repr__(self):
        return f"transform(position={self.position}, rotation={self.rotation}, scale={self.scale})"
    
    def __eq__(self, other):
        if isinstance(other, transform):
            return self.position == other.position and self.rotation == other.rotation and self.scale == other.scale
        return False

    def __str__(self):
        return f"transform(position={self.position}, rotation={self.rotation}, scale={self.scale})"

    def load_matrix(self, m: mat4):
        self.c_transform = CGL_transform_create_from_matrix(m.c_mat4)

    def set_parent(self, parent: transform):
        self.c_transform.parent = &parent.c_transform
    
    def update(self):
        CGL_transform_update(&self.c_transform)
        

cdef class matrix:
    cdef int m
    cdef int n
    cdef CGL_matrix* c_matrix
    
    @property
    def size(self) -> tuple:
        return (self.m, self.n)
    
    @property
    def trace(self) -> float:
        return CGL_matrix_trace(self.c_matrix)
    
    @property
    def determinant(self) -> float:
        return CGL_matrix_determinant(self.c_matrix)
    
    @property
    def inverse(self) -> matrix:
        return self.calc_inverse()
    
    @property
    def transpose(self) -> matrix:
        return self.calc_transpose()
    
    @property
    def adjugate(self) -> matrix:
        return self.calc_adjugate()

    def __init__(self, m: int, n: int):
        self.c_matrix = CGL_matrix_create(m, n)
        self.m = m
        self.n = n
    
    def __dealloc__(self):
        CGL_matrix_destroy(self.c_matrix)
    
    def load_array(self, array: list, m: int, n: int):
        CGL_matrix_destroy(self.c_matrix)
        cdef float* array_ptr = <float*> stdlib.malloc(m * n * sizeof(float))
        for i in range(m * n):
            array_ptr[i] = array[i]
        self.c_matrix = CGL_matrix_create_from_array(array_ptr, m, n)
        stdlib.free(array_ptr)
    
    def __add__(self: matrix, other: matrix):
        if self.size != other.size:
            raise ValueError("Matrices must have the same size")
        result = matrix(self.m, self.n)
        CGL_matrix_add_to(self.c_matrix, other.c_matrix, result.c_matrix)
        return result
    
    def __sub__(self: matrix, other: matrix):
        if self.size != other.size:
            raise ValueError("Matrices must have the same size")
        result = matrix(self.m, self.n)
        CGL_matrix_sub_to(self.c_matrix, other.c_matrix, result.c_matrix)
        return result
    
    def __mul__(self: matrix, other: matrix):
        if self.n != other.m:
            raise ValueError("Matrices must have the same size")
        result = matrix(self.m, other.n)
        CGL_matrix_mul_to(self.c_matrix, other.c_matrix, result.c_matrix)
        return result
    
    def scaled(self: matrix, other: float):
        result = matrix(self.m, self.n)
        for i in range(self.m):
            for j in range(self.n):
                result[i, j] = self[i, j] * other
        return result
    
    def scale(self: matrix, other: float):
        CGL_matrix_scale_to(self.c_matrix, other)

    def calc_transpose(self: matrix) -> matrix:
        result = matrix(self.n, self.m)
        CGL_matrix_transpose_to(self.c_matrix, result.c_matrix)
        return result

    def transpose_inplace(self: matrix):
        CGL_matrix_transpose_inplace(self.c_matrix)

    def load_identity(self: matrix):
        CGL_matrix_identity_to(self.m, self.n, self.c_matrix)

    def __getitem__(self: matrix, key: tuple):
        if len(key) != 2:
            raise ValueError("Matrix index must be a tuple of 2 integers")
        return CGL_matrix_get_elem(self.c_matrix, key[0], key[1])
    
    def __setitem__(self: matrix, key: tuple, value: float):
        if len(key) != 2:
            raise ValueError("Matrix index must be a tuple of 2 integers")
        CGL_matrix_set_elem(self.c_matrix, key[0], key[1], value)
    
    def __repr__(self: matrix):
        result = "matrix(\n"
        for i in range(self.m):
            result += "    ["
            for j in range(self.n):
                result += f"{self[i, j]}"
                if j != self.n - 1:
                    result += ", "
            result += "]\n"
        result += ")"
        return result
    
    def __str__(self):
        result = "matrix(\n"
        for i in range(self.m):
            result += "    ["
            for j in range(self.n):
                result += f"{self[i, j]}"
                if j != self.n - 1:
                    result += ", "
            result += "]\n"
        result += ")"
        return result
    
    def set_row(self: matrix, row: int, values: list):
        if len(values) != self.n:
            raise ValueError("Values must have the same size as the matrix")
        cdef float* values_ptr = <float*> stdlib.malloc(self.n * sizeof(float))
        for i in range(self.n):
            values_ptr[i] = values[i]
        CGL_matrix_set_row(self.c_matrix, row, values_ptr)
        stdlib.free(values_ptr)

    def __eq__(self: matrix, other: matrix) -> bool:
        return CGL_matrix_is_equal(self.c_matrix, other.c_matrix)

    def get_row(self: matrix, row: int) -> list:
        cdef float* values_ptr = <float*> stdlib.malloc(self.n * sizeof(float))
        CGL_matrix_get_row(self.c_matrix, row, values_ptr)
        result = []
        for i in range(self.n):
            result.append(values_ptr[i])
        stdlib.free(values_ptr)
        return result
    
    def set_column(self: matrix, column: int, values: list):
        if len(values) != self.m:
            raise ValueError("Values must have the same size as the matrix")
        cdef float* values_ptr = <float*> stdlib.malloc(self.m * sizeof(float))
        for i in range(self.m):
            values_ptr[i] = values[i]
        CGL_matrix_set_col(self.c_matrix, column, values_ptr)
        stdlib.free(values_ptr)
    
    def get_column(self: matrix, column: int) -> list:
        cdef float* values_ptr = <float*> stdlib.malloc(self.m * sizeof(float))
        CGL_matrix_get_col(self.c_matrix, column, values_ptr)
        result = []
        for i in range(self.m):
            result.append(values_ptr[i])
        stdlib.free(values_ptr)
        return result
    
    def is_square(self: matrix) -> bool:
        return CGL_matrix_is_square(self.c_matrix)
    
    def is_symmetric(self: matrix) -> bool:
        return CGL_matrix_is_symmetric(self.c_matrix)
    
    def is_ortho(self: matrix) -> bool:
        return CGL_matrix_is_orthogonal(self.c_matrix)

    def is_identity(self: matrix) -> bool:
        return CGL_matrix_is_identity(self.c_matrix)
    
    def is_zero(self: matrix) -> bool:
        return CGL_matrix_is_zero(self.c_matrix)

    def calc_inverse(self: matrix) -> matrix:
        if not self.is_square():
            raise ValueError("Matrix must be square")
        result = matrix(self.m, self.n)
        CGL_matrix_inverse_to(self.c_matrix, result.c_matrix)
        return result
    
    def clone(self: matrix) -> matrix:
        result = matrix(self.m, self.n)
        CGL_matrix_copy_to(self.c_matrix, result.c_matrix)
        return result
    
    def print_mat(self: matrix):
        CGL_matrix_print(self.c_matrix)
    
    def gaus_jordan(self: matrix) -> matrix:
        if not self.is_square():
            raise ValueError("Matrix must be square")
        result = matrix(self.m, self.n)
        CGL_matrix_gauss_jordan_to(self.c_matrix, result.c_matrix)
        return result
    
    def submatrix(self: matrix, row: int, column: int, m: int, n: int) -> matrix:
        if row + m > self.m or column + n > self.n:
            raise ValueError("Submatrix must be inside the matrix")
        result = matrix(m, n)
        CGL_matrix_submatrix_to(self.c_matrix, row, column, m, n, result.c_matrix)
        return result

    def calc_adjugate(self: matrix) -> matrix:
        if not self.is_square():
            raise ValueError("Matrix must be square")
        result = matrix(self.m, self.n)
        CGL_matrix_adjugate_to(self.c_matrix, result.c_matrix)
        return result
    
    def make_zero(self: matrix):
        CGL_matrix_make_zero(self.c_matrix)
    
    def randomize(self: matrix, mian: float, maax: float):
        for i in range(self.m):
            for j in range(self.n):
                self[i, j] = random.uniform(mian, maax)

KEY_UNKNOWN            = -1
KEY_SPACE              = 32
KEY_APOSTROPHE         = 39 
KEY_COMMA              = 44 
KEY_MINUS              = 45 
KEY_PERIOD             = 46 
KEY_SLASH              = 47 
KEY_0                  = 48
KEY_1                  = 49
KEY_2                  = 50
KEY_3                  = 51
KEY_4                  = 52
KEY_5                  = 53
KEY_6                  = 54
KEY_7                  = 55
KEY_8                  = 56
KEY_9                  = 57
KEY_SEMICOLON          = 59  
KEY_EQUAL              = 61  
KEY_A                  = 65
KEY_B                  = 66
KEY_C                  = 67
KEY_D                  = 68
KEY_E                  = 69
KEY_F                  = 70
KEY_G                  = 71
KEY_H                  = 72
KEY_I                  = 73
KEY_J                  = 74
KEY_K                  = 75
KEY_L                  = 76
KEY_M                  = 77
KEY_N                  = 78
KEY_O                  = 79
KEY_P                  = 80
KEY_Q                  = 81
KEY_R                  = 82
KEY_S                  = 83
KEY_T                  = 84
KEY_U                  = 85
KEY_V                  = 86
KEY_W                  = 87
KEY_X                  = 88
KEY_Y                  = 89
KEY_Z                  = 90
KEY_LEFT_BRACKET       = 91  
KEY_BACKSLASH          = 92  
KEY_RIGHT_BRACKET      = 93  
KEY_GRAVE_ACCENT       = 96  
KEY_WORLD_1            = 161 
KEY_WORLD_2            = 162 
KEY_ESCAPE             = 256
KEY_ENTER              = 257
KEY_TAB                = 258
KEY_BACKSPACE          = 259
KEY_INSERT             = 260
KEY_DELETE             = 261
KEY_RIGHT              = 262
KEY_LEFT               = 263
KEY_DOWN               = 264
KEY_UP                 = 265
KEY_PAGE_UP            = 266
KEY_PAGE_DOWN          = 267
KEY_HOME               = 268
KEY_END                = 269
KEY_CAPS_LOCK          = 280
KEY_SCROLL_LOCK        = 281
KEY_NUM_LOCK           = 282
KEY_PRINT_SCREEN       = 283
KEY_PAUSE              = 284
KEY_F1                 = 290
KEY_F2                 = 291
KEY_F3                 = 292
KEY_F4                 = 293
KEY_F5                 = 294
KEY_F6                 = 295
KEY_F7                 = 296
KEY_F8                 = 297
KEY_F9                 = 298
KEY_F10                = 299
KEY_F11                = 300
KEY_F12                = 301
KEY_F13                = 302
KEY_F14                = 303
KEY_F15                = 304
KEY_F16                = 305
KEY_F17                = 306
KEY_F18                = 307
KEY_F19                = 308
KEY_F20                = 309
KEY_F21                = 310
KEY_F22                = 311
KEY_F23                = 312
KEY_F24                = 313
KEY_F25                = 314
KEY_KP_0               = 320
KEY_KP_1               = 321
KEY_KP_2               = 322
KEY_KP_3               = 323
KEY_KP_4               = 324
KEY_KP_5               = 325
KEY_KP_6               = 326
KEY_KP_7               = 327
KEY_KP_8               = 328
KEY_KP_9               = 329
KEY_KP_DECIMAL         = 330
KEY_KP_DIVIDE          = 331
KEY_KP_MULTIPLY        = 332
KEY_KP_SUBTRACT        = 333
KEY_KP_ADD             = 334
KEY_KP_ENTER           = 335
KEY_KP_EQUAL           = 336
KEY_LEFT_SHIFT         = 340
KEY_LEFT_CONTROL       = 341
KEY_LEFT_ALT           = 342
KEY_LEFT_SUPER         = 343
KEY_RIGHT_SHIFT        = 344
KEY_RIGHT_CONTROL      = 345
KEY_RIGHT_ALT          = 346
KEY_RIGHT_SUPER        = 347
KEY_MENU               = 348
MOUSE_BUTTON_1         = 0
MOUSE_BUTTON_2         = 1
MOUSE_BUTTON_3         = 2
MOUSE_BUTTON_4         = 3
MOUSE_BUTTON_5         = 4
MOUSE_BUTTON_6         = 5
MOUSE_BUTTON_7         = 6
MOUSE_BUTTON_8         = 7
MOUSE_BUTTON_LAST      = MOUSE_BUTTON_8
MOUSE_BUTTON_LEFT      = MOUSE_BUTTON_1
MOUSE_BUTTON_RIGHT     = MOUSE_BUTTON_2
MOUSE_BUTTON_MIDDLE    = MOUSE_BUTTON_3
RELEASE                = 0
PRESS                  = 1
REPEAT                 = 2

__WINDOW_TABLE = {}

cdef void __pass_through_window_close_callback(CGL_window* window):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    window_object.close_callback(window_object)

cdef void __pass_through_window_key_callback(CGL_window* window, int key, int scancode, int action, int mods):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    window_object.key_callback(window_object, key, scancode, action, mods)

cdef void __pass_through_window_mouse_button_callback(CGL_window* window, int button, int action, int mods):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    window_object.mouse_button_callback(window_object, button, action, mods)

cdef void __pass_through_window_mouse_position_callback(CGL_window* window, double xpos, double ypos):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    window_object.mouse_position_callback(window_object, xpos, ypos)

cdef void __pass_through_window_mouse_scroll_callback(CGL_window* window, double xoffset, double yoffset):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    window_object.mouse_scroll_callback(window_object, xoffset, yoffset)

cdef void __pass_through_window_framebuffer_size_callback(CGL_window* window, int width, int height):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    window_object.framebuffer_size_callback(window_object, width, height)

cdef void __pass_through_window_drag_n_drop_callback(CGL_window* window, const char** paths, int count):
    window_object = __WINDOW_TABLE[CGL_utils_super_fast_hash(<void*>window, sizeof(CGL_window*))]    
    path_list = []
    for i in range(count):
        path_list.append(paths[i].decode('utf-8'))
    window_object.drag_n_drop_callback(window_object, path_list)
    
cdef class window:
    cdef CGL_window* c_window
    cdef bool is_decorated
    cdef bool has_been_destroyed
    cdef str title
    cdef object key_callback
    cdef object mouse_button_callback
    cdef object mouse_position_callback
    cdef object mouse_scroll_callback
    cdef object framebuffer_size_callback
    cdef object close_callback
    cdef object drag_n_drop_callback

    @property
    def close_callback(self):
        return self.close_callback
    
    @property
    def key_callback(self):
        return self.key_callback
    
    @property
    def mouse_button_callback(self):
        return self.mouse_button_callback
    
    @property
    def mouse_position_callback(self):
        return self.mouse_position_callback
    
    @property
    def mouse_scroll_callback(self):
        return self.mouse_scroll_callback
    
    @property
    def framebuffer_size_callback(self):
        return self.framebuffer_size_callback
    
    @property
    def drag_n_drop_callback(self):
        return self.drag_n_drop_callback
    
    @property
    def width(self) -> int:
        return self.get_size()[0]
    
    @property
    def height(self) -> int:
        return self.get_size()[1]
    
    @property
    def x(self) -> int:
        return self.get_position()[0]
    
    @property
    def y(self) -> int:
        return self.get_position()[1]

    
    def __init__(self, width: int, height: int, title: str = "CGL Window", decorated: bool = True):
        self.is_decorated = decorated
        self.title = title
        if decorated:
            self.c_window = CGL_window_create(width, height, title.encode('utf-8'))
        else:
            self.c_window = CGL_window_create_undecorated(width, height, title.encode('utf-8'))
        if self.c_window == NULL:
            raise RuntimeError("Failed to create window")
        self.has_been_destroyed = False
        __WINDOW_TABLE[self.get_id()] = self
        
    def __dealloc__(self):
        self.destroy()
    
    def __hash__(self) -> int:
        return self.get_id()
    
    def destroy(self):
        if self.has_been_destroyed:
            return
        __WINDOW_TABLE.pop(self.get_id())
        CGL_window_destroy(self.c_window)
        self.has_been_destroyed = True
    
    def get_id(self) -> int:
        return CGL_utils_super_fast_hash(<void*>self.c_window, sizeof(CGL_window*))

    def poll_events(self):
        CGL_window_poll_events(self.c_window)
    
    def should_close(self) -> bool:
        return CGL_window_should_close(self.c_window)
    
    def swap_buffers(self):
        CGL_window_swap_buffers(self.c_window)
    
    def set_title(self, title: str):
        CGL_window_set_title(self.c_window, title.encode('utf-8'))
    
    def set_size(self, width: int, height: int):
        CGL_window_set_size(self.c_window, width, height)
    
    def set_position(self, x: int, y: int):
        CGL_window_set_position(self.c_window, x, y)
    
    def set_hidden(self, hidden: bool):
        CGL_window_set_hidden(self.c_window, hidden)
    
    def set_user_data(self, data: object):
        CGL_window_set_user_data(self.c_window, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_window_get_user_data(self.c_window)
    
    def get_size(self) -> tuple:
        cdef int width, height
        CGL_window_get_size(self.c_window, &width, &height)
        return (width, height)
    
    def set_close_callback(self, callback: Callable):
        self.close_callback = callback
        CGL_window_set_close_callback(self.c_window, __pass_through_window_close_callback)
    
    def set_key_callback(self, callback: Callable):
        self.key_callback = callback
        CGL_window_set_key_callback(self.c_window, __pass_through_window_key_callback)
    
    def set_mouse_button_callback(self, callback: Callable):
        self.mouse_button_callback = callback
        CGL_window_set_mouse_button_callback(self.c_window, __pass_through_window_mouse_button_callback)
    
    def set_mouse_position_callback(self, callback: Callable):
        self.mouse_position_callback = callback
        CGL_window_set_mouse_position_callback(self.c_window, __pass_through_window_mouse_position_callback)
    
    def set_mouse_scroll_callback(self, callback: Callable):
        self.mouse_scroll_callback = callback
        CGL_window_set_mouse_scroll_callback(self.c_window, __pass_through_window_mouse_scroll_callback)
    
    def set_framebuffer_size_callback(self, callback: Callable):
        self.framebuffer_size_callback = callback
        CGL_window_set_framebuffer_size_callback(self.c_window, __pass_through_window_framebuffer_size_callback)
    
    def set_drag_n_drop_callback(self, callback: Callable):
        self.drag_n_drop_callback = callback
        CGL_window_set_drag_n_drop_callback(self.c_window, __pass_through_window_drag_n_drop_callback)

    def get_position(self) -> tuple:
        cdef int x, y
        CGL_window_get_position(self.c_window, &x, &y)
        return (x, y)
    
    def get_framebuffer_size(self) -> tuple:
        cdef int width, height
        CGL_window_get_framebuffer_size(self.c_window, &width, &height)
        return (width, height)

    def resecure_callbaks(self):
        CGL_window_resecure_callbacks(self.c_window)
    
    def make_context_current(self):
        CGL_window_make_context_current(self.c_window)
    
    def get_glfw_handle(self) -> object:
        return <object>CGL_window_get_glfw_handle(self.c_window)
    
    def get_key(self, key: int) -> int:
        return CGL_window_get_key(self.c_window, key)
    
    def is_key_pressed(self, key: int) -> bool:
        return CGL_window_is_key_pressed(self.c_window, key)

    def get_mouse_button(self, button: int) -> int:
        return CGL_window_get_mouse_button(self.c_window, button)
    
    def get_cursor_pos(self) -> tuple:
        cdef double x, y
        CGL_window_get_mouse_position(self.c_window, &x, &y)
        return (x, y)
    
    def __repr__(self):
        return f"window({self.width}, {self.height}, {self.title})"
    
    def __str__(self):
        return f"window({self.width}, {self.height}, {self.title})"
    
cdef class image:
    cdef CGL_image c_image
    cdef object image_data

    @property
    def width(self) -> int:
        return self.c_image.width
    
    @property
    def height(self) -> int:
        return self.c_image.height
    
    @property
    def channels(self) -> int:
        return self.c_image.channels
    
    @property
    def data(self) -> object:
        return self.image_data

    def __init__(self):
        self.c_image = CGL_image()
    
    def __hash__(self):
        return hash(self.image_data)
    
    def __repr__(self):
        return f"image({self.width}, {self.height}, {self.channels})"
    
    def __str__(self):
        return f"image({self.width}, {self.height}, {self.channels})"
    
    def load_image(self, path: str):
        img = PIL.Image.open(path)
        self.c_image.width = img.width
        self.c_image.height = img.height
        self.c_image.channels = 3
        self.c_image.bytes_per_channel = 8
        self.image_data = img.tobytes()
        self.c_image.data = <void*>self.image_data

cdef class mesh_vertex:
    cdef CGL_mesh_vertex c_vertex

    @property
    def position(self) -> vec4:
        return vec4(self.c_vertex.position.x, self.c_vertex.position.y, self.c_vertex.position.z, self.c_vertex.position.w)
    
    @position.setter
    def position(self, value: vec4):
        self.c_vertex.position.x = value.x
        self.c_vertex.position.y = value.y
        self.c_vertex.position.z = value.z
        self.c_vertex.position.w = value.w
    
    @property
    def normal(self) -> vec4:
        return vec4(self.c_vertex.normal.x, self.c_vertex.normal.y, self.c_vertex.normal.z, self.c_vertex.normal.w)
    
    @normal.setter
    def normal(self, value: vec4):
        self.c_vertex.normal.x = value.x
        self.c_vertex.normal.y = value.y
        self.c_vertex.normal.z = value.z
        self.c_vertex.normal.w = value.w
    
    @property
    def texture_coordinates(self) -> vec4:
        return vec4(self.c_vertex.texture_coordinates.x, self.c_vertex.texture_coordinates.y, self.c_vertex.texture_coordinates.z, self.c_vertex.texture_coordinates.w)
    
    @texture_coordinates.setter
    def texture_coordinates(self, value: vec4):
        self.c_vertex.texture_coordinates.x = value.x
        self.c_vertex.texture_coordinates.y = value.y
        self.c_vertex.texture_coordinates.z = value.z
        self.c_vertex.texture_coordinates.w = value.w
    
    @property
    def bone_wieghts(self) -> vec4:
        return vec4(self.c_vertex.bone_wieghts.x, self.c_vertex.bone_wieghts.y, self.c_vertex.bone_wieghts.z, self.c_vertex.bone_wieghts.w)
    
    @bone_wieghts.setter
    def bone_wieghts(self, value: vec4):
        self.c_vertex.bone_wieghts.x = value.x
        self.c_vertex.bone_wieghts.y = value.y
        self.c_vertex.bone_wieghts.z = value.z
        self.c_vertex.bone_wieghts.w = value.w
    
    @property
    def bone_ids(self) -> ivec4:
        return ivec4(self.c_vertex.bone_ids.x, self.c_vertex.bone_ids.y, self.c_vertex.bone_ids.z, self.c_vertex.bone_ids.w)
    
    @bone_ids.setter
    def bone_ids(self, value: ivec4):
        self.c_vertex.bone_ids.x = value.x
        self.c_vertex.bone_ids.y = value.y
        self.c_vertex.bone_ids.z = value.z
        self.c_vertex.bone_ids.w = value.w
    
    def __init__(self, c_vertex = None):
        if c_vertex is not None:
            self.c_vertex = c_vertex
        else:
            self.c_vertex = CGL_mesh_vertex()
    
    def __hash__(self):
        return hash((self.position, self.normal, self.texture_coordinates, self.bone_wieghts, self.bone_ids))
    
    def __repr__(self):
        return f"mesh_vertex({self.position}, {self.normal}, {self.texture_coordinates}, {self.bone_wieghts}, {self.bone_ids})"
    
    def __str__(self):
        return f"mesh_vertex({self.position}, {self.normal}, {self.texture_coordinates}, {self.bone_wieghts}, {self.bone_ids})"
    

cdef class mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh
    cdef bool has_been_destroyed

    @property
    def index_count(self) -> int:
        return self.c_mesh.index_count

    @property
    def index_count_used(self) -> int:
        return self.c_mesh.index_count_used
    
    @property
    def indices(self) -> list:
        return [self.c_mesh.indices[i] for i in range(self.index_count_used)]
    
    @indices.setter
    def indices(self, value: list):
        if len(value) > self.index_count:
            raise ValueError("The number of indices is greater than the maximum number of indices")
        self.c_mesh.index_count_used = len(value)
        for i in range(self.index_count_used):
            self.c_mesh.indices[i] = value[i]
    
    @property
    def vertex_count(self) -> int:
        return self.c_mesh.vertex_count
    
    @property
    def vertex_count_used(self) -> int:
        return self.c_mesh.vertex_count_used
    
    @property
    def vertices(self) -> list:
        return [mesh_vertex(self.c_mesh.vertices[i]) for i in range(self.vertex_count_used)]

    def __init__(self):
        self.c_mesh = NULL
        self.has_been_destroyed = True
    
    def __dealloc__(self):
        self.destroy()
    
    def __hash__(self):
        return <int><CGL_ulong>self.c_mesh
    
    def __repr__(self):
        return f"mesh_cpu({self.index_count}, {self.index_count_used}, {self.vertex_count}, {self.vertex_count_used})"
    
    def __str__(self):
        return f"mesh_cpu({self.index_count}, {self.index_count_used}, {self.vertex_count}, {self.vertex_count_used})"

    cdef set_c_mesh(self, CGL_mesh_cpu* c_mesh):
        if not self.has_been_destroyed:
            self.destroy()
        self.c_mesh = c_mesh
        self.has_been_destroyed = False

    def destroy(self):
        if not self.has_been_destroyed:
            self.has_been_destroyed = True
            CGL_mesh_cpu_destroy(self.c_mesh)

    def recalculate_normals(self):
        CGL_mesh_cpu_recalculate_normals(self.c_mesh)

    def load_obj(self, path: str):
        self.c_mesh = CGL_mesh_cpu_load_obj(path.encode("utf-8"))
        if self.c_mesh == NULL:
            raise RuntimeError("Failed to load mesh from obj file")
        self.has_been_destroyed = False
    
    def add_mesh(self, mesh: mesh_cpu):
        CGL_mesh_cpu_add_mesh(self.c_mesh, mesh.c_mesh)
    
    def add_cube(self, use_texture_coordinates: bool = True):
        CGL_mesh_cpu_add_cube(self.c_mesh, use_texture_coordinates)
    
    def add_triangle(self, a: vec3, b: vec3, c: vec3):
        CGL_mesh_cpu_add_triangle(self.c_mesh, a.c_vec3, b.c_vec3, c.c_vec3)
    
    def add_quad(self, a: vec3, b: vec3, c: vec3, d: vec3):
        CGL_mesh_cpu_add_quad(self.c_mesh, a.c_vec3, b.c_vec3, c.c_vec3, d.c_vec3)
    
    def add_sphere(self, res_u: int, res_v: int):
        CGL_mesh_cpu_add_sphere(self.c_mesh, res_u, res_v)

    def add_cylinder(self, start: vec3, end: vec3, radius0: float, radius1: float, res: int):
        CGL_mesh_cpu_add_cylinder(self.c_mesh, start.c_vec3, end.c_vec3, radius0, radius1, res)

    def offset_vetices(self, offset: vec3):
        CGL_mesh_cpu_offset_vertices(self.c_mesh, offset.c_vec3)
    
    def scale_vertices(self, scale: float):
        CGL_mesh_cpu_scale_vertices(self.c_mesh, scale)
    
    # def rotate_vertices(self, rotation: quat):
    #     CGL_mesh_cpu_rotate_vertices(self.c_mesh, rotation.c_quat)
    
    def transform_vertices(self, transform: mat4):
        CGL_mesh_cpu_transform_vertices(self.c_mesh, transform.c_mat4)
    
    def get_c_initialization_code(self, function_name: str = "load_object") -> str:
        cdef CGL_byte* buff = <CGL_byte*>stdlib.malloc(1024 * 1024 * 4)
        CGL_mesh_cpu_generate_c_initialization_code(self.c_mesh, buff, function_name.encode("utf-8"))
        byts = <bytes>buff
        result = copy.deepcopy(byts)
        stdlib.free(buff)
        return result.decode("utf-8")


def mesh_cpu_create(index_count: int, vertex_count: int) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_create(index_count, vertex_count)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_load_obj(path: str) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_load_obj(path.encode("utf-8"))
    if c_mesh == NULL:
        raise RuntimeError("Failed to load mesh from obj file")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_triangle(a: vec3, b: vec3, c: vec3) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_triangle(a.c_vec3, b.c_vec3, c.c_vec3)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create triangle mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_quad(a: vec3, b: vec3, c: vec3, d: vec3) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_quad(a.c_vec3, b.c_vec3, c.c_vec3, d.c_vec3)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create quad mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_plane(front: vec3, right: vec3, resolution: int, scale: float) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_plane(front.c_vec3, right.c_vec3, resolution, scale)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create plane mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_cube(use_3d_tex_coords: bool) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_cube(use_3d_tex_coords)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create cube mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_sphere(res_u: int, res_v: int) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_sphere(res_u, res_v)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create sphere mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

def mesh_cpu_cylinder(start: vec3, end: vec3, radius0: float, radius1: float, resolution: int) -> mesh_cpu:
    cdef CGL_mesh_cpu* c_mesh = CGL_mesh_cpu_create_cylinder(start.c_vec3, end.c_vec3, radius0, radius1, resolution)
    if c_mesh == NULL:
        raise RuntimeError("Failed to create cylinder mesh")
    res = mesh_cpu()
    res.set_c_mesh(c_mesh)
    return res

CUBEMAP_POSITIVE_X = 0x8515 
CUBEMAP_NEGATIVE_X = 0x8516 
CUBEMAP_POSITIVE_Y = 0x8517 
CUBEMAP_NEGATIVE_Y = 0x8518 
CUBEMAP_POSITIVE_Z = 0x8519 
CUBEMAP_NEGATIVE_Z = 0x851A 

cdef class texture:
    cdef CGL_texture* c_texture
    cdef bool has_been_destroyed
    
    @property
    def width(self):
        return self.get_size()[0]
    
    @property
    def height(self):
        return self.get_size()[1]

    def __init__(self):
        self.c_texture = NULL
        self.has_been_destroyed = True
    
    def __dealloc__(self):
        self.destroy()
    
    def __hash__(self):
        return self.get_internal_handle()
    
    def __repr__(self):
        return f"texture({self.get_internal_handle()})"
    
    def __eq__(self, other):
        if isinstance(other, texture):
            return self.get_internal_handle() == other.get_internal_handle()
        return False
    
    def __str__(self):
        return f"texture({self.get_internal_handle()})"
    
    cdef set_c_texture(self, CGL_texture* c_texture):
        if not self.has_been_destroyed:
            self.destroy()
        self.c_texture = c_texture
        self.has_been_destroyed = False       
    
    def destroy(self):
        if not self.has_been_destroyed:
            CGL_texture_destroy(self.c_texture)
            self.has_been_destroyed = True
    
    def load_image(self, image: image):
        self.c_texture = CGL_texture_create(&image.c_image)
        if self.c_texture == NULL:
            raise RuntimeError("Failed to create texture from image")
        self.has_been_destroyed = False
    
    def bind(self, slot: int):
        CGL_texture_bind(self.c_texture, slot)
    
    def cubemap_set_face(self, face: int, image: image):
        CGL_texture_cubemap_set_face(self.c_texture, face, &image.c_image)
    
    def array_set_layer_data(self, layer: int, data: bytes):
        CGL_texture_array_set_layer_data(self.c_texture, layer, <void*>data)
    
    def set_data(self, data: bytes):
        CGL_texture_set_data(self.c_texture, <void*>data)
    
    def set_sub_data(self, x: int, y: int, width: int, height: int, data: bytes):
        CGL_texture_set_sub_data(self.c_texture, x, y, width, height, <void*>data)
    
    def set_pixel_data(self, x: int, y: int, data: bytes):
        CGL_texture_set_pixel_data(self.c_texture, x, y, <void*>data)
    
    def set_user_data(self, data: object):
        CGL_texture_set_user_data(self.c_texture, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_texture_get_user_data(self.c_texture)
    
    def get_internal_handle(self) -> int:
        return CGL_texture_get_internal_handle(self.c_texture)
    
    def get_size(self) -> tuple:
        cdef CGL_int width
        cdef CGL_int height
        CGL_texture_get_size(self.c_texture, &width, &height)
        return (width, height)

    def set_scaling_method(self, scaling_method: int):
        CGL_texture_set_scaling_method(self.c_texture, scaling_method)
    
    def set_wrapping_method(self, wrapping_method: int):
        CGL_texture_set_wrapping_method(self.c_texture, wrapping_method)
    


def texture_create_from_image(image: image) -> texture:
    cdef CGL_texture* c_texture = CGL_texture_create(&image.c_image)
    if c_texture == NULL:
        raise RuntimeError("Failed to create texture from image")
    res = texture()
    res.set_c_texture(c_texture)
    return res

def texture_create_blank(width: int, height: int, formatf: GLenum, internal_format: GLenum, typef: GLenum) -> texture:
    cdef CGL_texture* c_texture = CGL_texture_create_blank(width, height, formatf, internal_format, typef)
    if c_texture == NULL:
        raise RuntimeError("Failed to create blank texture")
    res = texture()
    res.set_c_texture(c_texture)
    return res

def texture_create_array(width: int, height: int, depth: int, formatf: GLenum, internal_format: GLenum, typef: GLenum) -> texture:
    cdef CGL_texture* c_texture = CGL_texture_create_array(width, height, depth, formatf, internal_format, typef)
    if c_texture == NULL:
        raise RuntimeError("Failed to create texture array")
    res = texture()
    res.set_c_texture(c_texture)
    return res

def texture_creae_3d(width: int, height: int, depth: int, formatf: GLenum, internal_format: GLenum, typef: GLenum) -> texture:
    cdef CGL_texture* c_texture = CGL_texture_create_3d(width, height, depth, formatf, internal_format, typef)
    if c_texture == NULL:
        raise RuntimeError("Failed to create 3d texture")
    res = texture()
    res.set_c_texture(c_texture)
    return res

def texture_create_cubemap() -> texture:
    cdef CGL_texture* c_texture = CGL_texture_create_cubemap()
    if c_texture == NULL:
        raise RuntimeError("Failed to create cubemap texture")
    res = texture()
    res.set_c_texture(c_texture)
    return res

cdef class framebuffer:
    cdef CGL_framebuffer* c_framebuffer
    cdef bool has_been_destroyed

    def __init__(self):
        self.c_framebuffer = NULL
        self.has_been_destroyed = True
    
    def __dealloc__(self):
        self.destroy()
    
    def __repr__(self):
        return f"framebuffer({self.get_id()})"
    
    def __str__(self):
        return f"framebuffer({self.get_id()})"
    
    def __hash__(self):
        return self.get_id()

    def destroy(self):
        if not self.has_been_destroyed:
            CGL_framebuffer_destroy(self.c_framebuffer)
            self.has_been_destroyed = True
    
    def get_id(self) -> int:
        return CGL_utils_super_fast_hash(&self.c_framebuffer, sizeof(CGL_framebuffer*))
    
    cdef set_c_framebuffer(self, CGL_framebuffer* c_framebuffer):
        if not self.has_been_destroyed:
            self.destroy()
        self.c_framebuffer = c_framebuffer
        self.has_been_destroyed = False
    
    def add_color_attachment(self, tex: texture):
        CGL_framebuffer_add_color_attachment(self.c_framebuffer, tex.c_texture)
    
    def get_color_attacment(self, index: int) -> texture:
        cdef CGL_texture* c_texture = CGL_framebuffer_get_color_attachment(self.c_framebuffer, index)
        if c_texture == NULL:
            raise RuntimeError("Failed to get color attachment")
        res = texture()
        res.set_c_texture(c_texture)
        return res
    
    def bind(self):
        CGL_framebuffer_bind(self.c_framebuffer)

    def set_user_data(self, data: object):
        CGL_framebuffer_set_user_data(self.c_framebuffer, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_framebuffer_get_user_data(self.c_framebuffer)
    
    def read_pixels(self, x: int, y: int, width: int, height: int) -> bytes:
        cdef CGL_int size = width * height * 4
        cdef void* data = stdlib.malloc(size)
        CGL_framebuffer_read_pixels(self.c_framebuffer, x, y, width, height, data)
        temp = <bytes>data
        res = copy.deepcopy(temp)
        stdlib.free(data)
        return res

    def get_mouse_pick_id(self, x: int, y: int, index: int) -> int:
        return CGL_framebuffer_get_mouse_pick_id(self.c_framebuffer, x, y, index)
    
    def get_color_texture(self) -> texture:
        cdef CGL_texture* c_texture = CGL_framebuffer_get_color_texture(self.c_framebuffer)
        if c_texture == NULL:
            raise RuntimeError("Failed to get color texture")
        res = texture()
        res.set_c_texture(c_texture)
        return res

    def get_depth_texture(self) -> texture:
        cdef CGL_texture* c_texture = CGL_framebuffer_get_depth_texture(self.c_framebuffer)
        if c_texture == NULL:
            raise RuntimeError("Failed to get depth texture")
        res = texture()
        res.set_c_texture(c_texture)
        return res

def framebuffer_create_from_default(wnd: window) -> framebuffer:
    cdef CGL_framebuffer* c_framebuffer = CGL_framebuffer_create_from_default(wnd.c_window)
    if c_framebuffer == NULL:
        raise RuntimeError("Failed to create framebuffer from default")
    res = framebuffer()
    res.set_c_framebuffer(c_framebuffer)
    return res

def framebuffer_create(width: int, height: int) -> framebuffer:
    cdef CGL_framebuffer* c_framebuffer = CGL_framebuffer_create(width, height)
    if c_framebuffer == NULL:
        raise RuntimeError("Failed to create framebuffer")
    res = framebuffer()
    res.set_c_framebuffer(c_framebuffer)
    return res

def framebuffer_create_basic(width: int, height: int) -> framebuffer:
    cdef CGL_framebuffer* c_framebuffer = CGL_framebuffer_create_basic(width, height)
    if c_framebuffer == NULL:
        raise RuntimeError("Failed to create basic framebuffer")
    res = framebuffer()
    res.set_c_framebuffer(c_framebuffer)
    return res

def gl_clear(r: float, g: float, b: float, a: float):
    CGL_gl_clear(r, g, b, a)

def gl_init() -> bool:
    return CGL_gl_init()

def gl_shutdown():
    CGL_gl_shutdown()

def gl_render_screen_quad():
    CGL_gl_render_screen_quad()

cdef class mesh_gpu:
    cdef CGL_mesh_gpu* c_mesh
    cdef bool has_been_destroyed
    cdef str name

    @property
    def name(self):
        return self.name

    def __init__(self, name: str = "GPU Mesh"):
        self.c_mesh = CGL_mesh_gpu_create()
        self.has_been_destroyed = False
        self.name = name
    
    def __dealloc__(self):
        self.destroy()
    
    def __repr__(self):
        return f"mesh_gpu({self.name})"
    
    def __str__(self):  
        return f"mesh_gpu({self.name})"
    
    def __hash__(self):
        return self.get_id()
    
    def destroy(self):
        if not self.has_been_destroyed:
            CGL_mesh_gpu_destroy(self.c_mesh)
            self.has_been_destroyed = True
    
    cdef set_c_mesh(self, CGL_mesh_gpu* c_mesh):
        if not self.has_been_destroyed:
            self.destroy()
        self.c_mesh = c_mesh
        self.has_been_destroyed = False
    
    def get_id(self) -> int:
        return CGL_utils_super_fast_hash(&self.c_mesh, sizeof(CGL_mesh_gpu*))
    
    def render(self):
        CGL_mesh_gpu_render(self.c_mesh)
    
    def render_instanced(self, count: int):
        CGL_mesh_gpu_render_instanced(self.c_mesh, count)
    
    def set_user_data(self, data: object):
        CGL_mesh_gpu_set_user_data(self.c_mesh, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_mesh_gpu_get_user_data(self.c_mesh)
    
    def upload_mesh_cpu(self, mesh: mesh_cpu, static_draw: bool = True):
        CGL_mesh_gpu_upload(self.c_mesh, mesh.c_mesh, static_draw)
    

cdef class shader:
    cdef CGL_shader* c_shader
    cdef bool has_been_destroyed
    cdef str name
    cdef dict uniform_map

    @property
    def name(self):
        return self.name

    def __init__(self, name: str = "Shader"):
        self.has_been_destroyed = True
        self.name = name
        self.uniform_map = {}
    
    def __dealloc__(self):
        self.destroy()
    
    def __repr__(self):
        return f"shader({self.name})"
    
    def __str__(self):  
        return f"shader({self.name})"
    
    def __hash__(self):
        return self.get_id()
    
    def destroy(self):
        if not self.has_been_destroyed:
            CGL_shader_destroy(self.c_shader)
            self.has_been_destroyed = True
        self.uniform_map = {}
    
    cdef set_c_shader(self, CGL_shader* c_shader):
        if not self.has_been_destroyed:
            self.destroy()
        self.c_shader = c_shader
        self.has_been_destroyed = False
    
    def get_id(self) -> int:
        return CGL_utils_super_fast_hash(&self.c_shader, sizeof(CGL_shader*))
    
    def set_user_data(self, data: object):
        CGL_shader_set_user_data(self.c_shader, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_shader_get_user_data(self.c_shader)
    
    def load_from_file(self, vertex_shader_path: str, fragment_shader_path: str) -> bool:
        cdef CGL_shader* c_shader = CGL_shader_create_from_files(vertex_shader_path.encode("utf-8"), fragment_shader_path.encode("utf-8"), NULL)
        if c_shader == NULL:
            raise RuntimeError("Failed to create shader")
        self.set_c_shader(c_shader)
        has_been_destroyed = False
        return True
    
    def load(self, vertex_shader_source: str, fragment_shader_source: str) -> bool:
        cdef CGL_shader* c_shader = CGL_shader_create(vertex_shader_source.encode("utf-8"), fragment_shader_source.encode("utf-8"), NULL)
        if c_shader == NULL:
            raise RuntimeError("Failed to create shader")
        self.set_c_shader(c_shader)
        has_been_destroyed = False
        return True
    
    def load_compute_from_file(self, compute_shader_path: str) -> bool:
        cdef CGL_shader* c_shader = CGL_shader_compute_create_from_files(compute_shader_path.encode("utf-8"), NULL)
        if c_shader == NULL:
            raise RuntimeError("Failed to create shader")
        self.set_c_shader(c_shader)
        has_been_destroyed = False
        return True
    
    def load_compute(self, compute_shader_source: str) -> bool:
        cdef CGL_shader* c_shader = CGL_shader_compute_create(compute_shader_source.encode("utf-8"), NULL)
        if c_shader == NULL:
            raise RuntimeError("Failed to create shader")
        self.set_c_shader(c_shader)
        has_been_destroyed = False
        return True

    def bind(self):
        CGL_shader_bind(self.c_shader)
    
    def get_uniform_location(self, name: str) -> int:
        if name in self.uniform_map:
            return self.uniform_map[name]
        cdef int location = CGL_shader_get_uniform_location(self.c_shader, name.encode("utf-8"))
        self.uniform_map[name] = location
        return location
    
    def set_uniform_mat4(self, name: str, mat: mat4):
        CGL_shader_set_uniform_mat4(self.c_shader, self.get_uniform_location(name), &mat.c_mat4)
    
    def set_uniform_vec4(self, name: str, vec: vec4):
        CGL_shader_set_uniform_vec4(self.c_shader, self.get_uniform_location(name), &vec.c_vec4)
    
    def set_uniform_vec3(self, name: str, vec: vec3):
        CGL_shader_set_uniform_vec3(self.c_shader, self.get_uniform_location(name), &vec.c_vec3)
    
    def set_uniform_vec2(self, name: str, vec: vec2):
        CGL_shader_set_uniform_vec2(self.c_shader, self.get_uniform_location(name), &vec.c_vec2)    
    
    def set_uniform_float(self, name: str, value: float):
        CGL_shader_set_uniform_float(self.c_shader, self.get_uniform_location(name), value)
    
    def set_uniform_int(self, name: str, value: int):
        CGL_shader_set_uniform_int(self.c_shader, self.get_uniform_location(name), value)
    
    def set_uniform_bool(self, name: str, value: bool):
        CGL_shader_set_uniform_bool(self.c_shader, self.get_uniform_location(name), value)
    
    def set_uniform_doublt(self, name: str, value: double):
        CGL_shader_set_uniform_double(self.c_shader, self.get_uniform_location(name), value)
    
    def set_uniform_vec2v(self, name: str, x: float, y: float):
        CGL_shader_set_uniform_vec2v(self.c_shader, self.get_uniform_location(name), x, y)
    
    def set_uniform_vec3v(self, name: str, x: float, y: float, z: float):
        CGL_shader_set_uniform_vec3v(self.c_shader, self.get_uniform_location(name), x, y, z)
    
    def set_uniform_vec4v(self, name: str, x: float, y: float, z: float, w: float):
        CGL_shader_set_uniform_vec4v(self.c_shader, self.get_uniform_location(name), x, y, z, w)
    
    def set_uniform_ivec2v(self, name: str, x: int, y: int):
        CGL_shader_set_uniform_ivec2v(self.c_shader, self.get_uniform_location(name), x, y)
    
    def set_uniform_ivec3v(self, name: str, x: int, y: int, z: int):
        CGL_shader_set_uniform_ivec3v(self.c_shader, self.get_uniform_location(name), x, y, z)
    
    def set_uniform_ivec4v(self, name: str, x: int, y: int, z: int, w: int):
        CGL_shader_set_uniform_ivec4v(self.c_shader, self.get_uniform_location(name), x, y, z, w)   
    
    def compute_dispatch(self, x: int, y: int, z: int):
        CGL_shader_compute_dispatch(self.c_shader, x, y, z)

cdef class ssbo:
    cdef CGL_ssbo* c_ssbo
    cdef bool has_been_destroyed

    @property
    def size(self) -> int:
        return CGL_ssbo_get_size(self.c_ssbo)

    def __init__(self):
        self.c_ssbo = CGL_ssbo_create(0)
        self.has_been_destroyed = False
    
    def __dealloc__(self):
        if not self.has_been_destroyed:
            self.destroy()

    def __repr__(self):
        return f"ssbo({self.get_id()})"
    
    def __str__(self):
        return f"ssbo({self.get_id()})"
    
    def __hash__(self):
        return self.get_id()
    
    def bind(self):
        CGL_ssbo_bind(self.c_ssbo)
    
    def bind_base(self, index: int):
        CGL_ssbo_bind2(self.c_ssbo, index)
    
    def set_data(self, data: bytes, static_draw: bool = True):
        CGL_ssbo_set_data(self.c_ssbo, len(data), <void*>data,  static_draw)
    
    def set_sub_data(self, data: bytes, offset: int, static_draw: bool = True):
        CGL_ssbo_set_sub_data(self.c_ssbo, offset, len(data), <void*>data, static_draw)

    def get_data(self, max_size = 100000000) -> bytes:
        cdef size_t size
        cdef void* buff = stdlib.malloc(max_size)
        CGL_ssbo_get_data(self.c_ssbo, &size, buff)
        byts = <bytes>buff
        result = copy.deepcopy(byts)
        stdlib.free(buff)
        return result
    
    def get_sub_data(self, offset: int, size) -> bytes:
        cdef void* buff = stdlib.malloc(size)
        CGL_ssbo_get_sub_data(self.c_ssbo, offset, size, buff)
        byts = <bytes>buff
        result = copy.deepcopy(byts)
        stdlib.free(buff)
        return result
    
    def get_size(self) -> int:
        return CGL_ssbo_get_size(self.c_ssbo)
    
    def set_user_data(self, data):
        CGL_ssbo_set_user_data(self.c_ssbo, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_ssbo_get_user_data(self.c_ssbo)

    def get_id(self) -> int:
        return CGL_utils_super_fast_hash(&self.c_ssbo, sizeof(CGL_ssbo*))
    

    def destroy(self):
        if not self.has_been_destroyed:
            CGL_ssbo_destroy(self.c_ssbo)
            self.has_been_destroyed = True
    
def ssbo_copy(dst: ssbo, src: ssbo, src_offset: int, dst_offset: int, size: int):
    CGL_ssbo_copy(dst.c_ssbo, src.c_ssbo, src_offset, dst_offset, size)

cdef class ubo:
    cdef CGL_ubo* c_ubo
    cdef bool has_been_destroyed

    @property
    def size(self) -> int:
        return CGL_ubo_get_size(self.c_ubo)
    
    def __init__(self):
        self.c_ubo = CGL_ubo_create()
        self.has_been_destroyed = False
    
    def __dealloc__(self):
        self.destroy()
    
    def __repr__(self):
        return f"ubo({self.get_id()})"
    
    def __str__(self):
        return f"ubo({self.get_id()})"
    
    def __hash__(self):
        return self.get_id()
    
    def destroy(self):
        if not self.has_been_destroyed:
            CGL_ubo_destroy(self.c_ubo)
            self.has_been_destroyed = True

    def bind(self, shd: shader, name: str, binding: int):
        CGL_ubo_bind(self.c_ubo, shd.c_shader, name.encode("utf-8"), binding)
    
    def set_data(self, data: bytes, static_draw: bool = True):
        CGL_ubo_set_data(self.c_ubo, len(data), <void*>data, static_draw)
    
    def set_sub_data(self, offset: int, data: bytes, static_draw: bool = True):
        CGL_ubo_set_sub_data(self.c_ubo, offset, len(data), <void*>data, static_draw)
    
    def get_data(self, max_size: int = 100000000) -> bytes:
        cdef size_t size
        cdef void* buff = stdlib.malloc(max_size)
        CGL_ubo_get_data(self.c_ubo, &size, buff)
        byts = <bytes>buff
        result = copy.deepcopy(byts)
        stdlib.free(buff)
        return result
    
    def get_sub_data(self, offset: int, size: int) -> bytes:
        cdef void* buff = stdlib.malloc(size)
        CGL_ubo_get_sub_data(self.c_ubo, offset, size, buff)
        byts = <bytes>buff
        result = copy.deepcopy(byts)
        stdlib.free(buff)
        return result
    
    def get_size(self) -> int:
        return CGL_ubo_get_size(self.c_ubo)
    
    def set_user_data(self, data: object):
        CGL_ubo_set_user_data(self.c_ubo, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_ubo_get_user_data(self.c_ubo)
    
    def get_id(self) -> int:
        return CGL_utils_super_fast_hash(&self.c_ubo, sizeof(CGL_ubo*))


cdef class shape:
    cdef CGL_shape c_shape

    @property
    def vertices_count(self) -> int:
        return self.c_shape.vertices_count
    
    @property
    def vertices(self) -> list:
        result = []
        for i in range(self.vertices_count):
            result.append(vec3(self.c_shape.vertices[i].x, self.c_shape.vertices[i].y, self.c_shape.vertices[i].z))
        return result
    
    @property
    def position(self) -> vec3:
        return vec3(self.c_shape.position.x, self.c_shape.position.y, self.c_shape.position.z)
    
    @position.setter
    def position(self, value: vec3):
        self.c_shape.position.x = value.x
        self.c_shape.position.y = value.y
        self.c_shape.position.z = value.z
    
    @property
    def rotation(self) -> vec3:
        return vec3(self.c_shape.rotation.x, self.c_shape.rotation.y, self.c_shape.rotation.z)
    
    @rotation.setter
    def rotation(self, value: vec3):
        self.c_shape.rotation.x = value.x
        self.c_shape.rotation.y = value.y
        self.c_shape.rotation.z = value.z
    
    @property
    def scale(self) -> vec3:
        return vec3(self.c_shape.scale.x, self.c_shape.scale.y, self.c_shape.scale.z)
    
    @scale.setter
    def scale(self, value: vec3):
        self.c_shape.scale.x = value.x
        self.c_shape.scale.y = value.y
        self.c_shape.scale.z = value.z

    def __init__(self, vertices_count: int):
        CGL_shape_init(&self.c_shape, vertices_count)
    
    def __dealloc__(self):
        CGL_shape_destroy(&self.c_shape)
    
    def __repr__(self):
        return f"shape({self.vertices_count}, {self.position}, {self.rotation}, {self.scale})"
    
    def __str__(self):
        return f"shape({self.vertices_count}, {self.position}, {self.rotation}, {self.scale})"
    
    def __getitem__(self, index: int) -> vec3:
        return vec3(self.c_shape.vertices[index].x, self.c_shape.vertices[index].y, self.c_shape.vertices[index].z)
    
    def __setitem__(self, index: int, value: vec3):
        self.c_shape.vertices[index].x = value.x
        self.c_shape.vertices[index].y = value.y
        self.c_shape.vertices[index].z = value.z
    
    def __len__(self):
        return self.vertices_count

def sat_collison_overlap_on_axis(a: shape, b: shape, axis: vec2) -> (bool, float):
    cdef float overlap_amount
    result = CGL_sat_collision_overlap_on_axis(&a.c_shape, &b.c_shape, axis.c_vec2, &overlap_amount)
    return (result, overlap_amount)

def  sat_collision_detect(a: shape, b: shape) -> tuple:
    n_vector = vec2(0, 0)
    result = CGL_sat_collision_detect(&a.c_shape, &b.c_shape, &n_vector.c_vec2)
    return (result, n_vector)

def sat_collision_calculate_axes(a: shape, max_axes: int = 10000) -> list:
    cdef int axes_count
    cdef CGL_vec2* axes = <CGL_vec2*>stdlib.malloc(max_axes * sizeof(CGL_vec2))
    CGL_sat_collision_calculate_axes(&a.c_shape, axes, &axes_count)
    result = []
    for i in range(axes_count):
        result.append(vec2(axes[i].x, axes[i].y))
    stdlib.free(axes)
    return result

def is_point_in_circle(point: vec2, circle_radius: float) -> bool:
    return CGL_utils_is_point_in_circle(point.c_vec2, circle_radius)

def is_point_in_triangle(point: vec2, a: vec2, b: vec2, c: vec2) -> bool:
    return CGL_utils_is_point_in_triangle(point.c_vec2, a.c_vec2, b.c_vec2, c.c_vec2)

def calculate_circumcircle(a: vec2, b: vec2, c: vec2) -> tuple:
    center = vec2(0, 0)
    cdef float f_radius = 0
    CGL_utils_calculate_circumcircle(a.c_vec2, b.c_vec2, c.c_vec2, &center.c_vec2, &f_radius)
    return (center, f_radius)

def calculate_super_triangle(vertices: list, padding: float) -> tuple:
    cdef CGL_vec2* c_vertices = <CGL_vec2*>stdlib.malloc(len(vertices) * sizeof(CGL_vec2))
    for i in range(len(vertices)):
        c_vertices[i].x = vertices[i].x
        c_vertices[i].y = vertices[i].y
    r_a = vec2(0, 0)
    r_b = vec2(0, 0)
    r_c = vec2(0, 0)
    CGL_utils_calculate_super_triangle(c_vertices, len(vertices), &r_a.c_vec2, &r_b.c_vec2, &r_c.c_vec2, padding)
    stdlib.free(c_vertices)
    return (r_a, r_b, r_c)

def calculate_bounding_square(vertices: list, padding: float) -> tuple:
    cdef CGL_vec2* c_vertices = <CGL_vec2*>stdlib.malloc(len(vertices) * sizeof(CGL_vec2))
    for i in range(len(vertices)):
        c_vertices[i].x = vertices[i].x
        c_vertices[i].y = vertices[i].y
    r_a = vec2(0, 0)
    r_b = vec2(0, 0)
    r_c = vec2(0, 0)
    r_d = vec2(0, 0)
    CGL_utils_calculate_bounding_square(c_vertices, len(vertices), &r_a.c_vec2, &r_b.c_vec2, &r_c.c_vec2, &r_d.c_vec2, padding)
    stdlib.free(c_vertices)
    return (r_a, r_b, r_c, r_d)

def calculate_bounding_box(vertices: list, padding: float) -> tuple:
    cdef CGL_vec2* c_vertices = <CGL_vec2*>stdlib.malloc(len(vertices) * sizeof(CGL_vec2))
    for i in range(len(vertices)):
        c_vertices[i].x = vertices[i].x
        c_vertices[i].y = vertices[i].y
    r_a = vec2(0, 0)
    r_b = vec2(0, 0)
    r_c = vec2(0, 0)
    r_d = vec2(0, 0)
    CGL_utils_calculate_bounding_box(c_vertices, len(vertices), &r_a.c_vec2, &r_b.c_vec2, &r_c.c_vec2, &r_d.c_vec2, padding)
    stdlib.free(c_vertices)
    return (r_a, r_b, r_c, r_d)

def random_gaussian(mean: float, std_dev: float) -> float:
    return CGL_utils_random_gaussian(mean, std_dev)

def gjk_shape_default_support(a: shape, d: vec3) -> vec3:
    return CGL_gjk_shape_default_support(&a.c_shape, d.c_vec3)

def gjk_default_support(a: shape, b: shape, d: vec3) -> vec3:
    return CGL_gjk_default_support(&a.c_shape, &b.c_shape, d.c_vec3)

def gjk_check_collision_2d(a: shape, b: shape) -> tuple:
    cdef CGL_vec3* simplex_out_ptr = <CGL_vec3*>stdlib.malloc(sizeof(CGL_vec3) * 3)
    result = CGL_gjk_check_collision_2d(&a.c_shape, &b.c_shape, simplex_out_ptr)
    simplex_out = []
    for i in range(3):
        simplex_out.append(vec3(simplex_out_ptr[i].x, simplex_out_ptr[i].y, simplex_out_ptr[i].z))
    stdlib.free(simplex_out_ptr)
    return (result, simplex_out)

def gjk_epa_2d(a: shape, b: shape, simplex: list) ->  vec3:
    cdef CGL_vec3* simplex_ptr = <CGL_vec3*>stdlib.malloc(sizeof(CGL_vec3) * 3)
    for i in range(3):
        simplex_ptr[i].x = simplex[i].x
        simplex_ptr[i].y = simplex[i].y
        simplex_ptr[i].z = simplex[i].z
    result = CGL_gjk_epa_2d(&a.c_shape, &b.c_shape, simplex_ptr)
    stdlib.free(simplex_ptr)
    return vec3(result.x, result.y, result.z)

def aabb_contains_point(aabb_min: vec2, aabb_max: vec2, point: vec2) -> bool:
    return CGL_aabb_contains_point(aabb_min.c_vec2, aabb_max.c_vec2, point.c_vec2)

def aabb_intersects_aabb(aabb_min_a: vec2, aabb_max_a: vec2, aabb_min_b: vec2, aabb_max_b: vec2) -> bool:
    return CGL_aabb_intersects_aabb(aabb_min_a.c_vec2, aabb_max_a.c_vec2, aabb_min_b.c_vec2, aabb_max_b.c_vec2)

def triangulate_points_incremental(points: list) -> list:
    cdef CGL_vec2* c_points = <CGL_vec2*>stdlib.malloc(len(points) * sizeof(CGL_vec2))
    for i in range(len(points)):
        c_points[i].x = points[i].x
        c_points[i].y = points[i].y
    cdef int* c_indices = <int*>stdlib.malloc(len(points) * sizeof(int) * 3)
    cdef int c_num_indices = 0
    CGL_triangulate_points_incremental(c_points, len(points), c_indices, &c_num_indices)
    indices = []
    for i in range(c_num_indices / 3):
        indices.append((c_indices[i * 3], c_indices[i * 3 + 1], c_indices[i * 3 + 2]))
    stdlib.free(c_points)
    stdlib.free(c_indices)
    return indices

BLOOM_SHADER_MODE_PREFILTER    = 0
BLOOM_SHADER_MODE_DOWNSAMPLE   = 1
BLOOM_SHADER_MODE_UPSAMPLE     = 2
BLOOM_SHADER_MODE_COMPOSITE    = 3

cdef class bloom:
    cdef CGL_bloom* c_bloom
    cdef int width
    cdef int height
    cdef int iterations

    @property
    def width(self):
        return self.width
    
    @property
    def height(self):
        return self.height
    
    @property
    def iterations(self):
        return self.iterations
    

    @property
    def threshold(self):
        return CGL_bloom_get_threshold(self.c_bloom)
    
    @threshold.setter
    def threshold(self, threshold: float):
        CGL_bloom_set_threshold(self.c_bloom, threshold)
    
    @property
    def knee(self):
        return CGL_bloom_get_knee(self.c_bloom)
    
    @knee.setter
    def knee(self, knee: float):
        CGL_bloom_set_knee(self.c_bloom, knee)

    def __init__(self, width: int, height: int, iterations: int):
        self.c_bloom = CGL_bloom_create(width, height, iterations)
        self.width = width
        self.height = height
        self.iterations = iterations
    
    def __dealloc__(self):
        CGL_bloom_destroy(self.c_bloom)
    
    def __repr__(self):
        return f"bloom({self.width}, {self.height}, {self.iterations})"
    
    def __str__(self):
        return f"bloom({self.width}, {self.height}, {self.iterations})"
    
    def __hash__(self):
        return CGL_utils_super_fast_hash(<void*>self.c_bloom, sizeof(CGL_bloom*))

    def set_threshold(self, threshold: float):
        CGL_bloom_set_threshold(self.c_bloom, threshold)
    
    def get_threshold(self) -> float:
        return CGL_bloom_get_threshold(self.c_bloom)
    
    def set_knee(self, knee: float):
        CGL_bloom_set_knee(self.c_bloom, knee)
    
    def get_knee(self) -> float:
        return CGL_bloom_get_knee(self.c_bloom)
    
    def set_offset(self, offset_x: float, offset_y: float):
        CGL_bloom_set_offset(self.c_bloom, offset_x, offset_y)

    def apply(self, tex: texture):
        CGL_bloom_apply(self.c_bloom, tex.c_texture)
    
    def apply2(self, texture_src: texture, texture_dst: texture):
        CGL_bloom_apply2(self.c_bloom, texture_src.c_texture, texture_dst.c_texture)
    
    def get_lod_texture(self, lod: int) -> texture:
        result = texture()
        result.set_c_texture(CGL_bloom_get_lod_texture(self.c_bloom, lod))
        # this is a hack to prevent the texture from 
        # being destroyed when the result goes out of scope
        # as this texture object is managed by the bloom object
        result.has_been_destroyed = True
        return result
    
    def get_prefiltered_texture(self) -> texture:
        result = texture()
        result.set_c_texture(CGL_bloom_get_prefiltered_texture(self.c_bloom))
        # this is a hack to prevent the texture from 
        # being destroyed when the result goes out of scope
        # as this texture object is managed by the bloom object
        result.has_been_destroyed = True
        return result
    
cdef class tilemap:
    cdef CGL_tilemap* c_tilemap
    cdef int tile_count_x
    cdef int tile_count_y
    cdef int tile_size_x
    cdef int tile_size_y
    cdef int ssbo_binding

    @property
    def tile_count_x(self):
        return self.tile_count_x
    
    @property
    def tile_count_y(self):
        return self.tile_count_y
    
    @property
    def tile_size_x(self):
        return self.tile_size_x
    
    @property
    def tile_size_y(self):
        return self.tile_size_y
    
    @property
    def ssbo_binding(self):
        return self.ssbo_binding
    
    @property
    def auto_upload(self):
        return self.get_auto_upload()

    @auto_upload.setter
    def auto_upload(self, auto_upload: bool):
        self.set_auto_upload(auto_upload)

    def __init__(self, tile_count_x: int, tile_count_y: int, tile_size_x: int, tile_size_y: int, ssbo_binding: int):
        self.c_tilemap = CGL_tilemap_create(tile_count_x, tile_count_y, tile_size_x, tile_size_y, ssbo_binding)
        self.tile_count_x = tile_count_x
        self.tile_count_y = tile_count_y
        self.tile_size_x = tile_size_x
        self.tile_size_y = tile_size_y
        self.ssbo_binding = ssbo_binding
    
    def __dealloc__(self):
        CGL_tilemap_destroy(self.c_tilemap)
    
    def __repr__(self):
        return f"tilemap({self.tile_count_x}, {self.tile_count_y}, {self.tile_size_x}, {self.tile_size_y}, {self.ssbo_binding})"
    
    def __str__(self):
        return f"tilemap({self.tile_count_x}, {self.tile_count_y}, {self.tile_size_x}, {self.tile_size_y}, {self.ssbo_binding})"
    
    def __hash__(self):
        return CGL_utils_super_fast_hash(<void*>self.c_tilemap, sizeof(CGL_tilemap*))
    
    def set_auto_upload(self, auto_upload: bool):
        CGL_tilemap_set_auto_upload(self.c_tilemap, auto_upload)
    
    def get_auto_upload(self) -> bool:
        return CGL_tilemap_get_auto_upload(self.c_tilemap)
    
    def upload(self):
        CGL_tilemap_upload(self.c_tilemap)
    
    def set_tile_color(self, tile_x: int, tile_y: int, r: float, g: float, b: float):
        CGL_tilemap_set_tile_color(self.c_tilemap, tile_x, tile_y, r, g, b)
    
    def set_tile_texture_from_array(self, tile_x: int, tile_y: int, texture_index: int):
        CGL_tilemap_set_tile_texture_from_array(self.c_tilemap, tile_x, tile_y, texture_index)
    
    def set_tile_texture_from_tileset(self, tile_x: int, tile_y: int, texture_x_min: float, texture_y_min: float, texture_x_max: float, texture_y_max: float):
        CGL_tilemap_set_tile_texture_from_tileset(self.c_tilemap, tile_x, tile_y, texture_x_min, texture_y_min, texture_x_max, texture_y_max)
    
    def set_all_tile_color(self, r: float, g: float, b: float):
        CGL_tilemap_set_all_tile_color(self.c_tilemap, r, g, b)
    
    def set_all_tile_texture_from_array(self, texture_index: int):
        CGL_tilemap_set_all_tile_texture_from_array(self.c_tilemap, texture_index)

    def set_all_tile_texture_from_tileset(self, texture_x_min: float, texture_y_min: float, texture_x_max: float, texture_y_max: float):
        CGL_tilemap_set_all_tile_texture_from_tileset(self.c_tilemap, texture_x_min, texture_y_min, texture_x_max, texture_y_max)
    
    def clear_tile(self, tile_x: int, tile_y: int):
        CGL_tilemap_clear_tile(self.c_tilemap, tile_x, tile_y)
    
    def clear_all_tile(self):
        CGL_tilemap_clear_all_tile(self.c_tilemap)
    
    def render(self, scale_x: float, scale_y: float, offset_x: float, offset_y: float, tex: texture = None):
        cdef CGL_texture* c_tex = NULL
        if tex is not None:
            c_tex = tex.c_texture
        CGL_tilemap_render(self.c_tilemap, scale_x, scale_y, offset_x, offset_y, c_tex)
    
    def reset(self):
        CGL_tilemap_reset(self.c_tilemap)
    
cdef class font_character:
    cdef CGL_font_character c_font_character

    @property
    def size(self) -> vec2:
        return vec2(self.c_font_character.size.x, self.c_font_character.size.y)
    
    @property
    def bearing(self) -> vec2:
        return vec2(self.c_font_character.bearing.x, self.c_font_character.bearing.y)
    
    @property
    def advance(self) -> vec2:
        return vec2(self.c_font_character.advance.x, self.c_font_character.advance.y)
    
    @property
    def normalized_size(self) -> vec2:
        return vec2(self.c_font_character.normalized_size.x, self.c_font_character.normalized_size.y)
    
    @property
    def normalized_offset(self) -> vec2:
        return vec2(self.c_font_character.normalized_offset.x, self.c_font_character.normalized_offset.y)
    
    @property
    def bearing_normalized(self) -> vec2:
        return vec2(self.c_font_character.bearing_normalized.x, self.c_font_character.bearing_normalized.y)

    @property
    def advance_normalized(self) -> vec2:
        return vec2(self.c_font_character.advance_normalized.x, self.c_font_character.advance_normalized.y)
    
    @property
    def character(self) -> char:
        return self.c_font_character.ch
    
    def __init__(self, CGL_font_character c_font_character):
        self.c_font_character = c_font_character
    
    def __repr__(self):
        return f"font_character({self.c_font_character})"
    
    def __str__(self):
        return f"font_character({self.c_font_character})"
    
    def __hash__(self):
        return self.c_font_character.ch
    
cdef class font:
    cdef CGL_font* c_font
    cdef bool has_been_destroyed
    cdef str meta_data

    def __init__(self):
        self.c_font = NULL
        self.has_been_destroyed = True
        self.meta_data = "not_loaded"
    
    def __dealloc__(self):
        self.destroy()
    
    def destroy(self):
        if not self.has_been_destroyed:
            CGL_font_destory(self.c_font)
            self.has_been_destroyed = True
        self.meta_data = "not_loaded"
    
    
    def load_from_memory(self, data: bytes):
        self.has_been_destroyed = False
        self.c_font = CGL_font_load_from_memory(data, len(data))
        self.meta_data = "memory_font"
    
    def load(self, path: str):
        self.has_been_destroyed = False
        self.c_font = CGL_font_load(path.encode("utf-8"))
        self.meta_data = str(f"file_font{path}")
    
    def get_atlas(self) -> texture:
        result = texture()
        result.set_c_texture(CGL_font_get_atlas(self.c_font))
        result.has_been_destroyed = True
        return result
    
    def build_atlas(self, width: int, height: int, font_size: int) -> bool:
        return CGL_font_build_atlas(self.c_font, width, height, font_size)
    
    def bake_to_texture(self, strng: str) -> tuple:
        result_tex = texture()
        cdef size_t width
        cdef size_t height
        result_tex.set_c_texture(CGL_text_bake_to_texture(strng.encode("utf-8"), len(strng), self.c_font, &width, &height))
        return (result_tex, width, height)
    
    def get_characters(self):
        cdef CGL_font_character* c_font_characters
        cdef list result = []
        c_font_characters = CGL_font_get_characters(self.c_font)
        for i in range(127):
            result.append(font_character(c_font_characters[i]))
        return result


def text_init() -> bool:
    return CGL_text_init()

def text_shutdown():
    CGL_text_shutdown()

   
cdef class widgets:

    def __init__(self):
        raise RuntimeError("widgets is a static class")

    @staticmethod
    def create_context(max_vertices: int, max_indices: int) -> object:
        return <object>CGL_widgets_context_create(max_vertices, max_indices)
    
    @staticmethod
    def destroy_context(ctx: object):
        CGL_widgets_context_destory(<CGL_widgets_context*>ctx)
    
    @staticmethod
    def get_current_context() -> object:
        # NOTE: this is a type in the original C code,
        #       not some kind of a bug here
        #       and will be later fixed
        return <object>CGL_window_get_current_context()
    
    @staticmethod
    def set_current_context(ctx: object):
        # NOTE: this is a type in the original C code,
        #       not some kind of a bug here
        #       and will be later fixed
        CGL_window_set_current_context(<CGL_widgets_context*>ctx)
    
    @staticmethod
    def init():
        CGL_widgets_init()
    
    @staticmethod
    def shutdown():
        CGL_widgets_shutdown()
    
    @staticmethod
    def begin():
        CGL_widgets_begin()
    
    @staticmethod
    def begin_int(scale_x: float, scale_y: float, offset_x: float, offset_y: float):
        CGL_widgets_begin_int(scale_x, scale_y, offset_x, offset_y)

    @staticmethod
    def end():
        CGL_widgets_end()

    @staticmethod
    def flush():
        CGL_widgets_flush()
    
    @staticmethod
    def flush_if_required():
        CGL_widgets_flush_if_required()
    
    @staticmethod
    def add_vertex(v: mesh_vertex):
        CGL_widgets_add_vertex(&v.c_vertex)
    
    @staticmethod
    def add_mesh(m: mesh_cpu):
        CGL_widgets_add_mesh(m.c_mesh)
    
    @staticmethod
    def add_vertex_p(v: vec3):
        CGL_widgets_add_vertex_p(v.c_vec3)
    
    @staticmethod
    def add_vertex_p3f(x: float, y: float, z: float):
        CGL_widgets_add_vertex_p3f(x, y, z)
    
    @staticmethod
    def add_vertex_pt(v: vec3, t: vec2):
        CGL_widgets_add_vertex_pt(v.c_vec3, t.c_vec2)
    
    @staticmethod
    def add_vertex_p3ft(x: float, y: float, z: float, t: vec2):
        CGL_widgets_add_vertex_p3ft(x, y, z, t.c_vec2)
    
    @staticmethod
    def add_vertex_pt2f(v: vec3, t0: float, t1: float):
        CGL_widgets_add_vertex_pt2f(v.c_vec3, t0, t1)
    
    @staticmethod
    def add_vertex_p3ft2f(x: float, y: float, z: float, u: float, v: float):
        CGL_widgets_add_vertex_p3ft2f(x, y, z, u, v)
    
    @staticmethod
    def set_stroke_color(c: vec4):
        CGL_widgets_set_stroke_color(c.c_vec4)
    
    @staticmethod
    def set_stroke_colorf(r: float, g: float, b: float, a: float = 1.0):
        CGL_widgets_set_stroke_colorf(r, g, b, a)

    @staticmethod
    def set_stroke_thickness(t: float):
        CGL_widgets_set_stroke_thicnkess(t)
    
    @staticmethod
    def set_fill_color(c: vec4):
        CGL_widgets_set_fill_color(c.c_vec4)
    
    @staticmethod
    def set_fill_colorf(r: float, g: float, b: float, a: float = 1.0):
        CGL_widgets_set_fill_colorf(r, g, b, a)
    
    @staticmethod
    def set_fill_mode(enebled: bool):
        CGL_widgets_set_fill_mode(enebled)
    
    @staticmethod
    def set_projection_matrix(m: mat4):
        CGL_widgets_set_projection_matrix(&m.c_mat4)
    
    @staticmethod
    def enable_diffuse_shading(light_pos: vec3, light_color: vec3):
        CGL_widgets_enable_diffuse_shading(light_pos.c_vec3, light_color.c_vec3)
    
    @staticmethod
    def disable_diffuse_shading():
        CGL_widgets_disable_diffuse_shading()
    
    @staticmethod
    def set_view_matrix(m: mat4):
        CGL_widgets_set_view_matrix(&m.c_mat4)
    
    @staticmethod
    def set_model_matrix(m: mat4):
        CGL_widgets_set_model_matrix(&m.c_mat4)
    
    @staticmethod
    def set_texture(tex: texture):
        CGL_widgets_set_texture(tex.c_texture)
    
    @staticmethod
    def set_font_texture(tex: texture):
        CGL_widgets_set_font_texture(tex.c_texture)
    
    @staticmethod
    def set_texture_coordinate_so(scale_x: float, scale_y: float, offset_x: float, offset_y: float):
        CGL_widgets_set_texture_coordinate_so(scale_x, scale_y, offset_x, offset_y)
    
    @staticmethod
    def apply_transformations_on_cpu():
        CGL_widgets_apply_transformations_on_cpu()
    
    @staticmethod
    def apply_transformations_on_gpu():
        CGL_widgets_apply_transformations_on_gpu()
    
    @staticmethod
    def add_triangle(a: vec3, b: vec3, c: vec3):
        CGL_widgets_add_triangle(a.c_vec3, b.c_vec3, c.c_vec3)
    
    @staticmethod
    def add_quad(a: vec3, b: vec3, c: vec3, d: vec3):
        CGL_widgets_add_quad(a.c_vec3, b.c_vec3, c.c_vec3, d.c_vec3)
    
    @staticmethod
    def add_quad_8f(a_x: float, a_y: float, b_x: float, b_y: float, c_x: float, c_y: float, d_x: float, d_y: float):
        CGL_widgets_add_quad_8f(a_x, a_y, b_x, b_y, c_x, c_y, d_x, d_y)
    
    @staticmethod
    def add_line(start: vec3, end: vec3):
        CGL_widgets_add_line(start.c_vec3, end.c_vec3)
    
    @staticmethod
    def add_line2f(start_x: float, start_y: float, end_x: float, end_y: float):
        CGL_widgets_add_line2f(start_x, start_y, end_x, end_y)
    
    @staticmethod
    def add_rect(start: vec3, size: vec2):
        CGL_widgets_add_rect(start.c_vec3, size.c_vec2)
    
    @staticmethod
    def add_rect2f(start_x: float, start_y: float, size_x: float, size_y: float):
        CGL_widgets_add_rect2f(start_x, start_y, size_x, size_y)
    
    @staticmethod
    def add_circle(position: vec3, radius: float):
        CGL_widgets_add_circle(position.c_vec3, radius)
    
    @staticmethod
    def add_circle2f(position_x: float, position_y: float, radius: float):
        CGL_widgets_add_circle2f(position_x, position_y, radius)
    
    @staticmethod
    def add_circler(position: vec3, radius: float, segments: int):
        CGL_widgets_add_circler(position.c_vec3, radius, segments)
    
    @staticmethod
    def add_circler2fr(position_x: float, position_y: float, radius: float, segments: int):
        CGL_widgets_add_circle2fr(position_x, position_y, radius, segments)
    
    @staticmethod
    def add_oval(position: vec3, radius: vec2):
        CGL_widgets_add_oval(position.c_vec3, radius.c_vec2)
    
    @staticmethod
    def add_oval2f(position_x: float, position_y: float, radius_x: float, radius_y: float):
        CGL_widgets_add_oval2f(position_x, position_y, radius_x, radius_y)
    
    @staticmethod
    def add_arc2f(position_x: float, position_y: float, radius: float, start_angle: float, end_angle: float, segments: int):
        CGL_widgets_add_arc2f(position_x, position_y, radius, start_angle, end_angle, segments)
    
    @staticmethod
    def add_character(ch: str, x: float, y: float, sx: float, sy: float) -> bool:
        return CGL_widgets_add_character(ch[0], x, y, sx, sy)
    
    @staticmethod
    def add_string(st: str, x: float, y: float, sx: float, sy: float) -> bool:
        return CGL_widgets_add_string(st.encode('utf-8'), x, y, sx, sy)

    @staticmethod
    def add_cubic_bazier(start: vec3, end: vec3, control_1: vec3, control_2: vec3, res: int):
        CGL_widgets_add_cubic_bazier(start.c_vec3, end.c_vec3, control_1.c_vec3, control_2.c_vec3, res)
    
    @staticmethod
    def add_cubic_bazier2v(start: vec2, end: vec2, control_1: vec2, control_2: vec2, res: int):
        CGL_widgets_add_cubic_bazier2v(start.c_vec2, end.c_vec2, control_1.c_vec2, control_2.c_vec2, res)

    @staticmethod
    def add_cubic_bazier2f(start_x: float, start_y: float, end_x: float, end_y: float, control_1_x: float, control_1_y: float, control_2_x: float, control_2_y: float, res: int):
        CGL_widgets_add_cubic_bazier2f(start_x, start_y, end_x, end_y, control_1_x, control_1_y, control_2_x, control_2_y, res)
    
    @staticmethod
    def add_cubic_bazier_points(start: vec3, end: vec3, control_1: vec3, control_2: vec3, res: int):
        CGL_widgets_add_cubic_bazier_points(start.c_vec3, end.c_vec3, control_1.c_vec3, control_2.c_vec3, res)
    
    @staticmethod
    def add_cubic_bazier_points2v(start: vec2, end: vec2, control_1: vec2, control_2: vec2, res: int):
        CGL_widgets_add_cubic_bazier_points2v(start.c_vec2, end.c_vec2, control_1.c_vec2, control_2.c_vec2, res)
    
    @staticmethod
    def add_cubic_bazier_points2f(start_x: float, start_y: float, end_x: float, end_y: float, control_1_x: float, control_1_y: float, control_2_x: float, control_2_y: float, res: int):
        CGL_widgets_add_cubic_bazier_points2f(start_x, start_y, end_x, end_y, control_1_x, control_1_y, control_2_x, control_2_y, res)

    @staticmethod
    def add_shape_out_line(shape: shape):
        CGL_widgets_add_shape_out_line(&shape.c_shape)
    
    @staticmethod
    def add_string_with_font(st: str, font: font, x: float, y: float, sx: float, scale_y: float) -> float:
        return CGL_widgets_add_string_with_font(st.encode('utf-8'), font.c_font, x, y, sx, scale_y)

NOISE_TYPE_PERLIN        = 0
NOISE_TYPE_OPENSIMPLEX   = 1
NOISE_TYPE_OPENSIMPLEX2S = 2
NOISE_TYPE_VALUE         = 3
NOISE_TYPE_VALUECUBIC    = 4
NOISE_TYPE_WORLEY        = 5
NOISE_TYPE_COUNT         = 6

NOISE_FRACTAL_TYPE_NONE     = 0
NOISE_FRACTAL_TYPE_FBM      = 1
NOISE_FRACTAL_TYPE_BILLOW   = 2
NOISE_FRACTAL_TYPE_RIGID    = 3
NOISE_FRACTAL_TYPE_PINGPONG = 4
NOISE_FRACTAL_TYPE_COUNT    = 5

cdef class noise_params:
    cdef CGL_noise_params c_noise_params

    @property
    def type(self) -> int:
        return self.c_noise_params.type
    
    @type.setter
    def type(self, value: int):
        self.c_noise_params.type = value
    
    @property
    def fractal_type(self) -> int:
        return self.c_noise_params.fractal_type
    
    @fractal_type.setter
    def fractal_type(self, value: int):
        self.c_noise_params.fractal_type = value
    
    @property
    def octaves(self) -> int:
        return self.c_noise_params.octaves

    @octaves.setter
    def octaves(self, value: int):
        self.c_noise_params.octaves = value
    
    @property
    def lacunarity(self) -> float:
        return self.c_noise_params.lacunarity
    
    @lacunarity.setter
    def lacunarity(self, value: float):
        self.c_noise_params.lacunarity = value
    
    @property
    def gain(self) -> float:
        return self.c_noise_params.gain
    
    @gain.setter
    def gain(self, value: float):
        self.c_noise_params.gain = value
    
    @property
    def frequency(self) -> float:
        return self.c_noise_params.frequency
    
    @frequency.setter
    def frequency(self, value: float):
        self.c_noise_params.frequency = value
    
    @property
    def weighted_strength(self) -> float:
        return self.c_noise_params.weighted_strength
    
    @weighted_strength.setter
    def weighted_strength(self, value: float):
        self.c_noise_params.weighted_strength = value
    
    @property
    def ping_pong_strength(self) -> float:
        return self.c_noise_params.ping_pong_strength
    
    @ping_pong_strength.setter
    def ping_pong_strength(self, value: float):
        self.c_noise_params.ping_pong_strength = value
    
    def __init__(self):
        self.c_noise_params = CGL_noise_params()
        CGL_noise_params_default(&self.c_noise_params)
    
    def __repr__(self):
        return f"noise_params(type={self.type}, fractal_type={self.fractal_type}, octaves={self.octaves}, lacunarity={self.lacunarity}, gain={self.gain}, frequency={self.frequency}, weighted_strength={self.weighted_strength}, ping_pong_strength={self.ping_pong_strength})"
    
    def __str__(self):
        return f"noise_params(type={self.type}, fractal_type={self.fractal_type}, octaves={self.octaves}, lacunarity={self.lacunarity}, gain={self.gain}, frequency={self.frequency}, weighted_strength={self.weighted_strength}, ping_pong_strength={self.ping_pong_strength})"

def noise_init():
    CGL_noise_init()

def noise_shutdown():
    CGL_noise_shutdown()

def noise_get(x: float, y: float, z: float, np: noise_params = noise_params() ) -> float:
    return CGL_noise_get(&np.c_noise_params, x, y, z)

def noise_perlin(x: float, y: float, z: float) -> float:
    return CGL_noise_perlin(x, y, z)

def noise_opensimplex(x: float, y: float, z: float) -> float:
    return CGL_noise_opensimplex(x, y, z)

def noise_opensimplex2s(x: float, y: float, z: float) -> float:
    return CGL_noise_opensimplex2s(x, y, z)

def noise_value(x: float, y: float, z: float) -> float:
    return CGL_noise_value(x, y, z)

def noise_valuecubic(x: float, y: float, z: float) -> float:
    return CGL_noise_valuecubic(x, y, z)

def noise_worley(x: float, y: float, z: float) -> float:
    return CGL_noise_worley(x, y, z)

    