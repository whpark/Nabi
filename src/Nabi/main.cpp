#include "pch.h"
#include "MainWnd.h"

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);

	a.setStyle("fusion");

	xMainWnd w;
	w.show();
	return a.exec();
}
