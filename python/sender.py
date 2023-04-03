import socket
from time import sleep

#Global variables
TYPE_ENGINE_ON = b'\x10'
TYPE_DRIVE = b'\x11'
TYPE_LIGHTS = b'\x12'

UDP_HEADER = b'CIS'
UDP_LVL = b'\x01'
UDP_PASSWORD = b'\x00\x00\x00\x00\x00\x00'

MOTOR_STATUS = 0
THROTTLE = 0
STEERING = 0
LIGHTS = 0
LIGHTS_STATUS_UPDATED = False

udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def set_to_uint_offset(val):
    ret = int(32767 * val / 100)
    if ret > 32767:
        return 32767
    elif ret < -32767:
        return -32765
    else:
        return ret

def set_motor_status(status):
    global MOTOR_STATUS

    MOTOR_STATUS = status

def set_throttle(throttle):
    global THROTTLE

    THROTTLE = set_to_uint_offset(throttle)

def set_steering(steering):
    global STEERING
    STEERING = set_to_uint_offset(steering)

def set_lights_status(status):
    global LIGHTS
    global LIGHTS_STATUS_UPDATED
    LIGHTS = 65535 * status
    LIGHTS_STATUS_UPDATED = True

def send_frame(frame, ip, port):
    udp_sock.sendto(frame, (ip, port))

def frame_set_motor():
    frame = bytes(UDP_HEADER+UDP_LVL+UDP_PASSWORD+TYPE_ENGINE_ON+(b'\x01' if MOTOR_STATUS == 1 else b'\x00'))
    return frame

def frame_set_values():
    frame = bytes(UDP_HEADER+UDP_LVL+UDP_PASSWORD+TYPE_DRIVE+THROTTLE.to_bytes(2, byteorder='big', signed=True)+STEERING.to_bytes(2, byteorder='big', signed=True))
    return frame
    
def frame_set_lights():
    frame = bytes(UDP_HEADER+UDP_LVL+UDP_PASSWORD+TYPE_LIGHTS+LIGHTS.to_bytes(2, byteorder='big', signed=False))
    return frame

def sender(event, IP, PORT, DELAY):   
    global LIGHTS_STATUS_UPDATED

    while True:
        sleep(DELAY)
        send_frame(frame_set_values(), IP, PORT)
        send_frame(frame_set_motor(), IP, PORT)
        #send_frame(frame_set_lights(), IP, PORT)

        if event.is_set():
            break;




