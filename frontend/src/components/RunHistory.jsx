import React, { useState, useEffect } from 'react';
import './RunHistory.css';

export default function RunHistory() {
    const [runs, setRuns] = useState([]);
    const [selectedRun, setSelectedRun] = useState(null);
    const [runContent, setRunContent] = useState(null);
    const [loading, setLoading] = useState(false);

    useEffect(() => {
        fetchRuns();
    }, []);

    const fetchRuns = async () => {
        try {
            const response = await fetch('/api/runs');
            const data = await response.json();
            setRuns(data);
        } catch (error) {
            console.error('Failed to fetch runs:', error);
        }
    };

    const handleRunClick = async (run) => {
        setSelectedRun(run);
        setLoading(true);
        try {
            const response = await fetch(`/api/runs/${run.filename}`);
            const data = await response.json();
            setRunContent(data.content);
        } catch (error) {
            console.error('Failed to fetch run content:', error);
            setRunContent('Error loading report');
        } finally {
            setLoading(false);
        }
    };

    const formatDate = (dateString) => {
        const date = new Date(dateString);
        return date.toLocaleString();
    };

    const formatFileSize = (bytes) => {
        if (bytes < 1024) return bytes + ' B';
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + ' KB';
        return (bytes / (1024 * 1024)).toFixed(2) + ' MB';
    };

    return (
        <div className="run-history">
            <div className="history-header">
                <h2>Simulation Run History</h2>
                <button className="refresh-btn" onClick={fetchRuns}>
                    🔄 Refresh
                </button>
            </div>

            <div className="history-content">
                <div className="runs-list">
                    <h3>Previous Runs</h3>
                    {runs.length === 0 ? (
                        <div className="empty-state">
                            <p>No simulation runs yet. Start a simulation to see results here.</p>
                        </div>
                    ) : (
                        <div className="runs-table">
                            <div className="runs-header">
                                <div className="run-col-date">Date</div>
                                <div className="run-col-size">Size</div>
                            </div>
                            {runs.map((run, idx) => (
                                <div
                                    key={idx}
                                    className={`run-item ${selectedRun?.filename === run.filename ? 'active' : ''}`}
                                    onClick={() => handleRunClick(run)}
                                >
                                    <div className="run-col-date">{formatDate(run.mtime)}</div>
                                    <div className="run-col-size">{formatFileSize(run.size)}</div>
                                </div>
                            ))}
                        </div>
                    )}
                </div>

                <div className="run-viewer">
                    {selectedRun ? (
                        <>
                            <h3>{selectedRun.filename}</h3>
                            {loading ? (
                                <div className="loading">Loading report...</div>
                            ) : runContent ? (
                                <div className="report-content">
                                    <pre>{runContent}</pre>
                                </div>
                            ) : (
                                <div className="error">Failed to load report</div>
                            )}
                        </>
                    ) : (
                        <div className="empty-viewer">
                            <p>Select a run to view the detailed report</p>
                        </div>
                    )}
                </div>
            </div>
        </div>
    );
}
