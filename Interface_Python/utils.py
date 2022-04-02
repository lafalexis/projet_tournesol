import pandas as pd
import matplotlib.pyplot as plt
import struct
from tqdm import tqdm
from pytz import timezone

from PyQt5 import QtWidgets

import struct

# Some constant definitions : lists of measurements.
MEAS_AS7262 = ['450nm', '500nm', '550nm', '570nm', '600nm', '650nm']
MEAS_HDC1080 = ['temp', 'rh%']
MEAS_RTD = ['temp']
MEAS_ANEMOMETER = ['wind']
SENSORLIST = ['as7262', 'hdc1080', 'rtd', 'anemometer']
EXCELNAME = 'database.xlsx'

# Some useful definitions : bytes count for used data types.
BYTE_COUNT_FLOAT = 4
BYTE_COUNT_TIME = 8

class ReadBinaryData:
    def __init__(self, progressbar: QtWidgets.QProgressBar, frame_size, fn=r'D:\DATALOG.BIN'):

        self.bytes = None
        self.frame_list = None
        self.dfs = None
        self.frame_size = frame_size
        self.sample_count = 0
        self.progressbar = progressbar
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

        for i, frame in enumerate(self.frame_list):

            self.progressbar.setValue(int((i/self.sample_count)*100))
            # If the checksums don't match, reject the frame.
            if checksum(frame) is False:
                continue

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

        #df_light.index = pd.to_datetime(df_light.index, unit='s').tz_localize('canada/eastern')
        df_light.index = pd.to_datetime(df_light.index, unit='s').tz_localize(None)
        df_temp_rh.index = pd.to_datetime(df_temp_rh.index, unit='s').tz_localize(None)
        df_soil_temp.index = pd.to_datetime(df_soil_temp.index, unit='s').tz_localize(None)
        df_anemometer.index = pd.to_datetime(df_anemometer.index, unit='s').tz_localize(None)

        self.dfs = {'as7262': df_light,
                    'hdc1080': df_temp_rh,
                    'rtd': df_soil_temp,
                    'anemometer': df_anemometer}

class GenerateDerivativeData:
    def __init__(self, dfs):
        self.dfs = dfs
        self.gen_data()
        self.save_xlsx()

    def gen_data(self):
        self.estimate_light_intensity(self.dfs['as7262'])

    def estimate_light_intensity(self, light_spectral_data: pd.DataFrame):
        self.dfs['light_intensity'] = light_spectral_data.sum(axis=1)

    def estimate_photo_period(self, light_intensity_data):
        pass

    def estimate_air_thermo_period(self, air_temp_data):
        pass

    def save_xlsx(self):
        with pd.ExcelWriter(EXCELNAME, mode='w') as writer:
            for key, df in self.dfs.items():
                df.to_excel(writer, sheet_name=key, index_label='datetime')

class GenerateDerivativeData:
    def __init__(self, dfs):
        self.dfs = dfs
        self.gen_data()
        self.save_xlsx()

    def gen_data(self):
        self.estimate_light_intensity(self.dfs['as7262'])

    def estimate_light_intensity(self, light_spectral_data: pd.DataFrame):
        self.dfs['light_intensity'] = light_spectral_data.sum(axis=1)

    def estimate_photo_period(self, light_intensity_data):
        pass

    def estimate_air_thermo_period(self, air_temp_data):
        pass

    def save_xlsx(self):
        with pd.ExcelWriter('database.xlsx', mode='w') as writer:
            for key, df in self.dfs.items():
                df.to_excel(writer, sheet_name=key, index_label='datetime')


def checksum(frame: bytearray, checksum_bytes=2):
    frame = list(frame)
    xsum = struct.unpack('>H', bytes([frame.pop(-i) for i in range(checksum_bytes, 0, -1)]))[0]
    calculated_xsum = sum(frame)

    return calculated_xsum == xsum