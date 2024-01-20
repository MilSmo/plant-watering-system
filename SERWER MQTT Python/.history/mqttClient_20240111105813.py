import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/topic/moisture")  # Upewnij się, że temat jest zgodny

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

client = mqtt.Client(client_id=' python')
client.on_connect = on_connect
client.on_message = on_message

client.connect("172.20.10.9", 1883, 60)  # Adres IP serwera MQTT
client.loop_forever()
