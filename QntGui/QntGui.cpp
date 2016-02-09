
#include <QtWidgets/QApplication>
#include "QntGui.h"
#include "Canvan.h"
//#include "QntConfig.h"
//#include "ScBridge.h"

//#include "SC_LanguageClient.h"


#ifdef QNT_SCBRIDGE

#include "ScBridge.h"

#endif


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


	

#ifdef QNT_SCBRIDGE


	QStringList arguments(QApplication::arguments());
	arguments.pop_front(); // application path

	// Pass files to existing instance and quit

	QntInstanceGuard guard;
	if (guard.tryConnect(arguments))
		return 0;

	
	win.msgConsole(QString("QNT_SCBRIDGE exist"));
	//win.msgConsole(bridge.getClientName());

#endif

#ifndef QNT_SCBRIDGE

	win.msgConsole(QString("QNT_SCBRIDGE dnot exist")); 

#endif

	return app.exec();
}
