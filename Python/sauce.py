from datetime import datetime
import time

if __name__ == '__main__':

    print(datetime.now())
    time_now = int(datetime.now().timestamp())
    print(time_now)
    #s = '2000-01-01 00:00:0'
    s = '2022-03-14 15:16:17'
    print(time.mktime(datetime.strptime(s, '%Y-%m-%d %H:%M:%S').timetuple()))

    #print(datetime.fromtimestamp(1647296402))