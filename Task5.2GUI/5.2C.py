import RPi.GPIO as GPIO
from tkinter import *

# Setup
GPIO.setmode(GPIO.BCM)
pins = [17, 27, 22]
pwms = []

for pin in pins:
    GPIO.setup(pin, GPIO.OUT)
    pwm = GPIO.PWM(pin, 100)
    pwm.start(0)
    pwms.append(pwm)

def set_brightness(led_index, value):
    pwms[led_index].ChangeDutyCycle(int(value))

# GUI Setup
root = Tk()
root.title("LED Brightness Control")

for i in range(3):
    Scale(root, from_=0, to=100, orient=HORIZONTAL,
          label=f"LED {i+1} Brightness",
          command=lambda val, idx=i: set_brightness(idx, val)).pack()

def on_close():
    for pwm in pwms:
        pwm.stop()
    GPIO.cleanup()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_close)
root.mainloop()
