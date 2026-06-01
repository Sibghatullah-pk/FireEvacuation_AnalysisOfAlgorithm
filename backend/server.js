const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');
const WebSocket = require('ws');
const http = require('http');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server, perMessageDeflate: false, clientTracking: true });

// Middleware
app.use(cors());
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, '..', 'frontend', 'build')));

// Configuration
const PORT = process.env.PORT || 5000;
const CPP_EXECUTABLE = path.join(__dirname, '..', 'fire_sim.exe');
const RUNS_DIR = path.join(__dirname, '..', 'output');

// Ensure output directory exists
if (!fs.existsSync(RUNS_DIR)) {
    fs.mkdirSync(RUNS_DIR, { recursive: true });
}

// Store active simulations
const activeSims = new Map();

/**
 * Spawn C++ simulator with given config
 * Config shape: { width, height, peopleCount, fireCount, obstacleDensity, fireSpreadChance, difficulty, autoMode, speed }
 */
function spawnSimulation(config, ws) {
    return new Promise((resolve, reject) => {
        // Build command-line arguments for the C++ executable
        // For now, we'll use environment variables or stdin
        const simProcess = spawn(CPP_EXECUTABLE, [], {
            stdio: ['pipe', 'pipe', 'pipe'],
            cwd: path.dirname(CPP_EXECUTABLE)
        });

        let buffer = '';
        const simId = Date.now();

        simProcess.stdout.on('data', (data) => {
            buffer += data.toString();

            // Parse STATE: prefixed messages
            const lines = buffer.split('\n');
            for (let i = 0; i < lines.length - 1; i++) {
                const line = lines[i];
                if (line.startsWith('STATE:')) {
                    try {
                        const jsonStr = line.substring(6);
                        const state = JSON.parse(jsonStr);

                        // Broadcast to all connected clients
                        wss.clients.forEach(client => {
                            if (client.readyState === WebSocket.OPEN && client.simId === simId) {
                                client.send(JSON.stringify({ type: 'state', data: state }));
                            }
                        });
                    } catch (e) {
                        console.error('Failed to parse state JSON:', e);
                    }
                }
            }
            buffer = lines[lines.length - 1];
        });

        simProcess.stderr.on('data', (data) => {
            console.error(`Simulator stderr: ${data}`);
        });

        simProcess.on('close', (code) => {
            console.log(`Simulator exited with code ${code}`);
            activeSims.delete(simId);

            // Signal completion to client
            wss.clients.forEach(client => {
                if (client.readyState === WebSocket.OPEN && client.simId === simId) {
                    client.send(JSON.stringify({ type: 'complete', code }));
                }
            });

            resolve(simId);
        });

        simProcess.on('error', (err) => {
            console.error('Failed to spawn simulator:', err);
            reject(err);
        });

        // Send config to simulator via stdin (placeholder for future enhancement)
        simProcess.stdin.write(JSON.stringify(config) + '\n');

        activeSims.set(simId, { process: simProcess, config });
    });
}

/**
 * WebSocket connection handler
 */
wss.on('connection', (ws, req) => {
    console.log(`[WS] Connected (${wss.clients.size} clients)`);
    ws.isAlive = true;

    ws.on('pong', () => {
        ws.isAlive = true;
    });

    ws.on('message', async (message) => {
        try {
            const msg = JSON.parse(message.toString());

            if (msg.type === 'start') {
                console.log(`[WS] Starting simulation`);
                const simId = await spawnSimulation(msg.config, ws);
                ws.simId = simId;
                ws.send(JSON.stringify({ type: 'started', simId }));
            } else if (msg.type === 'stop') {
                const sim = activeSims.get(ws.simId);
                if (sim) {
                    sim.process.kill();
                }
            }
        } catch (e) {
            console.error(`[WS] Error: ${e.message}`);
            try {
                ws.send(JSON.stringify({ type: 'error', message: e.message }));
            } catch (err) {
                console.error(`[WS] Send failed: ${err.message}`);
            }
        }
    });

    ws.on('close', () => {
        console.log(`[WS] Disconnected (${wss.clients.size} clients)`);
    });

    ws.on('error', (err) => {
        console.error(`[WS] Error: ${err.message}`);
    });
});

// Heartbeat ping to prevent connection drops
const heartbeatInterval = setInterval(() => {
    wss.clients.forEach((ws) => {
        if (ws.isAlive === false) return ws.terminate();
        ws.isAlive = false;
        ws.ping();
    });
}, 30000);

/**
 * REST API endpoints
 */

// Health check
app.get('/api/health', (req, res) => {
    res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

// Get run history
app.get('/api/runs', (req, res) => {
    try {
        const files = fs.readdirSync(RUNS_DIR);
        const runs = files
            .filter(f => f.startsWith('simulation_report_'))
            .map(f => {
                const stat = fs.statSync(path.join(RUNS_DIR, f));
                return { filename: f, size: stat.size, mtime: stat.mtime };
            })
            .sort((a, b) => b.mtime - a.mtime);

        res.json(runs);
    } catch (e) {
        res.status(500).json({ error: e.message });
    }
});

// Get specific run report
app.get('/api/runs/:filename', (req, res) => {
    try {
        const filePath = path.join(RUNS_DIR, req.params.filename);

        // Validate path to prevent directory traversal
        if (!filePath.startsWith(RUNS_DIR)) {
            return res.status(403).json({ error: 'Forbidden' });
        }

        const content = fs.readFileSync(filePath, 'utf-8');
        res.json({ filename: req.params.filename, content });
    } catch (e) {
        res.status(404).json({ error: 'Not found' });
    }
});

// Fallback to index.html for SPA routing
app.get('*', (req, res) => {
    const indexPath = path.join(__dirname, '..', 'frontend', 'build', 'index.html');
    if (fs.existsSync(indexPath)) {
        res.sendFile(indexPath);
    } else {
        res.status(404).send('Frontend not built. Run: cd frontend && npm run build');
    }
});

// Error handling
app.use((err, req, res, next) => {
    console.error(err.stack);
    res.status(500).json({ error: err.message });
});

// Start server
server.listen(PORT, '0.0.0.0', () => {
    console.log(`🔥 Fire Evacuation Backend running on http://localhost:${PORT}`);
    console.log(`WebSocket endpoint: ws://localhost:${PORT}`);
    console.log(`API endpoints:`);
    console.log(`  GET  /api/health        - Health check`);
    console.log(`  GET  /api/runs          - List previous runs`);
    console.log(`  GET  /api/runs/:file    - Get run report`);
});

// Graceful shutdown
process.on('SIGTERM', () => {
    console.log('[Server] SIGTERM, closing...');
    clearInterval(heartbeatInterval);
    server.close(() => process.exit(0));
});
