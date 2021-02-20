
import socketio
import json
from drone import Drone
from flask import Flask, jsonify, render_template
from flask_socketio import *
import cflib
from cflib.crazyflie import Crazyflie


app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins='*')

# Initialize the low-level drivers (don't list the debug drivers)
cflib.crtp.init_drivers(enable_debug_driver=False)
# Scan for Crazyflies and use the first one found
print('Scanning interfaces for Crazyflies...')
available = cflib.crtp.scan_interfaces()
print('Crazyflies found:')
drones = [Drone("radio://0/80/250K"), Drone("radio://0/71/250K")]

@socketio.on('TOGGLE_LED')
def ledToggler(data):
    print(data['id'])
    drones[data['id']].toggleLED()
    print("LED TOGGLER")
@socketio.on('REFRESH')
def ledToggler(data):
    data_to_send = {
        'id' : data['id'],
        'vbat' : drones[data['id']].getVBat(),
        'isConnected' : drones[data['id']].getIsConnected()
    }
    socketio.emit('drone_data', data_to_send)
@socketio.on('TAKEOFF')
def takeOff(data):
    print("TAKE OFF")
@socketio('RETURN_BASE')
def returnToBase(data):
    print("Returning to base")  

if __name__ == '__main__':
    app.run()
