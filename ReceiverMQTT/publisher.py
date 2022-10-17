import paho.mqtt.client as paho

def on_publish(client,userdata,result):             
    print("data published \n")
    pass

def publish_MQTT():
    client = mqtt.Client('SoftwareCaptacio')
    client.on_publish = on_publish
    client.connect('test.mosquitto.org')
    while True:
        missatge = {}
        missatge['id'] = '1'
        missatge['heart']=round(random.uniform(26.0, 36.0), 2)
        missatge['accelerometer']=round(random.uniform(20.0, 60.0), 2)
        client.publish('SensorTFGD',missatge)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    publish_MQTT()