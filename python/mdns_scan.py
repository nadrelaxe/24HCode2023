from zeroconf import ServiceBrowser, ServiceListener, Zeroconf
import time
from typing import Set
import ipaddress
import socket

class MyListener(ServiceListener):

    def __init__(self) -> None:
        """Keep track of found services in a set."""
        self.found_services_name: Set[str] = set()
        self.found_services_info = []
    
    def update_service(self, zc: Zeroconf, type_: str, name: str) -> None:
        #print(f"Service {name} updated")
        """Keep track of found services in a set."""

    def remove_service(self, zc: Zeroconf, type_: str, name: str) -> None:
        #print(f"Service {name} removed")
        """Service removed."""       

    def add_service(self, zc: Zeroconf, type_: str, name: str) -> None:
        #print(f"Service {name} added, service info: {info}")
        """Service added."""
        self.found_services_name.add(name)
        info = zc.get_service_info(type_, name)
        self.found_services_info.append(info)

def GetClientLists():
    
    timeout = 5
    zeroconf = Zeroconf()
    listener = MyListener()
    browser = ServiceBrowser(zeroconf, "_arduino._tcp.local.", listener)

    # wait for responses
    time.sleep(timeout)

    browser.cancel()

    # close down anything we opened
    if zeroconf is None:
        zeroconf.close()

    #Adapt the format of the result
    devicesFound = tuple(listener.found_services_info)

    addressList = []
    nameList = []

    for device in devicesFound:
        if device.name.find("Car") != -1:
            addressList.append(socket.inet_ntoa(device.addresses[0]))
            nameList.append(device.name.split('.')[0])

    return nameList,addressList
