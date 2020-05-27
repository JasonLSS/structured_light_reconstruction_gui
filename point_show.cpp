#include "point_show.h"
#include "ui_point_show.h"

#pragma execution_character_set("utf-8")
const int g_nBorder = 6;

point_show::point_show(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::point_show)
{
    ui->setupUi(this);
    //窗口无边框化
    setWindowFlags(Qt::FramelessWindowHint);
    //鼠标事件跟踪
    setMouseTracking(true);
    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;
    //标题栏基本图标设置
    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnMax->setIcon(QIcon(":/image/max1.png"));
    ui->btnExit->setIcon(QIcon(":/image/exit.png"));
    ui->btnBack->setIcon(QIcon(":/image/back.png"));

//设置背景颜色
    setStyleSheet("QMainWindow{color:#E8E8E8;}");
    connect(ui->btnBack,SIGNAL(clicked()),this,SLOT(login_mainwindow()));
    m_pOpenglWidget = new MyQOpenglWidget(ui->openGLWidget);
    m_pOpenglWidget->resize(ui->openGLWidget->width(),ui->openGLWidget->height());
    connect(ui->openbt,SIGNAL(clicked()),this,SLOT(open_file()));
}

point_show::~point_show()
{
    delete ui;
}
//返回主页面
void point_show::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
    win->show();
    this->close();
}


void point_show::open_file()
{
    std::vector<QVector3D> cloud;

    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("打开文件"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("text(*.txt)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fileDialog->exec())
    {
    fileNames = fileDialog->selectedFiles();
    }

    cloud= ReadVec3PointCloud(fileNames[0]);
    m_pOpenglWidget->showPointCloud(cloud);

}

std::vector<QVector3D> point_show::ReadVec3PointCloud(QString path)
{
    std::vector<QVector3D> cloud;
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        qDebug() << "There is no file" ;
        return cloud;
    }
    QTextStream in(&file);
    QString ramData = in.readAll();
    QStringList list = ramData.split("\n");
    QStringList listline;
    cloud.resize(list.count()-1);
    for (int i = 0; i < list.count() - 1; i++)
    {
        listline = list.at(i).split(" ");
        if(listline.size()>=3)
        {
            cloud[i].setX((listline.at(0).toFloat()));
            cloud[i].setY((listline.at(1).toFloat()));
            cloud[i].setZ((listline.at(2).toFloat()));
        }
    }
    qDebug()<<cloud;
    return cloud;
}

//双击放大缩小
bool point_show::eventFilter(QObject *obj, QEvent *e)
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
bool point_show::nativeEvent(const QByteArray &eventType, void *message, long *result)
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


void point_show::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void point_show::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
    QWidget::mouseMoveEvent(e);
}

void point_show::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;

}


//最小化
void point_show::on_btnMin_clicked()
{
    showMinimized();
}
//最大化
void point_show::on_btnMax_clicked()
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
void point_show::on_btnExit_clicked()
{
    qApp->exit();
}

