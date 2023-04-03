import threading
from threading import Event
from time import sleep

from listener import *
from mdns_scan import *
from control import *
from sender import *

#Global variables
OVERRIDE_MDNS = False
DEST_IP = '192.168.24.120'
MULTICAST_IP = '239.255.0.1'
MULTICAST_PORT = 4211
DISP_DELAY = 0.001
TX_DELAY = 0.01
TX_PORT = 4210

#function called to list all the available robots and ask to choose one
def selectRobot():
    isValidInput = False
    nameList, addressList = GetClientLists()
    
    for i in range(len(nameList)):
        print(str(i) + ": " + nameList[i])

    print("q: Quit the program")

    while isValidInput == False:
        choice = input("Select your Merguez : ")

        if(choice == 'q'):
            return -1
        if(choice == '42'):
            print("Eh biloute! A le ch'tiot l'a pas les 42 carette ed Merguez")
        
        try:
            index = int(choice)
            if(index >= 0 and index <= len(nameList)):
                isValidInput = True
            else:
                print("NullPointerException, try again!")
        except ValueError:
            print("This is not a number")
    
    return addressList[index]         

#main function
def main():
    #start the keyboard interaction
    marcel_se_reveille()

    #get the IP of the car to control
    LaCharette = DEST_IP if OVERRIDE_MDNS else selectRobot() 
    if(LaCharette == -1):
        exit(0)

    #Instanciate and start all the threads
    stopEvent = Event()
    th_listener = threading.Thread(target=listener, args = (stopEvent, MULTICAST_IP, MULTICAST_PORT, LaCharette))
    th_sender = threading.Thread(target=sender, args = (stopEvent, LaCharette, TX_PORT, TX_DELAY))
    th_listener.start()
    th_sender.start()

    #While the user do not press the 'x' key, the program will keep listening
    while True:
        if(marcel_conduit() == 0):
            break
        displayer()
        sleep(DISP_DELAY)

    #Exiting the threeads
    stopEvent.set()
    th_listener.join()
    th_sender.join()

    #Bye-Bye! 
    exit(0)

if __name__ == '__main__':
    main()