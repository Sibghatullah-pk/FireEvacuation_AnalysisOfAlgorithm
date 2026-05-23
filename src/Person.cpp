#include "Person.h"
#include <algorithm>
#include <limits>
#include <sstream>

using namespace std;

Person::Person(const Position& pos, int personId) 
    : position(pos), target(-1, -1), alive(true), rescued(false), id(personId), 
      currentAlgorithm(PathfindingType::A_STAR), pathRecalculationCounter(0), lastFireDanger(0.0) {}

vector<Position> Person::findPathBFS(const Grid& grid, const Position& start, const Position& goal) {
    return PathfindingAlgorithms::findPathBFS(grid, start, goal);
}

Position Person::findNearestExit(const Grid& grid) {
    Position nearest(-1, -1);
    double minDistance = numeric_limits<double>::max();
    
    for (const Position& exit : grid.getExits()) {
        double distance = position.distanceTo(exit);
        if (distance < minDistance) {
            minDistance = distance;
            nearest = exit;
        }
    }
    
    return nearest;
}

void Person::calculatePath(const Grid& grid, const vector<Position>& firePositions, int stepCount) {
    target = findNearestExit(grid);
    if (target.x != -1 && target.y != -1) {
        // Adapt algorithm based on current situation
        adaptAlgorithm(grid, firePositions, stepCount);
        
        // Use the selected algorithm to find path
        path = PathfindingAlgorithms::findPath(currentAlgorithm, grid, position, target, firePositions, stepCount);
        pathRecalculationCounter++;
    }
}

void Person::adaptAlgorithm(const Grid& grid, const vector<Position>& firePositions, int stepCount) {
    // Calculate current fire danger
    double currentFireDanger = 0.0;
    if (!firePositions.empty()) {
        double minDistance = numeric_limits<double>::max();
        for (const Position& firePos : firePositions) {
            minDistance = min(minDistance, position.distanceTo(firePos));
        }
        currentFireDanger = minDistance < 5.0 ? (5.0 - minDistance) : 0.0;
    }
    
    // Adapt algorithm based on changing conditions
    if (abs(currentFireDanger - lastFireDanger) > 2.0 || pathRecalculationCounter % 5 == 0) {
        currentAlgorithm = PathfindingAlgorithms::selectOptimalAlgorithm(grid, position, target, firePositions, stepCount);
    }
    
    lastFireDanger = currentFireDanger;
}

bool Person::move(Grid& grid) {
    if (!alive || rescued) return false;
    
    if (grid.getCell(position) == 'F') {
        alive = false;
        return false;
    }
    
    if (path.empty()) {
        // This will be called with proper parameters from update()
        if (path.empty()) return false;
    }
    
    if (grid.getCell(position) != 'E') {
        grid.setCell(position, ' ');
    }
    
    position = path[0];
    path.erase(path.begin());
    
    if (grid.getCell(position) == 'E') {
        rescued = true;
        return true;
    }
    
    if (grid.getCell(position) == 'F') {
        alive = false;
        return false;
    }
    
    grid.setCell(position, 'P');
    return true;
}

void Person::update(Grid& grid, const vector<Position>& firePositions, int stepCount) {
    if (!alive || rescued) return;
    
    // Check if current path is blocked by fire or needs recalculation
    bool needsRecalculation = false;
    
    if (!path.empty() && grid.getCell(path[0]) == 'F') {
        needsRecalculation = true;
    }
    
    // Recalculate path periodically or when needed
    if (path.empty() || needsRecalculation || pathRecalculationCounter % 3 == 0) {
        path.clear();
        calculatePath(grid, firePositions, stepCount);
    }
    
    move(grid);
}

string Person::getAlgorithmName() const {
    switch (currentAlgorithm) {
        case PathfindingType::BFS: return "BFS";
        case PathfindingType::DIJKSTRA: return "Dijkstra";
        case PathfindingType::A_STAR: return "A*";
        case PathfindingType::HYBRID_ADAPTIVE: return "Hybrid";
        case PathfindingType::MULTI_OBJECTIVE: return "Multi-Obj";
        default: return "Unknown";
    }
}