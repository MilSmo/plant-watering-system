import mqtt from 'mqtt';
import express, { Request, Response } from 'express';
import { open, Database } from 'sqlite';
import sqlite3 from 'sqlite3';

const app = express();
const dbPath = 'mqtt_data.db';
let shouldReadMoisture = true;

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