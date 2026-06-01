@echo off
REM Production start script - starts only the backend server
REM Assumes frontend has been built with: npm run build

echo.
echo ========================================
echo Fire Evacuation Simulator - Production
echo ========================================
echo.

REM Check if fire_sim.exe exists
if not exist "fire_sim.exe" (
    echo [ERROR] fire_sim.exe not found.
    echo Please run build.bat first.
    pause
    exit /b 1
)

REM Check if frontend build exists
if not exist "frontend\build\index.html" (
    echo [ERROR] Frontend build not found.
    echo Please run: cd frontend && npm run build
    pause
    exit /b 1
)

echo [1] Starting backend server...
echo.

cd backend
npm start
