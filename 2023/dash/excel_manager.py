import openpyxl

class ExcelManager:
    def __init__(self, excel_file_path):
            self.excel_file_path = excel_file_path

    def create_excel_file(self):
        wb = openpyxl.Workbook()
        sheet = wb.active
        sheet.append(["TimeStamp", "Latitude", "Longitude", "Speed (km/h)"])
        wb.save(self.excel_file_path)

    def save_gps_data(self, latitude, longitude, timestamp, speed):
        wb = openpyxl.load_workbook(self.excel_file_path)
        sheet = wb.active
        sheet.append([timestamp, latitude, longitude, speed])
        wb.save(self.excel_file_path)