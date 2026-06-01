# Full-Stack Setup Guide: Fire Evacuation Simulator

## Overview

This project is now a complete full-stack application:
- **C++ Backend**: Simulation engine with pathfinding algorithms
- **Node.js API**: Express server + WebSocket for real-time state streaming
- **React Frontend**: Modern SPA with grid visualization, controls, and analytics

## Prerequisites

- **Windows**: C++ compiler (MinGW, MSVC)
- **Node.js**: v16+ ([download](https://nodejs.org/))
- **npm**: Comes with Node.js

## Project Structure

```
FireEvacuation_AnalysisOfAlgorithm/
├── src/                      # C++ source files
├── include/                  # C++ header files
│   └── StateOutput.h         # NEW: Real-time state emitter
├── backend/                  # Node.js Express server
│   ├── package.json
│   ├── server.js
│   └── node_modules/
├── frontend/                 # React SPA
│   ├── package.json
│   ├── vite.config.js
│   ├── index.html
│   ├── src/
│   │   ├── App.jsx
│   │   ├── components/
│   │   ├── hooks/
│   │   └── main.jsx
│   └── build/                # Output folder (after build)
├── output/                   # Simulation reports
└── README.md
```

## Quick Start

### 1. Build C++ Simulator

```bash
cd S:\FireEvacuation_AnalysisOfAlgorithm

# Compile with state output support
g++ -std=c++17 -Iinclude src/main.cpp src/SimulationManager.cpp src/Grid.cpp src/Fire.cpp src/Person.cpp src/PathfindingAlgorithms.cpp src/StateOutput.cpp -o fire_sim.exe
```

### 2. Install Backend Dependencies

```bash
cd backend
npm install
```

### 3. Install Frontend Dependencies

```bash
cd frontend
npm install
```

### 4. Build React Frontend

```bash
cd frontend
npm run build
```

This creates a `build/` folder with optimized static files.

### 5. Start Backend Server

```bash
cd backend
npm start
```

You should see:
```
🔥 Fire Evacuation Backend running on http://localhost:5000
WebSocket endpoint: ws://localhost:5000
```

### 6. Access Frontend

Open your browser and navigate to:
```
http://localhost:5000
```

The React app will automatically connect to the backend via WebSocket.

---

## Development Workflow

### Frontend Development (with hot reload)

```bash
cd frontend
npm run dev
```

This starts Vite dev server on `http://localhost:3000` with hot module replacement.
The proxy automatically routes `/api` calls to `http://localhost:5000`.

### Backend Development (with auto-restart)

```bash
cd backend
npm install nodemon --save-dev
npm run dev
```

---

## Architecture

### Data Flow

```
React UI
  ↓
WebSocket → Backend (Node.js)
  ↓
Spawns C++ process (fire_sim.exe)
  ↓
C++ emits STATE: JSON messages
  ↓
Backend parses & broadcasts via WebSocket
  ↓
React renders grid + stats in real-time
```

### State Format

The C++ simulator now outputs JSON state at each simulation step via stdout with `STATE:` prefix:

```json
STATE:{
  "step": 42,
  "stats": {
    "total": 8,
    "rescued": 3,
    "burned": 1,
    "alive": 4
  },
  "dominantAlgorithm": "A_STAR",
  "grid": [
    {"x": 0, "y": 0, "type": "obstacle", "heat": 0.0},
    {"x": 1, "y": 0, "type": "fire", "heat": 1.0},
    ...
  ],
  "people": [
    {"id": 0, "x": 10, "y": 5, "alive": true, "rescued": false},
    ...
  ]
}
```

---

## API Endpoints

### REST API

- `GET /api/health` — Health check
- `GET /api/runs` — List previous simulation runs
- `GET /api/runs/:filename` — Get specific run report

### WebSocket Events

#### Client → Server

```javascript
// Start simulation
{ "type": "start", "config": { ... } }

// Stop simulation
{ "type": "stop" }
```

#### Server → Client

```javascript
// Simulation started
{ "type": "started", "simId": 1234567890 }

// State update (every step)
{ "type": "state", "data": { ... } }

// Simulation complete
{ "type": "complete", "code": 0 }

// Error
{ "type": "error", "message": "..." }
```

---

## Features

### Frontend UI

- **Simulation Grid**: Real-time visualization with Canvas
  - Fire (red), People (blue), Obstacles (dark), Exits (green)
  - Heat map gradient for fire intensity

- **Live Stats Dashboard**:
  - Total population, rescued count, burned count
  - Survival rate & evacuation progress bars
  - Current dominant algorithm display

- **Control Panel**:
  - 3 difficulty presets (Easy, Normal, Hard)
  - Adjustable map size, population, fire spread, obstacle density
  - Speed control for auto-mode
  - Start/Stop buttons

- **Run History**:
  - Browse all previous simulation results
  - View detailed reports with timing, survival metrics

- **Light Theme**: Professional, classic UI with smooth transitions

---

## Configuration

### Difficulty Presets

| Preset | Size | People | Fire | Obstacles | Spread |
|--------|------|--------|------|-----------|--------|
| Easy   | 40×20 | 6 | 1 | 8% | 35% |
| Normal | 50×25 | 8 | 2 | 12% | 55% |
| Hard   | 65×30 | 10 | 3 | 18% | 75% |

---

## Troubleshooting

### WebSocket Connection Failed

**Problem**: Frontend shows "● Disconnected"

**Solution**:
1. Ensure backend is running: `cd backend && npm start`
2. Check port 5000 is not in use: `netstat -ano | findstr :5000`
3. Check firewall settings

### C++ Executable Not Found

**Problem**: Backend crashes with "Cannot spawn fire_sim.exe"

**Solution**:
1. Rebuild C++: `g++ -std=c++17 -Iinclude src/*.cpp -o fire_sim.exe`
2. Verify `fire_sim.exe` is in the project root
3. Check Windows Defender or antivirus isn't blocking execution

### Frontend Shows Blank Page

**Problem**: React app doesn't load

**Solution**:
1. Run `cd frontend && npm run build` to rebuild
2. Restart backend: `cd backend && npm start`
3. Clear browser cache (Ctrl+Shift+Delete)
4. Check browser console for errors (F12)

### No State Updates

**Problem**: Simulation runs but grid doesn't update

**Solution**:
1. Verify C++ code includes `StateOutput.cpp` in build command
2. Check browser WebSocket tab in Network inspector
3. Run backend with debug: `NODE_DEBUG=http node backend/server.js`

---

## Production Deployment

### Build for Production

```bash
# Build C++
g++ -O3 -std=c++17 -Iinclude src/*.cpp -o fire_sim.exe

# Build frontend
cd frontend
npm run build

# Backend is already production-ready
```

### Deploy to Cloud (e.g., Heroku, Render)

1. Create `Procfile`:
   ```
   web: cd backend && npm start
   ```

2. Ensure `frontend/build/` is included in Git

3. Deploy:
   ```bash
   git push heroku main
   ```

---

## Performance Optimization

- C++ compilation flags: `-O3` for optimization, `-march=native` for CPU-specific speedups
- Frontend: Vite produces minified bundles (~100KB gzipped)
- WebSocket: Compression and binary protocols can be added for large state objects
- Grid rendering: Canvas is efficient; consider WebGL for 10K+ grid cells

---

## Next Steps

- [ ] Add PDF export for simulation reports
- [ ] Implement result comparison & analytics dashboard
- [ ] Add custom algorithm plugins
- [ ] Multi-player simulation mode
- [ ] Docker containerization
- [ ] CI/CD pipeline

---

## Support

For issues or feature requests, check:
- C++ headers in `include/`
- Backend logs in terminal
- Browser console (F12)
- Network tab for WebSocket frames
