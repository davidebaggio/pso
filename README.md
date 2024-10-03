# Swarm Optimization Algorithm with Raylib Visualization

This project implements the **Particle Swarm Optimization (PSO)** algorithm and visualizes the particle movements and convergence process using the **Raylib** library.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Dependencies](#dependencies)
- [License](#license)

## Introduction

Particle Swarm Optimization (PSO) is a popular optimization technique inspired by the social behavior of birds flocking or fish schooling. It is used to find the optimal solution by having a group of particles (agents) that explore the search space in collaboration. Each particle updates its position and velocity based on its best known position and the best position known by its neighbors.

This project provides a simple implementation of PSO and visualizes the swarm's movement and convergence process using the **Raylib** game development library.

## Features

- **Swarm Visualization**: See the particles (agents) move towards the solution in real time.
- **Real-time Interaction**: Adjust PSO parameters and restart the simulation.
- **Customizable Fitness Functions**: You can define your own functions for optimization.
- **3D Visualization**: Provides a clean, simple 3D environment for visualizing particle movement.

## Installation

1. Clone this repository:

   ```bash
   git clone https://github.com/davidebaggio/pso.git
   cd swarm-optimization-raylib
   ```

2. Install the required dependencies (Raylib):

   - If you're on **Linux**:

     ```bash
     sudo apt install raylib-dev
     ```

   - For **Windows** or **macOS**, follow the instructions on the official [Raylib website](https://www.raylib.com/) to install the library.

3. Compile the project:

   - On **Linux/macOS**:
     ```bash
     gcc -o swarm_optimization main.c -lraylib -lm
     ```
   - On **Windows**:
     ```bash
     make
     ```

4. Run the compiled program:

   ```bash
   ./swarm_optimization
   ```

## Dependencies

- **Raylib**: A simple and easy-to-use library to enjoy game programming. Used for rendering the visual aspects of the simulation.
- **C Standard Library**: Required for basic operations like mathematical computations and memory management.

To install Raylib on your system, refer to the official [Raylib Installation Guide](https://www.raylib.com/).

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
