import paho.mqtt.client as mqtt
import threading
import sqlite3
from flask import Flask, jsonify


should_read_moisture = True
app = Flask(__name__)

ESP_ID = '1'
topicMoisture = f"/{ESP_ID}/moisture"
topicPump = f"/{ESP_ID}/pump"
topicStopMoisture = f"/{ESP_ID}/stop_moisture"

# flask
def get_moisture_data():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()
    c.execute('SELECT * FROM moisture')
    data = [{'date_time': row[0], 'value': row[1]} for row in c.fetchall()]
    conn.close()
    return jsonify(data)

@app.route('/api/pump_actions', methods=['GET'])
def get_pump_actions():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()
    c.execute('SELECT * FROM pump_actions')
    data = [{'date_time': row[0], 'action': row[1]} for row in c.fetchall()]
    conn.close()
    return jsonify(data)

#baza danych:
def create_database():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    c.execute('''CREATE TABLE IF NOT EXISTS moisture(
                idESP TEXT,
                date_time TEXT,
                value REAL)''')

    c.execute('''CREATE TABLE IF NOT EXISTS pump_actions(
              idESP TEXT,
              date_time TEXT, 
              action TEXT)''')

    conn.commit()
    conn.close()

def print_database():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    print("Moisture Data:")
    for row in c.execute('SELECT * FROM moisture'):
        print(row)

    print("\nPump Actions Data:")
    for row in c.execute('SELECT * FROM pump_actions'):
        print(row)

    conn.close()


# mqtt
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(topicMoisture)
    client.subscribe(topicPump)
    client.subscribe(topicStopMoisture)

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection. Trying to reconnect...")

def on_message(client, userdata, msg):
    global should_read_moisture
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    if msg.topic == topicMoisture and should_read_moisture:
        # Zapisz wartość wilgotności
        c.execute("INSERT INTO moisture (idESP, date_time, value) VALUES (?, datetime('now'), ?)", (ESP_ID, msg.payload.decode()))       
        print(msg.payload.decode())
    elif msg.topic == topicPump:
        action = "ON" if msg.payload.decode() == "1" else "OFF"
        c.execute("INSERT INTO pump_actions (date_time, action) VALUES (datetime('now'), ?)", (action,))

    conn.commit()
    conn.close()


def user_input():
    global should_read_moisture

    while True:
        value = input("Enter command ('1' ON, '0' OFF, '3' Toggle Moisture Reading, '4' Print DB): \n")
        if value == '1' or value == '0':
            client.publish(topicPump, value)
        elif value == '3':
            should_read_moisture = not should_read_moisture
            if should_read_moisture:
                print("Moisture reading resumed.")
                client.publish(topicStopMoisture, "0")
            else:
                print("Moisture reading stopped.")
                client.publish(topicStopMoisture, "1")
        elif value == '4':
            print_database()
        else:
            print("Invalid input. Please enter a valid command.")


if __name__ == "__main__":
    client = mqtt.Client(client_id='python')
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    client.reconnect_delay_set(min_delay=1, max_delay=30)

    client.connect("172.20.10.9", 1883, 60)
    # app.run(host='0.0.0.0', port=8080)
    

    # Uruchomienie wątku do obsługi wejścia użytkownika
    thread = threading.Thread(target=user_input)
    thread.start()

    # Utworzenie bazy danych
    create_database()

    # Rozpoczęcie głównej pętli
    client.loop_forever()

