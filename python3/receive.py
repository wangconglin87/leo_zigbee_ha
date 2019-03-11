import serial
from time import sleep
from struct import *
import pymysql
import ha_util

connection = pymysql.connect(host='localhost',
                         user='leo',
                         password='golang*',
                         db='homeauto',
                         charset='utf8')
try:
    portx = "/dev/ttyUSB0"
    bps = 115200
    timex = 5
    ser = serial.Serial(portx, bps, timeout=timex)


    while True:
        data = ser.read_all()
        if data == '' or data == b'':
            continue
        else:
            print("Receive: ", data)
            print("Length: ", len(data))

            if len(data) == 12:
                tup = unpack('<HHQ', data)
                print("Unpack: ", tup)
                print("Magic Number:", hex(tup[0]))
                print("Short Address: ", hex(tup[1]))
                print("IEEE Address: ", hex(tup[2]))
                #change_device_active(tup[1], tup[2])
                if tup[0] == 0x7C00:
                    ha_util.active_device(connection, tup[1], tup[2])

except Exception as e:
    print("--ERROR--: ", e)

finally:
    connection.close()
