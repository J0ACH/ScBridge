#ifndef SCTESTER_H
#define SCTESTER_H

#include <QDebug>
#include <QTabWidget>
#include <QWidget>
#include <QResizeEvent>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "ScServer.h"
#include "ScLang.h"

using namespace SC;

class PageServer : public QWidget {
	Q_OBJECT

public:
	PageServer(QWidget *parent = Q_NULLPTR);

	public slots:
	void cmdLineEvaluated();

signals:
	void codeEvaluate(QString);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	ScServer * server;
	QGroupBox *groupSC, *groupConsole, *groupCmd;
	QCheckBox *serverRun;
	QTextEdit *console;
	QLineEdit *cmdLine;
	//QLabel *status;
};

class PageLang : public QWidget {
	Q_OBJECT

public:
	PageLang(QWidget *parent = Q_NULLPTR);

	public slots:
	void cmdLineEvaluated();
	void langStatusChanged(ScLang::InterpretState);
	
signals:
	void codeEvaluate(QString);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	ScLang * lang;
	QGroupBox *groupSC, *groupConsole, *groupCmd;
	QCheckBox *langRun;
	QTextEdit *console;
	QLineEdit *cmdLine;
	QLabel *status;
};

#endif // ! SCTESTER_H