
#include <QApplication>
#include "ScTester.h"

using namespace SC;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	PageServer *pServer = new PageServer();
	PageLang *pLang = new PageLang();

	QTabWidget *win = new QTabWidget();

	//ScTester *win = new ScTester();
	win->setGeometry(100, 100, 800, 550);
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

	server = new ScServer(this);
	server->setPath("C:/Program Files/SuperCollider-3.9.0");

	groupSC = new QGroupBox("Supercollider", this);
	serverRun = new QCheckBox("Server", groupSC);

	groupConsole = new QGroupBox("Console", this);
	console = new QTextEdit(groupConsole);
	console->setFont(QFont("Consolas", 8));
	console->setReadOnly(true);

	groupCmd = new QGroupBox("Cmd", this);
	cmdLine = new QLineEdit(groupCmd);

	QObject::connect(serverRun, SIGNAL(pressed()), server, SLOT(reverse()));
	QObject::connect(cmdLine, SIGNAL(returnPressed()), this, SLOT(cmdLineEvaluated()));
	QObject::connect(server, SIGNAL(print(QString)), console, SLOT(append(QString)));


	/*
	status = new QLabel(this);
	status->setText("Server");
	status->setGeometry(10, 10, 100, 30);
	status->show();
	*/
}

void PageServer::cmdLineEvaluated() {
	qDebug() << "PageServer::cmdLine EVALUATED";
}

void PageServer::resizeEvent(QResizeEvent *event) {
	QSize size = event->size();
	groupSC->setGeometry(10, 10, size.width() - 20, 100);
	serverRun->setGeometry(10, 10, groupSC->width() - 20, 30);

	groupConsole->setGeometry(10, 110, size.width() - 20, 300);
	console->setGeometry(10, 20, groupConsole->width() - 20, groupConsole->height() - 30);

	groupCmd->setGeometry(10, 410, size.width() - 20, 60);
	cmdLine->setGeometry(10, 20, groupCmd->width() - 20, groupCmd->height() - 30);

	//qDebug() << "PageServer::resizeEvent";
}

PageLang::PageLang(QWidget *parent) : QWidget(parent) {
	lang = new ScLang(this);
	lang->setPath("C:/Program Files/SuperCollider-3.9.0");

	groupSC = new QGroupBox("Supercollider", this);
	langRun = new QCheckBox("Interpretr", groupSC);
	status = new QLabel("Status: OFF", groupSC);

	groupConsole = new QGroupBox("Console", this);
	console = new QTextEdit(groupConsole);
	console->setFont(QFont("Consolas", 8));
	console->setReadOnly(true);

	groupCmd = new QGroupBox("Cmd", this);
	cmdLine = new QLineEdit(groupCmd);

	QObject::connect(langRun, SIGNAL(pressed()), lang, SLOT(reverse()));
	QObject::connect(cmdLine, SIGNAL(returnPressed()), this, SLOT(cmdLineEvaluated()));
	QObject::connect(lang, SIGNAL(print(QString)), console, SLOT(append(QString)));
	QObject::connect(
		lang, SIGNAL(changeState(ScLang::InterpretState)),
		this, SLOT(langStatusChanged(ScLang::InterpretState))
	);

	/*
	status = new QLabel(this);
	status->setText("Interpretr");
	status->setGeometry(10, 10, 100, 30);
	status->show();
	*/
}

void PageLang::cmdLineEvaluated() {
	qDebug() << "PageLang::cmdLine EVALUATED";
}

void PageLang::langStatusChanged(ScLang::InterpretState state) {
	switch (state)
	{
	case ScLang::InterpretState::OFF:
		status->setText("Status: OFF");
		langRun->setChecked(false);
		qDebug() << "PageLang::langStatusChanged: OFF";
		break;
	case ScLang::InterpretState::BOOTING:
		status->setText("Status: BOOTING");
		qDebug() << "PageLang::langStatusChanged: BOOTING";
		break;
	case ScLang::InterpretState::ON:
		status->setText("Status: ON");
		langRun->setChecked(true);
		qDebug() << "PageLang::langStatusChanged: ON";
		break;
	case ScLang::InterpretState::SHUTTING:
		status->setText("Status: SHUTTING");
		qDebug() << "PageLang::langStatusChanged: SHUTTING";
		break;
	}

}

void PageLang::resizeEvent(QResizeEvent *event) {
	QSize size = event->size();
	groupSC->setGeometry(10, 10, size.width() - 20, 100);
	langRun->setGeometry(10, 10, groupSC->width() - 20, 30);
	status->setGeometry(150, 10, 100, 30);

	groupConsole->setGeometry(10, 110, size.width() - 20, 300);
	console->setGeometry(10, 20, groupConsole->width() - 20, groupConsole->height() - 30);

	groupCmd->setGeometry(10, 410, size.width() - 20, 60);
	cmdLine->setGeometry(10, 20, groupCmd->width() - 20, groupCmd->height() - 30);

	//qDebug() << "PageServer::resizeEvent";
}





