#pragma once
#include "Position.h"
#include "Grid.h"
#include <vector>

class Fire {
private:
    std::vector<Position> firePositions;
    
public:
    Fire() {}
    
    void addFire(const Position& pos, Grid& grid);
    void spread(Grid& grid);
    const std::vector<Position>& getFirePositions() const;
};