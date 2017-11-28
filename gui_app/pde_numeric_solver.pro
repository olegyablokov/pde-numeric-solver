#TARGET = ../build/gui_app
TEMPLATE = app
QT += core widgets gui datavisualization script
DEFINES += QT_DEPRECATED_WARNINGS

DIR = ../build
NAME = gui_app
debug:CONFIGURATION = debug
else:CONFIGURATION = release

DESTDIR = $${DIR}/$${NAME}_$${CONFIGURATION}
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui

HEADERS += pde_settings.h \
    pde_solver.h \
    mainwindow.h
SOURCES += main.cpp \
    mainwindow.cpp \
    pde_solver.cpp \
    pde_settings.cpp
FORMS += mainwindow.ui

#_______________________________________
#DESTDIR = ../x64/Debug
#RESOURCES += mainwindow.qrc
#CONFIG += debug
#INCLUDEPATH += ./GeneratedFiles \
#    . \
#    ./GeneratedFiles/Debug
#DEPENDPATH += .
#MOC_DIR += ./GeneratedFiles/debug
#OBJECTS_DIR += debug
#UI_DIR += ./GeneratedFiles
#RCC_DIR += ./GeneratedFiles

#SUBDIRS += \
#    pde_numeric_solver.pro
