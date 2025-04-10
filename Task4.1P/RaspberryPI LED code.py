import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)       # Use physical pin numbering
GPIO.setup(19, GPIO.OUT)       # Set pin 19 (GPIO10) as output

print("Blinking LED 10 times...")

for i in range(10):            # Blink 10 times
    GPIO.output(19, GPIO.HIGH)
    time.sleep(0.3)
    GPIO.output(19, GPIO.LOW)
    time.sleep(0.3)

print("Done.")
GPIO.cleanup()                 # Clean up GPIO pin state