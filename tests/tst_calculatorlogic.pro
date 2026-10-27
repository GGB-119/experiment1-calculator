QT += testlib core
QT -= gui

CONFIG += c++17 console testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_calculatorlogic

INCLUDEPATH += ..
SOURCES += \
    tst_calculatorlogic.cpp \
    ../calculatorlogic.cpp
HEADERS += ../calculatorlogic.h
