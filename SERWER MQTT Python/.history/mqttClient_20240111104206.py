import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subskrybuj tutaj tematy
    client.subscribe("twój/temat")

# Funkcja wywoływana po otrzymaniu wiadomości z subskrybowanego tematu
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Podłącz do serwera Mosquitto
client.connect("172.20.10.9", 1883, 60)

# Rozpocznij pętlę, aby proces był ciągły
client.loop_forever()