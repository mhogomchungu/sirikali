TEMPLATE = app
CONFIG += console -std=c++14
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++14 -O2

SOURCES += main.cpp example.cpp

HEADERS += example.h task.h

