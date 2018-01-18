
#include <QApplication>
#include "ScTester.h"

using namespace SC;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	PageServer *pServer = new PageServer();
	PageLang *pLang = new PageLang();

	QTabWidget *win = new QTabWidget();

	//ScTester *win = new ScTester();
	win->setGeometry(100, 100, 800, 470);
	win->addTab(pServer, "Server");
	win->addTab(pLang, "Interpreter");
	win->show();
	/*
	ScBridge *lang = new ScBridge(win, "aaa");
	lang->setPath("C:/Program Files/SuperCollider-3.9.0");
	QObject::connect(&app, SIGNAL(aboutToQuit()), lang, SLOT(kill()));

	QTextEdit *console = new QTextEdit(win);
	console->setGeometry(20, 20, 680, 380);
	console->setFont(QFont("Consolas", 8));
	console->setReadOnly(true);
	console->show();
	QObject::connect(lang, SIGNAL(print(QString)), console, SLOT(append(QString)));

	QLineEdit *cmdLine = new QLineEdit(win);
	cmdLine->setGeometry(20, 420, 680, 30);
	cmdLine->show();
	QObject::connect(cmdLine, SIGNAL(returnPressed()), win, SLOT(cmdLineEvaluated()));

	QPushButton *buttonInter = new QPushButton(win);
	buttonInter->setGeometry(710, 20, 80, 30);
	buttonInter->setText("kill");
	buttonInter->show();


	//sc->begin();

	ScServer *server = new ScServer(win);
	QObject::connect(server, SIGNAL(print(QString)), console, SLOT(append(QString)));
	*/
	/*
	QObject::connect(
		cmdLine, &QLineEdit::returnPressed,
		cmdLineEnter
	);
	*/
	//QObject::connect(&app, SIGNAL(aboutToQuit()), server, SLOT(kill()));
	//QObject::connect(buttonInter, SIGNAL(pressed()), server, SLOT(kill()));

	//server->setPath("C:/Program Files/SuperCollider-3.9.0");
	//server->begin();
	//server->status();

	return app.exec();
}

PageServer::PageServer(QWidget *parent) : QWidget(parent) {
	QString scPath = "C:/Program Files/SuperCollider-3.9.0";

	server = new ScServer(this);

	groupSC = new QGroupBox("Supercollider", this);

	status = new QLabel(this);
	status->setText("Server");
	status->setGeometry(10, 10, 100, 30);
	status->show();
}

void PageServer::cmdLineEvaluated() {
	qDebug() << "PageServer::cmdLine EVALUATED";
}

PageLang::PageLang(QWidget *parent) : QWidget(parent) {
	QString scPath = "C:/Program Files/SuperCollider-3.9.0";

	lang = new ScLang(this);

	groupSC = new QGroupBox("Supercollider", this);

	status = new QLabel(this);
	status->setText("Interpretr");
	status->setGeometry(10, 10, 100, 30);
	status->show();
}

void PageLang::cmdLineEvaluated() {
	qDebug() << "PageLang::cmdLine EVALUATED";
}





