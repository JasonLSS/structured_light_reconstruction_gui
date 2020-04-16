#ifndef PROJECTOR_CALIBRATION_H
#define PROJECTOR_CALIBRATION_H
#include "mainwindow.h"

namespace Ui {
class projector_calibration;
}

class projector_calibration : public QMainWindow
{
    Q_OBJECT

public:
    explicit projector_calibration(QWidget *parent = nullptr);
    ~projector_calibration();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
    void on_btnMin_clicked();
    void on_btnMax_clicked();
    void on_btnExit_clicked();
    void login_mainwindow();

private:
    Ui::projector_calibration *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

};

#endif // PROJECTOR_CALIBRATION_H
