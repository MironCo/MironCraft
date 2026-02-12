# MironCraft

A Minecraft-inspired voxel engine written in C++ and OpenGL.

## Features

- Procedural terrain generation with multiple biomes (forest, hills, grassland)
- Tree generation
- Real-time shadow mapping
- Cel-shaded rendering with black outlines
- Block breaking with raycasting
- First-person player movement with physics and collision
- Face culling optimization (only renders visible block faces)

## Controls

| Key | Action |
|-----|--------|
| W/A/S/D | Move |
| Space | Jump |
| Mouse | Look around |
| Left Click | Break block |
| Q/E | Rotate sun/light |
| Escape | Release cursor |

## Building

Requires:
- C++17 compiler
- GLFW
- GLM
- OpenGL 3.3+

```bash
make        # Build
make run    # Build and run
make clean  # Clean build files
```
