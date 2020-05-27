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
    void login_camera_calibration();
    bool openCamara();
    void readFarme();
    void closeCamara();
    void collectFeature();
    QImage Mat2QImage(cv::Mat cvImg);
    camera_config readCameraCalibration();
    void startScaning();
    void stopScaning();
    void createScanPicture();
    void StegerLine(Mat frame);
    void ThinSubiteration1(Mat & pSrc, Mat & pDst);
    void ThinSubiteration2(Mat & pSrc, Mat & pDst);
    void normalizeLetter(Mat & inputarray, Mat & outputarray);
    void Line_reflect(Mat & inputarray, Mat & outputarray);
    void Delete_smallregions(Mat & pSrc, Mat & pDst);
    void fromCamToWorld( vector<Point2f> imgPoints, vector<Point3f> &worldPoints);
    void point_change(vector<Point2f> &imgPoints,float first_point_x,vector<cv::Point3f> &worldPoints);
    vector<Point2f> max_point(Mat & pSrc, Mat & pDst);

private:
    Ui::point_collect *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

    cv::Mat frame;
    cv::Mat start_frame;
    cv::VideoCapture capture;
    QImage  image;
    QTimer *timer;
    QTimer *scantimer;
    double rate; //FPS
    cv::VideoWriter writer;   //make a video record
    double num;
    camera_config camera_calibration_config;

    Mat g_srcImage, g_hsvImage, g_dstImage, g_midImage, g_grayImage, imgHSVMask, imgThreshold;//原始图、中间图和效果图
    int threshold_value = 3;	//阈值
    int aerasize = 10;				//面积因子

    int pic_num = 0;
    Mat scanpic;

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<cv::Mat> tvecsMat;
    std::vector<cv::Mat> rvecsMat;
    bool start_scan_flag = false;
    vector<cv::Point3f> worldPoints;
    int count_start = 0;
};

#endif // POINT_COLLECT_H
