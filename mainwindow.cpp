#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camera_calibration.h"
#include "projector_calibration.h"
#include "point_collect.h"
#include "point_show.h"
#include <QDebug>
#include <QDesktopWidget>
#include <qt_windows.h>

const int g_nBorder = 6;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    setMouseTracking(true);
//    setStyleSheet("QMainWindow{color:#E8E8E8;background-image: url(:/image/mainbackground.jpg);}");

    connect(ui->camerabt,SIGNAL(clicked(bool)),this,SLOT(login_camera_calibration()));
    connect(ui->projectorbt,SIGNAL(clicked(bool)),this,SLOT(login_projector_calibration()));
    connect(ui->pointshowbt,SIGNAL(clicked(bool)),this,SLOT(login_point_show()));
    connect(ui->pointcollectbt,SIGNAL(clicked(bool)),this,SLOT(login_point_collection()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::login_camera_calibration(){
    qDebug()<<"camera_calibration";
    camera_calibration *win = new camera_calibration;
    win->show();
    this->close();

}

void MainWindow::login_projector_calibration(){
    qDebug()<<"projector_calibration";
    projector_calibration *win = new projector_calibration;
    win->show();
    this->close();
}

void MainWindow::login_point_show(){
    qDebug()<<"point_show";
    point_show *win = new point_show;
    win->show();
    this->close();
}

void MainWindow::login_point_collection(){
    qDebug()<<"point_collection";
    point_collect *win = new point_collect;
    win->show();
    this->close();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
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

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

void MainWindow::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;
}

void MainWindow::on_btnMin_clicked()
{
    showMinimized();
}

void MainWindow::on_btnMax_clicked()
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

void MainWindow::on_btnExit_clicked()
{
    qApp->exit();
}





