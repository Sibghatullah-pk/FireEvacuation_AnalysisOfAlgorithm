#include "Grid.h"
#include "ConsoleUI.h"
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cmath>

using namespace std;

Grid::Grid(int w, int h, double density) : width(w), height(h), obstacleDensity(max(0.0, min(density, 0.35)))
{
    cells.resize(height, vector<char>(width, ' '));
    initializeGrid();
}

void Grid::initializeGrid()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cells[i][j] = ' ';
        }
    }

    exits.clear();

    // Create walls around the perimeter
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
            {
                cells[i][j] = '#';
            }
        }
    }

    vector<Position> plannedExits;
    plannedExits.push_back(Position(1, 0));
    plannedExits.push_back(Position(width - 2, 0));
    plannedExits.push_back(Position(width - 1, height / 2));

    auto isNearExit = [&plannedExits](const Position &pos)
    {
        for (const Position &exit : plannedExits)
        {
            if (abs(exit.x - pos.x) + abs(exit.y - pos.y) <= 1)
            {
                return true;
            }
        }
        return false;
    };

    int innerCells = max(0, (width - 2) * (height - 2));
    int obstacleCount = static_cast<int>(innerCells * obstacleDensity);
    int placed = 0;
    int attempts = 0;
    int maxAttempts = max(100, obstacleCount * 10);

    while (placed < obstacleCount && attempts < maxAttempts)
    {
        Position obstacle(rand() % max(1, width - 2) + 1, rand() % max(1, height - 2) + 1);
        attempts++;

        if (cells[obstacle.y][obstacle.x] != ' ' || isNearExit(obstacle))
        {
            continue;
        }

        cells[obstacle.y][obstacle.x] = '#';
        placed++;
    }

    for (const auto &exit : plannedExits)
    {
        if (isValid(exit))
        {
            exits.push_back(exit);
            cells[exit.y][exit.x] = 'E';
        }
    }
}

bool Grid::isValid(const Position &pos) const
{
    return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
}

char Grid::getCell(const Position &pos) const
{
    if (!isValid(pos))
        return '#';
    return cells[pos.y][pos.x];
}

void Grid::setCell(const Position &pos, char value)
{
    if (isValid(pos))
    {
        cells[pos.y][pos.x] = value;
    }
}

bool Grid::isWalkable(const Position &pos) const
{
    if (!isValid(pos))
        return false;
    char cell = getCell(pos);
    return cell == ' ' || cell == 'E';
}

vector<Position> Grid::getNeighbors(const Position &pos) const
{
    vector<Position> neighbors;
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    for (int i = 0; i < 4; i++)
    {
        Position neighbor(pos.x + dx[i], pos.y + dy[i]);
        if (isValid(neighbor))
        {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

const vector<Position> &Grid::getExits() const
{
    return exits;
}

void Grid::clearScreen()
{
    system("cls");
}

void Grid::display() const
{
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cout << cells[i][j];
        }
        cout << endl;
    }
}

void Grid::displayWithColors(const std::vector<Position> &firePositions) const
{
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // Calculate fire danger map
    vector<vector<int>> dangerMap(height, vector<int>(width, 0));
    for (const auto &fire : firePositions)
    {
        dangerMap[fire.y][fire.x] = 5;
    }

    // Spread danger zones based on fire proximity
    for (int dist = 1; dist <= 3; dist++)
    {
        for (const auto &fire : firePositions)
        {
            for (int dy = -dist; dy <= dist; dy++)
            {
                for (int dx = -dist; dx <= dist; dx++)
                {
                    int y = fire.y + dy;
                    int x = fire.x + dx;
                    if (isValid(Position(x, y)))
                    {
                        int d = abs(dx) + abs(dy);
                        dangerMap[y][x] = max(dangerMap[y][x], max(0, 5 - d));
                    }
                }
            }
        }
    }

    // Display with colors
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            char cell = cells[i][j];
            int danger = dangerMap[i][j];

            if (cell == 'P')
            {
                ConsoleUI::SetColor(ConsoleColor::CYAN, ConsoleColor::BLACK);
                cout << 'P';
            }
            else if (cell == 'F')
            {
                if (danger >= 5)
                {
                    ConsoleUI::SetColor(ConsoleColor::RED, ConsoleColor::BLACK);
                }
                else if (danger >= 3)
                {
                    ConsoleUI::SetColor(ConsoleColor::YELLOW, ConsoleColor::RED);
                }
                else
                {
                    ConsoleUI::SetColor(ConsoleColor::YELLOW, ConsoleColor::BLACK);
                }
                cout << 'F';
            }
            else if (cell == 'E')
            {
                ConsoleUI::SetColor(ConsoleColor::GREEN, ConsoleColor::BLACK);
                cout << 'E';
            }
            else if (cell == '#')
            {
                ConsoleUI::SetColor(ConsoleColor::DARK_GRAY, ConsoleColor::BLACK);
                cout << '#';
            }
            else if (danger > 0)
            {
                // Show danger zone with intensity
                if (danger >= 4)
                {
                    ConsoleUI::SetColor(ConsoleColor::RED, ConsoleColor::BLACK);
                    cout << '#';
                }
                else if (danger >= 2)
                {
                    ConsoleUI::SetColor(ConsoleColor::YELLOW, ConsoleColor::BLACK);
                    cout << '*';
                }
                else
                {
                    ConsoleUI::SetColor(ConsoleColor::DARK_YELLOW, ConsoleColor::BLACK);
                    cout << '.';
                }
            }
            else
            {
                ConsoleUI::SetColor(ConsoleColor::WHITE, ConsoleColor::BLACK);
                cout << ' ';
            }
        }
        ConsoleUI::ResetColor();
        cout << endl;
    }
}