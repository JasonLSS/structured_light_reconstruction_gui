#ifndef POINT_SHOW_H
#define POINT_SHOW_H
#include "mainwindow.h"

namespace Ui {
class point_show;
}

class point_show : public QMainWindow
{
    Q_OBJECT

public:
    explicit point_show(QWidget *parent = nullptr);
    ~point_show();

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
    Ui::point_show *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

};

#endif // POINT_SHOW_H
