//! Some popular activation functions and their derivatives.

#![allow(non_camel_case_types)]
use libc::c_float;

extern {

    // the activation functions
    fn CGL_utils_sigmoid(x: c_float) -> c_float;
    fn CGL_utils_sigmoid_derivative(x: c_float) -> c_float;
    fn CGL_utils_relu(x: c_float) -> c_float;
    fn CGL_utils_relu_derivative(x: c_float) -> c_float;
    fn CGL_utils_tanh(x: c_float) -> c_float;
    fn CGL_utils_tanh_derivative(x: c_float) -> c_float;
    fn CGL_utils_step(x: c_float) -> c_float;
    fn CGL_utils_step_derivative(x: c_float) -> c_float;
    fn CGL_utils_relu_leaky(x: c_float) -> c_float;
    fn CGL_utils_relu_leaky_derivative(x: c_float) -> c_float;
    fn CGL_utils_relu_smooth(x: c_float) -> c_float;
    fn CGL_utils_relu_smooth_derivative(x: c_float) -> c_float;
}


/// Calculates the sigmoid activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::sigmoid(x);
/// println!("sigmoid({}) = {}", x, result);
/// ```
pub fn sigmoid(x: f32) -> f32  {
    unsafe {
        CGL_utils_sigmoid(x as c_float) as f32
    }
}

/// Calculates the derivative of the sigmoid activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::sigmoid_derivative(x);
/// println!("sigmoid_derivative({}) = {}", x, result);
/// ```
pub fn sigmoid_derivative(x: f32) -> f32  {
    unsafe {
        CGL_utils_sigmoid_derivative(x as c_float) as f32
    }
}

/// Calculates the ReLU activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::relu(x);
/// println!("relu({}) = {}", x, result);
/// ```
pub fn relu(x: f32) -> f32  {
    unsafe {
        CGL_utils_relu(x as c_float) as f32
    }
}

/// Calculates the derivative of the ReLU activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::relu_derivative(x);
/// println!("relu_derivative({}) = {}", x, result);
/// ```
pub fn relu_derivative(x: f32) -> f32  {
    unsafe {
        CGL_utils_relu_derivative(x as c_float) as f32
    }
}

/// Calculates the hyperbolic tangent activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::tanh(x);
/// println!("tanh({}) = {}", x, result);
/// ```
pub fn tanh(x: f32) -> f32  {
    unsafe {
        CGL_utils_tanh(x as c_float) as f32
    }
}

/// Calculates the derivative of the hyperbolic tangent activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::tanh_derivative(x);
/// println!("tanh_derivative({}) = {}", x, result);
/// ```
pub fn tanh_derivative(x: f32) -> f32  {
    unsafe {
        CGL_utils_tanh_derivative(x as c_float) as f32
    }
}

/// Calculates the step activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::step(x);
/// println!("step({}) = {}", x, result);
/// ```
pub fn step(x: f32) -> f32  {
    unsafe {
        CGL_utils_step(x as c_float) as f32
    }
}

/// Calculates the derivative of the step activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::step_derivative(x);
/// println!("step_derivative({}) = {}", x, result);
/// ```
pub fn step_derivative(x: f32) -> f32  {
    unsafe {
        CGL_utils_step_derivative(x as c_float) as f32
    }
}

/// Calculates the leaky ReLU activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::relu_leaky(x);
/// println!("relu_leaky({}) = {}", x, result);
/// ```
pub fn relu_leaky(x: f32) -> f32  {
    unsafe {
        CGL_utils_relu_leaky(x as c_float) as f32
    }
}

/// Calculates the derivative of the leaky ReLU activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::relu_leaky_derivative(x);
/// println!("relu_leaky_derivative({}) = {}", x, result);
/// ```
pub fn relu_leaky_derivative(x: f32) -> f32  {
    unsafe {
        CGL_utils_relu_leaky_derivative(x as c_float) as f32
    }
}

/// Calculates the smooth ReLU activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::relu_smooth(x);
/// println!("relu_smooth({}) = {}", x, result);
/// ```
pub fn relu_smooth(x: f32) -> f32  {
    unsafe {
        CGL_utils_relu_smooth(x as c_float) as f32
    }
}

/// Calculates the derivative of the smooth ReLU activation function for a given input.
///
/// # Arguments
///
/// * `x` - A 32-bit float input value.
///
/// # Example
///
/// ```
/// let x = 0.5;
/// let result = cgl_rs::math::activations::relu_smooth_derivative(x);
/// println!("relu_smooth_derivative({}) = {}", x, result);
/// ```
pub fn relu_smooth_derivative(x: f32) -> f32  {
    unsafe {
        CGL_utils_relu_smooth_derivative(x as c_float) as f32
    }
}
