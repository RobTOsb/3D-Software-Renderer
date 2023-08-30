# 3D-Software-Renderer

This is my software renderer. I set out to make one as I have always been interested in computer graphics and I also wanted to get better at using C++. It is a forward renderer with a full PBR pipeline that renders light and materials realistically using a Cook-Torrance BRDF and implements a fresnel effect using the fresnel schlick approximation.

It uses the barycentric method to rasterising the triangles and is fully parallelised for good performance and uses tangent space normal mappig. It renders roughly ~100k vertices at 30 fps at 1920 X 1080 resolution on a AMD Ryzen 5 5600x.

- [3D-Software-Renderer](#3d-software-renderer)
  - [Example Images](#example-images)
  - [Motivation](#motivation)
  - [Features](#features)
  - [Dependencies](#dependencies)
  - [What needs to be done](#what-needs-to-be-done)
  - [Resources I used](#resources-i-used)
  - [Assets](#assets)


## Example Images

## Motivation

I've always liked video games, ever since I was a child. Computer graphics fascinated me and I liked looking at pretty pictures in games. I wanted to combine that love of games and computer graphics with my skillset of C++. I wanted to achieve at least 30fps in a relatively large model at 1280 X 720. i have managed to exceed this goal by getting around 30 fps at 1920 X 1080. This was also my first large project and wanted a chance to practice architecting my program using Object Oriented Programming (OOP). I think overall this project has been a success for me and I am proud of what I have achieved.

## Features

- Fully parallelised forward barycentric rasteriser.
- Tangent space normal mapping
- A roughness-metallic PBR rendering pipeline using a programmable vertex and fragment shader.
- A basic scene loader 
- A basic triangle clipper
- A fully controllable camera
- Parallax occlusion mapping
- Backface culling
- Texture repeating
- Point light list with linear and quadratic attenuation.


## Dependencies
- SDL2
- SDL Image
- Tinyobjloader
- glm


## What needs to be done

- View frustum culling needs to be implemented, this will give big performance gains on multi model scenes.
- An implementation of SIMD intrinsics to further improve PBR performance in the shader.
- Reduce the amount of barycentric coordinate calculations in the rasteriser hot path.

## Resources I used
- [TinyRenderer](https://github.com/ssloy/tinyrenderer)
- Real Time Rendering by Akenine-Moller et al.
- [SSGE by Angel Ortiz](https://github.com/Angelo1211/SoftwareRenderer)
- Real-Time Collision Detection by Christer Ericson
- [ScratchAPixel](https://www.scratchapixel.com/)
- [LearnOpenGL](https://learnopengl.com/)

## Assets


