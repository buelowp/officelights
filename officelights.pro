TEMPLATE = app
TARGET = officelights
CONFIG += qt debug
QT += network core

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lhue -lxkey8

SOURCES = AppService.cpp \
		keystore.cpp \
		main.cpp \
		Program.cpp \
		qtservice_unix.cpp \
		qtservice.cpp \
		qtunixserversocket.cpp \
		qtunixsocket.cpp \
		ButtonManager.cpp \
		LEDManager.cpp \
		HueManager.cpp
		
HEADERS = AppService.h \
		keystore.h \
		Program.h \
		qtservice_p.h \
		qtservice.h \
		qtunixserversocket.h \
		qtunixsocket.h \
		ButtonManager.h \
		LEDManager.h \
		HueManager.h
		
