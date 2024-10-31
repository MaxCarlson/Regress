# Regress

Regress is a C++ project designed as an **expression template** library for efficient matrix-matrix, matrix-vector, and vector-vector operations. The library includes modules for **expression evaluation**, **custom allocators**, and **high-performance matrix computations**. Leveraging object-oriented design, it is optimized for **computational efficiency**, including advanced use of **SIMD instructions** to accelerate matrix operations.

> **Note**: The Regress project was originally built around a being FFNN library, the focus of the library shifted to optimzed matrix-matrix operations.

## Table of Contents

- [Installation](#installation)
- [Project Structure](#project-structure)

## Installation

To set up the project locally, ensure that you have a C++ compiler (supporting C++17 or later) and Visual Studio installed.

1. Open `Regress.sln` in Visual Studio and build the solution to load the available project files.

## Project Structure

- **Regress/**: Contains a partially implemented feedforward neural network (FFNN). However, the focus of this library has shifted to optimizing matrix operations with expression templates.

- **Linear/**: Core linear algebra utilities, designed as a testbed for expression templates.
  - Entry point: `Main.cpp`
  - This module is intended to be compiled directly through Visual Studio. It provides a testing ground for optimized matrix and vector operations, including memory alignment and computational efficiency improvements.

  - Files and optimization modules include:
    - `Matrix.h`: Defines matrix operations for matrix and vector computations.
    - `AlignedAllocator.h`: Memory alignment utilities to enhance computational performance.
    - **Exprs/**: Houses expression templates to efficiently handle complex matrix expressions without redundant computations.
    - **Evaluators/**: Contains optimized evaluators that interpret expressions to reduce runtime overhead.
    - **Instructions/**: Integrates SIMD (Single Instruction, Multiple Data) optimizations, significantly boosting performance for matrix-matrix and matrix-vector operations by parallelizing computations.
    - **System/**: Provides system-level utilities to streamline resource management and maximize hardware capabilities.

- **test/**: Contains unit tests for the Linear library, designed to be loaded as a Visual Studio project to verify the functionality and performance of matrix operations, expression templates, and SIMD optimizations.

## Contributing

Contributions are more than welcome.
