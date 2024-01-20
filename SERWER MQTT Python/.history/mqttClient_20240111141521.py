import paho.mqtt.client as mqtt
import threading
import sqlite3

should_read_moisture = True


#baza danych:
def create_database():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    c.execute('''CREATE TABLE IF NOT EXISTS moisture
                 (date_time TEXT, value REAL)''')

    c.execute('''CREATE TABLE IF NOT EXISTS pump_actions
                 (date_time TEXT, action TEXT)''')

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


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/topic/moisture")
    client.subscribe("/topic/pump")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection. Trying to reconnect...")

def on_message(client, userdata, msg):
    global should_read_moisture
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    if msg.topic == "/topic/moisture" and should_read_moisture:
        # Zapisz wartość wilgotności
        c.execute("INSERT INTO moisture (date_time, value) VALUES (datetime('now'), ?)", (msg.payload.decode(),))
        print(msg.payload.decode())
    elif msg.topic == "/topic/pump":
        action = "ON" if msg.payload.decode() == "1" else "OFF"
        c.execute("INSERT INTO pump_actions (date_time, action) VALUES (datetime('now'), ?)", (action,))

    conn.commit()
    conn.close()


def user_input():
    global should_read_moisture

    while True:
        value = input("Enter command ('1' ON, '0' OFF, '3' Toggle Moisture Reading, '4' Print DB): \n")
        if value == '1' or value == '0':
            client.publish("/topic/pump", value)
        elif value == '3':
            should_read_moisture = not should_read_moisture
            if should_read_moisture:
                print("Moisture reading resumed.")
            else:
                print("Moisture reading stopped.")
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

    # Uruchomienie wątku do obsługi wejścia użytkownika
    thread = threading.Thread(target=user_input)
    thread.start()

    # Utworzenie bazy danych
    create_database()

    # Rozpoczęcie głównej pętli
    client.loop_forever()

