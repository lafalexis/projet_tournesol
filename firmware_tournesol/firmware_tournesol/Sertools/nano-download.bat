"%~dp0\bin\avrdude.exe" -C"%~dp0\bin\avrdude.conf" -v -patmega328p -carduino -P%1 -b57600 -D -Uflash:w:%2:i

start cmd /C python "%~dp0\Serial_monitor.py"
