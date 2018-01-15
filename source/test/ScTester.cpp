#ifndef SCBRIDGETESTER_H
#define SCBRIDGETESTER_H

#include <QApplication>
#include <QWidget>
#include "ScBridge.h"

using namespace ScBridge;

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	QWidget *win = new QWidget();

	win->setFixedSize(500, 200);
	win->show();

	return app.exec();
}

#endif // ! SCBRIDGETESTER_H
