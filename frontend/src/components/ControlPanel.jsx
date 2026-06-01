import React from 'react';
import './ControlPanel.css';

const PRESETS = {
    easy: {
        width: 40,
        height: 20,
        peopleCount: 6,
        fireCount: 1,
        obstacleDensity: 0.08,
        fireSpreadChance: 35,
    },
    normal: {
        width: 50,
        height: 25,
        peopleCount: 8,
        fireCount: 2,
        obstacleDensity: 0.12,
        fireSpreadChance: 55,
    },
    hard: {
        width: 65,
        height: 30,
        peopleCount: 10,
        fireCount: 3,
        obstacleDensity: 0.18,
        fireSpreadChance: 75,
    },
};

export default function ControlPanel({
    config,
    isRunning,
    isConnected,
    onConfigChange,
    onStart,
    onStop,
}) {
    const handlePresetClick = (preset) => {
        onConfigChange({ ...config, ...PRESETS[preset], difficulty: preset });
    };

    const handleConfigChange = (key, value) => {
        onConfigChange({ ...config, [key]: value });
    };

    return (
        <div className="control-panel">
            <section className="control-section">
                <h3>Difficulty Presets</h3>
                <div className="preset-buttons">
                    {['easy', 'normal', 'hard'].map((preset) => (
                        <button
                            key={preset}
                            className={`preset-btn ${config.difficulty === preset ? 'active' : ''}`}
                            onClick={() => handlePresetClick(preset)}
                            disabled={isRunning}
                        >
                            {preset.charAt(0).toUpperCase() + preset.slice(1)}
                        </button>
                    ))}
                </div>
            </section>

            <section className="control-section">
                <h3>Configuration</h3>

                <div className="control-group">
                    <label>Map Width: {config.width}</label>
                    <input
                        type="range"
                        min="30"
                        max="80"
                        value={config.width}
                        onChange={(e) => handleConfigChange('width', parseInt(e.target.value))}
                        disabled={isRunning}
                    />
                </div>

                <div className="control-group">
                    <label>Map Height: {config.height}</label>
                    <input
                        type="range"
                        min="15"
                        max="40"
                        value={config.height}
                        onChange={(e) => handleConfigChange('height', parseInt(e.target.value))}
                        disabled={isRunning}
                    />
                </div>

                <div className="control-group">
                    <label>People Count: {config.peopleCount}</label>
                    <input
                        type="range"
                        min="1"
                        max="15"
                        value={config.peopleCount}
                        onChange={(e) => handleConfigChange('peopleCount', parseInt(e.target.value))}
                        disabled={isRunning}
                    />
                </div>

                <div className="control-group">
                    <label>Fire Count: {config.fireCount}</label>
                    <input
                        type="range"
                        min="1"
                        max="5"
                        value={config.fireCount}
                        onChange={(e) => handleConfigChange('fireCount', parseInt(e.target.value))}
                        disabled={isRunning}
                    />
                </div>

                <div className="control-group">
                    <label>Obstacle Density: {(config.obstacleDensity * 100).toFixed(0)}%</label>
                    <input
                        type="range"
                        min="0"
                        max="30"
                        step="1"
                        value={config.obstacleDensity * 100}
                        onChange={(e) => handleConfigChange('obstacleDensity', parseInt(e.target.value) / 100)}
                        disabled={isRunning}
                    />
                </div>

                <div className="control-group">
                    <label>Fire Spread Chance: {config.fireSpreadChance}%</label>
                    <input
                        type="range"
                        min="10"
                        max="100"
                        step="5"
                        value={config.fireSpreadChance}
                        onChange={(e) => handleConfigChange('fireSpreadChance', parseInt(e.target.value))}
                        disabled={isRunning}
                    />
                </div>

                <div className="control-group">
                    <label>
                        <input
                            type="checkbox"
                            checked={config.autoMode}
                            onChange={(e) => handleConfigChange('autoMode', e.target.checked)}
                            disabled={isRunning}
                        />
                        Auto Mode
                    </label>
                </div>

                <div className="control-group">
                    <label>Speed: {config.speed}ms/step</label>
                    <input
                        type="range"
                        min="100"
                        max="2000"
                        step="100"
                        value={config.speed}
                        onChange={(e) => handleConfigChange('speed', parseInt(e.target.value))}
                        disabled={isRunning || !config.autoMode}
                    />
                </div>
            </section>

            <section className="control-section">
                <h3>Controls</h3>
                <div className="button-group">
                    <button
                        className="btn btn-primary"
                        onClick={onStart}
                        disabled={isRunning || !isConnected}
                    >
                        {isRunning ? '▶ Running' : '▶ Start Simulation'}
                    </button>
                    <button
                        className="btn btn-danger"
                        onClick={onStop}
                        disabled={!isRunning}
                    >
                        ⏹ Stop
                    </button>
                </div>
            </section>

            <section className="control-section info">
                <h4>Current Setup</h4>
                <div className="info-item">
                    <span>Map:</span>
                    <strong>{config.width} × {config.height}</strong>
                </div>
                <div className="info-item">
                    <span>Population:</span>
                    <strong>{config.peopleCount}</strong>
                </div>
                <div className="info-item">
                    <span>Fire Sources:</span>
                    <strong>{config.fireCount}</strong>
                </div>
                <div className="info-item">
                    <span>Obstacles:</span>
                    <strong>{(config.obstacleDensity * 100).toFixed(0)}%</strong>
                </div>
            </section>
        </div>
    );
}
