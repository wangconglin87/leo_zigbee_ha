from flask import Flask
import pymysql
import ha_util
import serial

app = Flask(__name__)

connection = pymysql.connect(host='localhost', user='leo', password='golang*', db='homeauto', charset='utf8')

@app.route('/')
def hello_world():
    return 'Hello World!'
@app.route('/check_all_devices')
def check_all_devices():
    row_count = ha_util.inactive_all_device(connection)

    portx = "/dev/ttyUSB0"
    bps = 115200
    timex = 5
    ser = serial.Serial(portx, bps, timeout=timex)

    if (ser.is_open):
        print("is open")

        ser.write(bytes([0]))
        ser.flush()

    ser.close()

    return 'Checking'

if __name__ == '__main__':
    app.run(host='0.0.0.0')
