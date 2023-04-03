from os import system
import socket
import struct
from time import sleep

DEF_VAL = -1
DEBUG_OUTPUT = False

#Global variables
STATUS_RSSI = DEF_VAL
STATUS_IR = DEF_VAL
STATUS_SIMU = DEF_VAL
STATUS_HL = DEF_VAL
STATUS_COLOR = DEF_VAL
STATUS_BATT = DEF_VAL
STATUS_IMU = DEF_VAL
STATUS_PILOT = DEF_VAL

#function to connect to the scoket
def connect_socket(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('', port))
    mreq = struct.pack("=4sl", socket.inet_aton(ip), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    return sock

#confirms if frames is correct
def is_frame_correct(frame):
    return frame[0:3] == b'CIS'
   
#function to display the data in human readable format
def sort_data(data):
    global STATUS_RSSI
    global STATUS_IR
    global STATUS_SIMU
    global STATUS_HL
    global STATUS_COLOR
    global STATUS_BATT
    global STATUS_IMU 
    global STATUS_PILOT

    if data[0] == 0x01:
        if(DEBUG_OUTPUT == True): print(data.hex())
        STATUS_RSSI = int.from_bytes(data[1:5], "little", signed=True) 
        STATUS_IR = data[6]
        STATUS_SIMU = 'x: ' + str(int.from_bytes(data[8:10], "little", signed=True)) + ' y: ' + str(int.from_bytes(data[10:12], "little", signed=True)) + ' t: ' + str(int.from_bytes(data[12:14], "little", signed=True))
        STATUS_HL = int.from_bytes(data[15:17], "little", signed=False)
    elif data[0] == 0x05:
        if(DEBUG_OUTPUT == True): print(data.hex())
        STATUS_COLOR = "{" + str(data[1]) + "," + str(data[2]) + "," + str(data[3]) + "},{" + str(data[4]) + "," + str(data[5]) + "," + str(data[6]) + "}"
        STATUS_BATT = "ADC : " + str(round(int.from_bytes(data[8:10], "little", signed=True) * 8.8 / 1000, 3)) + " SOC : " + str(round(int.from_bytes(data[10:12], "little", signed=True) / 128, 3))
        STATUS_IMU = "XL : {" + str(int.from_bytes(data[13:15], "little", signed=True)) + "," + str(int.from_bytes(data[15:17], "little", signed=True)) + "," + str(int.from_bytes(data[17:19], "little", signed=True)) + "} GY : {" + str(int.from_bytes(data[19:21], "little", signed=True)) + "," + str(int.from_bytes(data[21:23], "little", signed=True)) + "," + str(int.from_bytes(data[23:25], "little", signed=True)) + "}"
        STATUS_PILOT = "throt : " + str(int.from_bytes(data[26:28], "little", signed=True)) + ", steer : " + str(int.from_bytes(data[28:30], "little", signed=True)) + " and is " + ("OFF" if data[30] == 0 else "ON")
    else:
        print("NullPointerException")

#function to print the values of the car
def print_data():
    system('cls')
    print("RSSI = ", STATUS_RSSI, "dBm",
          "\t| IR = ", STATUS_IR,
          "\t| SIMU = ", STATUS_SIMU,
          "\t| LIGHTS = ", STATUS_HL,
          "\t| BAT = ", STATUS_BATT,
          "\t| IMU = ", STATUS_IMU,
          "\t| PILOT = ", STATUS_PILOT)

#function to display the received data
def display_frame(frame):
   if not is_frame_correct(frame):
        print("Frame is not expected")
   else:
        data = frame[3:]
        sort_data(data)        
        if(DEBUG_OUTPUT == True): print(data.hex())

#function called to iterate and keep listening on the bus
def listener(event, LISTEN_IP, LISTEN_PORT, ROBOT_IP):
    sock = connect_socket(LISTEN_IP, LISTEN_PORT)

    while True:
        data, address = sock.recvfrom(256)

        if ROBOT_IP in address :
            display_frame(data)
            if event.is_set():
                break

#function called to simply display the data
def displayer():
    print_data()