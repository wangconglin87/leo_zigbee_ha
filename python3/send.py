import serial
import serial.tools.list_ports

try:
    portx = "/dev/ttyUSB0"
    bps = 115200
    timex = 5
    ser = serial.Serial(portx, bps, timeout=timex)

    if (ser.is_open):
        print("is open")

        ser.write(bytes([0]))
        ser.flush()

    ser.close()


except Exception as e:
    print("--ERROR--: ", e)
