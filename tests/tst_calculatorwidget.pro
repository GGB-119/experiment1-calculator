QT += testlib widgets

CONFIG += c++17 console testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_calculatorwidget

INCLUDEPATH += ..
SOURCES += \
    tst_calculatorwidget.cpp \
    ../calculator.cpp \
    ../calculatorlogic.cpp
HEADERS += \
    ../calculator.h \
    ../calculatorlogic.h
FORMS += ../calculator.ui
RESOURCES += ../resources.qrc
