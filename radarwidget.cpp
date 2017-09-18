#include <QtWidgets>
#include "./radarwidget.h"


inline
double normalizeAngle(double x) {
  return x > 0? fmod(x, 360) : (360 - fmod(-x, 360));
}


// ---------------------------------------------------------------------------


RadarWidget::RadarWidget(QWidget *parent)
: QWidget(parent)
, background_(QPixmap("radar.png"))
{
  setMinimumSize(background_.size());
  setMaximumSize(background_.size());
  timerId_ = startTimer(27);
}


void RadarWidget::setBlipXY(const QString &id, qreal x, qreal y, int size)
{
  y = -y;

  qDebug() << x << y;

  auto &blip = blips_[id];  // do not repaint, wait for next frame
  blip.x = x;
  blip.y = y;
  blip.r = sqrt(x*x + y*y);
  blip.a = atan2(y, x)*180.0/M_PI;
  blip.size = size;
}


void RadarWidget::setBlipAzimuthDistance(const QString &id,
    qreal azimuth, qreal distance, int size)
{
  azimuth = (90 - azimuth)*M_PI/180;
  setBlipXY(id, distance*cos(azimuth), distance*sin(azimuth), size);
}


void RadarWidget::removeBlip(const QString &id)
{
  blips_.remove(id);  // do not repaint, wait for next frame
}


void RadarWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)

  QPainter painter(this);

  painter.drawPixmap(0, 0, background_);

  QRect rrr(QPoint(36, 36), QSize(440, 440));

  painter.save();

  painter.translate(rrr.center());
  painter.rotate(rotAngle_);
  painter.translate(-rrr.center());

  QConicalGradient grad1(rrr.center(), 0);

  QColor c0; c0.setHsv(58, 100, 200, 255);
  QColor c1; c1.setHsv(58, 100, 200, 127);
  QColor c2; c2.setHsv(58, 100,   0,   0);

  grad1.setColorAt(0.0, c1);
  grad1.setColorAt(1./360, c0);
  grad1.setColorAt(2./360, c0);
  grad1.setColorAt(5./360, c1);
  grad1.setColorAt(40./360, c2);

  painter.setPen(Qt::NoPen);
  painter.setBrush(grad1);
  painter.drawPie(rrr, 0, 40*16);

  painter.restore();

  painter.setPen(Qt::NoPen);
  const QPoint c(rrr.center());
  const QSize sz(rrr.size());
  const auto kx = 0.5*sz.width()/rMax_;
  const auto ky = 0.5*sz.height()/rMax_;
  for (const auto &i: blips_) {
    if (i.r > rMax_) continue;

    auto dA = normalizeAngle(rotAngle_ - normalizeAngle(i.a));
    QColor color;
    if (dA >= 0 && dA < 127) {
      color.setHsv(58, 100, 255, floor(255 - dA*2));
    } else {
      color.setRgb(0, 0, 0, 0);
    }
    painter.setBrush(color);

    const QSize ptsz(i.size, i.size);
    const double x = i.x*kx + c.x() - i.size/2;
    const double y = i.y*ky + c.y() - i.size/2;
    painter.drawEllipse(QRectF(QPointF(x, y), QSizeF(i.size, i.size)));
  }

  auto concentricRect = [c](int r) {
    QRect rect(QPoint(0, 0), QSize(2*r-1, 2*r-1));
    rect.moveCenter(c);
    return rect;
  };
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(Qt::NoBrush);
  painter.setPen(QPen(QColor("#dddddd"), 0));
  painter.setCompositionMode(QPainter::CompositionMode_Multiply);
  for (int i = 0; i < rrr.width()/6; ++i) {
    painter.drawEllipse(concentricRect(i*3));
  }
}


void RadarWidget::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == timerId_) {
    rotAngle_ = normalizeAngle(rotAngle_ + 1);
    repaint();
  }
}


#if 0
class DrawLib {
 public:
  void draw(QPainter &painter, const QRect &rect, bool x) {
    painter.save();
    painter.translate(rect.center());
    painter.rotate(45);
    painter.translate(-rect.center());
    QLinearGradient g(rect.left(),
        (rect.top() + rect.bottom())/2,
        rect.right(), 
        (rect.top() + rect.bottom())/2);
    g.setColorAt(x? 0.0 : 1.0, clight);
    g.setColorAt(x? 1.0 : 0.0, cshadow);
    painter.setBrush(g);
    painter.drawEllipse(rect);
    painter.restore();
  }

 private:
  QColor cshadow = QColor("#555753");
  QColor clight = QColor("#eeeeec");
  QColor cplain = QColor("#babdb6");
};


  if (0) {
    QColor cshadow("#555753"), clight("#eeeeec"), cplain("#babdb6");

    QLinearGradient g1(100, 100, 200, 200);
    g1.setColorAt(0.0, cshadow);
    g1.setColorAt(1.0, clight);
    QLinearGradient g2(110, 110, 190, 190);
    g2.setColorAt(1.0, cshadow);
    g2.setColorAt(0.0, clight);
    painter.setBrush(g1);
    painter.setPen(QPen(QColor("#2e3436"), 1));
    painter.drawEllipse(QRect(QPoint(100, 100), QPoint(200, 200)));
    painter.setPen(QPen(QColor("#babdb6"), 6));
    painter.setBrush(g2);
    painter.drawEllipse(QRect(QPoint(110, 110), QPoint(190, 190)));

    QRadialGradient g3(QPointF(150, 150), 30);
    for (int i = 0; i < 10; ++i) {
      g3.setColorAt(( i )*0.1, clight);
      g3.setColorAt((i+0.5)*0.1-0.1, cplain);
      g3.setColorAt((i+0.5)*0.1, clight);
    }
    painter.setBrush(g3);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRect(QPoint(115, 115), QPoint(185, 185)));

    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    QConicalGradient g4(QPoint(150, 150), 0);
    for (int i = 0; i < 10; ++i) {
      g4.setColorAt(( i )*0.1, QColor("#ffffff"));
      g4.setColorAt((i+0.5)*0.1, QColor("#cccccc"));
    }
    painter.setBrush(g4);
    painter.drawEllipse(QRect(QPoint(115, 115), QPoint(185, 185)));
  }


  const QPoint center(255, 255);
  auto concentricRect = [center](int r) {
    QRect rect(QPoint(0, 0), QSize(2*r-1, 2*r-1));
    rect.moveCenter(center);
    return rect;
  };

  DrawLib drawLib;
  painter.setPen(QPen(QColor("#000000"), 1));
  drawLib.draw(painter, concentricRect(250), 1);
  painter.setPen(QPen(QColor("#babdb6"), 10));
  drawLib.draw(painter, concentricRect(230), 0);

  QRadialGradient g(center, 220);
  g.setColorAt(0.0, QColor("#556b2f"));
  g.setColorAt(1.0, QColor("#495c28"));
  painter.setPen(QPen(QColor("#000000"), 1));
  painter.setBrush(g);
  painter.drawEllipse(concentricRect(220));  // ???

  //
  //
  //
  //
  //
  //

#endif

