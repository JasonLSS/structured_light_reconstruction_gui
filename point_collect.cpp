#include "point_collect.h"
#include "ui_point_collect.h"
#include "camera_calibration.h"

#pragma execution_character_set("utf-8")
const int g_nBorder = 6;

point_collect::point_collect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::point_collect)
{
    ui->setupUi(this);
    //窗口无边框化
    setWindowFlags(Qt::FramelessWindowHint);
    //鼠标事件跟踪
    setMouseTracking(true);
    mLocation = this->geometry();
    mIsMax = false;
    //标题栏基本图标设置
    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnMax->setIcon(QIcon(":/image/max1.png"));
    ui->btnExit->setIcon(QIcon(":/image/exit.png"));
    ui->btnBack->setIcon(QIcon(":/image/back.png"));
//设置背景颜色
    setStyleSheet("QMainWindow{color:#E8E8E8;}");
    connect(ui->btnBack,SIGNAL(clicked()),this,SLOT(login_mainwindow()));
    connect(ui->start_scanbt,SIGNAL(clicked()),this,SLOT(startScaning()));

    timer = new QTimer(this);
    scantimer = new QTimer(this);
    num = 1;

    scanpic = Mat(800, 1200, CV_8UC3, Scalar(255, 255, 255)); //绘制
    rectangle(scanpic, cv::Rect(0, 0, 1200, 800), cv::Scalar(0, 255, 0), 4);
    putText(scanpic, "Put target in the rectangle", cv::Point(200, 400), cv::FONT_HERSHEY_SIMPLEX,2, cv::Scalar(0, 0, 255), 8);
    imshow("scan",scanpic);
    waitKey(1);

    openCamara();
}

QImage point_collect::Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)                             //3 channels color image
    {
        cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
        qImg =QImage((cvImg.data),
                    cvImg.cols, cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
    return qImg;
}

void point_collect::openCamara()
{
    camera_calibration_config = readCameraCalibration();
    if (camera_calibration_config.distCoeffs.empty()){
        login_camera_calibration();
        return;
    }
    if (capture.isOpened())
            capture.release();     //decide if capture is already opened; if so,close it
        capture.open(0);           //open the default camera
        if (capture.isOpened())
        {
            rate= capture.get(CAP_PROP_FPS);
            capture >> frame;
            if (!frame.empty())
            {
                image = Mat2QImage(frame);
                ui->camera_view->setPixmap(QPixmap::fromImage(image));
                timer->setInterval(60);   //set timer match with FPS
                connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));
                connect(timer, SIGNAL(timeout()), this, SLOT(collectFeature()));
                timer->start();
            }
        }
        else{
            QMessageBox::warning(this,"警告","无法打开摄像头，请检测设备连接");
        }
}

void point_collect::readFarme()
{
    capture >> frame;
    if (!frame.empty())
    {
        cv::Mat cb_final;
        cv::undistort(frame, cb_final, camera_calibration_config.cameraMatrix, camera_calibration_config.distCoeffs);
        image = Mat2QImage(cb_final);
        ui->camera_view->setPixmap(QPixmap::fromImage(image));
    }
}

void point_collect::collectFeature()
{
    cv::Mat temp;
    if (!frame.empty())
    {
        //显示灰度图
//        QString path = QApplication::applicationDirPath()+"/1.png";
//        g_srcImage = imread(path.toStdString());
//        cvtColor(g_srcImage, g_hsvImage, CV_RGB2HSV);
        cvtColor(frame, g_hsvImage, CV_RGB2HSV);
        imshow("HSV", g_hsvImage);

        inRange(g_hsvImage, Scalar(30, 40, 40), Scalar(80, 240, 240), imgHSVMask);
        imshow("mask", imgHSVMask);
        g_midImage = Mat::zeros(imgHSVMask.size(), CV_8UC1);  //绘制

        //去除小面积区域
        Delete_smallregions(imgHSVMask, g_midImage);
        imshow("target", g_midImage);

        //normalizeLetter显示效果图
        normalizeLetter(g_midImage,g_dstImage);
        imshow("effect", g_dstImage);

        //曲线映射到原图
//        Line_reflect(g_dstImage,g_midImage);
//        imshow("src", g_midImage);

        //转换类型，保存skeleton图像
        normalize(g_dstImage, g_midImage, 0, 255, NORM_MINMAX, CV_8U);

        waitKey(1);
        QImage s = Mat2QImage(g_midImage);
        ui->feature_view->setPixmap(QPixmap::fromImage(s));
//        StegerLine();
    }
}


void point_collect::StegerLine()
{
    Mat img0 = frame;
    Mat img;
    cvtColor(img0, img0, CV_BGR2GRAY);
    img = img0.clone();

    //高斯滤波
    img.convertTo(img, CV_32FC1);
    GaussianBlur(img, img, Size(0, 0), 6, 6);

    //一阶偏导数
    Mat m1, m2;
    m1 = (Mat_<float>(1, 2) << 1, -1);  //x偏导
    m2 = (Mat_<float>(2, 1) << 1, -1);  //y偏导

    Mat dx, dy;
    filter2D(img, dx, CV_32FC1, m1);
    filter2D(img, dy, CV_32FC1, m2);

    //二阶偏导数
    Mat m3, m4, m5;
    m3 = (Mat_<float>(1, 3) << 1, -2, 1);   //二阶x偏导
    m4 = (Mat_<float>(3, 1) << 1, -2, 1);   //二阶y偏导
    m5 = (Mat_<float>(2, 2) << 1, -1, -1, 1);   //二阶xy偏导

    Mat dxx, dyy, dxy;
    filter2D(img, dxx, CV_32FC1, m3);
    filter2D(img, dyy, CV_32FC1, m4);
    filter2D(img, dxy, CV_32FC1, m5);

    //hessian矩阵
    double maxD = -1;
    int imgcol = img.cols;
    int imgrow = img.rows;
    vector<double> Pt;
    for (int i=0;i<imgcol;i++)
    {
        for (int j=0;j<imgrow;j++)
        {
            if (img0.at<uchar>(j,i)>200)
            {
                Mat hessian(2, 2, CV_32FC1);
                hessian.at<float>(0, 0) = dxx.at<float>(j, i);
                hessian.at<float>(0, 1) = dxy.at<float>(j, i);
                hessian.at<float>(1, 0) = dxy.at<float>(j, i);
                hessian.at<float>(1, 1) = dyy.at<float>(j, i);

                Mat eValue;
                Mat eVectors;
                eigen(hessian, eValue, eVectors);

                double nx, ny;
                double fmaxD = 0;
                if (fabs(eValue.at<float>(0,0))>= fabs(eValue.at<float>(1,0)))  //求特征值最大时对应的特征向量
                {
                    nx = eVectors.at<float>(0, 0);
                    ny = eVectors.at<float>(0, 1);
                    fmaxD = eValue.at<float>(0, 0);
                }
                else
                {
                    nx = eVectors.at<float>(1, 0);
                    ny = eVectors.at<float>(1, 1);
                    fmaxD = eValue.at<float>(1, 0);
                }

                double t = -(nx*dx.at<float>(j, i) + ny*dy.at<float>(j, i)) / (nx*nx*dxx.at<float>(j,i)+2*nx*ny*dxy.at<float>(j,i)+ny*ny*dyy.at<float>(j,i));

                if (fabs(t*nx)<=0.5 && fabs(t*ny)<=0.5)
                {
                    Pt.push_back(i);
                    Pt.push_back(j);
                }
            }
        }
    }

    for (int k = 0;k<Pt.size()/2;k++)
    {
        Point rpt;
        rpt.x = Pt[2 * k + 0];
        rpt.y = Pt[2 * k + 1];
        circle(img, rpt, 1, Scalar(0, 0, 255));
    }

    imshow("result", img);
}

void point_collect::closeCamara()
{
    capture.release();
    timer->stop();
    scantimer->stop();
    ui->label->setText("摄像头关闭");
    frame.release();
}

camera_config point_collect::readCameraCalibration(){
    camera_config temp;
    QString config_path = QApplication::applicationDirPath()+"/config/";
    QDir config_Dir(config_path);
    if(!config_Dir.exists()){
        QMessageBox::warning(this,"警告","未找到相机标定文件");
        return temp;
    }
    QFileInfo fileInfo(config_path + "camera.xml");
    if(!fileInfo.isFile())
    {
        QMessageBox::warning(this,"警告","未找到相机标定文件");
        return temp;
    }
    FileStorage fs((config_path+"camera.xml").toStdString(), FileStorage::READ);
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    fs["cameraMatrix"] >> cameraMatrix;
    fs["distCoeffs"] >> distCoeffs;
    fs.release();
    temp.distCoeffs = distCoeffs;
    temp.cameraMatrix = cameraMatrix;
    return temp;
}


void point_collect::ThinSubiteration1(Mat & pSrc, Mat & pDst) {
    int rows = pSrc.rows;
    int cols = pSrc.cols;
    pSrc.copyTo(pDst);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(pSrc.at<float>(i, j) == 1.0f) {
                /// get 8 neighbors
                /// calculate C(p)
                int neighbor0 = (int) pSrc.at<float>( i-1, j-1);
                int neighbor1 = (int) pSrc.at<float>( i-1, j);
                int neighbor2 = (int) pSrc.at<float>( i-1, j+1);
                int neighbor3 = (int) pSrc.at<float>( i, j+1);
                int neighbor4 = (int) pSrc.at<float>( i+1, j+1);
                int neighbor5 = (int) pSrc.at<float>( i+1, j);
                int neighbor6 = (int) pSrc.at<float>( i+1, j-1);
                int neighbor7 = (int) pSrc.at<float>( i, j-1);
                int C = int(~neighbor1 & ( neighbor2 | neighbor3)) +
                                 int(~neighbor3 & ( neighbor4 | neighbor5)) +
                                 int(~neighbor5 & ( neighbor6 | neighbor7)) +
                                 int(~neighbor7 & ( neighbor0 | neighbor1));
                if(C == 1) {
                    /// calculate N
                    int N1 = int(neighbor0 | neighbor1) +
                                     int(neighbor2 | neighbor3) +
                                     int(neighbor4 | neighbor5) +
                                     int(neighbor6 | neighbor7);
                    int N2 = int(neighbor1 | neighbor2) +
                                     int(neighbor3 | neighbor4) +
                                     int(neighbor5 | neighbor6) +
                                     int(neighbor7 | neighbor0);
                    int N = min(N1,N2);
                    if ((N == 2) || (N == 3)) {
                        /// calculate criteria 3
                        int c3 = ( neighbor1 | neighbor2 | ~neighbor4) & neighbor3;
                        if(c3 == 0) {
                            pDst.at<float>( i, j) = 0.0f;
                        }
                    }
                }
            }
        }
    }
}


void point_collect::ThinSubiteration2(Mat & pSrc, Mat & pDst) {
    int rows = pSrc.rows;
    int cols = pSrc.cols;
    pSrc.copyTo( pDst);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if (pSrc.at<float>( i, j) == 1.0f) {
                    /// get 8 neighbors
                    /// calculate C(p)
                int neighbor0 = (int) pSrc.at<float>( i-1, j-1);
                int neighbor1 = (int) pSrc.at<float>( i-1, j);
                int neighbor2 = (int) pSrc.at<float>( i-1, j+1);
                int neighbor3 = (int) pSrc.at<float>( i, j+1);
                int neighbor4 = (int) pSrc.at<float>( i+1, j+1);
                int neighbor5 = (int) pSrc.at<float>( i+1, j);
                int neighbor6 = (int) pSrc.at<float>( i+1, j-1);
                int neighbor7 = (int) pSrc.at<float>( i, j-1);
                int C = int(~neighbor1 & ( neighbor2 | neighbor3)) +
                        int(~neighbor3 & ( neighbor4 | neighbor5)) +
                        int(~neighbor5 & ( neighbor6 | neighbor7)) +
                        int(~neighbor7 & ( neighbor0 | neighbor1));
                if(C == 1) {
                    /// calculate N
                    int N1 = int(neighbor0 | neighbor1) +
                            int(neighbor2 | neighbor3) +
                            int(neighbor4 | neighbor5) +
                            int(neighbor6 | neighbor7);
                    int N2 = int(neighbor1 | neighbor2) +
                            int(neighbor3 | neighbor4) +
                            int(neighbor5 | neighbor6) +
                            int(neighbor7 | neighbor0);
                    int N = min(N1,N2);
                    if((N == 2) || (N == 3)) {
                        int E = (neighbor5 | neighbor6 | ~neighbor0) & neighbor7;
                        if(E == 0) {
                            pDst.at<float>(i, j) = 0.0f;
                        }
                    }
                }
            }
        }
    }
}


void point_collect::normalizeLetter(Mat & inputarray, Mat & outputarray) {
        bool bDone = false;
        int rows = inputarray.rows;
        int cols = inputarray.cols;

        inputarray.convertTo(inputarray,CV_32FC1);

        inputarray.copyTo(outputarray);

        outputarray.convertTo(outputarray,CV_32FC1);

        /// pad source
        Mat p_enlarged_src = Mat(rows + 2, cols + 2, CV_32FC1);
        for(int i = 0; i < (rows+2); i++) {
            p_enlarged_src.at<float>(i, 0) = 0.0f;
            p_enlarged_src.at<float>( i, cols+1) = 0.0f;
        }
        for(int j = 0; j < (cols+2); j++) {
                p_enlarged_src.at<float>(0, j) = 0.0f;
                p_enlarged_src.at<float>(rows+1, j) = 0.0f;
        }
        for(int i = 0; i < rows; i++) {
                for(int j = 0; j < cols; j++) {
                        if (inputarray.at<float>(i, j) >= threshold_value) {			//调参
                                p_enlarged_src.at<float>( i+1, j+1) = 1.0f;
                        }
                        else
                                p_enlarged_src.at<float>( i+1, j+1) = 0.0f;
                }
        }

        /// start to thin
        Mat p_thinMat1 = Mat::zeros(rows + 2, cols + 2, CV_32FC1);
        Mat p_thinMat2 = Mat::zeros(rows + 2, cols + 2, CV_32FC1);
        Mat p_cmp = Mat::zeros(rows + 2, cols + 2, CV_8UC1);

        while (bDone != true) {
                /// sub-iteration 1
                ThinSubiteration1(p_enlarged_src, p_thinMat1);
                /// sub-iteration 2
                ThinSubiteration2(p_thinMat1, p_thinMat2);
                /// compare
                compare(p_enlarged_src, p_thinMat2, p_cmp, CV_CMP_EQ);	//比较输入的src1和src2中的元素，真为255，否则为0
                /// check
                int num_non_zero = countNonZero(p_cmp);					//返回灰度值不为0的像素数
                if(num_non_zero == (rows + 2) * (cols + 2)) {
                        bDone = true;
                }
                /// copy
                p_thinMat2.copyTo(p_enlarged_src);
        }
        // copy result
        for(int i = 0; i < rows; i++) {
                for(int j = 0; j < cols; j++) {
                        outputarray.at<float>( i, j) = p_enlarged_src.at<float>( i+1, j+1);
                }
        }
}

void point_collect::Line_reflect(Mat & inputarray, Mat & outputarray)
{
    int rows = inputarray.rows;
    int cols = inputarray.cols;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if (inputarray.at<float>(i, j) == 1.0f) {
                outputarray.at<float>( i, j) = 0.0f;
            }
        }
    }
}

// 提取连通区域，并剔除小面积联通区域
void point_collect::Delete_smallregions(Mat & pSrc, Mat & pDst)
{
    vector<vector<Point>> contours;           //二值图像轮廓的容器
    vector<Vec4i> hierarchy;                  //4个int向量，分别表示后、前、父、子的索引编号
    findContours(pSrc, contours, hierarchy,RETR_LIST, CHAIN_APPROX_NONE);             //检测所有轮廓

    vector<vector<Point>>::iterator k;                    //迭代器，访问容器数据

    for (k = contours.begin(); k != contours.end();)      //遍历容器,设置面积因子
    {
        if (contourArea(*k, false) < aerasize)
        {//删除指定元素，返回指向删除元素下一个元素位置的迭代器
            k = contours.erase(k);
        }
        else
            ++k;
    }

    //contours[i]代表第i个轮廓，contours[i].size()代表第i个轮廓上所有的像素点
    for (int i = 0; i < contours.size(); i++)
    {
        for (int j = 0; j < contours[i].size(); j++)
        {
            //获取轮廓上点的坐标
            Point P = Point(contours[i][j].x, contours[i][j].y);
        }
        drawContours(pDst, contours,i, Scalar(255), -1, 8);
    }
}

void point_collect::startScaning()
{
    scantimer->setInterval(60);   //set timer match with FPS
    connect(scantimer, SIGNAL(timeout()), this, SLOT(createScanPicture()));
    scantimer->start();
}

void point_collect::createScanPicture(){
    if (pic_num > 1200){
        scanpic = Mat(800, 1200, CV_8UC3, Scalar(255, 255, 255));
        rectangle(scanpic, cv::Rect(0, 0, 1200, 800), cv::Scalar(0, 255, 0), 4);
        putText(scanpic, "Complete!", cv::Point(200, 400), cv::FONT_HERSHEY_SIMPLEX,2, cv::Scalar(0, 0, 255), 8);
        if (pic_num > 1260){
            scantimer->stop();
            pic_num = 0;
            scanpic = Mat(800, 1200, CV_8UC3, Scalar(255, 255, 255));
            rectangle(scanpic, cv::Rect(0, 0, 1200, 800), cv::Scalar(0, 255, 0), 4);
            putText(scanpic, "Put target in the rectangle", cv::Point(200, 400), cv::FONT_HERSHEY_SIMPLEX,2, cv::Scalar(0, 0, 255), 8);
        }
    }
    else if (pic_num < 1201){
        scanpic = Mat(800, 1200, CV_8UC3, Scalar(0, 0, 0));
        rectangle(scanpic, cv::Rect(0, 0, 1200, 800), cv::Scalar(0, 0, 255), 4);
        line(scanpic,cv::Point(pic_num,0),cv::Point(pic_num,800),cv::Scalar(0, 255, 0),5);
    }
    ++pic_num;
    imshow("scan",scanpic);
}

void point_collect::login_camera_calibration(){
    qDebug()<<"camera_calibration";
    camera_calibration *win = new camera_calibration;
    closeCamara();
    win->show();
    destroyAllWindows();
    this->close();

}


void point_collect::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
    closeCamara();
    win->show();
    destroyAllWindows();
    this->close();
}

point_collect::~point_collect()
{
    delete ui;
}

//最小化
void point_collect::on_btnMin_clicked()
{
    showMinimized();
}
//最大化
void point_collect::on_btnMax_clicked()
{
    if (mIsMax)
    {
        setGeometry(mLocation);
        ui->btnMax->setIcon(QIcon(":/image/max1.png"));
        ui->btnMax->setToolTip(QStringLiteral("最大化"));
    }
    else
    {
        mLocation = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        ui->btnMax->setIcon(QIcon(":/image/max2.png"));
        ui->btnMax->setToolTip(QStringLiteral("还原"));
    }
    mIsMax = !mIsMax;
}
//关闭
void point_collect::on_btnExit_clicked()
{
    closeCamara();
    destroyAllWindows();
    this->close();
}


//双击放大缩小
bool point_collect::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widgetTitle)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            on_btnMax_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}
//拖拉缩放窗口
bool point_collect::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* pMsg = (MSG*)message;
    switch (pMsg->message)
    {
        case WM_NCHITTEST:
        {
            QPoint pos = mapFromGlobal(QPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)));
            bool bHorLeft = pos.x() < g_nBorder;
            bool bHorRight = pos.x() > width() - g_nBorder;
            bool bVertTop = pos.y() < g_nBorder;
            bool bVertBottom = pos.y() > height() - g_nBorder;
            if (bHorLeft && bVertTop)
            {
                *result = HTTOPLEFT;
            }
            else if (bHorLeft && bVertBottom)
            {
                *result = HTBOTTOMLEFT;
            }
            else if (bHorRight && bVertTop)
            {
                *result = HTTOPRIGHT;
            }
            else if (bHorRight && bVertBottom)
            {
                *result = HTBOTTOMRIGHT;
            }
            else if (bHorLeft)
            {
                *result = HTLEFT;
            }
            else if (bHorRight)
            {
                *result = HTRIGHT;
            }
            else if (bVertTop)
            {
                *result = HTTOP;
            }
            else if (bVertBottom)
            {
                *result = HTBOTTOM;
            }
            else
            {
                return false;
            }
            return true;
        }
    default:
        break;
    }
    return QWidget::nativeEvent(eventType, message, result);
}


void point_collect::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void point_collect::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
    QWidget::mouseMoveEvent(e);
}

void point_collect::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;

}


