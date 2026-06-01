@echo off
REM Quick start script - starts backend and frontend in separate windows
REM Run this after: build.bat or npm install in backend/frontend

echo.
echo ========================================
echo Fire Evacuation Simulator - Quick Start
echo ========================================
echo.

echo [1] Starting backend server on http://localhost:5000...
start "Fire Evacuation - Backend" cmd /k "cd backend && npm start"

timeout /t 2

echo [2] Starting frontend dev server on http://localhost:3000...
start "Fire Evacuation - Frontend" cmd /k "cd frontend && npm run dev"

echo.
echo ========================================
echo Servers starting...
echo ========================================
echo.
echo Backend: http://localhost:5000
echo Frontend dev: http://localhost:3000 (with hot reload)
echo.
echo Both servers should be ready in ~10 seconds.
echo.
pause
