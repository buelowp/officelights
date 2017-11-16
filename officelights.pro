TEMPLATE = app
TARGET = officelights
CONFIG += qt debug
QT += network core

QMAKE_CXXFLAGS += -std=c++11

DESTDIR = bin
OBJECTS_DI = .obj
MOC_DIR = .moc

LIBS += -lhue -lxkey8 -lfastled

INCLUDEPATH = /usr/include/FastLED

SOURCES = \
	keystore.cpp \
	main.cpp \
	Program.cpp \
	ButtonManager.cpp \
	LEDManager.cpp \
	HueManager.cpp
		
HEADERS = \
	keystore.h \
	Program.h \
	ButtonManager.h \
	LEDManager.h \
	HueManager.h
		
