import sounddevice as sd

def callback(indata, outdata, frames, time, status):
    outdata[:] = indata

with sd.Stream(callback=callback):
    print("Listening to audio (Press Enter to stop)...")
    input()
