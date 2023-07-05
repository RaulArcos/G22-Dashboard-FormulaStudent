from time import sleep
import can


class CanBus:

    def connect_can_bus(self, channel, bustype):
        try:
            self.bus_ = can.interface.Bus(channel, bustype)
            return True
        except OSError as e:
            print("No encontré el CAN_BUS, reintentando...")
            sleep(1)
            self.connect_can_bus(channel, bustype)

    def receive_can_bus_data(self):
        try:
            while True:
                message = self.bus_.recv()
        except KeyboardInterrupt:
            self.bus_.shutdown()