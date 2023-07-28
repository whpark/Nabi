#include "pch.h"
#include "MainWnd.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	xMainWnd w;
	w.show();
	return a.exec();
}
