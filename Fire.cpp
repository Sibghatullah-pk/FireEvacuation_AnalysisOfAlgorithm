#include "Fire.h"
#include <cstdlib>

using namespace std;

void Fire::addFire(const Position& pos, Grid& grid) {
    firePositions.push_back(pos);
    grid.setCell(pos, 'F');
}

void Fire::spread(Grid& grid) {
    vector<Position> newFires;
    
    for (const Position& firePos : firePositions) {
        for (const Position& neighbor : grid.getNeighbors(firePos)) {
            char cell = grid.getCell(neighbor);
            if (cell == ' ' || cell == 'P') {
                if (rand() % 100 < 60) {
                    newFires.push_back(neighbor);
                }
            }
        }
    }
    
    for (const Position& pos : newFires) {
        firePositions.push_back(pos);
        grid.setCell(pos, 'F');
    }
}

const vector<Position>& Fire::getFirePositions() const {
    return firePositions;
}