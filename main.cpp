#include "SimulationManager.h"
#include "ConsoleUI.h"
#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

HANDLE g_ConsoleHandle = nullptr;

void showMainMenu()
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

    ConsoleUI::PrintAt(10, 21, "Controls During Simulation:", ConsoleColor::GREEN);
    ConsoleUI::PrintAt(10, 22, "Q = Quit  |  M = Switch Mode  |  +/- = Speed  |  S = Statistics", ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, 23, "H = Heat map | Saved files appear in the output folder", ConsoleColor::WHITE);
    ConsoleUI::PrintAt(10, 25, "Select (1/2/3) or Q to quit: ", ConsoleColor::YELLOW);
}

int main()
{
    SetConsoleTitle(TEXT("🔥 Fire Evacuation Simulator - Advanced AI Pathfinding 🔥"));
    ConsoleUI::Initialize();
    ConsoleUI::HideCursor();

    while (true)
    {
        showMainMenu();
        char choice = _getch();

        if (choice == 'q' || choice == 'Q')
        {
            ConsoleUI::ShowCursor();
            ConsoleUI::ClearScreen();
            cout << "Goodbye!" << endl;
            break;
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

        SimulationManager sim(50, 25);
        sim.run(autoMode, speed);

        ConsoleUI::PrintAt(10, 24, "Press any key to return to menu...", ConsoleColor::YELLOW);
        _getch();
    }

    return 0;
}