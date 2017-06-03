TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11 -O2

SOURCES += main.cpp example.cpp

HEADERS += example.h task.h

