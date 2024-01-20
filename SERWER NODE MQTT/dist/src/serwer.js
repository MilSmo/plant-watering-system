"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const mqtt_1 = __importDefault(require("mqtt"));
const express_1 = __importDefault(require("express"));
const sqlite_1 = require("sqlite");
const sqlite3_1 = __importDefault(require("sqlite3"));
const app = (0, express_1.default)();
const dbPath = 'mqtt_data.db';
let shouldReadMoisture = true;
function openDb() {
    return __awaiter(this, void 0, void 0, function* () {
        return (0, sqlite_1.open)({
            filename: dbPath,
            driver: sqlite3_1.default.Database
        });
    });
}
function createDatabase() {
    return __awaiter(this, void 0, void 0, function* () {
        const db = yield openDb();
        yield db.run(`CREATE TABLE IF NOT EXISTS moisture (date_time TEXT, value REAL)`);
        yield db.run(`CREATE TABLE IF NOT EXISTS pump_actions (date_time TEXT, action TEXT)`);
        yield db.close();
    });
}
//MQTT
const client = mqtt_1.default.connect('mqtt://172.20.10.9:1883');
client.on('connect', () => {
    console.log("Connected to MQTT broker");
    client.subscribe('/topic/moisture');
    client.subscribe('/topic/pump');
    client.subscribe('/topic/stop_moisture');
});
client.on('message', (topic, message) => __awaiter(void 0, void 0, void 0, function* () {
    const db = yield openDb();
    if (topic === '/topic/moisture' && shouldReadMoisture) {
        yield db.run("INSERT INTO moisture (date_time, value) VALUES (datetime('now'), ?)", [message.toString()]);
    }
    else if (topic === '/topic/pump') {
        const action = message.toString() === '1' ? 'ON' : 'OFF';
        yield db.run("INSERT INTO pump_actions (date_time, action) VALUES (datetime('now'), ?)", [action]);
    }
    yield db.close();
}));
// Express routes
app.get('/api/moisture_data', (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    const db = yield openDb();
    const rows = yield db.all('SELECT * FROM moisture');
    res.json(rows);
    yield db.close();
}));
app.get('/api/pump_actions', (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    const db = yield openDb();
    const rows = yield db.all('SELECT * FROM pump_actions');
    res.json(rows);
    yield db.close();
}));
app.listen(8080, () => __awaiter(void 0, void 0, void 0, function* () {
    console.log('Server is running on http://localhost:8080');
    yield createDatabase();
}));
// MQTT - wysyłanie komend do ESP32
app.post('/api/control_pump', (req, res) => {
    const command = req.query.command; // Przykład: /api/control_pump?command=on
    if (command === 'on' || command === 'off') {
        const mqttCommand = command === 'on' ? '1' : '0';
        client.publish('/topic/pump', mqttCommand);
        res.send(`Pump turned ${command}`);
    }
    else {
        res.status(400).send('Invalid command');
    }
});
app.post('/api/toggle_moisture_reading', (req, res) => {
    shouldReadMoisture = !shouldReadMoisture;
    const mqttCommand = shouldReadMoisture ? '0' : '1';
    client.publish('/topic/stop_moisture', mqttCommand);
    res.send(`Moisture reading ${shouldReadMoisture ? 'resumed' : 'stopped'}`);
});
