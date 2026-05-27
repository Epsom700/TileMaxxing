# TileMaxxing

TileMaxxing is a C++ implementation of the 2048 game that includes an AI solver. It operates directly in the terminal, utilizing ANSI escape sequences to display a vividly colored game board. The AI is powered by Monte Carlo Tree Search (MCTS) and can execute neural network inference via an ONNX model (`tile_value_net.onnx`) for enhanced evaluation.

## Features

* **Terminal Interface**: Play the game manually with standard WASD keys, featuring color-coded 2048 tiles.
* **MCTS AI Solver**: An intelligent solver employing Monte Carlo Tree Search that utilizes distinct chance nodes and decision nodes to navigate the game's randomness and determine optimal moves. By pressing 'M', the AI executes 200 rollouts to predict the best possible swipe.
* **ONNX Runtime Integration**: Automatically downloads and links prebuilt Microsoft ONNX Runtime binaries (v1.18.0) via CMake to execute evaluations on macOS and Linux platforms seamlessly.

## Getting Started

### Prerequisites
This project requires CMake (minimum version 3.16) and a compiler supporting the C++17 standard.

### Building
The build process compiles several executables including the main game (`tilemax`), data capture tools (`data_capture`), and test suites (`test_board`, `test_mcts`). 

1. Create a build directory: `mkdir build && cd build`
2. Generate build files: `cmake ..`
3. Compile the executables: `make`
4. Run the game: `./tilemax`

## Controls

* **W** - Swipe Up
* **A** - Swipe Left
* **S** - Swipe Down
* **D** - Swipe Right
* **M** - MCTS AI Best Move
* **Q** - Quit

## AI Performance

**High-Score Runs(Value Network):**
* Game ID 34: score 52684
* Game ID 28, score 51084
* Game ID 58, score 58716

**Highest-Score(Pure Heuristics)**
* Game ID 103: 75,636
* Game ID 68: 67,340
* Game ID 54: 63,812

## How It Works (Flow Diagram)

Below is a representation of the game's execution flow and how the MCTS system interfaces with gameplay:

```mermaid
graph TD
    A[Start Game] --> B[Render Board]
    B --> C{Wait for User Input}
    
    %% Manual Flow
    C -->|W/A/S/D| D[Apply Swipe Move]
    
    %% AI Flow
    C -->|M| E[Initialize MCTS]
    E --> F[Select Node using UCB1]
    F --> G[Expand Node]
    G --> H[Rollout & Neural Net Evaluation]
    H --> I[Backpropagate Score]
    I -.-> |Iterate 200 Rollouts| F
    I --> J[Apply Best MCTS Move]
    
    %% Board Update
    J --> K{Is Move Legal?}
    D --> K
    K -->|Yes| L[Spawn New Tile]
    K -->|No| B
    L --> M{Is Game Over?}
    M -->|No| B
    M -->|Yes| N[End Game & Show Score]
    
    %% Quit
    C -->|Q| N

mermaid```


Running v2 bootstraap right now, Let's hope I am back soon.