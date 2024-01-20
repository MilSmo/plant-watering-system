import mqtt from 'mqtt';
import express, { Request, Response } from 'express';
import { open, Database } from 'sqlite';
import sqlite3 from 'sqlite3';

const app = express();
const dbPath = 'mqtt_data.db';
let shouldReadMoisture = true;
const port = 8080; // Użyj właściwego portu
app.listen(port, '0.0.0.0', () => {
    console.log(`Server is running and accessible within the local network at http://0.0.0.0:${port}`);
});

async function openDb(): Promise<Database> {
    return open({
        filename: dbPath,
        driver: sqlite3.Database
    });
}

async function createDatabase(): Promise<void> {
    const db = await openDb();
    await db.run(`CREATE TABLE IF NOT EXISTS moisture (date_time TEXT, value REAL)`);
    await db.run(`CREATE TABLE IF NOT EXISTS pump_actions (date_time TEXT, action TEXT)`);
    await db.close();
}


//MQTT

const client = mqtt.connect('mqtt://172.20.10.9:1883');


client.on('connect', () => {
    console.log("Connected to MQTT broker");
    client.subscribe('/topic/moisture');
    client.subscribe('/topic/pump');
    client.subscribe('/topic/stop_moisture');
});

client.on('message', async (topic, message) => {
    const db = await openDb();
    if (topic === '/topic/moisture' && shouldReadMoisture) {
        await db.run("INSERT INTO moisture (date_time, value) VALUES (datetime('now'), ?)", [message.toString()]);
    } else if (topic === '/topic/pump') {
        const action = message.toString() === '1' ? 'ON' : 'OFF';
        await db.run("INSERT INTO pump_actions (date_time, action) VALUES (datetime('now'), ?)", [action]);
    }
    await db.close();
});

// Express routes
app.get('/api/moisture_data', async (req: Request, res: Response) => {
    const db = await openDb();
    const rows = await db.all('SELECT * FROM moisture');
    res.json(rows);
    await db.close();
});

app.get('/api/pump_actions', async (req: Request, res: Response) => {
    const db = await openDb();
    const rows = await db.all('SELECT * FROM pump_actions');
    res.json(rows);
    await db.close();
});

app.listen(8080, async () => {
    console.log('Server is running on http://localhost:8080');
    await createDatabase();
});


// MQTT - wysyłanie komend do ESP32
app.post('/api/control_pump', (req: Request, res: Response) => {
    const command = req.query.command as string; 
    if (command === 'on' || command === 'off') {
        const mqttCommand = command === 'on' ? '1' : '0';
        client.publish('/topic/pump', mqttCommand);
        res.send(`Pump turned ${command}`);
    } else {
        res.status(400).send('Invalid command');
    }
});

app.post('/api/toggle_moisture_reading', (req: Request, res: Response) => {
    shouldReadMoisture = !shouldReadMoisture;
    const mqttCommand = shouldReadMoisture ? '0' : '1';
    client.publish('/topic/stop_moisture', mqttCommand);
    res.send(`Moisture reading ${shouldReadMoisture ? 'resumed' : 'stopped'}`);
});