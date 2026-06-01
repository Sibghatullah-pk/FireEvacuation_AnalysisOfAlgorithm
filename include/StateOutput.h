#pragma once
#include "Position.h"
#include "Grid.h"
#include <vector>
#include <string>
#include <map>

// Forward declarations
class Person;
class Fire;

struct GridCell
{
    int x;
    int y;
    std::string type; // "empty", "obstacle", "fire", "person", "exit"
    double heatLevel; // 0.0 to 1.0
};

struct PersonState
{
    int id;
    int x;
    int y;
    bool alive;
    bool rescued;
    std::string currentAlgorithm;
    int pathLength;
};

struct SimulationState
{
    int step;
    int totalPeople;
    int rescuedCount;
    int burnedCount;
    int aliveCount;
    std::vector<GridCell> grid;
    std::vector<PersonState> people;
    std::vector<std::string> statistics;
    double avgFireDanger;
};

class StateOutput
{
public:
    // Generate JSON state snapshot
    static std::string generateStateJSON(
        int step,
        const Grid &grid,
        const std::vector<Person> &people,
        const Fire &fire,
        int rescuedCount,
        int burnedCount,
        const std::string &dominantAlgorithm = "");

    // Emit state to stdout for piping to Node.js backend
    static void emitState(const std::string &jsonState);

    // Helper to sanitize strings for JSON
    static std::string escapeJSON(const std::string &str);
};
