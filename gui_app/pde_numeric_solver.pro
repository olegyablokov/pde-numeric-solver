TEMPLATE = app
QT += core widgets gui datavisualization script
DEFINES += QT_DEPRECATED_WARNINGS

DIR = ../build
NAME = gui_app

CONFIG(release, debug|release) {
  CONFIGURATION = release
}
CONFIG(debug, debug|release) {
  CONFIGURATION = debug
}

DESTDIR = $${DIR}/$${NAME}_$${CONFIGURATION}
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui

HEADERS += mainwindow.h \
	../pde_solver/pde_solver_heat_equation.h \
	../pde_solver/pde_solver_wave_equation.h \
	../math_module/math_module.h \
	../pde_solver/pde_solver_base.h \
	../pde_solver/pde_settings.h \
    ../pde_solver/pde_solver_structs.h
SOURCES += main.cpp \
    mainwindow.cpp \
	../pde_solver/pde_settings.cpp \
	../pde_solver/pde_solver_heat_equation.cpp \
	../pde_solver/pde_solver_wave_equation.cpp \
	../pde_solver/pde_solver_base.cpp \
	../math_module/math_module.cpp
FORMS += mainwindow.ui

DISTFILES += \
    ../TODO.txt

RESOURCES += \
    mainwindow.qrc
