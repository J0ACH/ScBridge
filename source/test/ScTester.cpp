
#include <QApplication>
#include "ScTester.h"

using namespace SC;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	PageServer *pServer = new PageServer();
	PageLang *pLang = new PageLang();

	QTabWidget *win = new QTabWidget();

	win->setGeometry(100, 100, 800, 550);
	win->addTab(pServer, "Server");
	win->addTab(pLang, "Interpreter");
	win->show();

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

	QObject::connect(serverRun, SIGNAL(released()), server, SLOT(reverse()));
	QObject::connect(cmdLine, SIGNAL(returnPressed()), this, SLOT(cmdLineEvaluated()));
	QObject::connect(server, SIGNAL(print(QString)), console, SLOT(append(QString)));
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

	QObject::connect(langRun, SIGNAL(released()), lang, SLOT(switchInterpretr()));
	QObject::connect(cmdLine, SIGNAL(returnPressed()), this, SLOT(cmdLineEvaluated()));
	QObject::connect(lang, SIGNAL(print(QString)), console, SLOT(append(QString)));
	QObject::connect(
		lang, SIGNAL(changeState(ScLang::InterpretState)),
		this, SLOT(langStatusChanged(ScLang::InterpretState))
	);
}

void PageLang::cmdLineEvaluated() {
	//console->append("PageLang::cmdLine EVALUATED");
	lang->evaluate(cmdLine->text());
}

void PageLang::langStatusChanged(ScLang::InterpretState state) {
	switch (state)
	{
	case ScLang::InterpretState::OFF:
		status->setText("Status: OFF");
		langRun->setChecked(false);
		break;
	case ScLang::InterpretState::BOOTING:
		status->setText("Status: BOOTING");
		break;
	case ScLang::InterpretState::ON:
		status->setText("Status: ON");
		langRun->setChecked(true);
		break;
	case ScLang::InterpretState::SHUTTING:
		status->setText("Status: SHUTTING");
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
}





