TEMPLATE = app
TARGET = officelights
CONFIG += qt debug
QT += network core

QMAKE_CXXFLAGS += -std=c++11

DESTDIR = bin
OBJECTS_DIR = .obj
MOC_DIR = .moc

LIBS += -lhue -lxkey8 -lfastled -lpiehid -lssd1306

INCLUDEPATH = /usr/include/FastLED statemachines

SOURCES = \
	keystore.cpp \
	main.cpp \
	Program.cpp \
	ButtonManager.cpp \
	LEDManager.cpp \
	HueManager.cpp \
	Oled.cpp \
	statemachines/HueInitState.cpp \
	statemachines/HueEventState.cpp \
	statemachines/HueOffState.cpp \
	statemachines/HueOnState.cpp \
	statemachines/HueTransitionToOffState.cpp \
	statemachines/HueTransitionToOnState.cpp \
	statemachines/HueEventTimeout.cpp \
	ledprogs/twinkles.cpp \
	ledprogs/christmas.cpp \
	ledprogs/christmastree.cpp
		
HEADERS = \
	keystore.h \
	Program.h \
	ButtonManager.h \
	LEDManager.h \
	HueManager.h \
	Oled.h \
	statemachines/HueInitState.h \
	statemachines/HueEventState.h \
	statemachines/HueOffState.h \
	statemachines/HueOnState.h \
	statemachines/HueTransitionToOffState.h \
	statemachines/HueTransitionToOnState.h \
	statemachines/HueEventTimeout.h \
	ledprogs/twinkles.h \
	ledprogs/christmas.h \
	ledprogs/christmastree.h
		
