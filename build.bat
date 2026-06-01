@echo off
REM Build script for Fire Evacuation Simulator with full-stack support
REM Requires: g++ (MinGW), Node.js, npm

echo.
echo ========================================
echo Fire Evacuation Simulator - Full Build
echo ========================================
echo.

setlocal enabledelayedexpansion

REM Check if g++ is available
where g++ >nul 2>nul
if errorlevel 1 (
    echo [ERROR] g++ not found. Please install MinGW or add it to PATH.
    exit /b 1
)

REM Check if Node.js is available
where node >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Node.js not found. Please install from https://nodejs.org/
    exit /b 1
)

echo [1/5] Building C++ Simulator...
g++ -std=c++17 -Iinclude src/main.cpp src/SimulationManager.cpp src/Grid.cpp src/Fire.cpp src/Person.cpp src/PathfindingAlgorithms.cpp src/StateOutput.cpp -o fire_sim.exe
if errorlevel 1 (
    echo [ERROR] C++ compilation failed.
    exit /b 1
)
echo [✓] C++ build successful: fire_sim.exe

echo.
echo [2/5] Installing backend dependencies...
cd backend
call npm install
if errorlevel 1 (
    echo [ERROR] Backend npm install failed.
    cd ..
    exit /b 1
)
cd ..
echo [✓] Backend dependencies installed

echo.
echo [3/5] Installing frontend dependencies...
cd frontend
call npm install
if errorlevel 1 (
    echo [ERROR] Frontend npm install failed.
    cd ..
    exit /b 1
)
cd ..
echo [✓] Frontend dependencies installed

echo.
echo [4/5] Building frontend (React + Vite)...
cd frontend
call npm run build
if errorlevel 1 (
    echo [ERROR] Frontend build failed.
    cd ..
    exit /b 1
)
cd ..
echo [✓] Frontend build successful

echo.
echo ========================================
echo [✓] Build Complete!
echo ========================================
echo.
echo Next steps:
echo.
echo 1. Start the backend server:
echo    cd backend && npm start
echo.
echo 2. Open your browser to:
echo    http://localhost:5000
echo.
echo For development with hot reload:
echo    cd frontend && npm run dev
echo.
pause
