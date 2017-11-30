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
    mainwindow.h \
    pde_solver_heat_equation.h \
    pde_solver_wave_equation.h \
    math_module.h \
    pde_solver_base.h
SOURCES += main.cpp \
    mainwindow.cpp \
    pde_settings.cpp \
    pde_solver_heat_equation.cpp \
    pde_solver_wave_equation.cpp \
    math_module.cpp \
    pde_solver_base.cpp
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../opt/cubature/build/release/ -lcubature
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../opt/cubature/build/debug/ -lcubature
else:unix: LIBS += -L$$PWD/../../../../../opt/cubature/build/ -lcubature

INCLUDEPATH += $$PWD/../../../../../opt/cubature
DEPENDPATH += $$PWD/../../../../../opt/cubature

DISTFILES += \
    ../TODO.txt
