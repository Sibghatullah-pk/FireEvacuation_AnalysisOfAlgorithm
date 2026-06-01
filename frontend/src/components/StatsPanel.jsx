import React from 'react';
import './StatsPanel.css';

export default function StatsPanel({ stats, dominantAlgorithm, step }) {
    const survivalRate = stats.total > 0 ? ((stats.rescued / stats.total) * 100).toFixed(1) : 0;
    const burnedRate = stats.total > 0 ? ((stats.burned / stats.total) * 100).toFixed(1) : 0;

    return (
        <div className="stats-panel">
            <div className="stats-header">
                <h3>Live Statistics</h3>
                {step !== undefined && <span className="step-counter">Step: {step}</span>}
            </div>

            <div className="stats-grid">
                <div className="stat-card total">
                    <div className="stat-value">{stats.total}</div>
                    <div className="stat-label">Total People</div>
                </div>

                <div className="stat-card rescued">
                    <div className="stat-value">{stats.rescued}</div>
                    <div className="stat-label">Rescued</div>
                    <div className="stat-percentage">{survivalRate}%</div>
                </div>

                <div className="stat-card burned">
                    <div className="stat-value">{stats.burned}</div>
                    <div className="stat-label">Burned</div>
                    <div className="stat-percentage">{burnedRate}%</div>
                </div>

                <div className="stat-card alive">
                    <div className="stat-value">{stats.alive}</div>
                    <div className="stat-label">Evacuating</div>
                </div>
            </div>

            {dominantAlgorithm && (
                <div className="algorithm-info">
                    <h4>Active Algorithm</h4>
                    <div className="algorithm-badge">{dominantAlgorithm}</div>
                </div>
            )}

            <div className="progress-section">
                <h4>Progress</h4>

                <div className="progress-bar">
                    <div className="progress-label">Rescued</div>
                    <div className="progress-container">
                        <div
                            className="progress-fill rescued"
                            style={{ width: `${stats.total > 0 ? (stats.rescued / stats.total) * 100 : 0}%` }}
                        ></div>
                    </div>
                </div>

                <div className="progress-bar">
                    <div className="progress-label">Burned</div>
                    <div className="progress-container">
                        <div
                            className="progress-fill burned"
                            style={{ width: `${stats.total > 0 ? (stats.burned / stats.total) * 100 : 0}%` }}
                        ></div>
                    </div>
                </div>

                <div className="progress-bar">
                    <div className="progress-label">Evacuating</div>
                    <div className="progress-container">
                        <div
                            className="progress-fill evacuating"
                            style={{ width: `${stats.total > 0 ? (stats.alive / stats.total) * 100 : 0}%` }}
                        ></div>
                    </div>
                </div>
            </div>
        </div>
    );
}
