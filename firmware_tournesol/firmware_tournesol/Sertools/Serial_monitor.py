import serial

s = serial.Serial('COM4', baudrate=9600, bytesize=8, stopbits=serial.STOPBITS_ONE)
data = []
while 1:
    data.append(println := s.read())
    if b''.join([println]).decode('utf-8') == "\n":
        log_msg = b''.join(data).decode('utf-8')
        print(log_msg, end='')
        if 'calibration' in log_msg:
            with open(f'{log_msg[0:log_msg.find(" : ")].replace(" ", "_")}.txt', 'a') as f:
                f.write(log_msg[log_msg.find(" : ") + len(" : "):])
        data = []
s.close()
