# Fire Evacuation Analysis of Algorithm

A Windows console simulation that visualizes how people evacuate a building while fire spreads through the grid. The project compares several pathfinding strategies, adapts algorithm choice to changing danger, and includes a settings system for difficulty, building size, fire spread, obstacle density, and optional output saving.

## What It Does

- Simulates multiple people navigating toward exits on a grid map.
- Spreads fire over time and forces people to recalculate paths as conditions change.
- Uses a mix of BFS, Dijkstra, A*, hybrid adaptive routing, and multi-objective pathfinding.
- Shows live status in the console with step count, rescued count, burned count, and optional heat map/statistics views.
- Can save final artifacts for each run into `output/` when enabled.
- Lets you choose a difficulty preset that changes building size, fire spread rate, obstacle density, and population size.

## Controls

From the main menu:

- `1` - Auto mode
- `2` - Manual mode
- `3` - Fast auto mode
- `4` - Open settings
- `Q` - Quit

In settings:

- `1` - Easy preset, small building, lighter fire spread
- `2` - Normal preset, medium building, balanced challenge
- `3` - Hard preset, large building, heavier fire spread
- `4` - Toggle the default output-saving preference
- `5` - Return to the main menu

During a simulation:

- `Q` - Quit the current run
- `M` - Switch between auto and manual mode
- `+` / `-` - Speed up or slow down the simulation
- `S` - Show detailed algorithm statistics
- `H` - Show the heat map
- `Space` - Advance one step in manual mode

## Screenshots

These screenshots are stored in the output folder for quick access:

- [Main simulation screenshot](output/Output!.png)
- [Additional simulation screenshot](output/Output!!.png)

## Output Folder

Every completed run writes two files into `output/`:

- `output/simulation_report_<timestamp>.txt` - Run summary, status breakdown, and person details
- `output/final_grid_<timestamp>.txt` - Final grid snapshot at the end of the simulation

Before each run, the program asks whether you want to save the output for that run. If you choose no, the simulation still runs normally and skips exporting files.

When saving is enabled, the paths are also printed in the console after each run so they are easy to open from VS Code or Windows Explorer.

## Project Structure

- `src/main.cpp` - Application entry point and menu
- `src/SimulationManager.cpp` and `include/SimulationManager.h` - Simulation loop, display logic, and output export
- `src/Grid.cpp` and `include/Grid.h` - Map generation, obstacle generation, and rendering
- `src/Fire.cpp` and `include/Fire.h` - Fire spread logic with configurable spread rate
- `src/Person.cpp` and `include/Person.h` - Person behavior and adaptive evacuation logic
- `src/PathfindingAlgorithms.cpp` and `include/PathfindingAlgorithms.h` - BFS, Dijkstra, A*, hybrid, and multi-objective routing
- `include/ConsoleUI.h` - Console rendering helpers
- `include/Position.h` - Grid coordinate type

## Notes

- The project is designed for Windows because it uses console-specific APIs.
- The simulator creates the `output/` folder automatically if saving is enabled and it does not already exist.
- If you want to inspect a run after closing the program, open the files in `output/`.

## Build

Use this command from the project root:

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/SimulationManager.cpp src/Grid.cpp src/Fire.cpp src/Person.cpp src/PathfindingAlgorithms.cpp -o fire_sim.exe
```

