import serial
from time import sleep
from struct import *
import pymysql

def change_device_active(short_address, ieee_address):
    connection = pymysql.connect(host='localhost',
                 user='leo',
                 password='golang*',
                 db='homeauto',
                 charset='utf8')

    try:
        with connection.cursor() as cursor:
            sql = "select device_id from device where device_ieee_address = %s"
            cursor.execute(sql, ieee_address)
            result = cursor.fetchone()
            if result != None
                print(result[0])
            #connection.commit()

    finally:
        connection.close()
    return

try:
    portx = "/dev/ttyUSB0"
    bps = 115200
    timex = 5
    ser = serial.Serial(portx, bps, timeout=timex)

    if (ser.is_open):
        print("is open")

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
                change_device_active(tup[1], tup[2])


except Exception as e:
    print("--ERROR--: ", e)
