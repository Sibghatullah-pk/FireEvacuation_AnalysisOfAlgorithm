import React, { useState, useCallback, useEffect } from 'react';
import { useWebSocket } from './hooks/useWebSocket';
import SimulationCanvas from './components/SimulationCanvas';
import ControlPanel from './components/ControlPanel';
import StatsPanel from './components/StatsPanel';
import RunHistory from './components/RunHistory';
import './App.css';

export default function App() {
    const [activeTab, setActiveTab] = useState('simulator');
    const [config, setConfig] = useState({
        width: 50,
        height: 25,
        peopleCount: 8,
        fireCount: 2,
        obstacleDensity: 0.12,
        fireSpreadChance: 55,
        difficulty: 'normal',
        autoMode: true,
        speed: 800,
    });

    const [simulationState, setSimulationState] = useState(null);
    const [stats, setStats] = useState({ total: 0, rescued: 0, burned: 0, alive: 0 });
    const [isRunning, setIsRunning] = useState(false);

    const { send, isConnected } = useWebSocket((message) => {
        if (message.type === 'state') {
            setSimulationState(message.data);
            setStats(message.data.stats);
        } else if (message.type === 'complete') {
            setIsRunning(false);
        } else if (message.type === 'started') {
            setIsRunning(true);
        }
    });

    const handleStartSimulation = useCallback(() => {
        if (isConnected) {
            send({
                type: 'start',
                config,
            });
        }
    }, [config, isConnected, send]);

    const handleStopSimulation = useCallback(() => {
        send({ type: 'stop' });
    }, [send]);

    const handleConfigChange = useCallback((newConfig) => {
        setConfig(newConfig);
    }, []);

    return (
        <div className="app">
            <header className="app-header">
                <h1>🔥 Fire Evacuation Simulator</h1>
                <div className="header-status">
                    <span className={`status-badge ${isConnected ? 'connected' : 'disconnected'}`}>
                        {isConnected ? '● Connected' : '● Disconnected'}
                    </span>
                    <span className={`running-badge ${isRunning ? 'running' : 'idle'}`}>
                        {isRunning ? '⏳ Running' : '⏸ Idle'}
                    </span>
                </div>
            </header>

            <div className="tabs">
                <button
                    className={`tab ${activeTab === 'simulator' ? 'active' : ''}`}
                    onClick={() => setActiveTab('simulator')}
                >
                    Simulator
                </button>
                <button
                    className={`tab ${activeTab === 'history' ? 'active' : ''}`}
                    onClick={() => setActiveTab('history')}
                >
                    History
                </button>
            </div>

            {activeTab === 'simulator' && (
                <div className="main-content">
                    <div className="left-panel">
                        <SimulationCanvas state={simulationState} />
                        <StatsPanel stats={stats} dominantAlgorithm={simulationState?.dominantAlgorithm} step={simulationState?.step} />
                    </div>
                    <div className="right-panel">
                        <ControlPanel
                            config={config}
                            isRunning={isRunning}
                            isConnected={isConnected}
                            onConfigChange={handleConfigChange}
                            onStart={handleStartSimulation}
                            onStop={handleStopSimulation}
                        />
                    </div>
                </div>
            )}

            {activeTab === 'history' && (
                <div className="history-content">
                    <RunHistory />
                </div>
            )}
        </div>
    );
}
