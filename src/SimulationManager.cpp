#include "SimulationManager.h"
#include "ConsoleUI.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

SimulationManager::SimulationManager(const SimulationConfig &simulationConfig)
    : config(simulationConfig), grid(config.width, config.height, config.obstacleDensity), stepCount(0), autoMode(true), rescued(0), burned(0), simulationSpeed(800), showStatistics(false), showHeatMap(false)
{
    srand(time(nullptr));
    initializeSimulation();
}

string SimulationManager::buildTimestamp() const
{
    time_t now = time(nullptr);
    tm localTime{};
    localtime_s(&localTime, &now);

    ostringstream stream;
    stream << put_time(&localTime, "%Y%m%d_%H%M%S");
    return stream.str();
}

string SimulationManager::buildStatusBreakdown() const
{
    int algorithmCount[5] = {0};
    for (const Person &person : people)
    {
        if (person.isAlive() && !person.isRescued())
        {
            algorithmCount[(int)person.getCurrentAlgorithm()]++;
        }
    }

    ostringstream stream;
    stream << "Algorithm breakdown:\n";
    stream << "  BFS: " << algorithmCount[(int)PathfindingType::BFS] << '\n';
    stream << "  Dijkstra: " << algorithmCount[(int)PathfindingType::DIJKSTRA] << '\n';
    stream << "  A*: " << algorithmCount[(int)PathfindingType::A_STAR] << '\n';
    stream << "  Hybrid adaptive: " << algorithmCount[(int)PathfindingType::HYBRID_ADAPTIVE] << '\n';
    stream << "  Multi-objective: " << algorithmCount[(int)PathfindingType::MULTI_OBJECTIVE] << '\n';
    return stream.str();
}

string SimulationManager::buildRunSummary() const
{
    int totalPeople = (int)people.size();
    double successRate = totalPeople > 0 ? (rescued * 100.0 / totalPeople) : 0.0;

    ostringstream stream;
    stream << "Fire Evacuation Simulation Report\n";
    stream << "=================================\n\n";
    stream << "Steps: " << stepCount << '\n';
    stream << "Total people: " << totalPeople << '\n';
    stream << "Rescued: " << rescued << '\n';
    stream << "Burned: " << burned << '\n';
    stream << "Success rate: " << fixed << setprecision(1) << successRate << "%\n";
    stream << "Current mode at finish: " << (autoMode ? "AUTO" : "MANUAL") << '\n';
    stream << "Simulation speed: " << simulationSpeed << " ms per step\n\n";
    stream << "Map size: " << config.width << "x" << config.height << '\n';
    stream << "Obstacle density: " << fixed << setprecision(2) << config.obstacleDensity << '\n';
    stream << "Fire spread chance: " << config.fireSpreadChance << "%\n\n";
    stream << buildStatusBreakdown() << '\n';
    stream << "People status:\n";

    for (const Person &person : people)
    {
        stream << "  Person " << person.getId() << ": ";
        if (!person.isAlive())
        {
            stream << "BURNED\n";
        }
        else if (person.isRescued())
        {
            stream << "RESCUED\n";
        }
        else
        {
            stream << person.getAlgorithmName() << " at (" << person.getPosition().x << ", " << person.getPosition().y << ")\n";
        }
    }

    return stream.str();
}

string SimulationManager::buildGridSnapshot() const
{
    ostringstream stream;
    stream << "Final Grid Snapshot\n";
    stream << "===================\n\n";

    for (int y = 0; y < grid.getHeight(); y++)
    {
        for (int x = 0; x < grid.getWidth(); x++)
        {
            stream << grid.getCell(Position(x, y));
        }
        stream << '\n';
    }

    return stream.str();
}

void SimulationManager::exportRunArtifacts() const
{
    if (!config.saveOutput)
    {
        cout << "\n  Output saving is disabled for this run." << endl;
        return;
    }

    fs::path outputDir("output");
    fs::create_directories(outputDir);

    string stamp = buildTimestamp();
    fs::path reportPath = outputDir / ("simulation_report_" + stamp + ".txt");
    fs::path gridPath = outputDir / ("final_grid_" + stamp + ".txt");

    ofstream reportFile(reportPath);
    if (reportFile.is_open())
    {
        reportFile << buildRunSummary();
        reportFile.close();
    }

    ofstream gridFile(gridPath);
    if (gridFile.is_open())
    {
        gridFile << buildGridSnapshot();
        gridFile.close();
    }

    cout << "\n  +---------------------------------------------------------------------+" << endl;
    ConsoleUI::SetColor(ConsoleColor::GREEN);
    cout << "  |  Output saved to:                                                    |" << endl;
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "  |  Report: " << left << setw(60) << reportPath.string() << "|" << endl;
    cout << "  |  Grid snapshot: " << left << setw(54) << gridPath.string() << "|" << endl;
    cout << "  +---------------------------------------------------------------------+" << endl;
    ConsoleUI::ResetColor();
}

void SimulationManager::initializeSimulation()
{
    int numPeople = config.peopleCount;
    for (int i = 0; i < numPeople; i++)
    {
        Position pos;
        int attempts = 0;
        do
        {
            pos.x = rand() % (grid.getWidth() - 4) + 2;
            pos.y = rand() % (grid.getHeight() - 4) + 2;
            attempts++;
        } while (!grid.isWalkable(pos) && attempts < 50);

        if (attempts < 50)
        {
            people.emplace_back(pos, i);
            grid.setCell(pos, 'P');
        }
    }

    int numFires = config.fireCount;
    for (int i = 0; i < numFires; i++)
    {
        Position pos;
        int attempts = 0;
        do
        {
            pos.x = rand() % (grid.getWidth() - 4) + 2;
            pos.y = rand() % (grid.getHeight() - 4) + 2;
            attempts++;
        } while (grid.getCell(pos) != ' ' && attempts < 50);

        if (attempts < 50)
        {
            fire.addFire(pos, grid);
        }
    }
}

void SimulationManager::step()
{
    stepCount++;

    for (Person &person : people)
    {
        person.update(grid, fire.getFirePositions(), stepCount);
    }

    if (stepCount % 2 == 0)
    {
        fire.spread(grid, config.fireSpreadChance);
    }

    updateStats();
}

void SimulationManager::updateStats()
{
    rescued = 0;
    burned = 0;

    for (const Person &person : people)
    {
        if (person.isRescued())
            rescued++;
        else if (!person.isAlive())
            burned++;
    }
}

void SimulationManager::displayLegend()
{
    ConsoleUI::SetColor(ConsoleColor::CYAN);
    cout << "  LEGEND: ";
    ConsoleUI::SetColor(ConsoleColor::CYAN);
    cout << "P";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "=Person  ";
    ConsoleUI::SetColor(ConsoleColor::RED);
    cout << "F";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "=Fire  ";
    ConsoleUI::SetColor(ConsoleColor::GREEN);
    cout << "E";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "=Exit  ";
    ConsoleUI::SetColor(ConsoleColor::DARK_GRAY);
    cout << "#";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "=Wall  ";
    ConsoleUI::SetColor(ConsoleColor::YELLOW);
    cout << "*";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "=Danger Zone" << endl;
}

void SimulationManager::displayStats()
{
    // Status bar at bottom
    int totalPeople = people.size();
    int evacuating = totalPeople - rescued - burned;

    cout << "  +---------------------------------------------------------------------+" << endl;
    cout << "  | STEP: ";
    ConsoleUI::SetColor(ConsoleColor::YELLOW);
    cout << setw(4) << stepCount;
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "  |  ";

    ConsoleUI::SetColor(ConsoleColor::CYAN);
    cout << "[ ] Evacuating: " << setw(2) << evacuating;
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "  |  ";

    ConsoleUI::SetColor(ConsoleColor::GREEN);
    cout << "[+] Rescued: " << setw(2) << rescued;
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "  |  ";

    ConsoleUI::SetColor(ConsoleColor::RED);
    cout << "[X] Burned: " << setw(2) << burned;
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "  |" << endl;
    cout << "  +---------------------------------------------------------------------+" << endl;
}

void SimulationManager::displayDetailedStats()
{
    cout << "\n";
    ConsoleUI::DrawBox(2, 0, 76, 22, ConsoleColor::CYAN);
    ConsoleUI::PrintAt(5, 1, "=== DETAILED ALGORITHM STATISTICS ===", ConsoleColor::YELLOW);

    int y = 3;
    cout << "\n";

    int algorithmCount[5] = {0};
    for (const Person &person : people)
    {
        if (person.isAlive() && !person.isRescued())
        {
            algorithmCount[(int)person.getCurrentAlgorithm()]++;
        }
    }

    ConsoleUI::SetColor(ConsoleColor::GREEN);
    cout << "  Active People by Algorithm:" << endl;
    cout << "  +- A*: " << algorithmCount[2] << " people" << endl;
    cout << "  +- Dijkstra: " << algorithmCount[1] << " people" << endl;
    cout << "  +- BFS: " << algorithmCount[0] << " people" << endl;
    cout << "  +- Hybrid: " << algorithmCount[3] << " people" << endl;

    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << "\n  Detailed Status:" << endl;
    y = 11;
    for (const Person &person : people)
    {
        if (person.isAlive())
        {
            cout << "  Person " << person.getId() << " [";
            if (person.isRescued())
            {
                ConsoleUI::SetColor(ConsoleColor::GREEN);
                cout << "RESCUED";
            }
            else
            {
                ConsoleUI::SetColor(ConsoleColor::CYAN);
                cout << person.getAlgorithmName();
            }
            ConsoleUI::SetColor(ConsoleColor::WHITE);
            cout << "]";

            if (!person.isRescued())
            {
                cout << " - Position (" << person.getPosition().x << "," << person.getPosition().y << ")";
            }
            cout << endl;
            y++;
        }
        else
        {
            cout << "  Person " << person.getId() << " [";
            ConsoleUI::SetColor(ConsoleColor::RED);
            cout << "BURNED";
            ConsoleUI::SetColor(ConsoleColor::WHITE);
            cout << "]" << endl;
            y++;
        }
    }
    ConsoleUI::ResetColor();
}

void SimulationManager::displayHeatMap() const
{
    cout << "\n  HEAT MAP VISUALIZATION:" << endl;
    cout << "  ";
    for (int i = 0; i < grid.getWidth(); i++)
        cout << "─";
    cout << endl;

    for (int y = 0; y < grid.getHeight(); y++)
    {
        cout << "  |";
        for (int x = 0; x < grid.getWidth(); x++)
        {
            int dist = 999;
            for (const auto &fire : fire.getFirePositions())
            {
                int d = abs(fire.x - x) + abs(fire.y - y);
                dist = min(dist, d);
            }

            if (dist == 0)
            {
                ConsoleUI::SetColor(ConsoleColor::RED);
                cout << 'X';
            }
            else if (dist <= 2)
            {
                ConsoleUI::SetColor(ConsoleColor::DARK_RED);
                cout << '#';
            }
            else if (dist <= 4)
            {
                ConsoleUI::SetColor(ConsoleColor::YELLOW);
                cout << '@';
            }
            else if (dist <= 6)
            {
                ConsoleUI::SetColor(ConsoleColor::DARK_YELLOW);
                cout << '.';
            }
            else
            {
                ConsoleUI::SetColor(ConsoleColor::WHITE);
                cout << ' ';
            }
        }
        ConsoleUI::ResetColor();
        cout << "|" << endl;
    }
    cout << "  ";
    for (int i = 0; i < grid.getWidth(); i++)
        cout << "-";
    cout << endl;
}

bool SimulationManager::isSimulationComplete()
{
    return rescued + burned >= people.size() || stepCount > 150;
}

void SimulationManager::handleSpeedChange(char key, int &speed)
{
    if (key == '+' || key == '=')
    {
        speed = max(200, speed - 200);
    }
    else if (key == '-' || key == '_')
    {
        speed = min(1500, speed + 200);
    }
}

void SimulationManager::run(bool initialAutoMode, int speed)
{
    ConsoleUI::HideCursor();
    autoMode = initialAutoMode;
    simulationSpeed = speed;

    grid.clearScreen();

    while (!isSimulationComplete())
    {
        ConsoleUI::ClearScreen();

        // Display grid with colors
        grid.displayWithColors(fire.getFirePositions());

        // Display statistics
        cout << endl;
        displayStats();
        cout << endl;
        displayLegend();

        // Control instructions
        cout << "\n  CONTROLS: ";
        ConsoleUI::SetColor(ConsoleColor::YELLOW);
        cout << "[Q]";
        ConsoleUI::SetColor(ConsoleColor::WHITE);
        cout << "uit  ";
        ConsoleUI::SetColor(ConsoleColor::YELLOW);
        cout << "[M]";
        ConsoleUI::SetColor(ConsoleColor::WHITE);
        cout << "ode  ";
        ConsoleUI::SetColor(ConsoleColor::YELLOW);
        cout << "[S]";
        ConsoleUI::SetColor(ConsoleColor::WHITE);
        cout << "tatistics  ";
        ConsoleUI::SetColor(ConsoleColor::YELLOW);
        cout << "[+/-]";
        ConsoleUI::SetColor(ConsoleColor::WHITE);
        cout << "Speed  ";
        ConsoleUI::SetColor(ConsoleColor::YELLOW);
        cout << "[H]";
        ConsoleUI::SetColor(ConsoleColor::WHITE);
        cout << "eatmap" << endl;

        if (autoMode)
        {
            cout << "  [AUTO MODE] - Q quit, M manual, S stats, H heatmap\n";
            Sleep(simulationSpeed);

            if (_kbhit())
            {
                char key = _getch();
                if (key == 'q' || key == 'Q')
                    break;
                if (key == 'm' || key == 'M')
                {
                    autoMode = false;
                    cout << "\n  Switched to MANUAL MODE - Press SPACE to step" << endl;
                    Sleep(1500);
                }
                if (key == 's' || key == 'S')
                {
                    displayDetailedStats();
                    cout << "\n  Press any key to continue..." << endl;
                    _getch();
                }
                if (key == 'h' || key == 'H')
                {
                    displayHeatMap();
                    cout << "\n  Press any key to continue..." << endl;
                    _getch();
                }
                handleSpeedChange(key, simulationSpeed);
            }
        }
        else
        {
            cout << "  [MANUAL MODE] - Press SPACE to step, [M] for auto\n";
            char input = _getch();

            if (input == 'q' || input == 'Q')
                break;
            if (input == 'm' || input == 'M')
            {
                autoMode = true;
                cout << "\n  Switched to AUTO MODE\n";
                Sleep(1000);
                continue;
            }
            if (input == 's' || input == 'S')
            {
                displayDetailedStats();
                cout << "\n  Press any key to continue..." << endl;
                _getch();
                continue;
            }
            if (input == 'h' || input == 'H')
            {
                displayHeatMap();
                cout << "\n  Press any key to continue..." << endl;
                _getch();
                continue;
            }
            handleSpeedChange(input, simulationSpeed);
            if (input != ' ')
                continue;
        }

        step();
    }

    // Final display
    ConsoleUI::ClearScreen();
    grid.displayWithColors(fire.getFirePositions());
    cout << endl;
    displayStats();
    cout << endl;
    displayLegend();

    // Final results
    cout << "\n  +======================================================================+" << endl;
    cout << "  |        *** SIMULATION COMPLETE ***                               |" << endl;
    cout << "  +======================================================================+" << endl;

    cout << "  | Total Steps: ";
    ConsoleUI::SetColor(ConsoleColor::YELLOW);
    cout << setw(50) << left << to_string(stepCount) + " |";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << endl;

    cout << "  | Rescued: ";
    ConsoleUI::SetColor(ConsoleColor::GREEN);
    cout << setw(54) << left << to_string(rescued) + " |";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << endl;

    cout << "  | Lost: ";
    ConsoleUI::SetColor(ConsoleColor::RED);
    cout << setw(57) << left << to_string(burned) + " |";
    ConsoleUI::SetColor(ConsoleColor::WHITE);
    cout << endl;

    if (people.size() > 0)
    {
        double successRate = (rescued * 100.0 / people.size());
        cout << "  | Success Rate: ";
        if (successRate >= 80)
        {
            ConsoleUI::SetColor(ConsoleColor::GREEN);
        }
        else if (successRate >= 50)
        {
            ConsoleUI::SetColor(ConsoleColor::YELLOW);
        }
        else
        {
            ConsoleUI::SetColor(ConsoleColor::RED);
        }
        cout << fixed << setprecision(1) << setw(46) << left << (to_string(successRate) + "%") + " |";
        ConsoleUI::SetColor(ConsoleColor::WHITE);
        cout << endl;
    }

    cout << "  +======================================================================+" << endl;

    exportRunArtifacts();

    ConsoleUI::ShowCursor();
    cout << "\n  Press any key to return to menu..." << endl;
    _getch();
}