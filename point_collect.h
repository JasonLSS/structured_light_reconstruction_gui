#ifndef POINT_COLLECT_H
#define POINT_COLLECT_H
#include "mainwindow.h"

namespace Ui {
class point_collect;
}

class point_collect : public QMainWindow
{
    Q_OBJECT

public:
    explicit point_collect(QWidget *parent = nullptr);
    ~point_collect();

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
    Ui::point_collect *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

};

#endif // POINT_COLLECT_H
