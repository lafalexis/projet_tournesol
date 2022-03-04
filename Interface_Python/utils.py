import pandas as pd
import struct

# Some constant definitions : lists of measurements.
MEAS_AS7262 = ['450nm', '500nm', '550nm', '570nm', '600nm', '650nm']
MEAS_HDC1080 = ['temp', 'rh%']
MEAS_RTD = ['temp']
MEAS_ANEMOMETER = ['wind']

# Some useful definitions : bytes count for used data types.
BYTE_COUNT_FLOAT = 4
BYTE_COUNT_TIME = 8


class ReadBinaryData:
    def __init__(self, frame_size, fn=r'D:\DATALOG.BIN'):
        self.bytes = None
        self.frame_list = None
        self.dfs = None
        self.frame_size = frame_size
        self.sample_count = 0
        self.import_data(fn)
        self.split_data()

    def import_data(self, fn):
        with open(fn, 'rb') as f:
            self.bytes = f.read()
            self.frame_list = [list(self.bytes[i:i + self.frame_size]) for i in range(0, len(self.bytes), self.frame_size)]
            self.sample_count = len(self.bytes) / self.frame_size

    def split_data(self):
        df_light = pd.DataFrame(columns=MEAS_AS7262)
        df_temp_rh = pd.DataFrame(columns=MEAS_HDC1080)
        df_soil_temp = pd.DataFrame(columns=MEAS_RTD)
        df_anemometer = pd.DataFrame(columns=MEAS_ANEMOMETER)

        for frame in self.frame_list:
            # The time is the first element.
            sample_time = struct.unpack('>Q', bytes([frame.pop(0) for _ in range(BYTE_COUNT_TIME)]))[0]
            # Then the spectral measurements.
            light_data = [struct.unpack('f', bytes([frame.pop(0) for _ in range(BYTE_COUNT_FLOAT)]))[0]
                          for _ in range(len(MEAS_AS7262))]
            df_light.loc[sample_time] = light_data
            # Then the air temperature and relative humidity.
            temp_rh_data = (struct.unpack('f', bytes([frame.pop(0) for _ in range(BYTE_COUNT_FLOAT)]))[0],
                            struct.unpack('f', bytes([frame.pop(0) for _ in range(BYTE_COUNT_FLOAT)]))[0])
            df_temp_rh.loc[sample_time] = temp_rh_data
            # Then the soil temperature.
            # Then the wind speed.

        self.dfs = {'as7262': df_light,
                    'hdc1080': df_temp_rh,
                    'rtd': df_soil_temp,
                    'anemometer': df_anemometer}