last = None

value = machine.GetSystemBus().ReadDoubleWord(0x40020C14)
state = (value >> 12) & 1

if state != last:
    print("PD12 LED:", "ON" if state else "OFF")
    last = state
