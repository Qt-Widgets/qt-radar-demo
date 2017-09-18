#include <QtWidgets>
#include "./radarwidget.h"


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  RadarWidget radarWidget;
  radarWidget.show();
  return app.exec();
}

