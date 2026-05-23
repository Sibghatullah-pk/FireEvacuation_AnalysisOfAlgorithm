# Fire Evacuation Analysis of Algorithm

A Windows console simulation that visualizes how people evacuate a building while fire spreads through the grid. The project compares several pathfinding strategies, adapts algorithm choice to changing danger, and now exports a final run summary plus a grid snapshot into an output folder.

## What It Does

- Simulates multiple people navigating toward exits on a grid map.
- Spreads fire over time and forces people to recalculate paths as conditions change.
- Uses a mix of BFS, Dijkstra, A*, hybrid adaptive routing, and multi-objective pathfinding.
- Shows live status in the console with step count, rescued count, burned count, and optional heat map/statistics views.
- Saves final artifacts for each run into `output/`.

## Controls

From the main menu:

- `1` - Auto mode
- `2` - Manual mode
- `3` - Fast auto mode
- `Q` - Quit

During a simulation:

- `Q` - Quit the current run
- `M` - Switch between auto and manual mode
- `+` / `-` - Speed up or slow down the simulation
- `S` - Show detailed algorithm statistics
- `H` - Show the heat map
- `Space` - Advance one step in manual mode

## Output Folder

Every completed run writes two files into `output/`:

- `output/simulation_report_<timestamp>.txt` - Run summary, status breakdown, and person details
- `output/final_grid_<timestamp>.txt` - Final grid snapshot at the end of the simulation

These paths are also printed in the console after each run so they are easy to open from VS Code or Windows Explorer.

## Project Structure

- `main.cpp` - Application entry point and menu
- `SimulationManager.cpp/.h` - Simulation loop, display logic, and output export
- `Grid.cpp/.h` - Map generation and rendering
- `Fire.cpp/.h` - Fire spread logic
- `Person.cpp/.h` - Person behavior and adaptive evacuation logic
- `PathfindingAlgorithms.cpp/.h` - BFS, Dijkstra, A*, hybrid, and multi-objective routing
- `ConsoleUI.h` - Console rendering helpers
- `Position.h` - Grid coordinate type

## Notes

- The project is designed for Windows because it uses console-specific APIs.
- The simulator creates the `output/` folder automatically if it does not already exist.
- If you want to inspect a run after closing the program, open the files in `output/`.
