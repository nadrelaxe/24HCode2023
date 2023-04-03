from msvcrt import getch, kbhit
import keyboard
import time

from sender import *

#Vega Missile control, attention, ça pousse!
 
class Marcel:

    coefRotate = 8
    coefPressed = 1
    coefRelease = 1

    max_accel = 30
    max_brake = -100
    max_turn = 100
    forward = 0
    angle = 0

    engineStatus = False
    light = False

    def __init__(self) -> None:
        pass

    def Avance(self):
        if self.forward < self.max_accel :
            self.forward = self.forward * 1.3 + self.coefPressed
            if self.forward >= self.max_accel : self.forward = self.max_accel

    def Recule(self):
        if self.forward > self.max_brake :
            self.forward = self.forward - self.coefPressed
            if self.forward <= self.max_brake : self.forward = self.max_brake

    def Tribord(self):
        if self.angle < self.max_turn :
            self.angle += self.coefRotate
            if self.angle >= self.max_turn : self.angle = self.max_turn

    def Babord(self):
        if self.angle > -self.max_turn :
            self.angle -= self.coefRotate
            if self.angle <= -self.max_turn : self.angle = -self.max_turn

    def LeveLePied(self):
        if self.forward > 50 :
            self.forward -= self.coefRelease
        elif self.forward < -50 :
            self.forward += self.coefRelease
        else:
            self.forward = 0

    def LacheLeVolant(self):
        self.angle = 0

    def EnRoute(self):
        self.engineStatus = True

    def StopeTout(self):
        self.engineStatus = False    

marcel = 0

def marcel_se_reveille():
    global marcel
    marcel = Marcel()

def marcel_gere_les_pedales():
    if keyboard.is_pressed("e"):
        marcel.EnRoute()

    if keyboard.is_pressed("o"):
        #launch control de tonton Farine
        marcel.forward = 100
        marcel.angle = 0
        marcel.EnRoute()
        return
        
    if keyboard.is_pressed("p"):
        marcel.forward = 0
        marcel.angle = 0
        marcel.StopeTout()
        return

    if keyboard.is_pressed("z"):
        marcel.Avance()    
    elif keyboard.is_pressed("s"):
        marcel.Recule()
    else:
        marcel.LeveLePied()

    if keyboard.is_pressed("q"):
        marcel.Babord()    
    elif keyboard.is_pressed("d"):
        marcel.Tribord()
    else:
        marcel.LacheLeVolant()
        
def marcel_conduit():
    marcel_gere_les_pedales()    
    
    if keyboard.is_pressed("l"):
        marcel.light = True
    elif keyboard.is_pressed("k"):
        marcel.light = False

    set_motor_status(marcel.engineStatus)
    set_throttle(marcel.forward)
    set_steering(marcel.angle)
    set_lights_status(marcel.light)

    if keyboard.is_pressed("x"):
        while kbhit(): getch()
        marcel.StopeTout()
        return 0
    else:
        return 1