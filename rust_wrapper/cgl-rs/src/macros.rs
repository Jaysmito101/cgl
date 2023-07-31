//! This contains the macros used in the CGL library.

// The vector macros

macro_rules! impl_vector_binary_ops {
    ($vector:ident, $n:expr) => {
        // Implementation of addition
        impl std::ops::Add<$vector> for $vector {
            type Output = $vector;

            fn add(self, rhs: $vector) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = self[i] + rhs[i];
                }
                result
            }
        }

        // Implementation of multiplication
        impl std::ops::Mul<$vector> for $vector {
            type Output = $vector;

            fn mul(self, rhs: $vector) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = self[i] * rhs[i];
                }
                result
            }
        }

        // Implementation of scalar multiplication
        impl std::ops::Mul<f32> for $vector {
            type Output = $vector;

            fn mul(self, rhs: f32) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = self[i] * rhs;
                }
                result
            }
        }

        // Implementation of subtraction
        impl std::ops::Sub<$vector> for $vector {
            type Output = $vector;

            fn sub(self, rhs: $vector) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = self[i] - rhs[i];
                }
                result
            }
        }

        // Implementation of division
        impl std::ops::Div<$vector> for $vector {
            type Output = $vector;

            fn div(self, rhs: $vector) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = self[i] / rhs[i];
                }
                result
            }
        }

        // Implementation of scalar division
        impl std::ops::Div<f32> for $vector {
            type Output = $vector;

            fn div(self, rhs: f32) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = self[i] / rhs;
                }
                result
            }
        }

        // Implementation of negation
        impl std::ops::Neg for $vector {
            type Output = $vector;

            fn neg(self) -> $vector {
                let mut result = $vector::zero();
                for i in 0..$n {
                    result[i] = -self[i];
                }
                result
            }
        }
    };
}

pub(crate) use impl_vector_binary_ops;