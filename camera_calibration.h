#ifndef CAMERA_CALIBRATION_H
#define CAMERA_CALIBRATION_H
#include "mainwindow.h"

namespace Ui {
class camera_calibration;
}


class camera_calibration : public QMainWindow
{
    Q_OBJECT

public:
    explicit camera_calibration(QWidget *parent = nullptr);
    ~camera_calibration();


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
    void openCamara();
    void readFarme();
    void closeCamara();
    void takingPictures();
    void clearFolder();
    void findChess();
    void writeCalibration(cv::Mat cameraMatrix,cv::Mat distCoeffs);
    void close_calibration();
    camera_config readCalibration();


private:
    Ui::camera_calibration *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

    cv::Mat frame;
    cv::VideoCapture capture;
    QImage  image;
    QTimer *timer;
    double rate; //FPS
    cv::VideoWriter writer;   //make a video record
    double num;




};

#endif // CAMERA_CALIBRATION_H
