#pragma once
#include "Position.h"
#include "Grid.h"
#include "PathfindingAlgorithms.h"
#include <vector>
#include <string>

class Person
{
private:
    Position position;
    Position target;
    std::vector<Position> path;
    bool alive;
    bool rescued;
    int id;
    PathfindingType currentAlgorithm;
    int pathRecalculationCounter;
    double lastFireDanger;

public:
    Person(const Position &pos, int personId);

    std::vector<Position> findPathBFS(const Grid &grid, const Position &start, const Position &goal);
    Position findNearestExit(const Grid &grid);
    void calculatePath(const Grid &grid, const std::vector<Position> &firePositions, int stepCount);
    bool move(Grid &grid);
    void update(Grid &grid, const std::vector<Position> &firePositions, int stepCount);
    void adaptAlgorithm(const Grid &grid, const std::vector<Position> &firePositions, int stepCount);

    bool isAlive() const { return alive; }
    bool isRescued() const { return rescued; }
    const Position &getPosition() const { return position; }
    int getId() const { return id; }
    PathfindingType getCurrentAlgorithm() const { return currentAlgorithm; }
    std::string getAlgorithmName() const;
};