#include "projector_calibration.h"
#include "ui_projector_calibration.h"

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


}

projector_calibration::~projector_calibration()
{
    delete ui;
}

void projector_calibration::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
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
