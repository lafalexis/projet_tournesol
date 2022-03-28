import serial

s = serial.Serial('COM3', baudrate=9600, bytesize=8, stopbits=serial.STOPBITS_ONE)
data = []
while 1:
    data.append(s.read())
    if len(data) > 0:
        print(b''.join(data).decode('utf-8'), end='')
        data = []
s.close()