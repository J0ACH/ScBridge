#ifndef SCBRIDGETESTER_H
#define SCBRIDGETESTER_H

#include <QApplication>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

#include "ScBridge.h"
#include "ScServer.h"

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

	QPushButton *buttonInter = new QPushButton(win);
	buttonInter->setGeometry(710, 20, 80, 30);
	buttonInter->setText("kill");
	buttonInter->show();

	QObject::connect(sc, SIGNAL(print(QString)), console, SLOT(append(QString)));
	QObject::connect(buttonInter, SIGNAL(pressed()), sc, SLOT(kill()));
	QObject::connect(&app, SIGNAL(aboutToQuit()), sc, SLOT(kill()));
	
	//sc->begin();

	ScServer *server = new ScServer(win);
	QObject::connect(server, SIGNAL(print(QString)), console, SLOT(append(QString)));

	server->setPath("C:/Program Files/SuperCollider-3.9.0");
	server->begin();

	return app.exec();
}

#endif // ! SCBRIDGETESTER_H
