#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Project/tabproject.h"
#include "resourceloader.h"
#include "welcomedialog.h"
#include <QtWidgets>
#include <QDebug>
#include <QValidator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->widgetPlayer, &QTabLineGLWidget::signalTabLineChanged, this, &MainWindow::onPlayerTabLineChanged);
    setWindowIcon(QIcon(":/image/resource/logo.ico"));
    setAcceptDrops(true);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);


    setWindowTitle(tr("Player"));

    QStringList nums;
    nums << "Auto" << "100" << "200" << "300";
    ui->comboBoxLineHeight->addItems(nums);
    connect(ui->comboBoxLineHeight, &QLineHeightComboBox::signalLineHeightChanged, [&](int h)
    {
        cfg.fixedHeight = h;
        g_userCfg->setUserTpfConfig(cfg);
        ui->widgetPlayer->slogOnTabLineHeightChanged(h);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    qDebug() << TAB_INST->getSingerName();

    cfg.tpf = TAB_INST->currentProject();
    if (g_userCfg->getUserTpfConfig(cfg))
    {
        TAB_INST->adjustSpeed(cfg.adjustedBpm);
        ui->comboBoxLineHeight->setHeight(cfg.fixedHeight);
    }


    ui->widgetPlayer->reset();
    ui->widgetPlayer->init();

    ui->progress->setRange(0, TAB_INST->getTabLineCount()-1);
    ui->progress->setValue(0);

    setWindowTitle(QFileUtil::getFileName(TAB_INST->currentProject()));
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    event->accept();
    int delta = event->delta();
    bool bCtrlPressed = QGuiApplication::keyboardModifiers() == Qt::ControlModifier;
    if (bCtrlPressed)
    {
        ui->comboBoxLineHeight->setHeight(cfg.fixedHeight + ((delta > 0) ? 10 : -10));
    }
    else
    {
        if (delta > 0)
            on_btnLeft_clicked();
        else
            on_btnRight_clicked();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QImagePtr pShadow = g_resLoader.getImage(RID_IMG_SHADOW);
    if (pShadow)
    {
        QPainter painter(this);
        QRect rc = rect();
        QPainterUtil::paintScale9(&painter, *pShadow, rc, 8);
    }
}

void MainWindow::setWindowTitle(QString strTitle)
{
    QMainWindow::setWindowTitle(strTitle);
    ui->labelTitle->setText(strTitle);
}

bool MainWindow::openProject(QString strProj)
{
    if (TAB_INST->openProject(strProj))
    {
        initUI();
        return true;
    }
    else
    {
        QMessageBox box(this);
        box.setIcon(QMessageBox::Critical);
        box.setText(tr("Cannot open project!"));
        box.exec();

        cfg = UserTpfConfig();
        initUI();
    }

    return false;
}

void MainWindow::on_actionOpen_triggered()
{
    //QString strProj = QFileDialog::getOpenFileName(this,
    //    tr("Open Project"), QDir::currentPath(), tr("Tab Project Files (*.tpf)"));
    WelcomeDialog dlg(this);
    dlg.exec();

    QString strProj = dlg.getSelectedProjFile();
    if (strProj.isEmpty())
        return;

    openProject(strProj);
}

void MainWindow::on_btnPlay_clicked(bool checked)
{
    Q_UNUSED(checked);
    if (TAB_INST->currentProject().isEmpty())
        return;

    if (ui->widgetPlayer->getPlayStatus() == PS_PLAYING)
    {
        ui->widgetPlayer->pause();
        ui->btnPlay->setText(tr("Play"));
    }
    else
    {
        ui->btnPlay->setText(tr("Pause"));
        ui->widgetPlayer->play();
    }
}

void MainWindow::on_progress_valueChanged(int value)
{
    ui->widgetPlayer->setPlayingTabLine(value);
}

void MainWindow::on_btnLeft_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

    ui->widgetPlayer->playPrev();
}

void MainWindow::on_btnRight_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

    ui->widgetPlayer->playNext();
}

void MainWindow::on_btnSlowDown_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

    int bpm = TAB_INST->adjustSpeed(cfg.adjustedBpm-1);
    if (cfg.adjustedBpm != bpm)
    {
        cfg.adjustedBpm = bpm;
        g_userCfg->setUserTpfConfig(cfg);
    }

    ui->widgetPlayer->slowDown();
}

void MainWindow::on_btnSpeedUp_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

    int bpm = TAB_INST->adjustSpeed(cfg.adjustedBpm+1);
    if (cfg.adjustedBpm != bpm)
    {
        cfg.adjustedBpm = bpm;
        g_userCfg->setUserTpfConfig(cfg);
    }

    ui->widgetPlayer->speedUp();
}

void MainWindow::on_btnResetSpeed_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

    TAB_INST->adjustSpeed(0);
    if (cfg.adjustedBpm != 0)
    {
        cfg.adjustedBpm = 0;
        g_userCfg->setUserTpfConfig(cfg);
    }

    ui->widgetPlayer->resetSpeed();
}

void MainWindow::onPlayerTabLineChanged(int iTabLine)
{
    //finshed
    if (-1 == iTabLine)
    {
        ui->btnPlay->setText(tr("Play"));
        return;
    }

    ui->progress->setValue(iTabLine);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        static Qt::WindowStates wsOld = Qt::WindowNoState;
        Qt::WindowStates ws = windowState();
        if (ws == Qt::WindowFullScreen)
        {
            ws = wsOld;
        }
        else
        {
            wsOld = ws;
            ws = Qt::WindowFullScreen;
        }

        setWindowState(ws);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QRect rc = ui->labelTitle->rect();
        QPoint ptLT = ui->widgetTitle->mapToGlobal(rc.topLeft());
        QPoint ptRB = ui->widgetTitle->mapToGlobal(rc.bottomRight());
        rc = QRect(ptLT, ptRB);
        QPoint pt = event->globalPos();
        qDebug() << rc << pt;
        if (rc.contains(pt))
        {
            m_dragging = true;
            m_dragPosition = event->globalPos() - this->pos();
            event->accept();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() == Qt::LeftButton))
    {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging)
    {
        event->accept();
        m_dragging=false;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasFormat("text/uri-list"))
        return;

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.size() == 1 && QFileUtil::getFileExt(urls.front().path()).compare(PROJ_FILE_EXT, Qt::CaseInsensitive) == 0)
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.size() == 1 && QFileUtil::getFileExt(urls.front().toLocalFile()).compare(PROJ_FILE_EXT, Qt::CaseInsensitive) == 0)
        openProject(urls.front().toLocalFile());
}

void MainWindow::on_pushButtonClose_clicked()
{
    QGuiApplication::exit(0);
}

void MainWindow::on_pushButtonMax_clicked()
{
    static Qt::WindowStates wsOld = Qt::WindowNoState;
    Qt::WindowStates ws = windowState();
    if (ws == Qt::WindowMaximized)
    {
        ws = wsOld;
    }
    else
    {
        wsOld = ws;
        ws = Qt::WindowMaximized;
    }

    setWindowState(ws);
}

void MainWindow::on_pushButtonMin_clicked()
{
    setWindowState(Qt::WindowMinimized);
}
