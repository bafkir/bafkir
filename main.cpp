#include "Window.h"

#include <iostream>
#include <ctime>

#include <QApplication>
#include <QDateTime>

#include "OpenGLException.h"
#include "DebugApplication.h"

int main(int argc, char **argv) {
	DebugApplication app(argc, argv);

	qsrand(time(nullptr));

	Window dlg;
	dlg.show();
	return app.exec();
}
