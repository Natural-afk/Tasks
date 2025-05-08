import tkinter as tk
import speech_recognition as sr
import threading

# Voice recognition setup
recognizer = sr.Recognizer()
mic = sr.Microphone()

# GUI setup
window = tk.Tk()
window.title("Smart Home Light Control")
window.geometry("300x200")

status_label = tk.Label(window, text="Light Status: OFF", font=("Arial", 16))
status_label.pack(pady=40)

def update_status(state):
    if state == "on":
        status_label.config(text="Light Status: ON", fg="green")
    elif state == "off":
        status_label.config(text="Light Status: OFF", fg="red")

def listen_for_command():
    with mic as source:
        recognizer.adjust_for_ambient_noise(source)
        print("Listening...")
        audio = recognizer.listen(source)

    try:
        command = recognizer.recognize_google(audio).lower()
        print("You said:", command)

        if "turn on the light" in command:
            update_status("on")
        elif "turn off the light" in command:
            update_status("off")
        else:
            print("Command not recognized.")

    except sr.UnknownValueError:
        print("Could not understand audio.")
    except sr.RequestError as e:
        print("Error from API:", e)

def start_listening():
    threading.Thread(target=listen_for_command).start()

# Button to trigger voice recognition
listen_button = tk.Button(window, text="Speak", command=start_listening, font=("Arial", 14))
listen_button.pack()

# Start GUI loop
window.mainloop()
