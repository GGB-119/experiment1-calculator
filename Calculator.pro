QT       += core gui widgets

CONFIG   += c++17
TARGET   = Calculator
TEMPLATE = app

SOURCES += \
    main.cpp \
    calculator.cpp \
    calculatorlogic.cpp

HEADERS += \
    calculator.h \
    calculatorlogic.h

FORMS += \
    calculator.ui

RESOURCES += \
    resources.qrc
