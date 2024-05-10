"""
Main.py
Test suite connection to microcontroller (ESP32, M5Core2)
Semester 1, 2024
BIOE6901
"""

import json
import os
from flask import Flask, request
from flask_cors import CORS, cross_origin
from mqtt_thread import MqttThread

__author__ = "Rachel Chiong"

app = Flask(__name__)

# Handle CORS as necessary
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

# Get the directory of the script
script_dir = os.path.dirname(os.path.realpath(__file__))
# Construct the path to the mqtt_config.json file
mqtt_config_path = os.path.join(script_dir, "../../mqtt_config.json")

try:
    with open(mqtt_config_path, 'r') as file:
        data = json.load(file)
        Host = data.get('host')
        Topic = data.get('topic')
except FileNotFoundError:
    print("[ERROR] No MQTT Configuration found. Please include a mqtt_config.json file.")
    exit()

# Create thread stubs

Th_NRMTestSuite = MqttThread("NRMTestSuite", Host, Topic)

@app.route('/')
@cross_origin
def root():
    return 'Hello from Flask backend!'

@app.route('/default')
@cross_origin
def default_connect():
    Th_NRMTestSuite.config(Host, Topic)
    Th_NRMTestSuite.start()
    return {
        "response": "OK - successfully connected",
        "status": 200,
    }

@app.route('/tof')
@cross_origin
def get_telemetry():
    return Th_NRMTestSuite.get_payload()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001, debug=True)