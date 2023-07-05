import serial
import time
import pynmea2
import threading
import openpyxl
import datetime

def create_exel_file(excel_file_path):
    wb = openpyxl.Workbook()
    sheet = wb.active
    sheet.append(["TimeStamp", "Latitude", "Longitude", "Speed (km/h)"])
    wb.save(excel_file_path)

def save_gps_data(latitude, longitude, timestamp, speed ,excel_file_path):
    wb = openpyxl.load_workbook(excel_file_path)
    sheet = wb.active
    sheet.append([timestamp, latitude, longitude, speed])
    wb.save(excel_file_path)

def recieve_gps_data(port, excel_file_path):
    while(True):
        ser=serial.Serial(port,baudrate=9200,timeout=1)
        data = ser.readline().decode('utf-8')
        if data.startswith('$GPRMC'):
            try:
                msg = pynmea2.parse(data)
                save_gps_data(msg.latitude, msg.longitude, int(time.perf_counter()*1000), (msg.spd_over_grnd * 1.852), excel_file_path)
                print("Tiempo", int(time.perf_counter()*1000), "ms", "Latitud ", msg.latitude, " Longitud ", msg.longitude, " Speed ", (msg.spd_over_grnd * 1.852), " km/h")
            except pynmea2.ParseError:
                # Handle parse errors, if any
                print("Error convirtiendo el serial a texto.")

def recieve_can_bus_data():
    print()

if __name__ == "__main__":

    gps_port = "/dev/ttyS0"

    excel_file_path = "gps_data_" + datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + ".xlsx"
    create_exel_file(excel_file_path)

    gps_thread = threading.Thread(target=recieve_gps_data, args=(gps_port, excel_file_path))
    can_thread = threading.Thread(target=recieve_can_bus_data, args=())
    gps_thread.start()

    
