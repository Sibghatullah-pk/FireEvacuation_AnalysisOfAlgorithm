#include "StateOutput.h"
#include "Grid.h"
#include "Person.h"
#include "Fire.h"
#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <set>
#include <vector>

using namespace std;

string StateOutput::escapeJSON(const string &str)
{
    string result;
    for (char c : str)
    {
        switch (c)
        {
        case '"':
            result += "\\\"";
            break;
        case '\\':
            result += "\\\\";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        default:
            if (c < 32)
            {
                result += "\\u" + to_string((int)c);
            }
            else
            {
                result += c;
            }
        }
    }
    return result;
}

string StateOutput::generateStateJSON(
    int step,
    const Grid &grid,
    const std::vector<Person> &people,
    const Fire &fire,
    int rescuedCount,
    int burnedCount,
    const std::string &dominantAlgorithm)
{
    stringstream ss;
    ss << fixed << setprecision(2);

    ss << "{\n";
    ss << "  \"step\": " << step << ",\n";
    ss << "  \"stats\": {\n";
    ss << "    \"total\": " << people.size() << ",\n";
    ss << "    \"rescued\": " << rescuedCount << ",\n";
    ss << "    \"burned\": " << burnedCount << ",\n";
    ss << "    \"alive\": " << (people.size() - rescuedCount - burnedCount) << "\n";
    ss << "  },\n";
    ss << "  \"dominantAlgorithm\": \"" << escapeJSON(dominantAlgorithm) << "\",\n";

    // Grid state (cells)
    ss << "  \"grid\": [\n";
    vector<Position> firePositions = fire.getFirePositions();
    set<Position> fireSet(firePositions.begin(), firePositions.end());
    set<Position> exitSet(grid.getExits().begin(), grid.getExits().end());

    int gridW = grid.getWidth();
    int gridH = grid.getHeight();

    for (int y = 0; y < gridH; y++)
    {
        for (int x = 0; x < gridW; x++)
        {
            Position pos(x, y);
            string cellType = "empty";
            double heat = 0.0;

            if (!grid.isWalkable(pos))
            {
                cellType = "obstacle";
            }
            else if (fireSet.count(pos))
            {
                cellType = "fire";
                heat = 1.0;
            }
            else if (exitSet.count(pos))
            {
                cellType = "exit";
            }

            // Check if person is on this cell
            for (const auto &person : people)
            {
                if (person.getPosition() == pos)
                {
                    cellType = "person";
                    break;
                }
            }

            ss << "    {\"x\": " << x << ", \"y\": " << y << ", \"type\": \"" << cellType << "\", \"heat\": " << heat << "}";
            if (y < gridH - 1 || x < gridW - 1)
                ss << ",";
            ss << "\n";
        }
    }
    ss << "  ],\n";

    // People state
    ss << "  \"people\": [\n";
    for (size_t i = 0; i < people.size(); i++)
    {
        const auto &p = people[i];
        ss << "    {\n";
        ss << "      \"id\": " << i << ",\n";
        ss << "      \"x\": " << p.getPosition().x << ",\n";
        ss << "      \"y\": " << p.getPosition().y << ",\n";
        ss << "      \"alive\": " << (p.isAlive() ? "true" : "false") << ",\n";
        ss << "      \"rescued\": " << (p.isRescued() ? "true" : "false") << "\n";
        ss << "    }";
        if (i < people.size() - 1)
            ss << ",";
        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";

    return ss.str();
}

void StateOutput::emitState(const string &jsonState)
{
    // Prefix with "STATE:" so backend can identify state messages
    cout << "STATE:" << jsonState;
    cout.flush();
}
