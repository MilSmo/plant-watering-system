import paho.mqtt.client as mqtt
import threading

#baza danych:
def create_database():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    # Utwórz tabelę dla wilgotności
    c.execute('''CREATE TABLE IF NOT EXISTS moisture
                 (date_time TEXT, value REAL)''')

    # Utwórz tabelę dla włączeń pompy
    c.execute('''CREATE TABLE IF NOT EXISTS pump_actions
                 (date_time TEXT, action TEXT)''')

    conn.commit()
    conn.close()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/topic/moisture")
    client.subscribe("/topic/pump")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection. Trying to reconnect...")

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

def user_input():
    while True:
        value = input("Enter '1' to turn ON the pump or '0' to turn OFF: ")
        if value in ['0', '1']:
            client.publish("/topic/pump", value)
        else:
            print("Invalid input. Please enter '1' or '0'.")


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
