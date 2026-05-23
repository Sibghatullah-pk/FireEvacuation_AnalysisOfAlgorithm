#pragma once
#include "Position.h"
#include <vector>

class Grid
{
private:
    int width, height;
    double obstacleDensity;
    std::vector<std::vector<char>> cells;
    std::vector<Position> exits;

public:
    Grid(int w, int h, double density = 0.12);

    void initializeGrid();
    bool isValid(const Position &pos) const;
    char getCell(const Position &pos) const;
    void setCell(const Position &pos, char value);
    bool isWalkable(const Position &pos) const;
    std::vector<Position> getNeighbors(const Position &pos) const;
    const std::vector<Position> &getExits() const;
    void clearScreen();
    void display() const;
    void displayWithColors(const std::vector<Position> &firePositions) const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
};