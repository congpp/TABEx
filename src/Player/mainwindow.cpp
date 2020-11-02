#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Project/tabproject.h"
#include "resourceloader.h"
#include "welcomedialog.h"
#include <QtWidgets>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->widgetPlayer, &QTabLineGLWidget::signalTabLineChanged, this, &MainWindow::onPlayerTabLineChanged);
    setWindowTitle(tr("Player"));
    setWindowIcon(QIcon(":/image/resource/logo.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    qDebug() << TAB_INST->getSingerName();

    //ui->labelSongDesc->setText(TAB_INST->getMusicInfo());
    //ui->labelSingerName->setText(TAB_INST->getMusicTitle());
    //ui->labelSingerName->setText(TAB_INST->getSingerName());
    //ui->labelSingerInfo->setText(TAB_INST->getSingerInfo());
    //ui->btnCover->setStyleSheet();
    //ui->btnSingerImg->setImage(TAB_INST->getSingerImg());
    //ui->btnCoverImg->setImage(TAB_INST->getCoverImg());

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
    if (delta > 0)
        on_btnLeft_clicked();
    else
        on_btnRight_clicked();
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
    }

    return false;
}

void MainWindow::on_actionOpen_triggered()
{
    QString strProj = QFileDialog::getOpenFileName(this,
        tr("Open Project"), QDir::currentPath(), tr("Tab Project Files (*.tpf)"));

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

    ui->widgetPlayer->slowDown();
}

void MainWindow::on_btnSpeedUp_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

    ui->widgetPlayer->speedUp();
}

void MainWindow::on_btnResetSpeed_clicked()
{
    if (TAB_INST->currentProject().isEmpty())
        return;

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
