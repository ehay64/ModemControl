
ARCH = hostx86_64

CC = gcc
CFLAGS = -std=c11 -I ./lite/lite-qmi/inc

CPP = g++
CPPFLAGS = -std=c++11 -I ./lite/lite-qmi/inc

INCLUDE = -L ./lite/lite-qmi/lib/$(ARCH)/ -l lite-qmi -l pthread

main:
	$(CPP) $(CPPFLAGS) ./src/modem/nas/nas.cpp ./src/modem/wds/wds.cpp ./src/modem/modem.cpp ./src/main.cpp -o ./bin/modem $(INCLUDE)
