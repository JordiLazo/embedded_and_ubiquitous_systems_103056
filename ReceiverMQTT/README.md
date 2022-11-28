## RECEIVER MQTT

How to execute the server:

Step 1-
Create a new VENV with this command:

```
python -m venv venv
```

Step 2-
Activate the new VENV with the following commands:

```
cd env
cd Scripts
.\activate
```

Step 3-

Install all the necessary modules with 
```
pip install -r requirements.txt
```

Step 4-

Test the server with the following command:
```
mosquitto_pub.exe -h test.mosquitto.org -t esp/test -m 12
```
