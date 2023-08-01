//! Math Library of CGL

#![allow(non_camel_case_types)]

pub mod activations;
pub mod constants;

mod vector;
pub use self::vector::*;

mod matrix;
pub use self::matrix::*;