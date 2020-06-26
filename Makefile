
ARCH = arm

CPP = arm-linux-gnueabi-g++
CPPFLAGS = -static -mfloat-abi=softfp -std=c++11 -I ./lite/lite-qmi/inc

INCLUDE = -L ./lite/lite-qmi/lib/$(ARCH)/ -l lite-qmi -l pthread

main:
	$(CPP) $(CPPFLAGS) ./src/modem/nas/nas.cpp ./src/modem/wds/wds.cpp ./src/modem/modem.cpp ./src/main.cpp -o ./bin/modem $(INCLUDE)
