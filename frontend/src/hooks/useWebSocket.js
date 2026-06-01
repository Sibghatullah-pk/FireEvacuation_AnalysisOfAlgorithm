import { useEffect, useRef, useState, useCallback } from 'react';

export function useWebSocket(onMessage) {
    const ws = useRef(null);
    const [isConnected, setIsConnected] = useState(false);
    const reconnectAttempts = useRef(0);
    const reconnectTimeout = useRef(null);
    const onMessageRef = useRef(onMessage);

    // Keep ref updated
    useEffect(() => {
        onMessageRef.current = onMessage;
    }, [onMessage]);

    useEffect(() => {
        const connectWebSocket = () => {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const defaultHost = import.meta.env.DEV ? 'localhost:5000' : window.location.host;
            const wsUrl = `${protocol}//${defaultHost}`;

            console.log(`[WebSocket] Connecting to ${wsUrl}`);
            ws.current = new WebSocket(wsUrl);

            ws.current.onopen = () => {
                console.log('[WebSocket] Connected');
                setIsConnected(true);
                reconnectAttempts.current = 0;
            };

            ws.current.onmessage = (event) => {
                try {
                    const message = JSON.parse(event.data);
                    onMessageRef.current(message);
                } catch (e) {
                    console.error('[WebSocket] Parse error:', e.message);
                }
            };

            ws.current.onerror = (error) => {
                console.error('[WebSocket] Error:', error);
                setIsConnected(false);
            };

            ws.current.onclose = () => {
                console.log('[WebSocket] Closed');
                setIsConnected(false);

                // Exponential backoff reconnect
                const delayMs = Math.min(1000 * Math.pow(2, reconnectAttempts.current), 30000);
                console.log(`[WebSocket] Reconnect in ${delayMs}ms (attempt ${reconnectAttempts.current + 1})`);
                reconnectAttempts.current++;

                reconnectTimeout.current = setTimeout(connectWebSocket, delayMs);
            };
        };

        connectWebSocket();

        return () => {
            if (reconnectTimeout.current) clearTimeout(reconnectTimeout.current);
            if (ws.current) ws.current.close();
        };
    }, []); // Empty dependency - only run once on mount

    const send = useCallback((message) => {
        if (ws.current?.readyState === WebSocket.OPEN) {
            ws.current.send(JSON.stringify(message));
        } else {
            console.warn('[WebSocket] Not open, cannot send');
        }
    }, []);

    return { send, isConnected };
}
