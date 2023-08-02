//! Matrix math module. Includes a 4x4 matrix struct and associated methods.
//! NOTE: These matrix library is for graphics programming with OpenGL.
//!       For linear algebra, check the cgl_rs::math::linalg module.

use super::{Vector3, Vector4};


/// A 3x3 matrix struct used for graphics programming with OpenGL.
#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Matrix3x3 {
    m: [f32; 9]
}

/// A 4x4 matrix struct used for graphics programming with OpenGL.
#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Matrix4x4 {
    m: [f32; 16]
}


extern {
    fn CGL_mat3_det(m: &Matrix3x3) -> f32;
    fn CGL_mat3_transpose(m: &Matrix3x3) -> Matrix3x3;
    fn CGL_mat3_trace(m: &Matrix3x3) -> f32;

    fn CGL_mat4_zero_MACRO() -> Matrix4x4;
    fn CGL_mat4_identity_MACRO() -> Matrix4x4;
    fn CGL_mat4_scale_MACRO(x: f32, y: f32, z: f32) -> Matrix4x4;
    fn CGL_mat4_translate_MACRO(x: f32, y: f32, z: f32) -> Matrix4x4;
    fn CGL_mat4_rotate_x_MACRO(angle: f32) -> Matrix4x4;
    fn CGL_mat4_rotate_y_MACRO(angle: f32) -> Matrix4x4;
    fn CGL_mat4_rotate_z_MACRO(angle: f32) -> Matrix4x4;
    fn CGL_mat4_perspective_MACRO(fov: f32, aspect: f32, near: f32, far: f32) -> Matrix4x4;
    fn CGL_mat4_orthographic_MACRO(left: f32, right: f32, bottom: f32, top: f32, near: f32, far: f32) -> Matrix4x4;

    fn CGL_mat4_mul(a: &Matrix4x4, b: &Matrix4x4) -> Matrix4x4;
    fn CGL_mat4_det(m: &Matrix4x4) -> f32;
    fn CGL_mat4_det_by_lu(m: &Matrix4x4) -> f32;
    fn CGL_mat4_det_by_gauss(m: &Matrix4x4) -> f32;
    fn CGL_mat4_mul_vec4(m: &Matrix4x4, v: &Vector4) -> Vector4;
    fn CGL_mat4_inverse(m: &Matrix4x4) -> Matrix4x4;
    fn CGL_mat4_transpose(m: &Matrix4x4) -> Matrix4x4;
    fn CGL_mat4_adjoint(m: &Matrix4x4) -> Matrix4x4;
    fn CGL_mat4_gauss_elim(m: &Matrix4x4) -> Matrix4x4;
    fn CGL_mat4_rank(m: &Matrix4x4) -> i32;
    fn CGL_mat4_trace(m: &Matrix4x4) -> f32;
    fn CGL_mat4_to_mat3(m: &Matrix4x4) -> Matrix3x3;
    fn CGL_mat4_from_mat3(m: &Matrix3x3) -> Matrix4x4;
    fn CGL_mat4_rotate_about_axis(axis: &Vector3, angle: f32) -> Matrix4x4;
    fn CGL_mat4_look_at(eye: Vector3, target: Vector3, up: Vector3) -> Matrix4x4;
    fn CGL_mat4_lerp(a: &Matrix4x4, b: &Matrix4x4, t: f32) -> Matrix4x4;
    fn CGL_mat4_decompose_lu(m: &Matrix4x4, l: *mut Matrix4x4, u: *mut Matrix4x4) -> std::ffi::c_void;
}


impl std::ops::Add<Matrix3x3> for Matrix3x3 {
    type Output = Matrix3x3;

    fn add(self, rhs: Matrix3x3) -> Matrix3x3 {
        Matrix3x3 {
            m: [
                self.m[0] + rhs.m[0], self.m[1] + rhs.m[1], self.m[2] + rhs.m[2],
                self.m[3] + rhs.m[3], self.m[4] + rhs.m[4], self.m[5] + rhs.m[5],
                self.m[6] + rhs.m[6], self.m[7] + rhs.m[7], self.m[8] + rhs.m[8]
            ]
        }
    }
}

impl std::ops::Sub<Matrix3x3> for Matrix3x3 {
    type Output = Matrix3x3;

    fn sub(self, rhs: Matrix3x3) -> Matrix3x3 {
        Matrix3x3 {
            m: [
                self.m[0] - rhs.m[0], self.m[1] - rhs.m[1], self.m[2] - rhs.m[2],
                self.m[3] - rhs.m[3], self.m[4] - rhs.m[4], self.m[5] - rhs.m[5],
                self.m[6] - rhs.m[6], self.m[7] - rhs.m[7], self.m[8] - rhs.m[8]
            ]
        }
    }
}

impl std::ops::Mul<f32> for Matrix3x3 {
    type Output = Matrix3x3;

    fn mul(self, rhs: f32) -> Matrix3x3 {
        Matrix3x3 {
            m: [
                self.m[0] * rhs, self.m[1] * rhs, self.m[2] * rhs,
                self.m[3] * rhs, self.m[4] * rhs, self.m[5] * rhs,
                self.m[6] * rhs, self.m[7] * rhs, self.m[8] * rhs
            ]
        }
    }
}

impl std::fmt::Display for Matrix3x3 {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{{\n\t[{}, {}, {}]\n\t[{}, {}, {}]\n\t[{}, {}, {}]\n}}",
            self.m[0], self.m[3], self.m[6],
            self.m[1], self.m[4], self.m[7],
            self.m[2], self.m[5], self.m[8]
        )
    }
}

impl Matrix3x3 {
    /// Creates a new 3x3 matrix.
    /// 
    /// # Arguments
    /// 
    /// m0 ... m8: The values of the matrix.
    /// 
    /// # Returns
    /// 
    /// A new 3x3 matrix.
    /// 
    /// # Example
    /// 
    /// ```
    /// use cgl_rs::math::Matrix3x3;
    /// 
    /// let m = Matrix3x3::new(
    ///    1.0, 0.0, 0.0,
    ///    0.0, 1.0, 0.0,
    ///    0.0, 0.0, 1.0
    /// );
    /// ```
    pub fn new(
        m0: f32, m1: f32, m2: f32,
        m3: f32, m4: f32, m5: f32,
        m6: f32, m7: f32, m8: f32
    ) -> Matrix3x3 {
        Matrix3x3 {
            m: [
                m0, m3, m6,
                m1, m4, m7,
                m2, m5, m8
            ]
        }
    }


    /// Creates a new 3x3 matrix with all elements set to zero.
    ///
    /// # Returns
    ///
    /// A new 3x3 matrix with all elements set to zero.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix3x3;
    ///
    /// let m = Matrix3x3::zero();
    /// ```
    pub fn zero() -> Matrix3x3 {
        Matrix3x3 {
            m: [0.0; 9]
        }
    }



    /// Creates a new 3x3 identity matrix.
    ///
    /// # Returns
    ///
    /// A new 3x3 identity matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix3x3;
    ///
    /// let m = Matrix3x3::identity();
    /// ```
    pub fn identity() -> Matrix3x3 {
        Matrix3x3::new(
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 
            0.0, 0.0, 1.0
        )
    }


    /// Transposes the matrix.
    ///
    /// # Returns
    ///
    /// A new 3x3 matrix that is the transpose of the original matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix3x3;
    ///
    /// let m = Matrix3x3::new(
    ///     1.0, 2.0, 3.0,
    ///     4.0, 5.0, 6.0,
    ///     7.0, 8.0, 9.0
    /// );
    ///
    /// let m_transpose = m.transpose();
    /// ```
    pub fn transpose(&self) -> Matrix3x3 {
        unsafe {
            CGL_mat3_transpose(self)
        }
    }

    /// Calculates the trace of the matrix.
    ///
    /// # Returns
    ///
    /// The sum of the diagonal elements of the matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix3x3;
    ///
    /// let m = Matrix3x3::new(
    ///     1.0, 2.0, 3.0,
    ///     4.0, 5.0, 6.0,
    ///     7.0, 8.0, 9.0
    /// );
    ///
    /// let trace = m.trace();
    /// ```
    pub fn trace(&self) -> f32 {
        unsafe {
            CGL_mat3_trace(self) as f32
        }
    }

    /// Calculates the determinant of the matrix.
    ///
    /// # Returns
    ///
    /// The determinant of the matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix3x3;
    ///
    /// let m = Matrix3x3::new(
    ///     1.0, 2.0, 3.0,
    ///     4.0, 5.0, 6.0,
    ///     7.0, 8.0, 9.0
    /// );
    ///
    /// let det = m.determinant();
    /// ```
    pub fn determinant(&self) -> f32 {
        unsafe {
            CGL_mat3_det(self) as f32
        }
    }

}


impl std::ops::Add<Matrix4x4> for Matrix4x4 {
    type Output = Matrix4x4;

    fn add(self, rhs: Matrix4x4) -> Matrix4x4 {
        Matrix4x4 {
            m: [
                self.m[0] + rhs.m[0], self.m[1] + rhs.m[1], self.m[2] + rhs.m[2], self.m[3] + rhs.m[3],
                self.m[4] + rhs.m[4], self.m[5] + rhs.m[5], self.m[6] + rhs.m[6], self.m[7] + rhs.m[7],
                self.m[8] + rhs.m[8], self.m[9] + rhs.m[9], self.m[10] + rhs.m[10], self.m[11] + rhs.m[11],
                self.m[12] + rhs.m[12], self.m[13] + rhs.m[13], self.m[14] + rhs.m[14], self.m[15] + rhs.m[15]
            ]
        }
    }
}

impl std::ops::Sub<Matrix4x4> for Matrix4x4 {
    type Output = Matrix4x4;

    fn sub(self, rhs: Matrix4x4) -> Matrix4x4 {
        Matrix4x4 {
            m: [
                self.m[0] - rhs.m[0], self.m[1] - rhs.m[1], self.m[2] - rhs.m[2], self.m[3] - rhs.m[3],
                self.m[4] - rhs.m[4], self.m[5] - rhs.m[5], self.m[6] - rhs.m[6], self.m[7] - rhs.m[7],
                self.m[8] - rhs.m[8], self.m[9] - rhs.m[9], self.m[10] - rhs.m[10], self.m[11] - rhs.m[11],
                self.m[12] - rhs.m[12], self.m[13] - rhs.m[13], self.m[14] - rhs.m[14], self.m[15] - rhs.m[15]
            ]
        }
    }
}

impl std::ops::Mul<f32> for Matrix4x4 {
    type Output = Matrix4x4;

    fn mul(self, rhs: f32) -> Matrix4x4 {
        Matrix4x4 {
            m: [
                self.m[0] * rhs, self.m[1] * rhs, self.m[2] * rhs, self.m[3] * rhs,
                self.m[4] * rhs, self.m[5] * rhs, self.m[6] * rhs, self.m[7] * rhs,
                self.m[8] * rhs, self.m[9] * rhs, self.m[10] * rhs, self.m[11] * rhs,
                self.m[12] * rhs, self.m[13] * rhs, self.m[14] * rhs, self.m[15] * rhs
            ]
        }
    }
}

impl std::fmt::Display for Matrix4x4 {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{{\n\t[{}, {}, {}, {}]\n\t[{}, {}, {}, {}]\n\t[{}, {}, {}, {}]\n\t[{}, {}, {}, {}]\n}}",
            self.m[0], self.m[1], self.m[2], self.m[3],
            self.m[4], self.m[5], self.m[6], self.m[7],
            self.m[8], self.m[9], self.m[10], self.m[11],
            self.m[12], self.m[13], self.m[14], self.m[15]
        )
    }
}

impl Matrix4x4 {

    /// Creates a new matrix with the given values.
    /// 
    /// # Arguments
    /// 
    /// * `m00 .. m33` - The values of the matrix.
    /// 
    /// # Returns
    /// 
    /// A new identity matrix.
    /// 
    /// # Example
    /// 
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    /// 
    /// let m = Matrix4x4::new(
    ///    1.0, 0.0, 0.0, 0.0,
    ///    0.0, 1.0, 0.0, 0.0,
    ///    0.0, 0.0, 1.0, 0.0,
    ///    0.0, 0.0, 0.0, 1.0
    /// );
    /// 
    /// println!("{}", m);
    /// ```
    pub fn new(
        m00: f32, m01: f32, m02: f32, m03: f32,
        m10: f32, m11: f32, m12: f32, m13: f32,
        m20: f32, m21: f32, m22: f32, m23: f32,
        m30: f32, m31: f32, m32: f32, m33: f32 
    ) -> Matrix4x4 {
        Matrix4x4 {
            m: [
                m00, m10, m20, m30,
                m01, m11, m21, m31,
                m02, m12, m22, m32,
                m03, m13, m23, m33
            ]
        }
    }

    /// Creates a new 4x4 matrix from a 3x3 matrix.
    ///
    /// # Arguments
    ///
    /// * `m` - The 3x3 matrix to convert to a 4x4 matrix.
    ///
    /// # Returns
    ///
    /// A new 4x4 matrix with the same values as the input 3x3 matrix, with the fourth row and column set to zero.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::{Matrix3x3, Matrix4x4};
    ///
    /// let m3 = Matrix3x3::new(
    ///     1.0, 2.0, 3.0,
    ///     4.0, 5.0, 6.0,
    ///     7.0, 8.0, 9.0
    /// );
    ///
    /// let m4 = Matrix4x4::from_mat3(&m3);
    /// ```
    pub fn from_mat3(m: &Matrix3x3) -> Matrix4x4 {
        unsafe {
            CGL_mat4_from_mat3(&m) as Matrix4x4
        }
    }

    /// Returns a new matrix with all elements set to zero.
    ///
    /// # Returns
    ///
    /// A new matrix with all elements set to zero.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::zero();
    /// ```
    pub fn zero() -> Matrix4x4 {
        unsafe {
            CGL_mat4_zero_MACRO() as Matrix4x4
        }
    }


    /// Returns a new identity matrix.
    ///
    /// # Returns
    ///
    /// A new identity matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// ```
    pub fn identity() -> Matrix4x4 {
        unsafe {
            CGL_mat4_identity_MACRO() as Matrix4x4
        }
    }


    /// Returns a new scaling matrix.
    ///
    /// # Arguments
    ///
    /// * `x` - The scaling factor along the x-axis.
    /// * `y` - The scaling factor along the y-axis.
    /// * `z` - The scaling factor along the z-axis.
    ///
    /// # Returns
    ///
    /// A new scaling matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::scale(2.0, 3.0, 4.0);
    /// ```
    pub fn scale(x: f32, y: f32, z: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_scale_MACRO(x, y, z) as Matrix4x4
        }
    }


    /// Returns a new translation matrix.
    ///
    /// # Arguments
    ///
    /// * `x` - The translation along the x-axis.
    /// * `y` - The translation along the y-axis.
    /// * `z` - The translation along the z-axis.
    ///
    /// # Returns
    ///
    /// A new translation matrix.
    /// 
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::translate(1.0, 2.0, 3.0);
    /// ```
    pub fn translate(x: f32, y: f32, z: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_translate_MACRO(x, y, z) as Matrix4x4
        }
    }


    /// Returns a new rotation matrix around the x-axis.
    ///
    /// # Arguments
    ///
    /// * `angle` - The angle of rotation in radians.
    ///
    /// # Returns
    ///
    /// A new rotation matrix around the x-axis.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::rotate_x(0.5);
    /// ```
    pub fn rotate_x(angle: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_rotate_x_MACRO(angle) as Matrix4x4
        }
    }

    /// Returns a new rotation matrix around the y-axis.
    ///
    /// # Arguments
    ///
    /// * `angle` - The angle of rotation in radians.
    ///
    /// # Returns
    ///
    /// A new rotation matrix around the y-axis.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::rotate_y(0.5);
    /// ```
    pub fn rotate_y(angle: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_rotate_y_MACRO(angle) as Matrix4x4
        }
    }


    /// Returns a new rotation matrix around the z-axis.
    ///
    /// # Arguments
    ///
    /// * `angle` - The angle of rotation in radians.
    ///
    /// # Returns
    ///
    /// A new rotation matrix around the z-axis.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::rotate_z(0.5);
    /// ```
    pub fn rotate_z(angle: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_rotate_z_MACRO(angle) as Matrix4x4
        }
    }


    /// Returns a new rotation matrix that rotates around the given axis by the specified angle.
    ///
    /// # Arguments
    ///
    /// * `axis` - The axis to rotate around.
    /// * `angle` - The angle of rotation in radians.
    ///
    /// # Returns
    ///
    /// A new rotation matrix that rotates around the given axis by the specified angle.
    /// 
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::{Matrix4x4, Vector3};
    ///
    /// let axis = Vector3::new(1.0, 0.0, 0.0);
    /// let m = Matrix4x4::rotate_about_axis(&axis, 0.5);
    /// ```
    pub fn rotate_about_axis(axis: &Vector3, angle: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_rotate_about_axis(axis, angle) as Matrix4x4
        }
    }





    /// Returns a new perspective projection matrix.
    ///
    /// # Arguments
    ///
    /// * `fov` - The field of view angle in radians.
    /// * `aspect` - The aspect ratio of the projection.
    /// * `near` - The distance to the near clipping plane.
    /// * `far` - The distance to the far clipping plane.
    ///
    /// # Returns
    ///
    /// A new perspective projection matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    /// 
    /// let m = Matrix4x4::perspective(cgl_rs::math::constants::PI_2, 16.0/9.0, 0.1, 100.0);
    /// ```
    pub fn perspective(fov: f32, aspect: f32, near: f32, far: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_perspective_MACRO(fov, aspect, near, far) as Matrix4x4
        }
    }


    /// Returns a new orthographic projection matrix.
    ///
    /// # Arguments
    ///
    /// * `left` - The coordinate for the left vertical clipping plane.
    /// * `right` - The coordinate for the right vertical clipping plane.
    /// * `bottom` - The coordinate for the bottom horizontal clipping plane.
    /// * `top` - The coordinate for the top horizontal clipping plane.
    /// * `near` - The distance to the near clipping plane.
    /// * `far` - The distance to the far clipping plane.
    ///
    /// # Returns
    ///
    /// A new orthographic projection matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::orthographic(-1.0, 1.0, -1.0, 1.0, 0.1, 100.0);
    /// ```
    pub fn orthographic(left: f32, right: f32, bottom: f32, top: f32, near: f32, far: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_orthographic_MACRO(left, right, bottom, top, near, far) as Matrix4x4
        }
    }


    /// Returns a new view matrix that looks from the eye position towards the target position.
    ///
    /// # Arguments
    ///
    /// * `eye` - The position of the camera.
    /// * `target` - The position to look at.
    /// * `up` - The up direction of the camera.
    ///
    /// # Returns
    ///
    /// A new view matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::{Matrix4x4, Vector3};
    ///
    /// let mat = Matrix4x4::look_at(
    ///    Vector3::new(0.0, 0.0, 0.0),
    ///    Vector3::new(0.0, 0.0, -1.0),
    ///    Vector3::new(0.0, 1.0, 0.0)
    /// );
    /// ```
    pub fn look_at(eye: Vector3, target: Vector3, up: Vector3) -> Matrix4x4 {
        unsafe {
            CGL_mat4_look_at(eye, target, up) as Matrix4x4
        }
    }

    /// Multiplies this matrix by another matrix and returns the result.
    ///
    /// # Arguments
    ///
    /// * `other` - The matrix to multiply by.
    ///
    /// # Returns
    ///
    /// The resulting matrix of the multiplication.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m1 = Matrix4x4::identity();
    /// let m2 = Matrix4x4::rotate_z(0.5);
    /// let m3 = m1.mul(&m2);
    /// ```
    pub fn mul(&self, other: &Matrix4x4) -> Matrix4x4 {
        unsafe {
            CGL_mat4_mul(self, other) as Matrix4x4
        }
    }

    /// Calculates the determinant of this matrix.
    ///
    /// # Returns
    ///
    /// The determinant of this matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let det = m.determinant();
    /// ```
    pub fn determinant(&self) -> f32 {
        unsafe {
            CGL_mat4_det(self)
        }
    }

    /// Multiplies this matrix by a vector and returns the result.
    ///
    /// # Arguments
    ///
    /// * `other` - The vector to multiply by.
    ///
    /// # Returns
    ///
    /// The resulting vector of the multiplication.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::{Matrix4x4, Vector4};
    ///
    /// let m = Matrix4x4::identity();
    /// let v = Vector4::new(1.0, 2.0, 3.0, 1.0);
    /// let result = m.mul_vec4(&v);
    /// ```
    pub fn mul_vec4(&self, other: &Vector4) -> Vector4 {
        unsafe {
            CGL_mat4_mul_vec4(self, other) as Vector4
        }
    }

    /// Calculates the inverse of this matrix.
    ///
    /// # Returns
    ///
    /// The inverse of this matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let inv = m.inverse();
    /// ```
    pub fn inverse(&self) -> Matrix4x4 {
        unsafe {
            CGL_mat4_inverse(self) as Matrix4x4
        }
    }

    /// Transposes this matrix.
    ///
    /// # Returns
    ///
    /// The transposed matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let transposed = m.transpose();
    /// ```
    pub fn transpose(&self) -> Matrix4x4 {
        unsafe {
            CGL_mat4_transpose(self) as Matrix4x4
        }
    }

    /// Calculates the adjoint of this matrix.
    ///
    /// # Returns
    ///
    /// The adjoint of this matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let adj = m.adjoint();
    /// ```
    pub fn adjoint(&self) -> Matrix4x4 {
        unsafe {
            CGL_mat4_adjoint(self) as Matrix4x4
        }
    }


    /// Performs Gaussian elimination on this matrix.
    ///
    /// # Returns
    ///
    /// The matrix in row echelon form.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let row_echelon = m.gaussian_elimination();
    /// ```
    pub fn gaussian_elimination(&self) -> Matrix4x4 {
        unsafe {
            CGL_mat4_gauss_elim(self) as Matrix4x4
        }
    }

    /// Calculates the rank of this matrix.
    ///
    /// # Returns
    ///
    /// The rank of this matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let rank = m.rank();
    /// ```
    pub fn rank(&self) -> i32 {
        unsafe {
            CGL_mat4_rank(self)
        }
    }

    /// Calculates the trace of this matrix.
    ///
    /// # Returns
    ///
    /// The trace of this matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let trace = m.trace();
    /// ```
    pub fn trace(&self) -> f32 {
        unsafe {
            CGL_mat4_trace(self)
        }
    }

    /// Converts this Matrix4x4 to a Matrix3x3 by discarding the last row and column.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::{Matrix3x3, Matrix4x4};
    ///
    /// let m = Matrix4x4::identity();
    /// let m3 = m.to_mat3();
    /// ```
    pub fn to_mat3(&self) -> Matrix3x3 {
        unsafe {
            CGL_mat4_to_mat3(self) as Matrix3x3
        }
    }

    /// Decomposes this matrix into lower and upper triangular matrices using LU decomposition.
    ///
    /// # Returns
    ///
    /// A tuple containing the lower and upper triangular matrices.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m = Matrix4x4::identity();
    /// let (l, u) = m.decompose_lu();
    /// ```
    pub fn decompose_lu(&self) -> (Matrix4x4, Matrix4x4) {
        unsafe {
            let mut l = Matrix4x4::identity();
            let mut u = Matrix4x4::identity();
            CGL_mat4_decompose_lu(self, &mut l, &mut u);
            (l, u)
        }
    }

    /// Performs a linear interpolation between this matrix and another matrix.
    ///
    /// # Arguments
    ///
    /// * `other` - The other matrix to interpolate with.
    /// * `t` - The interpolation factor. Should be between 0.0 and 1.0.
    ///
    /// # Returns
    ///
    /// The interpolated matrix.
    ///
    /// # Example
    ///
    /// ```
    /// use cgl_rs::math::Matrix4x4;
    ///
    /// let m1 = Matrix4x4::identity();
    /// let m2 = Matrix4x4::scale(1.0, 2.0, 3.0);
    /// let m3 = m1.lerp(&m2, 0.5);
    /// ```
    pub fn lerp(&self, other: &Matrix4x4, t: f32) -> Matrix4x4 {
        unsafe {
            CGL_mat4_lerp(self, other, t) as Matrix4x4
        }
    }


}