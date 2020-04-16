#include "point_collect.h"
#include "ui_point_collect.h"

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
}

void point_collect::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
    win->show();
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


