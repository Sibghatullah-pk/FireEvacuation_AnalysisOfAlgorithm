#include "SimulationManager.h"
#include "ConsoleUI.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;

HANDLE g_ConsoleHandle = nullptr;

void applyDifficultyPreset(SimulationConfig &config, int preset)
{
    if (preset == 1)
    {
        config.width = 40;
        config.height = 20;
        config.peopleCount = 6;
        config.fireCount = 1;
        config.obstacleDensity = 0.08;
        config.fireSpreadChance = 35;
    }
    else if (preset == 2)
    {
        config.width = 50;
        config.height = 25;
        config.peopleCount = 8;
        config.fireCount = 2;
        config.obstacleDensity = 0.12;
        config.fireSpreadChance = 55;
    }
    else
    {
        config.width = 65;
        config.height = 30;
        config.peopleCount = 10;
        config.fireCount = 3;
        config.obstacleDensity = 0.18;
        config.fireSpreadChance = 75;
    }
}

string getDifficultyLabel(int preset)
{
    if (preset == 1)
    {
        return "Easy";
    }
    if (preset == 2)
    {
        return "Normal";
    }
    return "Hard";
}

string getDifficultyDescription(int preset)
{
    if (preset == 1)
    {
        return "Small map, low crowd, slow fire";
    }
    if (preset == 2)
    {
        return "Balanced size, standard crowd, medium fire";
    }
    return "Large map, denser obstacles, fast fire";
}

string makeBar(int value, int maxValue, int width)
{
    if (maxValue <= 0)
    {
        maxValue = 1;
    }

    int filled = (value * width) / maxValue;
    if (filled < 0)
    {
        filled = 0;
    }
    if (filled > width)
    {
        filled = width;
    }

    return string("[") + string(filled, '#') + string(width - filled, '-') + "]";
}

void drawPresetBlock(int y, int preset, int selectedPreset, const string &label, const string &description)
{
    ConsoleUI::PrintAt(7, y, string(preset == selectedPreset ? ">" : " ") + to_string(preset) + ". " + label, preset == selectedPreset ? ConsoleColor::YELLOW : ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, y + 1, description, ConsoleColor::CYAN);
}

void showSettingsMenu(SimulationConfig &config, int &difficultyPreset)
{
    while (true)
    {
        ConsoleUI::ClearScreen();
        ConsoleUI::DrawBox(1, 1, 78, 24, ConsoleColor::CYAN);
        ConsoleUI::PrintCentered(2, "SETTINGS MENU", ConsoleColor::YELLOW);
        ConsoleUI::PrintCentered(3, "Difficulty, building size, fire spread, and output preferences", ConsoleColor::WHITE);

        ConsoleUI::PrintAt(5, 5, "Difficulty Presets", ConsoleColor::GREEN);
        drawPresetBlock(7, 1, difficultyPreset, "Easy", getDifficultyDescription(1));
        drawPresetBlock(9, 2, difficultyPreset, "Normal", getDifficultyDescription(2));
        drawPresetBlock(11, 3, difficultyPreset, "Hard", getDifficultyDescription(3));

        ConsoleUI::PrintAt(43, 5, "Current Configuration", ConsoleColor::GREEN);
        ConsoleUI::PrintAt(43, 7, string("Preset: ") + getDifficultyLabel(difficultyPreset), ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 8, string("Size:   ") + to_string(config.width) + "x" + to_string(config.height), ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 9, string("Crowd:  ") + makeBar(config.peopleCount, 10, 12) + " " + to_string(config.peopleCount), ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 10, string("Fires:  ") + makeBar(config.fireCount, 3, 12) + " " + to_string(config.fireCount), ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 11, string("Obst.:  ") + makeBar((int)(config.obstacleDensity * 100), 20, 12) + " " + to_string((int)(config.obstacleDensity * 100)) + "%", ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 12, string("Fire:   ") + makeBar(config.fireSpreadChance, 100, 12) + " " + to_string(config.fireSpreadChance) + "%", ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 13, string("Save:   ") + (config.saveOutput ? "ON" : "OFF"), ConsoleColor::CYAN);
        ConsoleUI::PrintAt(43, 15, string("Best for: ") + getDifficultyDescription(difficultyPreset), ConsoleColor::GREEN);

        ConsoleUI::PrintAt(5, 12, "Actions", ConsoleColor::GREEN);
        ConsoleUI::PrintAt(5, 14, "4. Toggle save output default", ConsoleColor::WHITE);
        ConsoleUI::PrintAt(5, 15, "5. Return to main menu", ConsoleColor::WHITE);
        ConsoleUI::PrintAt(5, 17, "Keys: 1-3 presets | 4 toggle save | 5/Q exit", ConsoleColor::YELLOW);
        ConsoleUI::PrintAt(5, 19, "Selected preset applies immediately to the next run", ConsoleColor::WHITE);

        char choice = _getch();
        if (choice == '1' || choice == '2' || choice == '3')
        {
            difficultyPreset = choice - '0';
            applyDifficultyPreset(config, difficultyPreset);
        }
        else if (choice == '4')
        {
            config.saveOutput = !config.saveOutput;
        }
        else if (choice == '5' || choice == 'q' || choice == 'Q')
        {
            break;
        }
    }
}

bool askSaveOutput(bool defaultValue)
{
    ConsoleUI::ClearScreen();
    ConsoleUI::PrintCentered(10, string("Save output for this run?  [Y/N]  Default: ") + (defaultValue ? "Y" : "N"), ConsoleColor::YELLOW);

    while (true)
    {
        char choice = _getch();
        if (choice == 'y' || choice == 'Y')
        {
            return true;
        }
        if (choice == 'n' || choice == 'N')
        {
            return false;
        }
        if (choice == '\r')
        {
            return defaultValue;
        }
    }
}

void showMainMenu(const SimulationConfig &config, int difficultyPreset)
{
    ConsoleUI::ClearScreen();
    ConsoleUI::PrintCentered(2, "+=================================================+", ConsoleColor::CYAN);
    ConsoleUI::PrintCentered(3, "|   FIRE EVACUATION SIMULATOR                  |", ConsoleColor::YELLOW);
    ConsoleUI::PrintCentered(4, "|   Advanced AI Pathfinding Visualization      |", ConsoleColor::CYAN);
    ConsoleUI::PrintCentered(5, "+=================================================+", ConsoleColor::CYAN);

    ConsoleUI::PrintAt(10, 8, "Select Simulation Mode:", ConsoleColor::GREEN);
    ConsoleUI::PrintAt(10, 10, "1. AUTO MODE    - Continuous simulation (800ms/step)", ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, 11, "2. MANUAL MODE  - Step-by-step control", ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, 12, "3. FAST MODE    - Quick simulation (400ms/step)", ConsoleColor::WHITE);

    ConsoleUI::PrintAt(10, 15, "Features:", ConsoleColor::GREEN);
    ConsoleUI::PrintAt(10, 16, "* A* Algorithm - Optimal pathfinding", ConsoleColor::CYAN);
    ConsoleUI::PrintAt(10, 17, "* Dijkstra's - Safety-prioritized routes", ConsoleColor::CYAN);
    ConsoleUI::PrintAt(10, 18, "* BFS - Shortest path guarantee", ConsoleColor::CYAN);
    ConsoleUI::PrintAt(10, 19, "* Hybrid Adaptive - Dynamic algorithm switching", ConsoleColor::CYAN);
    ConsoleUI::PrintAt(10, 20, "* Output reports - Final summary and grid snapshot", ConsoleColor::CYAN);
    ConsoleUI::PrintAt(10, 21, string("* Difficulty: ") + getDifficultyLabel(difficultyPreset) + " | Size: " + to_string(config.width) + "x" + to_string(config.height), ConsoleColor::CYAN);

    ConsoleUI::PrintAt(10, 23, "Controls During Simulation:", ConsoleColor::GREEN);
    ConsoleUI::PrintAt(10, 24, "Q = Quit  |  M = Switch Mode  |  +/- = Speed  |  S = Statistics", ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, 25, "H = Heat map | Saved files appear in the output folder", ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, 27, "Select (1/2/3) for mode, 4 for settings, or Q to quit: ", ConsoleColor::YELLOW);
}

int main()
{
    SetConsoleTitle(TEXT("🔥 Fire Evacuation Simulator - Advanced AI Pathfinding 🔥"));
    ConsoleUI::Initialize();
    ConsoleUI::HideCursor();

    SimulationConfig config;
    int difficultyPreset = 2;
    applyDifficultyPreset(config, difficultyPreset);

    while (true)
    {
        showMainMenu(config, difficultyPreset);
        char choice = _getch();

        if (choice == 'q' || choice == 'Q')
        {
            ConsoleUI::ShowCursor();
            ConsoleUI::ClearScreen();
            cout << "Goodbye!" << endl;
            break;
        }

        if (choice == '4')
        {
            showSettingsMenu(config, difficultyPreset);
            continue;
        }

        int speed = 800;
        bool autoMode = true;

        if (choice == '1')
        {
            speed = 800;
            autoMode = true;
        }
        else if (choice == '2')
        {
            speed = -1; // Manual mode
            autoMode = false;
        }
        else if (choice == '3')
        {
            speed = 400;
            autoMode = true;
        }
        else
        {
            continue;
        }

        bool saveOutputForRun = askSaveOutput(config.saveOutput);
        SimulationConfig runConfig = config;
        runConfig.saveOutput = saveOutputForRun;

        SimulationManager sim(runConfig);
        sim.run(autoMode, speed);

        ConsoleUI::PrintAt(10, 24, "Press any key to return to menu...", ConsoleColor::YELLOW);
        _getch();
    }

    return 0;
}