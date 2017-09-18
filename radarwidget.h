#ifndef RADAR_WIDGET_
#define RADAR_WIDGET_

#include <QWidget>
#include <QMap>
#include <QPixmap>


/** Radar Widget. */
class RadarWidget: public QWidget { Q_OBJECT
 public:
  /** Constructor. */
  RadarWidget(QWidget *parent = 0);

  /** Set distance in meters. */
  void setDistance(qreal distance);

  /** Set radar blip position.
   * \param id    unique radar blip identifier
   * \param x     x coordinate from center to the North
   * \param y     y coordinate from center to the East
   * \param size  radar blip size in pixels
   */
  void setBlipXY(const QString &id, qreal x, qreal y, int size = 2);

  /** Set radar blip position.
   * \param id        unique radar blip identifier
   * \param azimuth   azimuth angle from North clockwise in degrees
   * \param distance  distance from center point in meters
   * \param size      radar blip size in pixels
   */
  void setBlipAzimuthDistance(const QString &id,
      qreal azimuth, qreal distance, int size = 2);

  /** Remove radar blip position.
   * \param id  unique radar blip identifier
   */
  void removeBlip(const QString &id);


 protected:
  void paintEvent(QPaintEvent *event) override;
  void timerEvent(QTimerEvent *event) override;


 private:
  void drawBackground(QPainter &painter);
  void drawSweep(QPainter &painter);
  void drawBlips(QPainter &painter);
  void drawForeground(QPainter &painter);


 private:
  struct Blip {
    qreal x, y;
    qreal r, a;
    int size;
  };
  qreal rMax_ = 50000.0;
  qreal rotAngle_ = 0.0;
  QMap<QString, Blip> blips_;
  QPixmap background_;
  int timerId_ = 0;
};


#endif  // RADAR_WIDGET_

