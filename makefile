vdl: vdl.o logger.o serial.o nmea.o dlgps.o sensehat.o loggermqtt.o font.h
	g++ -g -o vdl vdl.o logger.o serial.o nmea.o dlgps.o sensehat.o loggermqtt.o -lpaho-mqtt3c -lm -lRTIMULib
vdl.o: vdl.cpp vdl.h logger.h serial.h nmea.h dlgps.h sensehat.h loggermqtt.h font.h
	g++ -g -c vdl.cpp
logger.o: logger.cpp logger.h serial.h nmea.h dlgps.h sensehat.h loggermqtt.h
	g++ -g -c logger.cpp
serial.o: serial.cpp serial.h
	g++ -g -c serial.cpp
dlgps.o: dlgps.cpp dlgps.h
	g++ -g -c dlgps.cpp
nmea.o: nmea.cpp nmea.h
	g++ -g -c nmea.cpp
sensehat.o: sensehat.cpp sensehat.h font.h
	g++ -g -c sensehat.cpp
loggermqtt.o: loggermqtt.cpp loggermqtt.h
	g++ -g -c loggermqtt.cpp
clean:
	touch *
	rm *.o
