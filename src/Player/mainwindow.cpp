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
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    updateWindowTitle();

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
    g_userCfg->getUserTpfConfig(cfg);
    TAB_INST->adjustSpeed(cfg.adjustedBpm);
    ui->comboBoxLineHeight->setHeight(cfg.fixedHeight);

    ui->widgetPlayer->reset();
    ui->widgetPlayer->init();

    ui->progress->setRange(0, TAB_INST->getTabLineCount()-1);
    ui->progress->setValue(0);

    updateWindowTitle();
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

void MainWindow::updateWindowTitle()
{
    auto proj = TAB_INST;
    QString title = QFileUtil::getFileName(proj->currentProject());
    if (title.isEmpty())
    {
        title = tr("Player");
    }
    else
    {
        QString singer = proj->getSingerName();
        if (singer.isEmpty())
            singer = tr("Unknow");

        QString song = proj->getMusicTitle();
        if (song.isEmpty())
            song = tr("Unknow");

        title = singer + " - " + song + " (" + title + ")";
    }

    QMainWindow::setWindowTitle(title);
    ui->labelTitle->setText(title);
}

void MainWindow::setWindowState(Qt::WindowStates st)
{
    static const QString maxBtnStyle("QPushButton {"
                        "    border: 0px;"
                        "    width: 26px;"
                        "	background-color: rgba(255, 255, 255, 0);"
                        "	border-image: url(:/image/resource/image/%1.png) 0 52 0 0;"
                        "}"
                        "QPushButton:hover {"
                        "	background-color: rgba(255, 255, 255, 0);"
                        "	border-image: url(:/image/resource/image/%1.png) 0 26 0 26;"
                        "}"
                        "QPushButton:pressed {"
                        "	background-color: rgba(255, 255, 255, 0);"
                        "	border-image: url(:/image/resource/image/%1.png) 0 0 0 52;"
                        "} "
                        "QPushButton:disabled {"
                        "	background-color: rgba(255, 255, 255, 0);"
                        "}");

    ui->centralWidget->layout()->setMargin((st & Qt::WindowNoState) ? 8 : 0);
    ui->pushButtonMax->setStyleSheet(maxBtnStyle.arg((st & Qt::WindowMaximized) ? "btn_restore" : "btn_max"));
    ui->widgetTitle->setVisible((st & Qt::WindowFullScreen) == 0);
    QMainWindow::setWindowState(st);
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
        updatePlayButton(true);
    }
    else
    {
        updatePlayButton(false);
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
        updatePlayButton(true);
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
        if (m_hitTest != HT_C)
        {
            m_resizing = true;
            m_resizePosition = event->globalPos();
            m_resizeBeginRect = geometry();
            //qDebug() << "m_resizePosition = " << m_resizePosition;
        }
        else
        {
            QRect rc = ui->labelTitle->rect();
            QPoint ptLT = ui->widgetTitle->mapToGlobal(rc.topLeft());
            QPoint ptRB = ui->widgetTitle->mapToGlobal(rc.bottomRight());
            rc = QRect(ptLT, ptRB);
            QPoint pt = event->globalPos();
            //qDebug() << rc << pt;
            if (rc.contains(pt))
            {
                m_dragging = true;
                m_dragPosition = event->globalPos() - this->pos();
                event->accept();
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (m_dragging)
        {
            move(event->globalPos() - m_dragPosition);
            event->accept();
            return;
        }
        else if (m_resizing)
        {
            resizeRegion(event);
            event->accept();
            return;
        }
    }

    if (!m_resizing)
        hitTest(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "mouseReleaseEvent";
    if (m_dragging || m_resizing)
    {
        event->accept();
        m_dragging = false;
        m_resizing = false;
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
    Qt::WindowStates ws = windowState();
    if (ws == Qt::WindowMaximized)
    {
        ws = Qt::WindowNoState;
    }
    else
    {
        ws = Qt::WindowMaximized;
    }

    setWindowState(ws);
}

void MainWindow::on_pushButtonMin_clicked()
{
    setWindowState(Qt::WindowMinimized);
}

void MainWindow::hitTest(QMouseEvent *event)
{
    Qt::WindowStates st = windowState();
    if ((Qt::WindowFullScreen & st) || (Qt::WindowMaximized & st))
    {
        m_hitTest = HT_C;
        m_resizing = false;
        return;
    }

    QRect rc = geometry();
    int l=rc.left(), t=rc.top(), w=rc.width(), h=rc.height(), r=DRAG_FRAME_SIZE;
    struct HitTestInfo
    {
        QRect rc;
        HitTest ht;
        Qt::CursorShape cs;
    } htis[] =
    {

    /*LT*/{QRect(l, t, r, r), HT_LT, Qt::SizeFDiagCursor},
    /*RT*/{QRect(l + w - r, t, r, r), HT_RT, Qt::SizeBDiagCursor},
    /*LB*/{QRect(l, t + h - r, r, r), HT_LB, Qt::SizeBDiagCursor},
    /*RB*/{QRect(l + w - r, t + h - r, r, r), HT_RB, Qt::SizeFDiagCursor},
    /*L */{QRect (l, t + r, r, h - r - r), HT_L, Qt::SizeHorCursor},
    /*T */{QRect (l + r, t, w - r - r, r), HT_T, Qt::SizeVerCursor},
    /*R */{QRect (l + w - r, t + r, r, h - r - r), HT_R, Qt::SizeHorCursor},
    /*B */{QRect (l + r, t + h - r, w - r - r, r), HT_B, Qt::SizeVerCursor},
    /*C */{QRect (l + r, t + r, w - r - r, h - r - r), HT_C, Qt::ArrowCursor}
    };

    //计算四个方向的边距，记录鼠标状态
    QPoint pt = event->globalPos();
    for (auto hti : htis)
    if (hti.rc.contains(pt))
    {
        m_hitTest = hti.ht;
        setCursor(hti.cs);
    }
}

void MainWindow::resizeRegion(QMouseEvent *event)
{
    if (!m_resizing)
        return;

    QRect rc = m_resizeBeginRect;
    int dx = event->globalX() - m_resizePosition.rx();
    int dy = event->globalY() - m_resizePosition.ry();
    int minw = minimumWidth();
    int minh = minimumHeight();
    switch (m_hitTest)
    {
    case HT_LT:
         rc.adjust(dx,dy,0,0);
         if (rc.width() < minw) rc.setLeft(m_resizeBeginRect.right() - minw);
         if (rc.height() < minh) rc.setHeight(m_resizeBeginRect.bottom() - minh);
        break;
    case HT_RT:
         rc.adjust(0,dy,dx,0);
         if (rc.width() < minw) rc.setRight(m_resizeBeginRect.left() + minw);
         if (rc.height() < minh) rc.setHeight(m_resizeBeginRect.bottom() - minh);
        break;
    case HT_LB:
         rc.adjust(dx,0,0,dy);
         if (rc.width() < minw) rc.setLeft(m_resizeBeginRect.right() - minw);
         if (rc.height() < minh) rc.setBottom(m_resizeBeginRect.top() + minh);
        break;
    case HT_RB:
         rc.adjust(0,0,dx,dy);
         if (rc.width() < minw) rc.setRight(m_resizeBeginRect.left() + minw);
         if (rc.height() < minh) rc.setBottom(m_resizeBeginRect.top() + minh);
        break;
    case HT_L:
         rc.adjust(dx,0,0,0);
         if (rc.width() < minw) rc.setLeft(m_resizeBeginRect.right() - minw);
        break;
    case HT_T:
         rc.adjust(0,dy,0,0);
         if (rc.height() < minh) rc.setTop(m_resizeBeginRect.bottom() - minh);
        break;
    case HT_R:
         rc.adjust(0,0,dx,0);
         if (rc.width() < minw) rc.setRight(m_resizeBeginRect.left() + minw);
        break;
    case HT_B:
         rc.adjust(0,0,0,dy);
         if (rc.height() < minh) rc.setBottom(m_resizeBeginRect.top() + minh);
        break;
    default: break;
    }

    //qDebug() << event->globalPos() << m_resizePosition << dx << dy << rc << m_hitTest;
    setGeometry(rc);
}

void MainWindow::updatePlayButton(bool bPlay)
{
    static const QString playButtonStyle="QPushButton {"
                                         "    border: 0px;"
                                         "    background-color: rgba(255, 255, 255, 0);"
                                         "    border-image: url(:/image/resource/image/%1.png) 0 52 0 0;"
                                         "}"
                                         "QPushButton:hover {"
                                         "    background-color: rgba(255, 255, 255, 0);"
                                         "    border-image: url(:/image/resource/image/%1.png) 0 26 0 26;"
                                         "}"
                                         "QPushButton:pressed {"
                                         "    background-color: rgba(255, 255, 255, 0);"
                                         "    border-image: url(:/image/resource/image/%1.png) 0 0 0 52;"
                                         "} "
                                         "QPushButton:disabled {"
                                         "    background-color: rgba(255, 255, 255, 0);"
                                         "}";

    ui->btnPlay->setStyleSheet(playButtonStyle.arg(bPlay ? "btn_play" : "btn_pause"));
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::HoverMove)
    {
        QHoverEvent *hoverEvent = static_cast<QHoverEvent *>(event);
        QMouseEvent mouseEvent(QEvent::MouseMove, hoverEvent->pos(),
                               Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        mouseMoveEvent(&mouseEvent);
    }

    return QMainWindow::event(event);
}

void MainWindow::on_pushButtonOpen_clicked()
{
    on_actionOpen_triggered();
}
