import paho.mqtt.client as mqtt
import threading
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/topic/moisture")
    client.subscribe("/topic/pump")

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

def publish_pump():
    while True:
        client.publish("/topic/pump", "1")
        time.sleep(1)  # Włącz na 2 sekundy
        client.publish("/topic/pump", "0")
        time.sleep(2)  # Wyłącz na 4 sekundy

client = mqtt.Client(client_id='python')
client.on_connect = on_connect
client.on_message = on_message

client.connect("172.20.10.9", 1883, 60)

# Uruchomienie wątku do publikowania
thread = threading.Thread(target=publish_pump)
thread.start()

# Rozpoczęcie głównej pętli
client.loop_forever()
