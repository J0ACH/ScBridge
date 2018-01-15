#ifndef SCBRIDGETESTER_H
#define SCBRIDGETESTER_H

#include <QApplication>
#include <QWidget>
#include <QTextEdit>
#include "ScBridge.h"

using namespace SC;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	QWidget *win = new QWidget();
	win->setFixedSize(800, 400);
	win->show();

	ScBridge *sc = new ScBridge(win, "aaa");

	QTextEdit *console = new  QTextEdit(win);
	console->setGeometry(20, 20, 680, 360);
	console->setFont(QFont("Consolas", 8));
	console->setReadOnly(true);
	console->show();

	QObject::connect(sc, SIGNAL(print(QString)), console, SLOT(append(QString)));
	
	sc->begin();
	return app.exec();
}

#endif // ! SCBRIDGETESTER_H
