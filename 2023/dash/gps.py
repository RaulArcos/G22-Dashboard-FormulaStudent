import serial
import time
import pynmea2

class GPS:
    def __init__(self, port, excel_manager):
        self.port = port
        self.excel_manager = excel_manager

    def receive_gps_data(self):
        while True:
            ser = serial.Serial(self.port, baudrate=9200, timeout=1)
            data = ser.readline().decode('utf-8')
            if data.startswith('$GPRMC'):
                try:
                    msg = pynmea2.parse(data)
                    timestamp = int(time.perf_counter() * 1000)
                    speed = msg.spd_over_grnd * 1.852
                    self.excel_manager.save_gps_data(msg.latitude, msg.longitude, timestamp, speed)
                    print("Tiempo", timestamp, "ms", "Latitud ", msg.latitude, " Longitud ", msg.longitude,
                          " Speed ", speed, " km/h")
                except pynmea2.ParseError:
                    print("Error convirtiendo el serial a texto.")