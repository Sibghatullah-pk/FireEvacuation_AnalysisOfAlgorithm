#pragma once
#include "Position.h"
#include "Grid.h"
#include <vector>
#include <queue>
#include <map>
#include <functional>

enum class PathfindingType {
    BFS,
    DIJKSTRA,
    A_STAR,
    HYBRID_ADAPTIVE,
    MULTI_OBJECTIVE
};

struct PathNode {
    Position pos;
    double gCost;  // Distance from start
    double hCost;  // Heuristic distance to goal
    double fCost;  // Total cost (g + h)
    double fireDanger; // Fire proximity cost
    Position parent;
    
    PathNode(Position p = Position(-1, -1), double g = 0, double h = 0, double fire = 0) 
        : pos(p), gCost(g), hCost(h), fCost(g + h), fireDanger(fire), parent(-1, -1) {}
    
    bool operator>(const PathNode& other) const {
        return fCost > other.fCost;
    }
};

class PathfindingAlgorithms {
private:
    static double calculateHeuristic(const Position& start, const Position& goal);
    static double calculateFireDanger(const Position& pos, const Grid& grid, const std::vector<Position>& firePositions);
    static std::vector<Position> reconstructPath(const std::map<Position, Position>& cameFrom, 
                                               const Position& start, const Position& goal);
    
public:
    // Basic algorithms
    static std::vector<Position> findPathBFS(const Grid& grid, const Position& start, const Position& goal);
    static std::vector<Position> findPathDijkstra(const Grid& grid, const Position& start, const Position& goal, 
                                                 const std::vector<Position>& firePositions = {});
    static std::vector<Position> findPathAStar(const Grid& grid, const Position& start, const Position& goal, 
                                              const std::vector<Position>& firePositions = {});
    
    // Advanced hybrid algorithms
    static std::vector<Position> findPathHybridAdaptive(const Grid& grid, const Position& start, const Position& goal, 
                                                       const std::vector<Position>& firePositions, int stepCount);
    static std::vector<Position> findPathMultiObjective(const Grid& grid, const Position& start, const Position& goal, 
                                                       const std::vector<Position>& firePositions, 
                                                       double safetyWeight = 0.7, double speedWeight = 0.3);
    
    // Utility functions
    static PathfindingType selectOptimalAlgorithm(const Grid& grid, const Position& start, const Position& goal, 
                                                 const std::vector<Position>& firePositions, int stepCount);
    static std::vector<Position> findPath(PathfindingType type, const Grid& grid, const Position& start, 
                                         const Position& goal, const std::vector<Position>& firePositions = {}, 
                                         int stepCount = 0);
};