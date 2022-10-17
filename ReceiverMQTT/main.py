# This is a sample Python script.
import pandas as pd
import csv
from datetime import datetime
import json
import paho.mqtt.client as mqtt
from itertools import count
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Press Mayús+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
field_names = ['timestamp', 'id', 'heart', 'accelerometer']
def write_on_csv(json_missatge):
    with open('base_dades_didac.csv', 'a') as csv_file:
        dict_object = csv.DictWriter(csv_file, fieldnames=field_names)
        dt=datetime.now()
        ts = datetime.timestamp(dt)
        new_entry = {'timestamp': int(ts)}
        new_entry.update(json_missatge)
        print("\nEl missatge rebut es:",new_entry)
        dict_object.writerow(new_entry)


def on_message(client, userdata, message):
    missatge_deco=str(message.payload.decode("utf-8"))
    #print("el missatge es:",missatge_deco)
    missatge = json.loads(missatge_deco)
    write_on_csv(missatge)
    #print("message received ", str(message.payload.decode("utf-8")))


def subscribe_MQTT():
    client = mqtt.Client('SoftwareCaptacio')
    client.on_message = on_message
    client.connect('test.mosquitto.org')
    client.subscribe('Hearthrate')
    client.loop_forever()


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    subscribe_MQTT()
