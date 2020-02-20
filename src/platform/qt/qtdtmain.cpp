#include <QApplication>
#include "qtdtmainwindow.h"
#include <QFontDatabase>
#include <QFont>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include "qt_worker.h"
#include "qt_os_gui.h"
#include "qt_os_io.h"

using namespace std;
using namespace ai;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qtdtapp);

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icon.png"));

	QFile cssFile(":/style/main.css");
	if (cssFile.open(QIODevice::ReadOnly))
	{
		a.setStyleSheet(cssFile.readAll());
		cssFile.close();
	}

	int id = QFontDatabase::addApplicationFont(":/fonts/Nunito/Nunito-SemiBold.ttf");
	a.setFont(QFont(QFontDatabase::applicationFontFamilies(id).at(0)));

	auto gui = make_shared<qt_os_gui>(nullptr);
	auto portal = Portal::create
	(
		make_shared<qt_worker>(), 
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString(), 
		24,
		make_shared<qt_os_io>(a), 
		gui,
		ai::ScreenType::DESKTOP
	);
	gui->portal_ = portal;

	int r = a.exec();
	return r;
}
