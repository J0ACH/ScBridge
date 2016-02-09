
#include <QtWidgets/QApplication>
#include "QntGui.h"
#include "Canvan.h"
//#include "QntConfig.h"
//#include "ScBridge.h"

#include "main.hpp"
#include "settings/manager.hpp"
#include "session_manager.hpp"
#include "util/standard_dirs.hpp"
#include "../widgets/main_window.hpp"
#include "../widgets/help_browser.hpp"
#include "../widgets/lookup_dialog.hpp"
#include "../widgets/code_editor/highlighter.hpp"
#include "../widgets/style/style.hpp"
#include "../../../QtCollider/hacks/hacks_mac.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QDataStream>
#include <QDir>
#include <QFileOpenEvent>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>


//using namespace Qnt;
//using namespace ScIDE;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	//app.setWindowIcon(QIcon("Qnt_AppIcon_16px.ico"));
	//app.setStyle(QStyleFactory::create("Fusion"));
	//app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

	Canvan win(100, 100, 800, 400);
	win.setTitle("QuantGui");
	//win.setVersion(Qnt_VERSION_MAJOR, Qnt_VERSION_MINOR, Qnt_VERSION_PATCH);
	win.show();

	

	//app.setPalette(darkPalette);
	win.msgConsole(QString("pre-bridge"));

	//ScBridge bridge;
	win.msgConsole(QString("bridge"));


	

	/*
	QStringList arguments(QApplication::arguments());
	arguments.pop_front(); // application path

	// Pass files to existing instance and quit
	ScIDE::SingleInstanceGuard guard;
	if (guard.tryConnect(arguments))
		return 0;

	*/
	
	win.msgConsole(QString("QNT_SCBRIDGE exist"));
	//win.msgConsole(bridge.getClientName());


	app.exec();
	return 0;
}
