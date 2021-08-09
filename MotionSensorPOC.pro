#------------------------------------------------------------------
#
# Testproject to check for OpenGL functionality in a Qt application
#
#------------------------------------------------------------------

QT       += core gui widgets

TARGET = MotionSensorPOC
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS GL_DEBUG

#OPTIONS += sanitize_checks
contains( OPTIONS, sanitize_checks ) {

	CONFIG(debug, debug|release) {
		CONFIG += sanitizer
		CONFIG += sanitize_address
		CONFIG += sanitize_undefined
	}

	linux-g++ | linux-g++-64 | macx {
		QMAKE_CXXFLAGS_DEBUG   *= -fsanitize=address -fno-omit-frame-pointer
	}
}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

win32 {
	LIBS += -lopengl32
}

SOURCES += \
                AxisObject.cpp \
		BoxMesh.cpp \
		GridObject.cpp \
		KeyboardMouseHandler.cpp \
		OpenGLException.cpp \
		OpenGLWindow.cpp \
                PlaneMesh.cpp \
		SceneView.cpp \
		ShaderProgram.cpp \
                Smartphone.cpp \
		TextObject.cpp \
		Transform3D.cpp \
    Window.cpp \
                main.cpp

HEADERS += \
        AxisObject.h \
	BoxMesh.h \
	Camera.h \
	DebugApplication.h \
	GridObject.h \
	KeyboardMouseHandler.h \
	OpenGLException.h \
	OpenGLWindow.h \
        PlaneMesh.h \
	SceneView.h \
	ShaderProgram.h \
        Smartphone.h \
	TextObject.h \
	Transform3D.h \
	Vertex.h \
    Window.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	resources.qrc
