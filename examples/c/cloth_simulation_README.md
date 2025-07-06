# Cloth Simulation Example

This example demonstrates a real-time cloth simulation using the CGL graphics library.

## Features

- **Physics Simulation**: Uses compute shaders for GPU-accelerated physics
- **Verlet Integration**: Stable numerical integration for particle dynamics
- **Spring Constraints**: Structural and shear springs maintain cloth structure
- **Interactive Controls**: Real-time parameter adjustment
- **Visual Effects**: Point sprite rendering with lighting

## Technical Details

### Cloth Structure
- 32x32 grid of particles (1024 total particles)
- Each particle has position, velocity, previous position, and normal
- Spring connections between neighboring particles (horizontal, vertical, diagonal)

### Physics Simulation
- **Verlet Integration**: Position-based dynamics for stability
- **Spring Forces**: Hooke's law with configurable stiffness
- **External Forces**: Gravity and wind simulation
- **Constraints**: Top corners pinned, ground collision
- **Damping**: Velocity damping to prevent excessive oscillation

### Rendering
- **Compute Shaders**: Physics calculations run on GPU
- **Point Sprites**: Each particle rendered as a circular point
- **Lighting**: Simple diffuse lighting based on surface normals
- **Camera**: Rotating camera for better visualization

## Building

### Prerequisites
- GCC compiler
- OpenGL development libraries
- GLFW library
- GLAD OpenGL loader (included)

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install gcc libglfw3-dev libgl1-mesa-dev
```

### Compilation
```bash
# From the CGL repository root directory
gcc -o cloth_simulation examples/c/cloth_simulation.c glad/src/glad.c \
    -I. -Iglad/include -lGL -lglfw -lm -pthread -std=c99
```

## Running

```bash
./cloth_simulation
```

## Controls

| Key | Action |
|-----|--------|
| **SPACE** | Pause/Resume simulation |
| **R** | Reset cloth to initial state |
| **UP/DOWN** | Adjust gravity strength |
| **LEFT/RIGHT** | Adjust wind strength |
| **ESC** | Exit application |

## Physics Parameters

The simulation includes several configurable parameters:

- **Gravity**: Downward force affecting all particles
- **Wind**: Horizontal force for dynamic effects
- **Spring Strength**: Stiffness of cloth constraints
- **Damping**: Energy dissipation factor
- **Rest Length**: Natural length of spring connections

## Implementation Notes

### Compute Shader Stages
1. **Initialization**: Set up particle grid and constraints
2. **Physics Update**: Apply forces and integrate motion
3. **Normal Calculation**: Compute surface normals for lighting

### Performance Considerations
- GPU-based physics for high particle counts
- Memory barriers for compute shader synchronization
- Frame rate limiting to prevent excessive time steps

### Limitations
- Simplified collision detection (ground plane only)
- Point sprite rendering (no actual cloth mesh)
- Fixed topology (no tearing or cutting)

## Future Enhancements

Potential improvements for the simulation:

1. **Mesh Rendering**: Generate triangular mesh from particle grid
2. **Advanced Collisions**: Sphere and box collision objects
3. **Cloth Tearing**: Dynamic topology changes
4. **Texture Mapping**: Apply fabric textures
5. **Self-Collision**: Prevent cloth from intersecting itself
6. **Wind Simulation**: More realistic aerodynamic forces

## Educational Value

This example demonstrates:
- GPU compute shader programming
- Numerical integration techniques
- Constraint-based physics
- Real-time graphics optimization
- Interactive parameter tuning

The code is well-commented and follows CGL patterns, making it suitable for learning GPU-based physics simulation techniques.