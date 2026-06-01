# 🚀 Quick Reference Card

## One-Command Build & Run

### Windows Command Prompt
```batch
build.bat && start.bat
```

### Windows PowerShell
```powershell
.\build.ps1; .\start.bat
```

---

## Development Workflow

### Terminal 1: Backend (API + WebSocket)
```bash
cd backend
npm start
```

### Terminal 2: Frontend (React Dev Server)
```bash
cd frontend
npm run dev
```

Then open: **http://localhost:3000** (frontend with hot reload)

---

## Production Deployment

### Step 1: Build C++
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o fire_sim.exe
```

### Step 2: Build Frontend
```bash
cd frontend && npm run build
```

### Step 3: Start Production Server
```bash
cd backend && npm start
```

Then open: **http://localhost:5000**

---

## Common Commands

| Task | Command |
|------|---------|
| **Install all deps** | `npm install` in `backend/` and `frontend/` |
| **Build C++** | `g++ -std=c++17 -Iinclude src/*.cpp -o fire_sim.exe` |
| **Build frontend** | `cd frontend && npm run build` |
| **Dev mode** | `start-dev.bat` (opens 2 windows) |
| **Production** | `start.bat` (backend only) |
| **Backend dev** | `cd backend && npm run dev` (with auto-restart) |
| **Frontend dev** | `cd frontend && npm run dev` (with hot reload) |
| **Clean build** | Delete `fire_sim.exe`, `frontend/build/`, `node_modules/` |

---

## URLs

| Service | URL | Notes |
|---------|-----|-------|
| **Production** | http://localhost:5000 | Full app (backend + frontend) |
| **Frontend Dev** | http://localhost:3000 | React with hot reload |
| **Backend API** | http://localhost:5000/api | REST endpoints |
| **WebSocket** | ws://localhost:5000 | Real-time state |

---

## Troubleshooting

### "g++ not found"
→ Install MinGW: https://www.mingw-w64.org/

### "npm: command not found"
→ Install Node.js: https://nodejs.org/

### "Cannot spawn fire_sim.exe"
→ Run: `g++ -std=c++17 -Iinclude src/*.cpp -o fire_sim.exe`

### "WebSocket disconnected"
→ Ensure backend is running: `cd backend && npm start`

### "Frontend blank page"
→ Check browser console (F12) for errors
→ Ensure backend is serving on http://localhost:5000

---

## File Locations

- **C++ Executable**: `./fire_sim.exe`
- **React App**: `./frontend/src/`
- **Backend Server**: `./backend/server.js`
- **Build Output**: `./frontend/build/`
- **Simulation Reports**: `./output/`

---

## Architecture at a Glance

```
Browser
   ↓ HTTP/WebSocket
Node.js Server (Express)
   ↓ spawn/pipe
C++ Simulator (fire_sim.exe)
   ↓ STATE: JSON
Node.js Server
   ↓ WebSocket
Browser (React)
```

---

## Key Features

✅ Real-time grid visualization
✅ Live statistics dashboard
✅ Control panel with difficulty presets
✅ Run history viewer
✅ Professional light UI theme
✅ BFS, Dijkstra, A*, Hybrid algorithms
✅ Production-ready deployment

---

*Bookmark this card for quick reference!*
