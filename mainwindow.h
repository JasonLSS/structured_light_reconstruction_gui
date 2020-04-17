#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <qt_windows.h>
#include <QDebug>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\imgproc\types_c.h>
#include "qlabel.h"
#include <QTimer>
#include <QDir>
#include <QMessageBox>

using namespace cv;
using namespace std;
struct camera_config{
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void login_camera_calibration();
    void login_projector_calibration();
    void login_point_collection();
    void login_point_show();

private:
    Ui::MainWindow *ui;

    //标题拖动、双击最大化
    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;
};

#endif // MAINWINDOW_H
