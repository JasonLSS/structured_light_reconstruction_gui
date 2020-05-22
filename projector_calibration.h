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
    void login_camera_calibration();
    void openCamara();
    void readFarme();
    void closeCamara();
    void takingPictures();
    void clearFolder();
    void findChess();
    void writeProjectorCalibration(cv::Mat cameraMatrix,cv::Mat distCoeffs);
    void close_calibration();
    void fromCamToWorld(Mat cameraMatrix, vector<Mat> rV, vector<Mat> tV,
                        vector< vector<Point2f> > imgPoints, vector< vector<Point3f> > &worldPoints);
    camera_config readCameraCalibration();
    QImage Mat2QImage(cv::Mat cvImg);


private:
    Ui::projector_calibration *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

    cv::Mat frame;
    cv::VideoCapture capture;
    QImage image;
    QTimer *timer;
    double rate;
    cv::VideoWriter writer;
    double num;
    camera_config camera_calibration_config;
};

#endif // PROJECTOR_CALIBRATION_H
