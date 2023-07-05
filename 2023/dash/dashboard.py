from gps import GPS
from can_bus import CanBus
from excel_manager import ExcelManager
import datetime
import threading


if __name__ == "__main__":
    gps_port = "/dev/ttyS0"
    excel_file_path = "gps_logs/gps_data_" + datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + ".xlsx"
    excel_manager = ExcelManager(excel_file_path)
    excel_manager.create_excel_file()

    gps = GPS(gps_port, excel_manager)
    can = CanBus()

    gps_thread = threading.Thread(target=gps.receive_gps_data)
    gps_thread.start()
    
    can.connect_can_bus('can0', 'socketcan')
    can_thread = threading.Thread(target=can.receive_can_bus_data)

    