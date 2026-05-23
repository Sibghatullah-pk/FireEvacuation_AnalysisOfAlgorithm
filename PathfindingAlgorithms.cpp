#include "PathfindingAlgorithms.h"
#include <queue>
#include <set>
#include <algorithm>
#include <limits>
#include <cmath>

using namespace std;

double PathfindingAlgorithms::calculateHeuristic(const Position& start, const Position& goal) {
    // Manhattan distance for grid-based movement
    return abs(start.x - goal.x) + abs(start.y - goal.y);
}

double PathfindingAlgorithms::calculateFireDanger(const Position& pos, const Grid& grid, const vector<Position>& firePositions) {
    double minFireDistance = numeric_limits<double>::max();
    
    for (const Position& firePos : firePositions) {
        double distance = pos.distanceTo(firePos);
        minFireDistance = min(minFireDistance, distance);
    }
    
    // Higher danger when closer to fire
    if (minFireDistance < 3.0) {
        return 10.0 / (minFireDistance + 0.1); // Avoid division by zero
    }
    return 0.0;
}

vector<Position> PathfindingAlgorithms::reconstructPath(const map<Position, Position>& cameFrom, 
                                                       const Position& start, const Position& goal) {
    vector<Position> path;
    Position current = goal;
    
    while (!(current == start)) {
        path.push_back(current);
        auto it = cameFrom.find(current);
        if (it == cameFrom.end()) break;
        current = it->second;
    }
    
    reverse(path.begin(), path.end());
    return path;
}

vector<Position> PathfindingAlgorithms::findPathBFS(const Grid& grid, const Position& start, const Position& goal) {
    if (start == goal) return {start};
    
    queue<Position> q;
    map<Position, Position> cameFrom;
    set<Position> visited;
    
    q.push(start);
    visited.insert(start);
    
    while (!q.empty()) {
        Position current = q.front();
        q.pop();
        
        if (current == goal) {
            return reconstructPath(cameFrom, start, goal);
        }
        
        for (const Position& neighbor : grid.getNeighbors(current)) {
            if (visited.find(neighbor) == visited.end() && 
                (grid.isWalkable(neighbor) || neighbor == goal)) {
                visited.insert(neighbor);
                cameFrom[neighbor] = current;
                q.push(neighbor);
            }
        }
    }
    
    return {};
}

vector<Position> PathfindingAlgorithms::findPathDijkstra(const Grid& grid, const Position& start, const Position& goal, 
                                                        const vector<Position>& firePositions) {
    if (start == goal) return {start};
    
    priority_queue<PathNode, vector<PathNode>, greater<PathNode>> pq;
    map<Position, double> distances;
    map<Position, Position> cameFrom;
    set<Position> visited;
    
    pq.push(PathNode(start, 0, 0));
    distances[start] = 0;
    
    while (!pq.empty()) {
        PathNode current = pq.top();
        pq.pop();
        
        if (visited.find(current.pos) != visited.end()) continue;
        visited.insert(current.pos);
        
        if (current.pos == goal) {
            return reconstructPath(cameFrom, start, goal);
        }
        
        for (const Position& neighbor : grid.getNeighbors(current.pos)) {
            if (visited.find(neighbor) != visited.end() || 
                (!grid.isWalkable(neighbor) && neighbor != goal)) continue;
            
            double moveCost = 1.0;
            double fireDanger = calculateFireDanger(neighbor, grid, firePositions);
            double newDistance = current.gCost + moveCost + fireDanger;
            
            if (distances.find(neighbor) == distances.end() || newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                cameFrom[neighbor] = current.pos;
                pq.push(PathNode(neighbor, newDistance, 0));
            }
        }
    }
    
    return {};
}

vector<Position> PathfindingAlgorithms::findPathAStar(const Grid& grid, const Position& start, const Position& goal, 
                                                     const vector<Position>& firePositions) {
    if (start == goal) return {start};
    
    priority_queue<PathNode, vector<PathNode>, greater<PathNode>> openSet;
    map<Position, double> gScore;
    map<Position, Position> cameFrom;
    set<Position> closedSet;
    
    double startH = calculateHeuristic(start, goal);
    openSet.push(PathNode(start, 0, startH));
    gScore[start] = 0;
    
    while (!openSet.empty()) {
        PathNode current = openSet.top();
        openSet.pop();
        
        if (closedSet.find(current.pos) != closedSet.end()) continue;
        closedSet.insert(current.pos);
        
        if (current.pos == goal) {
            return reconstructPath(cameFrom, start, goal);
        }
        
        for (const Position& neighbor : grid.getNeighbors(current.pos)) {
            if (closedSet.find(neighbor) != closedSet.end() || 
                (!grid.isWalkable(neighbor) && neighbor != goal)) continue;
            
            double moveCost = 1.0;
            double fireDanger = calculateFireDanger(neighbor, grid, firePositions);
            double tentativeG = current.gCost + moveCost + fireDanger * 0.5; // Weight fire danger less in A*
            
            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                gScore[neighbor] = tentativeG;
                cameFrom[neighbor] = current.pos;
                double h = calculateHeuristic(neighbor, goal);
                openSet.push(PathNode(neighbor, tentativeG, h));
            }
        }
    }
    
    return {};
}

vector<Position> PathfindingAlgorithms::findPathHybridAdaptive(const Grid& grid, const Position& start, const Position& goal, 
                                                              const vector<Position>& firePositions, int stepCount) {
    // Adaptive algorithm selection based on situation
    double fireProximity = calculateFireDanger(start, grid, firePositions);
    double distanceToGoal = calculateHeuristic(start, goal);
    
    // Very early game with no immediate fire threat and short distance: use simple BFS
    if (stepCount < 5 && fireProximity < 1.0 && distanceToGoal <= 6.0) {
        return findPathBFS(grid, start, goal);
    }
    // Early game or far from fire: use A* for speed
    else if (stepCount < 20 || fireProximity < 2.0) {
        return findPathAStar(grid, start, goal, firePositions);
    }
    // High fire danger: use Dijkstra for safety
    else if (fireProximity > 5.0) {
        return findPathDijkstra(grid, start, goal, firePositions);
    }
    // Medium danger: use multi-objective
    else {
        return findPathMultiObjective(grid, start, goal, firePositions, 0.6, 0.4);
    }
}

vector<Position> PathfindingAlgorithms::findPathMultiObjective(const Grid& grid, const Position& start, const Position& goal, 
                                                              const vector<Position>& firePositions, 
                                                              double safetyWeight, double speedWeight) {
    if (start == goal) return {start};
    
    priority_queue<PathNode, vector<PathNode>, greater<PathNode>> openSet;
    map<Position, double> gScore;
    map<Position, Position> cameFrom;
    set<Position> closedSet;
    
    double startH = calculateHeuristic(start, goal);
    openSet.push(PathNode(start, 0, startH));
    gScore[start] = 0;
    
    while (!openSet.empty()) {
        PathNode current = openSet.top();
        openSet.pop();
        
        if (closedSet.find(current.pos) != closedSet.end()) continue;
        closedSet.insert(current.pos);
        
        if (current.pos == goal) {
            return reconstructPath(cameFrom, start, goal);
        }
        
        for (const Position& neighbor : grid.getNeighbors(current.pos)) {
            if (closedSet.find(neighbor) != closedSet.end() || 
                (!grid.isWalkable(neighbor) && neighbor != goal)) continue;
            
            double moveCost = 1.0;
            double fireDanger = calculateFireDanger(neighbor, grid, firePositions);
            double heuristic = calculateHeuristic(neighbor, goal);
            
            // Multi-objective cost function
            double safetyCost = fireDanger;
            double speedCost = moveCost + heuristic * 0.1;
            double totalCost = safetyWeight * safetyCost + speedWeight * speedCost;
            
            double tentativeG = current.gCost + totalCost;
            
            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                gScore[neighbor] = tentativeG;
                cameFrom[neighbor] = current.pos;
                openSet.push(PathNode(neighbor, tentativeG, heuristic));
            }
        }
    }
    
    return {};
}

PathfindingType PathfindingAlgorithms::selectOptimalAlgorithm(const Grid& grid, const Position& start, const Position& goal, 
                                                             const vector<Position>& firePositions, int stepCount) {
    double fireProximity = calculateFireDanger(start, grid, firePositions);
    double distanceToGoal = calculateHeuristic(start, goal);
    int fireCount = firePositions.size();
    
    // Decision tree for algorithm selection
    if (fireCount == 0 && distanceToGoal <= 5.0) {
        return PathfindingType::BFS; // No fire and short distance, use simple BFS
    }
    else if (fireCount == 0) {
        return PathfindingType::A_STAR; // No fire but longer distance, use A* for speed
    }
    else if (stepCount < 10 && fireProximity < 3.0 && distanceToGoal <= 8.0) {
        return PathfindingType::BFS; // Early game, low fire threat, short distance
    }
    else if (fireProximity > 8.0) {
        return PathfindingType::DIJKSTRA; // High danger, prioritize safety
    }
    else if (stepCount > 50 && fireCount > 5) {
        return PathfindingType::MULTI_OBJECTIVE; // Late game with lots of fire
    }
    else if (distanceToGoal > 15) {
        return PathfindingType::HYBRID_ADAPTIVE; // Long distance, adapt as needed
    }
    else {
        return PathfindingType::A_STAR; // Default to A* for efficiency
    }
}

vector<Position> PathfindingAlgorithms::findPath(PathfindingType type, const Grid& grid, const Position& start, 
                                                const Position& goal, const vector<Position>& firePositions, 
                                                int stepCount) {
    switch (type) {
        case PathfindingType::BFS:
            return findPathBFS(grid, start, goal);
        case PathfindingType::DIJKSTRA:
            return findPathDijkstra(grid, start, goal, firePositions);
        case PathfindingType::A_STAR:
            return findPathAStar(grid, start, goal, firePositions);
        case PathfindingType::HYBRID_ADAPTIVE:
            return findPathHybridAdaptive(grid, start, goal, firePositions, stepCount);
        case PathfindingType::MULTI_OBJECTIVE:
            return findPathMultiObjective(grid, start, goal, firePositions);
        default:
            return findPathAStar(grid, start, goal, firePositions);
    }
}