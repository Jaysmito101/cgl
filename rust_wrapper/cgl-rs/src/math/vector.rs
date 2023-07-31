//! Vector math module. Includes vector structs and functions.


/// A 2-dimensional vector with `x` and `y` components.
#[derive(Debug, Copy, Clone, PartialEq)] #[repr(C)]
pub struct Vector2 {
    pub x: f32,
    pub y: f32,
}

/// A 3-dimensional vector with `x`, `y` and `z` components.
#[derive(Debug, Copy, Clone, PartialEq)] #[repr(C)]
pub struct Vector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

/// A 4-dimensional vector with `x`, `y`, `z` and `w` components.
#[derive(Debug, Copy, Clone, PartialEq)] #[repr(C)]
pub struct Vector4 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
    pub w: f32,
}

/// Allows indexing into a `Vector2` by `usize` index.
impl std::ops::Index<usize> for Vector2 {
    type Output = f32;
    
    /// Returns a reference to the `f32` value at the given index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the value to retrieve. Must be `0` or `1`.
    ///
    /// # Returns
    ///
    /// A reference to the `f32` value at the given index.
    ///
    /// # Panics
    ///
    /// Panics if the index is not `0` or `1`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    /// let vec = Vector2::new(1.0, 2.0);
    /// assert_eq!(vec[0], 1.0);
    /// assert_eq!(vec[1], 2.0);
    /// ```
    fn index(&self, index: usize) -> &f32 {
        match index {
            0 => &self.x,
            1 => &self.y,
            _ => panic!("Index out of bounds for Vector2"),
        }
        
    }
}

/// Allows mutable indexing into a `Vector2` by `usize` index.
impl std::ops::IndexMut<usize> for Vector2 {
    
    /// Returns a mutable reference to the `f32` value at the given index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the value to retrieve. Must be `0` or `1`.
    ///
    /// # Returns
    ///
    /// A mutable reference to the `f32` value at the given index.
    ///
    /// # Panics
    ///
    /// Panics if the index is not `0` or `1`.
    /// 
    /// # Examples
    /// 
    /// ```
    /// use cgl_rs::math::Vector2;
    /// let mut vec = Vector2::new(1.0, 2.0);
    /// vec[0] = 3.0;
    /// assert_eq!(vec.x, 3.0);
    /// ```
    fn index_mut(&mut self, index: usize) -> &mut f32 {
        match index {
            0 => &mut self.x,
            1 => &mut self.y,
            _ => panic!("Index out of bounds for Vector2"),
        }
    }
}

impl std::fmt::Display for Vector2 {
        
        /// Formats the vector as a string.
        /// 
        /// # Arguments
        /// 
        /// * `f` - The formatter to use.
        /// 
        /// # Returns
        /// 
        /// A string representation of the vector.
        /// 
        /// # Examples
        /// 
        /// ```
        /// use cgl_rs::math::Vector2;
        /// let vec = Vector2::new(1.0, 2.0);
        /// ```
        fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
            write!(f, "Vector2({}, {})", self.x, self.y)
        }
}

crate::macros::impl_vector_binary_ops!(Vector2, 2);

impl Vector2 {
    
    /// Creates a new `Vector2` with the given `x` and `y` components.
    /// 
    /// # Arguments
    /// 
    /// * `x` - The `x` component of the vector.
    /// * `y` - The `y` component of the vector.
    /// 
    /// # Returns
    /// 
    /// A new `Vector2` with the given `x` and `y` components.
    /// 
    /// # Examples
    /// 
    /// ```
    /// use cgl_rs::math::Vector2;
    /// 
    /// let vec = Vector2::new(1.0, 2.0);
    /// ```
    pub fn new(x: f32, y: f32) -> Vector2 {
        Vector2 { x: x, y: y }
    }
    
    /// Creates a new `Vector2` with `x` and `y` components set to `0.0`.
    ///
    /// # Returns
    ///
    /// A new `Vector2` with `x` and `y` components set to `0.0`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::zero();
    /// ```
    pub fn zero() -> Vector2 {
        Vector2 { x: (0.0), y: (0.0) }
    }
    
    /// Creates a new `Vector2` with `x` and `y` components set to `1.0`.
    ///
    /// # Returns
    ///
    /// A new `Vector2` with `x` and `y` components set to `1.0`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::one();
    /// ```
    pub fn one() -> Vector2 {
        Vector2 { x: (1.0), y: (1.0) }
    }


    /// Calculates the dot product of this vector and another vector.
    ///
    /// # Arguments
    ///
    /// * `other` - The other vector to calculate the dot product with.
    ///
    /// # Returns
    ///
    /// The dot product of this vector and the other vector.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec1 = Vector2::new(1.0, 2.0);
    /// let vec2 = Vector2::new(3.0, 4.0);
    /// let dot_product = vec1.dot(&vec2);
    /// ```
    pub fn dot(&self, other: &Vector2) -> f32 {
        self.x * other.x + self.y * other.y
    }

    /// Calculates the cross product of this vector and another vector.
    /// 
    /// # Arguments
    /// 
    /// * `other` - The other vector to calculate the cross product with.
    /// 
    /// # Returns
    /// 
    /// The cross product of this vector and the other vector.
    /// 
    /// # Examples
    /// 
    /// ```
    /// use cgl_rs::math::Vector2;
    /// 
    /// let vec1 = Vector2::new(1.0, 2.0);
    /// let vec2 = Vector2::new(3.0, 4.0);
    /// let cross_product = vec1.cross(&vec2);
    /// ```
    pub fn cross(&self, other: &Vector2) -> f32 {
        self.x * other.y - self.y * other.x
    }

    /// Calculates the length of this vector.
    ///
    /// # Returns
    ///
    /// The length of this vector.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::new(3.0, 4.0);
    /// let length = vec.length();
    /// ```
    pub fn length(&self) -> f32 {
        (self.x * self.x + self.y * self.y).sqrt()
    }

    /// Rotates this vector about the origin by the given angle (in radians).
    ///
    /// # Arguments
    ///
    /// * `angle` - The angle (in radians) to rotate this vector by.
    ///
    /// # Returns
    ///
    /// A new `Vector2` representing the result of rotating this vector about the origin by the given angle.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::new(1.0, 0.0);
    /// let rotated_vec = vec.rotate_about_origin(std::f32::consts::PI / 2.0);
    /// ```
    pub fn rotate_about_origin(&self, angle: f32) -> Vector2 {
        let cos = angle.cos();
        let sin = angle.sin();
        Vector2::new(self.x * cos - self.y * sin, self.x * sin + self.y * cos)
    }

    /// Creates a new `Vector2` from the given angle (in radians).
    ///
    /// # Arguments
    ///
    /// * `angle` - The angle (in radians) to create the vector from.
    ///
    /// # Returns
    ///
    /// A new `Vector2` representing the given angle.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::from_angle(std::f32::consts::PI / 4.0);
    /// ```
    pub fn from_angle(angle: f32) -> Vector2 {
        Vector2::new(angle.cos(), angle.sin())
    }

    /// Calculates the angle (in radians) between this vector and the positive x-axis.
    ///
    /// # Returns
    ///
    /// The angle (in radians) between this vector and the positive x-axis.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::new(1.0, 1.0);
    /// let angle = vec.angle();
    /// ```
    pub fn angle(&self) -> f32 {
        self.y.atan2(self.x)
    }
    
    /// Calculates the angle (in radians) between this vector and another vector.
    ///
    /// # Arguments
    ///
    /// * `other` - The other vector to calculate the angle between.
    ///
    /// # Returns
    ///
    /// The angle (in radians) between this vector and the other vector.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec1 = Vector2::new(1.0, 0.0);
    /// let vec2 = Vector2::new(0.0, 1.0);
    /// let angle = vec1.angle_between(&vec2);
    /// ```
    pub fn angle_between(&self, other: &Vector2) -> f32 {
        let dot = self.dot(other);
        let det = self.x * other.y - self.y * other.x;
        det.atan2(dot)
    }

    /// Normalizes this vector in place.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let mut vec = Vector2::new(3.0, 4.0);
    /// vec.normalize();
    /// ```
    /// 
    /// # See also
    /// 
    /// * [`normalized`](#method.normalized)
    pub fn normalize(&mut self) {
        let length = self.length();
        self.x /= length;
        self.y /= length;
    }

    /// Returns a new normalized `Vector2` representing this vector.
    ///
    /// # Returns
    ///
    /// A new `Vector2` representing this vector, normalized.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::new(3.0, 4.0);
    /// let normalized_vec = vec.normalized();
    /// ```
    /// 
    /// # See also
    /// 
    /// * [`normalize`](#method.normalize)
    pub fn normalized(&self) -> Vector2 {
        let length = self.length();
        Vector2::new(self.x / length, self.y / length)
    }

    /// Returns a new `Vector2` that is perpendicular to this vector.
    ///
    /// # Returns
    ///
    /// A new `Vector2` that is perpendicular to this vector.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::new(1.0, 2.0);
    /// let perp_vec = vec.perpendicular();
    /// ```
    ///
    /// # See also
    ///
    /// * [`dot`](#method.dot)
    /// * [`angle_between`](#method.angle_between)
    pub fn perpendicular(&self) -> Vector2 {
        Vector2::new(-self.y, self.x)
    }

    /// Returns the reflection of this vector off a surface with the given normal.
    ///
    /// # Arguments
    ///
    /// * `normal` - The normal of the surface being reflected off of.
    ///
    /// # Returns
    ///
    /// The reflection of this vector off the surface with the given normal.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector2;
    ///
    /// let vec = Vector2::new(1.0, 2.0);
    /// let normal = Vector2::new(0.0, 1.0);
    /// let reflected_vec = vec.reflect(&normal);
    /// ```
    pub fn reflect(&self, normal: &Vector2) -> Vector2 {
        let dot = self.dot(normal);
        Vector2::new(self.x - 2.0 * dot * normal.x, self.y - 2.0 * dot * normal.y)
    }
    
}


/// Allows indexing into a `Vector2`.
impl std::ops::Index<usize> for Vector3 {
    type Output = f32;

    /// Returns a reference to the element at the given index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the element to return.
    ///
    /// # Panics
    ///
    /// Panics if the index is out of bounds for a `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::new(1.0, 2.0, 3.0);
    /// assert_eq!(vec[0], 1.0);
    /// assert_eq!(vec[1], 2.0);
    /// assert_eq!(vec[2], 3.0);
    /// ```
    fn index(&self, index: usize) -> &f32 {
        match index {
            0 => &self.x,
            1 => &self.y,
            2 => &self.z,
            _ => panic!("Index out of bounds for Vector3"),
        }
    }
}

/// Allows indexing into a `Vector3` mutably.
impl std::ops::IndexMut<usize> for Vector3 {
    /// Allows mutable indexing into a `Vector3`.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the element to return.
    ///
    /// # Panics
    ///
    /// Panics if the index is out of bounds for a `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let mut vec = Vector3::new(1.0, 2.0, 3.0);
    /// vec[0] = 4.0;
    /// vec[1] = 5.0;
    /// vec[2] = 6.0;
    /// ```
    fn index_mut(&mut self, index: usize) -> &mut f32 {
        match index {
            0 => &mut self.x,
            1 => &mut self.y,
            2 => &mut self.z,
            _ => panic!("Index out of bounds for Vector3"),
        }
    }
}

crate::macros::impl_vector_binary_ops!(Vector3, 3);

impl Vector3 {
    /// Creates a new `Vector3` with the given `x`, `y`, and `z` components.
    ///
    /// # Arguments
    ///
    /// * `x` - The `x` component of the new vector.
    /// * `y` - The `y` component of the new vector.
    /// * `z` - The `z` component of the new vector.
    ///
    /// # Returns
    ///
    /// A new `Vector3` with the given `x`, `y`, and `z` components.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::new(1.0, 2.0, 3.0);
    /// ```
    pub fn new(x: f32, y: f32, z: f32) -> Vector3 {
        Vector3 { x, y, z }
    }

    /// Creates a new `Vector3` from a `Vector2` and a `z` component.
    ///
    /// # Arguments
    ///
    /// * `vec` - The `Vector2` to use for the `x` and `y` components of the new vector.
    /// * `z` - The `z` component of the new vector.
    ///
    /// # Returns
    ///
    /// A new `Vector3` with the `x` and `y` components taken from `vec` and the `z` component set to `z`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector2, Vector3};
    ///
    /// let vec2 = Vector2::new(1.0, 2.0);
    /// let vec3 = Vector3::from_vec2(vec2, 3.0);
    /// assert_eq!(vec3.x, 1.0);
    /// assert_eq!(vec3.y, 2.0);
    /// assert_eq!(vec3.z, 3.0);
    /// ```
    pub fn from_vec2(vec: Vector2, z: f32) -> Vector3 {
        Vector3::new(vec.x, vec.y, z)
    }

    /// Returns a new `Vector3` with all components set to zero.
    ///
    /// # Returns
    ///
    /// A new `Vector3` with all components set to zero.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::zero();
    /// assert_eq!(vec.x, 0.0);
    /// assert_eq!(vec.y, 0.0);
    /// assert_eq!(vec.z, 0.0);
    /// ```
    pub fn zero() -> Vector3 {
        Vector3::new(0.0, 0.0, 0.0)
    }

    /// Returns a new `Vector3` with all components set to one.
    ///
    /// # Returns
    ///
    /// A new `Vector3` with all components set to one.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::one();
    /// assert_eq!(vec.x, 1.0);
    /// assert_eq!(vec.y, 1.0);
    /// assert_eq!(vec.z, 1.0);
    /// ```
    pub fn one() -> Vector3 {
        Vector3::new(1.0, 1.0, 1.0)
    }

    /// Computes the dot product of two `Vector3`s.
    ///
    /// # Arguments
    ///
    /// * `other` - The other `Vector3` to compute the dot product with.
    ///
    /// # Returns
    ///
    /// The dot product of the two `Vector3`s.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec1 = Vector3::new(1.0, 2.0, 3.0);
    /// let vec2 = Vector3::new(4.0, 5.0, 6.0);
    /// let dot_product = vec1.dot(vec2);
    /// assert_eq!(dot_product, 32.0);
    /// ```
    pub fn dot(&self, other: Vector3) -> f32 {
        self.x * other.x + self.y * other.y + self.z * other.z
    }

    /// Computes the cross product of two `Vector3`s.
    ///
    /// # Arguments
    ///
    /// * `other` - The other `Vector3` to compute the cross product with.
    ///
    /// # Returns
    ///
    /// The cross product of the two `Vector3`s.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec1 = Vector3::new(1.0, 2.0, 3.0);
    /// let vec2 = Vector3::new(4.0, 5.0, 6.0);
    /// let cross_product = vec1.cross(vec2);
    /// assert_eq!(cross_product.x, -3.0);
    /// assert_eq!(cross_product.y, 6.0);
    /// assert_eq!(cross_product.z, -3.0);
    /// ```
    pub fn cross(&self, other: Vector3) -> Vector3 {
        Vector3::new(
            self.y * other.z - self.z * other.y,
            -(self.x * other.z - self.z * other.x),
            self.x * other.y - self.y * other.x,
        )
    }

    // Computes the length of the `Vector3`.
    //
    // # Returns
    //
    // The length of the `Vector3`.
    //
    // # Examples
    //
    // ```
    // use cgl_rs::math::Vector3;
    //
    // let vec = Vector3::new(1.0, 2.0, 2.0);
    // let length = vec.length();
    // assert_eq!(length, 3.0);
    // ```
    pub fn length(&self) -> f32 {
        (self.x * self.x + self.y * self.y + self.z * self.z).sqrt()
    }

    /// Normalizes the `Vector3` in place.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let mut vec = Vector3::new(1.0, 2.0, 2.0);
    /// vec.normalize();
    /// assert_eq!(vec.length(), 1.0);
    /// ```
    pub fn normalize(&mut self) {
        let length = self.length();
        self.x /= length;
        self.y /= length;
        self.z /= length;
    }

    /// Returns a new `Vector3` that is the normalized version of this `Vector3`.
    ///
    /// # Returns
    ///
    /// A new `Vector3` that is the normalized version of this `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::new(1.0, 2.0, 2.0);
    /// let normalized_vec = vec.normalized();
    /// assert_eq!(normalized_vec.length(), 1.0);
    /// ```
    pub fn normalized(&self) -> Vector3 {
        let length = self.length();
        Vector3::new(self.x / length, self.y / length, self.z / length)
    }

    /// Returns a new `Vector2` containing the `x` and `y` components of this `Vector3`.
    ///
    /// # Returns
    ///
    /// A new `Vector2` containing the `x` and `y` components of this `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector2, Vector3};
    ///
    /// let vec = Vector3::new(1.0, 2.0, 3.0);
    /// let vec2 = vec.xy();
    /// assert_eq!(vec2.x, 1.0);
    /// assert_eq!(vec2.y, 2.0);
    /// ```
    pub fn xy(&self) -> Vector2 {
        Vector2::new(self.x, self.y)
    }

    /// Returns a new `Vector2` containing the `x` and `z` components of this `Vector3`.
    ///
    /// # Returns
    ///
    /// A new `Vector2` containing the `x` and `z` components of this `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector2, Vector3};
    ///
    /// let vec = Vector3::new(1.0, 2.0, 3.0);
    /// let vec2 = vec.xz();
    /// assert_eq!(vec2.x, 1.0);
    /// assert_eq!(vec2.y, 3.0);
    /// ```
    pub fn xz(&self) -> Vector2 {
        Vector2::new(self.x, self.z)
    }

    /// Returns a new `Vector2` containing the `y` and `z` components of this `Vector3`.
    ///
    /// # Returns
    ///
    /// A new `Vector2` containing the `y` and `z` components of this `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector2, Vector3};
    ///
    /// let vec = Vector3::new(1.0, 2.0, 3.0);
    /// let vec2 = vec.yz();
    /// assert_eq!(vec2.x, 2.0);
    /// assert_eq!(vec2.y, 3.0);
    /// ```
    pub fn yz(&self) -> Vector2 {
        Vector2::new(self.y, self.z)
    }

    /// Returns the angle between this `Vector3` and another `Vector3`.
    ///
    /// # Arguments
    ///
    /// * `other` - The other `Vector3` to calculate the angle with.
    ///
    /// # Returns
    ///
    /// The angle between this `Vector3` and `other` in radians.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec1 = Vector3::new(1.0, 0.0, 0.0);
    /// let vec2 = Vector3::new(0.0, 1.0, 0.0);
    /// let angle = vec1.angle_between(vec2);
    /// assert_eq!(angle, std::f32::consts::FRAC_PI_2);
    /// ```
    pub fn angle_between(&self, other: Vector3) -> f32 {
        let dot = self.dot(other);
        let length = self.length() * other.length();
        dot.acos() / length
    }

    /// Calculates the reflection of this `Vector3` off a surface with the given `normal`.
    ///
    /// # Arguments
    ///
    /// * `normal` - The normal of the surface to reflect off of.
    ///
    /// # Returns
    ///
    /// The reflection of this `Vector3` off the surface with the given `normal`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::new(1.0, 1.0, 0.0);
    /// let normal = Vector3::new(0.0, 1.0, 0.0);
    /// let reflected = vec.reflect(normal);
    /// assert_eq!(reflected, Vector3::new(1.0, -1.0, 0.0));
    /// ```
    pub fn reflect(&self, normal: Vector3) -> Vector3 {
        *self - normal * 2.0 * self.dot(normal)
    }

    /// Rotates this `Vector3` about the given `axis` by the given `angle` in radians.
    ///
    /// # Arguments
    ///
    /// * `axis` - The axis to rotate about.
    /// * `angle` - The angle to rotate by in radians.
    ///
    /// # Returns
    ///
    /// The rotated `Vector3`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector3;
    ///
    /// let vec = Vector3::new(1.0, 0.0, 0.0);
    /// let axis = Vector3::new(0.0, 0.0, 1.0);
    /// let angle = std::f32::consts::FRAC_PI_2;
    /// let rotated = vec.rotate_about_axis(axis, angle);
    /// assert_eq!(rotated, Vector3::new(0.0, 1.0, 0.0));
    /// ```
    pub fn rotate_about_axis(&self, axis: Vector3, angle: f32) -> Vector3 {
        let cos = angle.cos();
        let sin = angle.sin();
        let one_minus_cos = 1.0 - cos;

        let mut x = self.x * (cos + axis.x * axis.x * one_minus_cos)
            + self.y * (axis.x * axis.y * one_minus_cos - axis.z * sin)
            + self.z * (axis.x * axis.z * one_minus_cos + axis.y * sin);
        let mut y = self.x * (axis.y * axis.x * one_minus_cos + axis.z * sin)
            + self.y * (cos + axis.y * axis.y * one_minus_cos)
            + self.z * (axis.y * axis.z * one_minus_cos - axis.x * sin);
        let mut z = self.x * (axis.z * axis.x * one_minus_cos - axis.y * sin)
            + self.y * (axis.z * axis.y * one_minus_cos + axis.x * sin)
            + self.z * (cos + axis.z * axis.z * one_minus_cos);

        if x.abs() < 0.000001 {
            x = 0.0;
        }
        if y.abs() < 0.000001 {
            y = 0.0;
        }
        if z.abs() < 0.000001 {
            z = 0.0;
        }

        Vector3::new(x, y, z)
    }

}


impl std::ops::Index<usize> for Vector4 {
    type Output = f32;

    // Returns a reference to the element at the given `index` of this `Vector4`.
        ///
        /// # Arguments
        ///
        /// * `index` - The index of the element to retrieve.
        ///
        /// # Panics
        ///
        /// Panics if `index` is greater than or equal to 4.
        ///
        /// # Examples
        ///
        /// ```
        /// use cgl_rs::math::Vector4;
        ///
        /// let vec = Vector4::new(1.0, 2.0, 3.0, 4.0);
        /// assert_eq!(vec[0], 1.0);
        /// assert_eq!(vec[1], 2.0);
        /// assert_eq!(vec[2], 3.0);
        /// assert_eq!(vec[3], 4.0);
        /// ```
        fn index(&self, index: usize) -> &Self::Output {
            match index {
                0 => &self.x,
                1 => &self.y,
                2 => &self.z,
                3 => &self.w,
                _ => panic!("Index out of bounds for Vector3"),
            }
        }
}

impl std::ops::IndexMut<usize> for Vector4 {
    /// Returns a mutable reference to the element at the given `index` of this `Vector4`.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the element to retrieve.
    ///
    /// # Panics
    ///
    /// Panics if `index` is greater than or equal to 4.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector4;
    ///
    /// let mut vec = Vector4::new(1.0, 2.0, 3.0, 4.0);
    /// vec[0] = 5.0;
    /// assert_eq!(vec[0], 5.0);
    /// ```
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        match index {
            0 => &mut self.x,
            1 => &mut self.y,
            2 => &mut self.z,
            3 => &mut self.w,
            _ => panic!("Index out of bounds for Vector3"),
        }
    }
}

crate::macros::impl_vector_binary_ops!(Vector4, 4);

impl Vector4 {

    /// Creates a new `Vector4` with the given `x`, `y`, `z`, and `w` components.
    ///
    /// # Arguments
    ///
    /// * `x` - The `x` component of the new `Vector4`.
    /// * `y` - The `y` component of the new `Vector4`.
    /// * `z` - The `z` component of the new `Vector4`.
    /// * `w` - The `w` component of the new `Vector4`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector4;
    ///
    /// let vec = Vector4::new(1.0, 2.0, 3.0, 4.0);
    /// assert_eq!(vec.x, 1.0);
    /// assert_eq!(vec.y, 2.0);
    /// assert_eq!(vec.z, 3.0);
    /// assert_eq!(vec.w, 4.0);
    /// ```
    pub fn new(x: f32, y: f32, z: f32, w: f32) -> Vector4 {
        Vector4 { x, y, z, w }
    }


    /// Creates a new `Vector4` from a `Vector3` and a `w` component.
    ///
    /// # Arguments
    ///
    /// * `vec` - The `Vector3` to use as the `x`, `y`, and `z` components of the new `Vector4`.
    /// * `w` - The `w` component of the new `Vector4`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector3, Vector4};
    ///
    /// let vec3 = Vector3::new(1.0, 2.0, 3.0);
    /// let vec4 = Vector4::from_vec3(vec3, 4.0);
    /// assert_eq!(vec4.x, 1.0);
    /// assert_eq!(vec4.y, 2.0);
    /// assert_eq!(vec4.z, 3.0);
    /// assert_eq!(vec4.w, 4.0);
    /// ```
    pub fn from_vec3(vec: Vector3, w: f32) -> Vector4 {
        Vector4::new(vec.x, vec.y, vec.z, w)
    }

    /// Creates a new `Vector4` from a `Vector2`, `z`, and `w` components.
    ///
    /// # Arguments
    ///
    /// * `vec` - The `Vector2` to use as the `x` and `y` components of the new `Vector4`.
    /// * `z` - The `z` component of the new `Vector4`.
    /// * `w` - The `w` component of the new `Vector4`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector2, Vector4};
    ///
    /// let vec2 = Vector2::new(1.0, 2.0);
    /// let vec4 = Vector4::from_vec2(vec2, 3.0, 4.0);
    /// assert_eq!(vec4.x, 1.0);
    /// assert_eq!(vec4.y, 2.0);
    /// assert_eq!(vec4.z, 3.0);
    /// assert_eq!(vec4.w, 4.0);
    /// ```
    pub fn from_vec2(vec: Vector2, z: f32, w: f32) -> Vector4 {
        Vector4::new(vec.x, vec.y, z, w)
    }


    /// Returns a new `Vector4` with all components set to `0.0`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector4;
    ///
    /// let vec = Vector4::zero();
    /// assert_eq!(vec.x, 0.0);
    /// assert_eq!(vec.y, 0.0);
    /// assert_eq!(vec.z, 0.0);
    /// assert_eq!(vec.w, 0.0);
    /// ```
    pub fn zero() -> Vector4 {
        Vector4::new(0.0, 0.0, 0.0, 0.0)
    }

    /// Returns a new `Vector4` with all components set to `1.0`.
    ///
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::Vector4;
    ///
    /// let vec = Vector4::one();
    /// assert_eq!(vec.x, 1.0);
    /// assert_eq!(vec.y, 1.0);
    /// assert_eq!(vec.z, 1.0);
    /// assert_eq!(vec.w, 1.0);
    /// ```
    pub fn one() -> Vector4 {
        Vector4::new(1.0, 1.0, 1.0, 1.0)
    }


    /// Extracts the `x`, `y`, and `z` components of the `Vector4` as a new `Vector3`.
    /// 
    /// # Examples
    ///
    /// ```
    /// use cgl_rs::math::{Vector3, Vector4};
    ///
    /// let vec4 = Vector4::new(1.0, 2.0, 3.0, 4.0);
    /// let vec3 = vec4.xyz();
    /// assert_eq!(vec3.x, 1.0);
    /// assert_eq!(vec3.y, 2.0);
    /// assert_eq!(vec3.z, 3.0);
    /// ```
    pub fn xyz(&self) -> Vector3 {
        Vector3::new(self.x, self.y, self.z)
    }



}