# 🔥 Fire Evacuation Simulator - Full Stack

## What's New: React Frontend + Node.js Backend

This fire evacuation simulator has been upgraded to a professional full-stack application with real-time web visualization.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                   React SPA Frontend                     │
│  • Real-time grid visualization (Canvas)                │
│  • Live stats dashboard                                 │
│  • Control panel with presets                           │
│  • Run history viewer                                   │
│  • Light theme, classic UI                             │
└──────────────┬──────────────────────────────────────────┘
               │ WebSocket
┌──────────────▼──────────────────────────────────────────┐
│            Node.js Express Backend                       │
│  • REST API for runs & history                          │
│  • WebSocket server for real-time state streaming       │
│  • Spawns & monitors C++ simulator                      │
│  • Parses STATE: JSON output from C++                   │
└──────────────┬──────────────────────────────────────────┘
               │ stdio/pipes
┌──────────────▼──────────────────────────────────────────┐
│       C++ Simulation Engine (fire_sim.exe)              │
│  • BFS, Dijkstra, A*, Hybrid, Multi-Objective           │
│  • Fire spread physics                                  │
│  • Person evacuation AI                                 │
│  • NEW: StateOutput module for JSON streaming           │
└─────────────────────────────────────────────────────────┘
```

---

## Key Features

### Frontend (React + Vite)
✅ **Real-time Visualization**
- Canvas-based grid renderer
- Fire (red), people (blue), obstacles (dark), exits (green)
- Heat map for fire intensity

✅ **Live Dashboard**
- Step counter
- Rescued/burned/alive statistics
- Survival rate percentage
- Progress bars
- Active algorithm indicator

✅ **Control Panel**
- 3 difficulty presets (Easy/Normal/Hard)
- Adjustable parameters: map size, population, fire count, obstacles, spread rate
- Speed control for auto-mode
- Start/Stop buttons

✅ **Run History**
- Browse previous simulations
- View detailed reports
- File size and timestamps

✅ **Modern UI**
- Light theme with gradient accents
- Responsive layout (desktop/tablet)
- Smooth animations and transitions
- Professional color scheme (#667eea, #764ba2)

### Backend (Node.js + Express)
✅ **WebSocket Server**
- Real-time state streaming from C++
- Bi-directional simulation control

✅ **REST API**
- `/api/health` - Health check
- `/api/runs` - List simulation reports
- `/api/runs/:filename` - Get run details

✅ **Process Management**
- Spawns C++ executable with config
- Parses stdout for STATE: JSON messages
- Broadcasts to connected clients
- Handles clean shutdown

### Backend (C++ Enhancement)
✅ **StateOutput Module**
- `include/StateOutput.h` & `src/StateOutput.cpp`
- Emits JSON state at each simulation step
- Contains grid cells, person positions, stats
- Properly escaped JSON for safe parsing

---

## Quick Start

### 1. Build Everything
```bash
# Option A: Batch script (Windows)
build.bat

# Option B: PowerShell script (Windows)
powershell -ExecutionPolicy Bypass -File build.ps1

# Option C: Manual steps
g++ -std=c++17 -Iinclude src/*.cpp -o fire_sim.exe
cd backend && npm install
cd frontend && npm install && npm run build
```

### 2. Start Development Environment
```bash
# Option A: Automated (opens 2 windows)
start-dev.bat

# Option B: Manual - Terminal 1
cd backend && npm start

# Option B: Manual - Terminal 2
cd frontend && npm run dev
```

### 3. Access Application
- **Frontend + Backend**: http://localhost:5000
- **Frontend Dev Mode**: http://localhost:3000 (with hot reload)

### 4. Start Production
```bash
start.bat
# Opens backend on http://localhost:5000
```

---

## File Structure

```
FireEvacuation_AnalysisOfAlgorithm/
├── src/
│   ├── main.cpp
│   ├── SimulationManager.cpp
│   ├── Grid.cpp
│   ├── Fire.cpp
│   ├── Person.cpp
│   ├── PathfindingAlgorithms.cpp
│   └── StateOutput.cpp                    ✨ NEW
├── include/
│   ├── Position.h
│   ├── Grid.h
│   ├── Fire.h
│   ├── Person.h
│   ├── PathfindingAlgorithms.h
│   ├── SimulationManager.h
│   ├── ConsoleUI.h
│   └── StateOutput.h                      ✨ NEW
├── backend/                               ✨ NEW
│   ├── package.json
│   ├── server.js
│   └── node_modules/
├── frontend/                              ✨ NEW
│   ├── package.json
│   ├── vite.config.js
│   ├── index.html
│   ├── src/
│   │   ├── App.jsx
│   │   ├── App.css
│   │   ├── main.jsx
│   │   ├── hooks/
│   │   │   └── useWebSocket.js
│   │   └── components/
│   │       ├── SimulationCanvas.jsx
│   │       ├── SimulationCanvas.css
│   │       ├── ControlPanel.jsx
│   │       ├── ControlPanel.css
│   │       ├── StatsPanel.jsx
│   │       ├── StatsPanel.css
│   │       ├── RunHistory.jsx
│   │       └── RunHistory.css
│   └── build/                              (generated after npm run build)
├── output/                                 (simulation reports)
├── build.bat                               ✨ NEW
├── build.ps1                               ✨ NEW
├── start.bat                               ✨ NEW
├── start-dev.bat                           ✨ NEW
├── FULLSTACK_SETUP.md                      ✨ NEW
├── ARCHITECTURE.md                         (this file)
└── README.md                               (original)
```

---

## Technology Stack

| Layer | Technology | Purpose |
|-------|-----------|---------|
| Frontend | React 18 + Vite | SPA with real-time UI |
| Backend | Node.js + Express | API server & WebSocket |
| Simulator | C++ 17 | Algorithm implementation |
| Communication | WebSocket | Real-time state streaming |
| Canvas | Canvas 2D API | Grid visualization |
| Styling | CSS 3 | Light theme UI |

---

## Performance Characteristics

- **C++ Simulator**: ~10-50ms per step (depends on grid size)
- **Frontend Render**: ~16ms per frame (60 FPS)
- **Network Latency**: ~1ms (local) / ~50-200ms (cloud)
- **Memory**: ~50MB Node.js + ~100MB C++ process

---

## Deployment Ready

### Cloud Deployment (Heroku/Render)
1. ✅ Frontend pre-built to static files
2. ✅ Single Node.js process serves everything
3. ✅ No build steps required on deploy
4. ✅ Stateless WebSocket connections

### Environment Variables
```bash
PORT=5000                    # Backend port
NODE_ENV=production          # Production flag
```

---

## Roadmap

- [ ] Export reports as PDF with charts
- [ ] Analytics dashboard (multi-run comparison)
- [ ] Custom algorithm plugin system
- [ ] Multiplayer simulation mode (multiple browsers)
- [ ] Docker containerization
- [ ] GitHub Actions CI/CD
- [ ] Database for run persistence
- [ ] Admin panel for system monitoring

---

## Support & Documentation

- **Setup Guide**: [FULLSTACK_SETUP.md](FULLSTACK_SETUP.md)
- **Original README**: [README.md](README.md)
- **Browser Console**: Press `F12` for debugging
- **Backend Logs**: Check terminal output when running `npm start`
- **Network Inspector**: Check WebSocket tab for real-time messages

---

## What's Inside for DAA Project

✅ **Algorithm Implementations**
- BFS (Breadth-First Search)
- Dijkstra's Algorithm
- A* (A-Star) Pathfinding
- Hybrid Adaptive Selection
- Multi-Objective Optimization

✅ **Algorithm Analysis** (in code comments & thesis)
- Time complexity analysis
- Space complexity analysis
- Correctness proofs
- Trade-off discussions

✅ **Visualization for Evaluation**
- Professional frontend shows algorithm behavior
- Real-time step-by-step execution
- Statistics and performance metrics
- Run history for reproducibility

---

## Credits

- **C++ Core**: Original pathfinding & simulation logic
- **Frontend**: React + Vite with modern UI patterns
- **Backend**: Express + WebSocket with clean architecture
- **StateOutput**: JSON streaming module for real-time visualization

**Made for**: Design & Analysis of Algorithms (DAA) Semester Project

---

*Last Updated: June 1, 2026*
