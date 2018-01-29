
#include <QApplication>
#include "ScTester.h"

using namespace SC;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	PageServer *pServer = new PageServer();
	PageLang *pLang = new PageLang();

	QTabWidget *win = new QTabWidget();

	win->setGeometry(100, 100, 800, 700);
	win->addTab(pServer, "Server");
	win->addTab(pLang, "Interpreter");
	win->show();

	return app.exec();
}

// Server //////////////////////////////////////////////

PageServer::PageServer(QWidget *parent) : QWidget(parent) {

	server = new ScServer(this);
	server->setPath("C:/Program Files/SuperCollider-3.9.0");

	groupSC = new QGroupBox("Supercollider", this);
	serverRun = new QCheckBox("Server", groupSC);
	statusState = new QLabel("Status: OFF", groupSC);
	statusInfo = new QLabel("Info: Nan", groupSC);
	boxPort = new QSpinBox(groupSC);
	boxPort->setRange(8000, 60000);
	boxPort->setValue(8080);

	groupConsole = new QGroupBox("Console", this);
	console = new QTextEdit(groupConsole);
	console->setFont(QFont("Consolas", 8));
	console->setReadOnly(true);

	groupCmd = new QGroupBox("Cmd", this);
	cmdLine = new QLineEdit(groupCmd);

	groupSynth = new QGroupBox("Synths", this);
	boxSynth = new QSpinBox(groupSynth);
	boxSynth->setRange(1001, 2000);
	boxSynth->setValue(1002);
	startSynth = new QPushButton("new", groupSynth);
	killSynth = new QPushButton("kill", groupSynth);


	QObject::connect(serverRun, SIGNAL(released()), server, SLOT(switchServer()));
	QObject::connect(this, SIGNAL(codeEvaluate(QString)), server, SLOT(evaluate(QString)));
	QObject::connect(boxPort, SIGNAL(valueChanged(int)), this, SLOT(portChanged()));
	QObject::connect(cmdLine, SIGNAL(returnPressed()), this, SLOT(cmdLineEvaluated()));

	QObject::connect(server, SIGNAL(print(QString)), console, SLOT(append(QString)));
	QObject::connect(
		server, SIGNAL(changeState(ScServer::ServerState)),
		this, SLOT(serverStatusChanged(ScServer::ServerState))
	);
	QObject::connect(
		server, SIGNAL(statusReplay(int, int, int, int, float, float)),
		this, SLOT(statusReplay(int, int, int, int, float, float))
	);

	QObject::connect(startSynth, SIGNAL(pressed()), this, SLOT(synthNew()));
	QObject::connect(killSynth, SIGNAL(pressed()), this, SLOT(nodeFree()));
	QObject::connect(this, SIGNAL(s_new(int)), server, SLOT(s_new(int)));
	QObject::connect(this, SIGNAL(n_free(int)), server, SLOT(n_free(int)));
}

void PageServer::portChanged() {
	//qDebug() << "PageServer::portChanged" << boxPort;
	console->append(tr("PageServer::portChanged %1").arg(QString::number(boxPort->value())));
	server->setPort(boxPort->value());
}

void PageServer::statusReplay(int ugenCount, int synthCount, int groupCount, int defCount, float avgCPU, float peakCPU) {
	statusInfo->setText(
		tr("Info: ugenCnt:%1, synthCnt:%2, groupCnt:%3, defCount:%4, avgCPU:%5, peakCPU:%6").arg(
			QString::number(ugenCount),
			QString::number(synthCount),
			QString::number(groupCount),
			QString::number(defCount),
			QString::number(avgCPU),
			QString::number(peakCPU)
		)
	);
}

void PageServer::synthNew() {
	//qDebug() << "PageServer::portChanged" << boxPort;
	console->append(tr("PageServer::synthNew %1").arg(QString::number(boxSynth->value())));
	server->s_new("default", boxSynth->value());	
}
void PageServer::nodeFree() {
	//qDebug() << "PageServer::portChanged" << boxPort;
	console->append(tr("PageServer::nodeFree %1").arg(QString::number(boxSynth->value())));
	server->n_free(boxSynth->value());
}

void PageServer::cmdLineEvaluated() {
	//qDebug() << "PageServer::cmdLine EVALUATED";
	emit codeEvaluate(cmdLine->text());
}

void PageServer::serverStatusChanged(ScServer::ServerState state) {
	switch (state)
	{
	case ScServer::ServerState::OFF:
		statusState->setText("Status: OFF");
		serverRun->setChecked(false);
		break;
	case ScServer::ServerState::BOOTING:
		statusState->setText("Status: BOOTING");
		break;
	case ScServer::ServerState::ON:
		statusState->setText("Status: ON");
		serverRun->setChecked(true);
		break;
	case ScServer::ServerState::SHUTTING:
		statusState->setText("Status: SHUTTING");
		break;
	}
}

void PageServer::resizeEvent(QResizeEvent *event) {
	QSize size = event->size();
	groupSC->setGeometry(10, 10, size.width() - 20, 100);
	serverRun->setGeometry(10, 10, groupSC->width() - 20, 30);
	statusState->setGeometry(150, 10, 100, 30);
	boxPort->setGeometry(10, 40, 50, 20);
	statusInfo->setGeometry(10, 60, 500, 20);

	groupConsole->setGeometry(10, 110, size.width() - 20, 300);
	console->setGeometry(10, 20, groupConsole->width() - 20, groupConsole->height() - 30);

	groupCmd->setGeometry(10, 410, size.width() - 20, 60);
	cmdLine->setGeometry(10, 20, groupCmd->width() - 20, groupCmd->height() - 30);

	groupSynth->setGeometry(10, 470, size.width() - 20, 200);
	boxSynth->setGeometry(10, 10, 60, 20);
	startSynth->setGeometry(80, 10, 40, 20);
	killSynth->setGeometry(125, 10, 40, 20);
}

// Interpretr //////////////////////////////////////////////

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
	QObject::connect(this, SIGNAL(codeEvaluate(QString)), lang, SLOT(evaluate(QString)));
	QObject::connect(cmdLine, SIGNAL(returnPressed()), this, SLOT(cmdLineEvaluated()));
	QObject::connect(lang, SIGNAL(print(QString)), console, SLOT(append(QString)));
	QObject::connect(
		lang, SIGNAL(changeState(ScLang::InterpretState)),
		this, SLOT(langStatusChanged(ScLang::InterpretState))
	);
}

void PageLang::cmdLineEvaluated() {
	//console->append("PageLang::cmdLine EVALUATED");
//	lang->evaluate(cmdLine->text());
	emit codeEvaluate(cmdLine->text());
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





