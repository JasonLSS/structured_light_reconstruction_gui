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
    void openCamara();
    void readFarme();
    void closeCamara();
    void collectFeature();
    QImage Mat2QImage(cv::Mat cvImg);
    camera_config readCameraCalibration();
    void startScaning();
    void createScanPicture();
    void StegerLine();
    void ThinSubiteration1(Mat & pSrc, Mat & pDst);
    void ThinSubiteration2(Mat & pSrc, Mat & pDst);
    void normalizeLetter(Mat & inputarray, Mat & outputarray);
    void Line_reflect(Mat & inputarray, Mat & outputarray);
    void Delete_smallregions(Mat & pSrc, Mat & pDst);

private:
    Ui::point_collect *ui;

    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

    cv::Mat frame;
    cv::VideoCapture capture;
    QImage  image;
    QTimer *timer;
    QTimer *scantimer;
    double rate; //FPS
    cv::VideoWriter writer;   //make a video record
    double num;
    camera_config camera_calibration_config;

    Mat g_srcImage, g_hsvImage, g_dstImage, g_midImage, g_grayImage, imgHSVMask;//原始图、中间图和效果图
    int threshold_value = 60;	//阈值
    int aerasize = 200;				//面积因子

    int pic_num = 0;
    Mat scanpic;
};

#endif // POINT_COLLECT_H
