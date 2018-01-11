lessThan(QT_MAJOR_VERSION, 5): error("Qt5 or newer is required")
TEMPLATE = app
QT += core widgets gui datavisualization script
DEFINES += QT_DEPRECATED_WARNINGS

NAME = pde_solver_gui_app

CONFIG(release, debug|release) {
  CONFIGURATION = release
}
CONFIG(debug, debug|release) {
  CONFIGURATION = debug
}

OBJECTS_DIR = $${CONFIGURATION}/.obj
MOC_DIR = $${CONFIGURATION}/.moc
RCC_DIR = $${CONFIGURATION}/.rcc
UI_DIR = $${CONFIGURATION}/.ui
DESTDIR = $${CONFIGURATION}

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
