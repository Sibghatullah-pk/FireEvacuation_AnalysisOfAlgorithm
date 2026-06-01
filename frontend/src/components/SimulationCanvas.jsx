import React, { useEffect, useRef } from 'react';
import './SimulationCanvas.css';

const CELL_SIZE = 12;
const COLORS = {
    empty: '#ffffff',
    obstacle: '#333333',
    fire: '#ff4444',
    person: '#4444ff',
    exit: '#44ff44',
};

export default function SimulationCanvas({ state }) {
    const canvasRef = useRef(null);

    useEffect(() => {
        if (!state || !state.grid) return;

        const canvas = canvasRef.current;
        const ctx = canvas.getContext('2d');

        // Determine grid dimensions
        const width = Math.max(...state.grid.map(c => c.x)) + 1;
        const height = Math.max(...state.grid.map(c => c.y)) + 1;

        // Set canvas size
        canvas.width = width * CELL_SIZE;
        canvas.height = height * CELL_SIZE;

        // Clear canvas
        ctx.fillStyle = '#f0f0f0';
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        // Draw grid cells
        state.grid.forEach(cell => {
            const x = cell.x * CELL_SIZE;
            const y = cell.y * CELL_SIZE;

            let color = COLORS[cell.type] || COLORS.empty;

            // Add heat gradient for fire cells
            if (cell.type === 'fire' && cell.heat > 0) {
                const intensity = cell.heat;
                color = `rgba(255, ${Math.round(68 * (1 - intensity))}, ${Math.round(68 * (1 - intensity))}, 1)`;
            }

            ctx.fillStyle = color;
            ctx.fillRect(x, y, CELL_SIZE, CELL_SIZE);

            // Draw border
            ctx.strokeStyle = '#ddd';
            ctx.lineWidth = 0.5;
            ctx.strokeRect(x, y, CELL_SIZE, CELL_SIZE);
        });

        // Draw grid lines (optional, for clarity)
        ctx.strokeStyle = '#e0e0e0';
        ctx.lineWidth = 0.5;
        for (let x = 0; x <= width; x++) {
            ctx.beginPath();
            ctx.moveTo(x * CELL_SIZE, 0);
            ctx.lineTo(x * CELL_SIZE, height * CELL_SIZE);
            ctx.stroke();
        }
        for (let y = 0; y <= height; y++) {
            ctx.beginPath();
            ctx.moveTo(0, y * CELL_SIZE);
            ctx.lineTo(width * CELL_SIZE, y * CELL_SIZE);
            ctx.stroke();
        }
    }, [state]);

    return (
        <div className="simulation-canvas-container">
            <div className="canvas-header">
                <h3>Simulation Grid</h3>
                <div className="legend">
                    <div className="legend-item">
                        <div className="legend-color" style={{ backgroundColor: COLORS.empty }}></div>
                        Empty
                    </div>
                    <div className="legend-item">
                        <div className="legend-color" style={{ backgroundColor: COLORS.obstacle }}></div>
                        Obstacle
                    </div>
                    <div className="legend-item">
                        <div className="legend-color" style={{ backgroundColor: COLORS.fire }}></div>
                        Fire
                    </div>
                    <div className="legend-item">
                        <div className="legend-color" style={{ backgroundColor: COLORS.person }}></div>
                        Person
                    </div>
                    <div className="legend-item">
                        <div className="legend-color" style={{ backgroundColor: COLORS.exit }}></div>
                        Exit
                    </div>
                </div>
            </div>
            <div className="canvas-wrapper">
                <canvas ref={canvasRef} className="grid-canvas"></canvas>
            </div>
        </div>
    );
}
