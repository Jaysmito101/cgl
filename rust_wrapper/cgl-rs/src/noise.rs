//! The CGL Noise Module.

#![allow(non_camel_case_types)]

/// This is a controllable parameter of CGL library
/// But as of now, it cannot be controlled from Rust
pub type NoiseDataType = f32;

/// The type of noise to generate.
pub enum NoiseType {
    Perlin          = 0,
    OpenSimplex     = 1,
    OpenSimplex2S   = 2,
    Value           = 3,
    ValueCubic      = 4,
    Worley          = 5
}

/// The type of fractal to use when generating noise.
pub enum FractalType {
    None            = 0,
    FBM             = 1,
    Billow          = 2,
    Rigid           = 3,
    PingPong        = 4
}

#[repr(C)] #[derive(Debug, Copy, Clone)]
pub struct NoiseParams {
    pub type_id: i32,
    pub fractal_type: i32,
    pub octaves: i32,
    pub frequency: NoiseDataType,
    pub lacunarity: NoiseDataType,
    pub gain: NoiseDataType,
    pub weighted_strength: NoiseDataType,
    pub ping_pong_strength: NoiseDataType
}

extern {
    fn CGL_noise_init();
    fn CGL_noise_shutdown();
    fn CGL_noise_perlin(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
    fn CGL_noise_opensimplex(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
    fn CGL_noise_opensimplex2s(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
    fn CGL_noise_value(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
    fn CGL_noise_valuecubic(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
    fn CGL_noise_worley(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
    fn CGL_noise_params_default(params: *mut NoiseParams);
    fn CGL_noise_get(params: *const NoiseParams, x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType;
}

impl NoiseParams {
    /// Creates a new `NoiseParams` instance with default values.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn new() -> Self {
        let params = NoiseParams {
            type_id: 0,
            fractal_type: 0,
            octaves: 0,
            frequency: 0.0,
            lacunarity: 0.0,
            gain: 0.0,
            weighted_strength: 0.0,
            ping_pong_strength: 0.0
        };
        unsafe {
            CGL_noise_params_default(&params as *const NoiseParams as *mut NoiseParams);
        }
        params
    }

    /// Sets the type of noise to generate.
    /// 
    /// # Example
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_type(cgl_rs::noise::NoiseType::Perlin);
    /// cgl_rs::noise::shutdown();
    /// ````
    pub fn set_type(&mut self, noise_type: NoiseType) {
        self.type_id = noise_type as i32;
    }

    /// Sets the type of fractal to use when generating noise.
    /// 
    /// # Example
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_fractal_type(cgl_rs::noise::FractalType::FBM);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_fractal_type(&mut self, fractal_type: FractalType) {
        self.fractal_type = fractal_type as i32;
    }

    /// Sets the number of octaves to use when generating noise.
    /// 
    /// # Example
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_octaves(4);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_octaves(&mut self, octaves: i32) {
        self.octaves = octaves;
    }

    /// Sets the frequency of the noise.
    /// 
    /// # Example
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_frequency(0.01);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_frequency(&mut self, frequency: NoiseDataType) {
        self.frequency = frequency;
    }

    /// Sets the lacunarity of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_lacunarity(2.0);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_lacunarity(&mut self, lacunarity: NoiseDataType) {
        self.lacunarity = lacunarity;
    }

    /// Sets the gain of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_gain(0.5);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_gain(&mut self, gain: NoiseDataType) {
        self.gain = gain;
    }

    /// Sets the weighted strength of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_weighted_strength(0.5);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_weighted_strength(&mut self, weighted_strength: NoiseDataType) {
        self.weighted_strength = weighted_strength;
    }

    /// Sets the ping pong strength of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// params.set_ping_pong_strength(0.5);
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn set_ping_pong_strength(&mut self, ping_pong_strength: NoiseDataType) {
        self.ping_pong_strength = ping_pong_strength;
    }

    /// Gets the type of noise being generated.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let noise_type = params.get_type();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_type(&self) -> NoiseType {
        match self.type_id {
            0 => NoiseType::Perlin,
            1 => NoiseType::OpenSimplex,
            2 => NoiseType::OpenSimplex2S,
            3 => NoiseType::Value,
            4 => NoiseType::ValueCubic,
            5 => NoiseType::Worley,
            _ => panic!("Invalid noise type")
        }
    }

    /// Gets the type of fractal being used to generate noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let fractal_type = params.get_fractal_type();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_fractal_type(&self) -> FractalType {
        match self.fractal_type {
            0 => FractalType::None,
            1 => FractalType::FBM,
            2 => FractalType::Billow,
            3 => FractalType::Rigid,
            4 => FractalType::PingPong,
            _ => panic!("Invalid fractal type")
        }
    }

    /// Gets the number of octaves being used to generate noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let octaves = params.get_octaves();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_octaves(&self) -> i32 {
        self.octaves
    }

    /// Gets the frequency of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let frequency = params.get_frequency();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_frequency(&self) -> NoiseDataType {
        self.frequency
    }

    /// Gets the lacunarity of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let lacunarity = params.get_lacunarity();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_lacunarity(&self) -> NoiseDataType {
        self.lacunarity
    }

    /// Gets the gain of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let gain = params.get_gain();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_gain(&self) -> NoiseDataType {
        self.gain
    }

    /// Gets the weighted strength of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let weighted_strength = params.get_weighted_strength();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_weighted_strength(&self) -> NoiseDataType {
        self.weighted_strength
    }

    /// Gets the ping pong strength of the noise.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let params = cgl_rs::noise::NoiseParams::new();
    /// let ping_pong_strength = params.get_ping_pong_strength();
    /// cgl_rs::noise::shutdown();
    /// ```
    pub fn get_ping_pong_strength(&self) -> NoiseDataType {
        self.ping_pong_strength
    }

    /// Generates noise using the current parameters.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::noise::init();
    /// let mut params = cgl_rs::noise::NoiseParams::new();
    /// let noise = params.get_noise(0.0, 0.0, 0.0);
    /// cgl_rs::noise::shutdown();
    /// ```
    /// 
    /// # See also
    /// 
    /// * cgl_rs::noise::get
    pub fn get_noise(&self, x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
        unsafe {
            CGL_noise_get(self as *const NoiseParams as *mut NoiseParams, x, y, z)
        }
    }

}
/// Initializes the noise library.
/// 
/// This function must be called before any other noise functions are used.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// // Use noise functions here
/// cgl_rs::noise::shutdown();
/// ```
pub fn init() {
    unsafe {
        CGL_noise_init();
    }
}

/// Shuts down the noise library.
/// 
/// This function must be called when the noise library is no longer needed.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// // Use noise functions here
/// cgl_rs::noise::shutdown();
/// ```
pub fn shutdown() {
    unsafe {
        CGL_noise_shutdown();
    }
}


/// Gets the perlin noise value at the given coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let noise = cgl_rs::noise::perlin(0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
pub fn perlin(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_perlin(x, y, z)
    }
}


/// Gets the opensimplex noise value at the given coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let noise = cgl_rs::noise::opensimplex(0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
pub fn opensimplex(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_opensimplex(x, y, z)
    }
}

/// Gets the opensimplex2s noise value at the given coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let noise = cgl_rs::noise::opensimplex2s(0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
pub fn opensimplex2s(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_opensimplex2s(x, y, z)
    }
}

/// Gets the value noise value at the given coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let noise = cgl_rs::noise::value(0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
pub fn value(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_value(x, y, z)
    }
}


/// Gets the valuecubic noise value at the given coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let noise = cgl_rs::noise::valuecubic(0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
pub fn valuecubic(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_valuecubic(x, y, z)
    }
}


/// Gets the worley noise value at the given coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let noise = cgl_rs::noise::worley(0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
pub fn worley(x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_worley(x, y, z)
    }
}

/// Generates noise with the given NoiseParams and x, y, and z coordinates.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::noise::init();
/// let mut params = cgl_rs::noise::NoiseParams::new();
/// let noise = cgl_rs::noise::get(&params, 0.0, 0.0, 0.0);
/// cgl_rs::noise::shutdown();
/// ```
/// 
/// # See also
/// 
/// * cgl_rs::noise::NoiseParams::get_noise
pub fn get(params: &NoiseParams, x: NoiseDataType, y: NoiseDataType, z: NoiseDataType) -> NoiseDataType {
    unsafe {
        CGL_noise_get(params as *const NoiseParams, x, y, z)
    }
}
