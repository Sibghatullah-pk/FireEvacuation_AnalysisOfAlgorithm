#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <iomanip>

// Console color codes
namespace ConsoleColor
{
    const int BLACK = 0;
    const int DARK_BLUE = 1;
    const int DARK_GREEN = 2;
    const int DARK_CYAN = 3;
    const int DARK_RED = 4;
    const int DARK_MAGENTA = 5;
    const int DARK_YELLOW = 6;
    const int LIGHT_GRAY = 7;
    const int DARK_GRAY = 8;
    const int BLUE = 9;
    const int GREEN = 10;
    const int CYAN = 11;
    const int RED = 12;
    const int MAGENTA = 13;
    const int YELLOW = 14;
    const int WHITE = 15;
}

extern HANDLE g_ConsoleHandle;

class ConsoleUI
{
public:
    static void Initialize()
    {
        g_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    static void SetColor(int foreground, int background = 0)
    {
        SetConsoleTextAttribute(g_ConsoleHandle, (background << 4) | foreground);
    }

    static void ResetColor()
    {
        SetColor(ConsoleColor::WHITE, ConsoleColor::BLACK);
    }

    static void ClearScreen()
    {
        CONSOLE_SCREEN_BUFFER_INFO screenInfo;
        GetConsoleScreenBufferInfo(g_ConsoleHandle, &screenInfo);

        DWORD consoleSize = screenInfo.dwSize.X * screenInfo.dwSize.Y;
        COORD coord = {0, 0};
        DWORD count;

        FillConsoleOutputCharacter(g_ConsoleHandle, ' ', consoleSize, coord, &count);
        FillConsoleOutputAttribute(g_ConsoleHandle, screenInfo.wAttributes, consoleSize, coord, &count);
        SetConsoleCursorPosition(g_ConsoleHandle, coord);
    }

    static void HideCursor()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(g_ConsoleHandle, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(g_ConsoleHandle, &cursorInfo);
    }

    static void ShowCursor()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(g_ConsoleHandle, &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(g_ConsoleHandle, &cursorInfo);
    }

    static void PrintAt(int x, int y, const std::string &text, int color = ConsoleColor::WHITE)
    {
        COORD coord = {(SHORT)x, (SHORT)y};
        SetConsoleCursorPosition(g_ConsoleHandle, coord);
        SetColor(color);
        std::cout << text;
        ResetColor();
    }

    static void DrawBox(int x, int y, int width, int height, int color = ConsoleColor::CYAN)
    {
        SetColor(color);
        // Top border
        PrintAt(x, y, "+" + std::string(width - 2, '-') + "+", color);
        // Side borders
        for (int i = 1; i < height - 1; i++)
        {
            PrintAt(x, y + i, "|", color);
            PrintAt(x + width - 1, y + i, "|", color);
        }
        // Bottom border
        PrintAt(x, y + height - 1, "+" + std::string(width - 2, '-') + "+", color);
        ResetColor();
    }

    static void PrintCentered(int y, const std::string &text, int color = ConsoleColor::YELLOW)
    {
        int x = (80 - text.length()) / 2;
        PrintAt(x, y, text, color);
    }
};
