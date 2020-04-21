#include "projector_calibration.h"
#include "ui_projector_calibration.h"
#include "camera_calibration.h"
#pragma execution_character_set("utf-8")
const int g_nBorder = 6;

projector_calibration::projector_calibration(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::projector_calibration)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;

    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnMax->setIcon(QIcon(":/image/max1.png"));
    ui->btnExit->setIcon(QIcon(":/image/exit.png"));
    ui->btnBack->setIcon(QIcon(":/image/back.png"));

    setMouseTracking(true);
    setStyleSheet("QMainWindow{color:#E8E8E8;}");
    connect(ui->btnBack,SIGNAL(clicked(bool)),this,SLOT(login_mainwindow()));
    connect(ui->opencamerabt, SIGNAL(clicked()), this, SLOT(openCamara()));
    connect(ui->takephotobt, SIGNAL(clicked()), this, SLOT(takingPictures()));
    connect(ui->closecamerabt, SIGNAL(clicked()), this, SLOT(close_calibration()));
    connect(ui->clearbt, SIGNAL(clicked()), this, SLOT(clearFolder()));
    timer = new QTimer(this);
    ui->label->setText("摄像头关闭");
    num = 1;

}

QImage projector_calibration::Mat2QImage(cv::Mat cvImg)
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

void projector_calibration::openCamara()
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
//                image = Mat2QImage(frame);
//                ui->label->setPixmap(QPixmap::fromImage(image));
                timer->setInterval(60);   //set timer match with FPS
                connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));
                timer->start();
            }
        }
        else{
            QMessageBox::warning(this,"警告","无法打开摄像头，请检测设备连接");
        }
}

void projector_calibration::findChess()
{
    QString path = QApplication::applicationDirPath()+"/photo_projector/";
    QString count_path = QApplication::applicationDirPath()+"/photo_projector/count/";
    QDir dir(path);
    QDir dir_count(count_path);
    dir.setFilter(QDir::Files);
    int aqXnum = 9;
    int aqYnum = 6;
    cv::Size square_size = cv::Size(12, 12);
    cv::Size image_size;
    printf("Start scan corner\n");
    cv::Mat img;
    std::vector<cv::Point2f> image_points;
    std::vector<std::vector<cv::Point2f>> image_points_seq;
    for (int i = 0; i < dir.count(); i++){
        cv::Mat cb_source = cv::imread((path + dir[i]).toStdString());
        if (cv::findChessboardCorners(cb_source, cv::Size(aqXnum, aqYnum), image_points, 0) == 0) {
            printf("Error: Corners not find\n");
            continue;
        } else {
            cvtColor(cb_source, img, CV_RGBA2GRAY);
            image_size = cb_source.size();
            cv::cornerSubPix(img, image_points, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));

            image_points_seq.push_back(image_points);

            cv::Mat cb_corner;
            cb_corner = cb_source.clone();
            drawChessboardCorners(cb_corner, cv::Size(aqXnum, aqYnum), image_points, true);
            QString str;
            QString temp_path = count_path+str.setNum(i+1)+".png";
            qDebug()<<temp_path;

            if(!dir_count.exists()){
               dir_count.mkdir(count_path);
            }

            cv::imwrite(temp_path.toStdString(), cb_corner);
        }
    }
    std::cout << "Effective picture number:" << image_points_seq.size() << std::endl;

    QString st;
    st.setNum(image_points_seq.size());
    if(image_points_seq.empty()){
        QMessageBox::warning(this,"警告","标定失败，无有效图片");
        return;
    }
    QMessageBox::about(this,"提示","标定成功，有效图片数量:"+st+"张");

    std::vector<std::vector<cv::Point3f>> object_points;
    cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32FC1, cv::Scalar::all(0));
    cv::Mat distCoeffs = cv::Mat(1, 5, CV_32FC1, cv::Scalar::all(0));
    std::vector<cv::Mat> tvecsMat;
    std::vector<cv::Mat> rvecsMat;
    QDir num_dir(count_path);
    for (int s = 0;s < image_points_seq.size();s++){
        std::vector<cv::Point3f> realPoint;
        for (int i = 0; i < aqYnum; i++) {
            for (int j = 0; j < aqXnum; j++) {
                cv::Point3f tempPoint;

                tempPoint.x = i * square_size.width;
                tempPoint.y = j * square_size.height;
                tempPoint.z = 0;
                realPoint.push_back(tempPoint);
            }
        }
        object_points.push_back(realPoint);
    }

    cv::calibrateCamera(object_points, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat,
                        CALIB_FIX_K3);


    cv::Mat cb_source = cv::imread((path +dir[0]).toStdString());
    cv::Mat cb_final;
    cv::undistort(cb_source, cb_final, cameraMatrix, distCoeffs);
    QImage s = Mat2QImage(cb_final);
    ui->label->setPixmap(QPixmap::fromImage(s));
    writeProjectorCalibration(cameraMatrix,distCoeffs);
    std::cout << "cameraMatrix:\n" << cameraMatrix << std::endl;
    std::cout << "distCoeffs:\n" << distCoeffs << std::endl;
}

void projector_calibration::writeProjectorCalibration(cv::Mat cameraMatrix,cv::Mat distCoeffs){
    QString config_path = QApplication::applicationDirPath()+"/config/";
    QDir config_Dir(config_path);
    if(!config_Dir.exists()){
        config_Dir.mkdir(config_path);
    }
    FileStorage fs((config_path+"projector.xml").toStdString(), FileStorage::WRITE);
    fs << "cameraMatrix" << cameraMatrix;
    fs << "distCoeffs" << distCoeffs;
    fs.release();
}
camera_config projector_calibration::readCameraCalibration(){
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

void projector_calibration::readFarme()
{
    capture >> frame;
    if (!frame.empty())
    {
        cv::Mat cb_final;
        cv::undistort(frame, cb_final, camera_calibration_config.cameraMatrix, camera_calibration_config.distCoeffs);
        image = Mat2QImage(cb_final);
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
}

void projector_calibration::clearFolder()
{
    QMessageBox message(QMessageBox::Warning,"警告","是否清空标定文件夹？",QMessageBox::Yes|QMessageBox::No);
    if (message.exec()==QMessageBox::Yes)
    {
        QString path = QApplication::applicationDirPath()+"/photo_projector/";
        QDir dir(path);
        qDebug()<<"remove file:\n";
        for (int i = 0; i < dir.count(); i++){
            qDebug()<<dir[i];
            dir.remove(dir[i]);
        }
        QString count_path = QApplication::applicationDirPath()+"/photo_projector/count/";
        QDir count_dir(count_path);
        qDebug()<<"remove file:\n";
        for (int i = 0; i < count_dir.count(); i++){
            qDebug()<<count_dir[i];
            count_dir.remove(count_dir[i]);
        }
        num = 1;
    }
}

void projector_calibration::takingPictures()
{
    QString path = QApplication::applicationDirPath()+"/photo_projector/";
    QDir dir(path);
    if(!dir.exists()){
       dir.mkdir(path);
    }
    if (!frame.empty()){
        qDebug()<<QApplication::applicationDirPath()+"/photo_projector/" + QString::number(num) + ".jpg";
        cv::Mat cb_final;
        cv::undistort(frame, cb_final, camera_calibration_config.cameraMatrix, camera_calibration_config.distCoeffs);
        image = Mat2QImage(cb_final);
        if(image.save(QApplication::applicationDirPath()+"/photo_projector/" + QString::number(num) + ".jpg")){
            num += 1;
        }else{
            qDebug()<<"save failed";
        }
    }
    else{
         qDebug()<<"no camera";
         QMessageBox::warning(this,"警告","请先打开摄像头");
         ui->label->setText("摄像头关闭");
    }
}


void projector_calibration::closeCamara()
{
    capture.release();
    timer->stop();
    ui->label->setText("摄像头关闭");
    frame.release();

}

void projector_calibration::close_calibration()
{
    closeCamara();
    findChess();
}

projector_calibration::~projector_calibration()
{
    delete ui;
}

void projector_calibration::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
    closeCamara();
    win->show();
    this->close();
}

void projector_calibration::login_camera_calibration(){
    qDebug()<<"camera_calibration";
    camera_calibration *win = new camera_calibration;
    closeCamara();
    win->show();
    this->close();
}

bool projector_calibration::eventFilter(QObject *obj, QEvent *e)
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

bool projector_calibration::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

void projector_calibration::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void projector_calibration::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}


void projector_calibration::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;
}

void projector_calibration::on_btnMin_clicked()
{
    showMinimized();
}

void projector_calibration::on_btnMax_clicked()
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

void projector_calibration::on_btnExit_clicked()
{
    qApp->exit();
}
