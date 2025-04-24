import tkinter as tk
import RPi.GPIO as GPIO

# -------------------------------
# GPIO Setup
# -------------------------------
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# LED Pin Assignments
led_pins = {
    'Red': 17,
    'Green': 27,
    'Blue': 22
}

# Setup all pins as OUTPUT and set LOW
for pin in led_pins.values():
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

# -------------------------------
# Functions
# -------------------------------

# Turn on selected LED and turn off others
def turn_on_led():
    selected = led_var.get()
    for color, pin in led_pins.items():
        GPIO.output(pin, GPIO.HIGH if color == selected else GPIO.LOW)

# Exit GUI and cleanup GPIO
def exit_program():
    for pin in led_pins.values():
        GPIO.output(pin, GPIO.LOW)
    GPIO.cleanup()
    root.destroy()

# -------------------------------
# GUI Setup (Tkinter)
# -------------------------------
root = tk.Tk()
root.title("RPi LED Controller")
root.geometry("300x150")

# Tkinter variable to hold selected color
led_var = tk.StringVar(value='Red')

# Create radio buttons for LED selection
for color in led_pins:
    tk.Radiobutton(root, text=color, variable=led_var, value=color,
                   command=turn_on_led).pack(anchor='w', padx=20, pady=2)

# Exit button
tk.Button(root, text="Exit", command=exit_program, bg='gray', fg='white').pack(pady=10)

# Start GUI loop
root.mainloop()
