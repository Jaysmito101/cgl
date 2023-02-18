<p align="center"><img width="300" src="https://raw.githubusercontent.com/Jaysmito101/cgl/main/images/CGL_logo_low_mid_res.png" border="0">
</p>



<p align="center">

<img src="https://img.shields.io/github/issues-pr/Jaysmito101/cgl?style=for-the-badge" />
<img alt="Lines of code" src="https://img.shields.io/tokei/lines/github/Jaysmito101/cgl?style=for-the-badge" />
<img src="https://img.shields.io/github/repo-size/Jaysmito101/cgl?style=for-the-badge" />
<img alt="Maintenance" src="https://img.shields.io/maintenance/yes/2023?style=for-the-badge" />
<a href="https://patreon.com/jaysmito101"><img src="https://img.shields.io/endpoint.svg?url=https%3A%2F%2Fshieldsio-patreon.vercel.app%2Fapi%3Fusername%3Djaysmito101%26type%3Dpledges&style=for-the-badge" alt="Support me on Patreon" /></a>

</p>

<br/>



# Index
* [Intro](#cgl)
* [Platforms](#target-platforms)
* [Features](#what-does-cgl-provide)
* [Showreels](#showreels)

<br>

# CGL
CGL (C Graphics Library) is a multipurpose library mainly for recreational coding /  demo scenes / prototyping / small games / experimentation. This has a **lot of utilities for graphics**. And best of all all of it is inside a single header file `cgl.h`.  Also CGL is made purely in C but is also compatible with C++. 

**NOTE** : Do not think that header only means its going to increase compile time as the implementation needs be enabled only for 1 file using `#define CGL_IMPLEMENTATION`. See [Examples](./examples)

<br>

## Target Platforms

- <b><i>Windows</b></i>
- <b><i>Linux</b></i>
- <b><i>MacOS (untested)</b></i>
- <b><i>WebAssembly (Beta)</b></i>
- <b><i>Android (Coming Soon)</b></i>

<br>

## What does CGL Provide? 

* Windowing library (Optional)
  - You can completely disable it by `#define CGL_EXCLUDE_WINDOW_API`
  - This windowing library is primarily a wrapper GLFW along with a few extra functionalities. **Example** : In case you are using some library like `nuklear` for GUI it will mess up all `glfw` callbacks so with CGL you can restore the CGL callbacks with a call to `CGL_window_resecure_callbacks`

* Utility functionalities
  - Reading/Writing files
  - Random float/int/bool/vec2/vec3/color generation
  - CRC32/CRC64
  - ROT13 encryption
  - General Purpose Hashing Functions [refer here]( http://www.azillionmonkeys.com/qed/hash.html)
  - Colored printf (red, green, blue, gray/yellow)
  - Point/Triangle intersection check
  - TODO: [ MD5 / SHA 256 / SHA 128 / AES ]

* Noise API
  - Multiple faster Alternatives to libc's rand
  - Procedural Coherent Noise Algorithms
    - Perlin's Noise (Improved Version)
    - OpenSimplex2
    - Value Noise
    - Worley Noise (or Cellular Noise)
  - Fractals like FBm, Rigid, Billow, PingPong
  - Parameters for Octaves/Lacunarity/Weighted Strength/Gain
 
* Triangulation
   - Bower Watson Algorithm for Delaunay Triangulator
   
* Artificial Intelligence
  - Neural Netowrks
  - Backpropagation
  - Serializing/Deserializing networks
  - Multi Variable Linear Regression

* Graph Algorithms
  - A* Path Finding (general purpose)

* Data structures
  - List(dynamic array) + Stack (implemented together)
  - Hashtable -> This hastable is general purpose. Key can be string or a n-bit buffer. The value can be anything int, string, float, custom types, ...
     - Hashtable Iterator -> Iterate through the hashtable using a [simple](https://github.com/Jaysmito101/cgl/blob/main/examples/using_hashtable_iterator.c) API
  
* Logger
  - Can be enabled/disabled by `#define CGL_DISABLE_LOGGER`
  - Log to multiple log files simultaneously
  - Log to console with colored output for seperate log levels
  - Logger with auto timestamps
  
* Cross Platform Networking (Optional)
  - You can disable all networking by `#define CGL_EXCLUDE_NETWORKING`
  - Low-level sockets 
  - SSL sockets (optional) (requires  OpenSSL)
  - HTTP/HTTPS request (beta)
  
* General Purpose Markov Chains (Optional) [Example](./examples/markov_text_generation.c)
  - Can work with any type of data ( text / image / etc.  )
  - Train/Generate with 3 - 4 lines of code
  - Trainer implemented for text generation (n-gram based)
  - Custom trainer API for custom scenarios

* Cross Platform Threading
  - Threads
  - Mutex
  - Condition Variables (TODO)
  - NOTE: Implemented using `Win32 Threads` on Windows and `pthread` on Linux. (on Linux you need to link `pthread` to build)

* Bloom
  - Apply bloom to any tuxtures with just 1 line of code
  - Implementation based of Unity's bloom
  - Custom thresholding
  - Custom downsample/upsample passes
  - Entirely done in Compute Shaders
  
* 2D Collision Detection
  - Detect collisions between 2D polygons
  - Generate Seperating Axes for polygons
  - Get Overlap distance
  - GJK (Gilbert–Johnson–Keerthi distance algorithm)
  - EPA (Expanding Polytope Algorithm)
  - SAT (Seperate Axis Theorem)

* Marching Square
  - Fully Customizablt Marcher
  - Linear Interpolation supported
  - Generates 2D Mesh (Triangles) for CGL

* Toon Post Processor
  - Outline Effect
  - Toon Shading Effect
  - Hatching Effect
  - All in a single post process call ( no per object calculation)
  - Completely implemented in Compute Shader
  - Customizable

* CGL Ray Cast
  - Fast 2D Ray Cast
  - Custom Walls 
  - Bake to Triangle Mesh
  - Public ray cast functions

* CGL Node Editor
  - Very fast as its powered by CGL Widgets
  - Custom nodes, pins, links
  - Minimal & Powerful
  - Render your own nodes & links type API
  - Zoom In/Out
  - Global Offsets
 
* CGL Audio API
  - Cross Platform (OpenAL backend)
  - Simple API
  - WAV File Loader/Sampler  

* CGL Widgets (Optional)
  - You can disable it by `#define CGL_EXCLUDE_WIDGETS`
  - API Like [p5.js](https://p5js.org/)
  - Text widgets (render high quality crisp text without loading or baking any font)
  - Batch Renderer backend (very fast even for a large number of widgets)
  - draw (filled or stroked) :
    - triangle [`CGL_widgets_add_triangle`]
    - general quad [`CGL_widgets_add_quad`]
    - rectangle [`CGL_widgets_add_rect` `CGL_widgets_add_rect2f`]
    - line [`CGL_widgets_add_line`]
    - circle [`CGL_widgets_add_circle` `CGL_widgets_add_circle2f`]
    - oval [`CGL_widgets_add_oval`, `CGL_widgets_add_oval2f`]
    - arc 
  - Plot 
    - Scatter plot
    - Bar Graph (vertical/horizontal)
    - Pie Chart
    - Plot a function
  - Advanced Bezier Curve( lines or dotted) widget
  - Add individual vertices
  - Adjust stroke color/thickness
  - Customize Batch renderer max vertices capacity (for low memory systems)

* Math library
  - Advanced Matrix Library (this is seperate from matrix lib for graphics)
  - vec2/vec3/vec4
  - mat3/mat4 (for graphics)
  - add/sub/mul/div/scale/length/normalize/lerp/min/max/equal for vec2/vec3/vec4
  - rotate_x/rotate_y/rotate_z for vec3
  - scale/translate/rotate_x/rotate_y/rotate_z/add/sub/mul for mat4
  - perspective for mat4
  - transpose for mat4/(mat3 TODO)
  - Rotation Matrices using Goldman's Method
  - look_at matrix 
  - Quaternion math
  - Transform vectors
  - **NOTE:** Most of math functions are implemented via macros so will be **totally inclined** and quite fast without any unnecessary function calls

* High Level OpenGL API for (Optional)
  - You can completely disable it by `#define CGL_EXCLUDE_GRAPHICS_API`
  - Texture (2D / 2D Array / Cube map) 
  - Framebuffers
  - SSBO (Shader Storage Buffer Object)
  - UBO (Uniform Buffer Object)
  - Shaders 
    - Vertex & Fragment (Geometry Shader not included as its not very widely used)
    - Compute Shader API 

* CGL Mesh API
  - CGL has a high level API for handling meshes
  - 2 types of meshes are there
    - CPU mesh -> stores the data also used for mesh operations like
      - generate triangle
      - generate quad
      - load OBJ (beta)
      - generate cube
      - generate plane
      - generate  cylinder
      - generate sphere
      - generate mesh out of any parametric surface function [refer here](https://stackoverflow.com/a/31326534/14911094)
      - calculate normals 
      - perform operation on meshes
        - add 2 meshes
        - offset vertices
        - etc .
    - GPU mesh -> the pointer to the data stored on GPU side (internally handles the Vertex buffer, Index buffer, Vertex Array) and can used for
      - render
      - render instanced
      - render wireframe
      - render wireframe instanced
      
* CGL camera
  - CGL provides a proper camera abstraction
  - Perspective & Orthographic
  - It internally handles all matrix calculations (just input the position and rotation)
  - Auto calculates the Up, Right, Front vectors
 
* Text Rendering (Optional) (Requires [FreeType2](http://freetype.org/))
  - You can completely disable it with `#define CGL_EXCLUDE_TEXT_RENDER`
  - Load Fonts from `.ttf` files
  - Bake bitmaps for characters
  - Bake textures from strings

* Trail Renderer
  - Fast 3D Trail Renderer
  - Bake to mesh
  - Custom Shader support
  - Fully customizable
  
 
* Sky Renderer (Optional)
  - You can completely disable it with `#define CGL_EXCLUDE_SKY_RENDERER`
  - Supports both a Sky Box (cube mesh) and Sky Sphere/Dome (sphere mesh)
  - Supports Cube map Textured Sky 
  - Supports Realtime Procedurally Generated Sky ( + procedural clouds)
  - Render a beautiful sky with just 3 - 5 lines of code

* Phong Renderer (Optional) 
  - You can disable it via `#define CGL_EXCLUDE_PHONG_RENDERER`
  - It has:
    - Phone Pipeline -> it is the pipeline holding shader data and global engine data. Options available are
      - enable/disable blinn correction
      - enable/disable depth test
      - enable/disable gamma correction
      - setup ambient lighting
      - add/remove lights
    - Phong Light -> It can be of 3 types:
      - Directional -> it takes (direction, color, intensity)
      - Point -> it takes (position, color, itensity, constant, linear, quadratic)
      - Spot (TODO)
    - Phong Material
      - dissuse texture/color
      - specular texture/color
      - shininess
      - normal maps (TODO)

* Tilemap Renderer (Optional)
   - You can disable it with `#define CGL_EXCLUDE_TILEMAP_RENDERER`
   - Renders a NxN tilemap with a single line of code
   - Each tile can have following states
     - clear (transparent or disabled)
     - solid color
     - texture -> Texture can be supplied via:
       - texture 2d array -> you need to provide depth for each tile
       - tileset or texture atlas -> you have to provide bounds (normalized 0.0-1.0) of the area of the alas to be used on tile
    - NOTE: this tile render renders only 4 vertices and has only 1 draw call  (not a instanced call so its quite fast
    
  
 ## Things that are being worked on:
 
 * PBR renderer (optional)
 * IBL (optional)
 
 ## Showreels
 


https://user-images.githubusercontent.com/73700725/211901867-8d2ddf87-13fa-4115-827a-3b43ed29b92d.mp4

https://user-images.githubusercontent.com/73700725/211902216-5fee0cff-f7db-4a3e-a16b-c4962d275d90.mp4

https://user-images.githubusercontent.com/73700725/211901919-d32778b1-7967-4749-b22d-ab95bab8d88e.mp4
 
https://user-images.githubusercontent.com/73700725/205131064-2ea36253-7976-4b02-bfdf-290b5a8e2171.mp4
 
https://user-images.githubusercontent.com/73700725/205130311-87cdddbb-e96f-412a-a9b5-96b2f474f6d1.mp4

https://user-images.githubusercontent.com/73700725/205130454-017992a5-f786-4b7e-b330-9046edbec25c.mp4

https://user-images.githubusercontent.com/73700725/205130582-8f6a4ce8-d932-402e-ad88-24a2e7d090b7.mp4

https://user-images.githubusercontent.com/73700725/208747347-6f0c0d49-c11c-4bf0-8497-82806e53974f.mp4




**For more see [Examples](./examples) and [Showreel](./showreels)**

<br>
