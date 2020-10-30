#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qpreviewitem.h"
#include "../Project/tabproject.h"
#include "qappdata.h"
#include "qtablineitem.h"
#include "qprojectconfigdialog.h"
#include "qtablineconfigdialog.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Editor"));
    setWindowIcon(QIcon(":/image/resource/logo.ico"));

    connect(ui->splitterH, &QSplitter::splitterMoved, this, &MainWindow::onSpliterMoved);
    connect(ui->splitterV, &QSplitter::splitterMoved, this, &MainWindow::onSpliterMoved);

    connect(ui->previewList, &QPreviewWidget::itemChecked, ui->workingList, &QWorkingAreaWidget::ensureVisible);
    //connect(ui->previewWidget, &QPreviewWidget::newImageAdded, ui->workingWidget, &QWorkingAreaWidget::onNewImageAdded);
    //connect(ui->previewWidget, &QPreviewWidget::imageDeleted, ui->workingWidget, &QWorkingAreaWidget::onImageDeleted);
    //connect(ui->previewWidget, &QPreviewWidget::imageMoved, ui->workingWidget, &QWorkingAreaWidget::onImageMoved);

    connect(ui->workingList, &QWorkingAreaWidget::signalAddTabLine, this, &MainWindow::slotAddTabLine);

    g_appData.registerWidget(UI_NAME_MAINWINDOW, this);
    g_appData.registerWidget(UI_NAME_WORKING_LIST, ui->workingList);
    g_appData.registerWidget(UI_NAME_PREVIEW_LIST, ui->previewList);
    g_appData.registerWidget(UI_NAME_TABLINE_LIST, ui->tabLineListView);

    if (!TAB_INST->currentProject().isEmpty())
        project2UI();
    else
        TAB_INST->createPorject();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSpliterMoved(int, int)
{
    adjustLayouts();
}

void MainWindow::onPreviewItemChecked(int index)
{
    qDebug() << "Working list should check: " << index;
    //Q_UNUSED(strImgPath);
    //qDebug() << "Preview image check: " << strImgPath;
    //ui->scrollAreaWork->verticalScrollBar()->setValue(ui->workingWidget->layout()->itemAt(index)->widget()->rect().top());
    //ui->workingWidget->ensureVisible(index);
}

void MainWindow::slotAddTabLine(QString strImgPath, QRect rc)
{
    Q_UNUSED(strImgPath);
    Q_UNUSED(rc);

    TabLinePtr tl(new TabLine);
    tl->rcPos = rc;
    tl->strImg = strImgPath;
    tl->sections = 0;

    QTabLineConfigDialog dlg(this);
    dlg.setTabLine(tl);
    dlg.setModal(true);
    dlg.show();
    if (QDialog::Accepted != dlg.exec())
        return;

    //ui->tabLineWidget->addItem(tl);
    TAB_INST->addTabLine(tl);

    adjustLayouts();
}

void MainWindow::slotTabLineItemClicked(bool)
{

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    adjustLayouts();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    adjustLayouts();
}

void MainWindow::project2UI()
{
    TABProject* pProj = TABProject::getInstance();
    //pProj->createPorject(true);

    //auto imgs = pProj->getAllImage();
    //for (auto img : imgs)
    //{
    //    ui->previewWidget->addItem(img);
    //    ui->workingWidget->addItem(img);
    //}
    ui->previewList->update();

    //auto tls = pProj->getTabLineList();
    //for(auto tl : tls)
    //{
    //    ui->tabLineWidget->addItem(tl);
    //}
    ui->tabLineListView->update();

    adjustLayouts();

    setWindowTitle(QFileUtil::getFileName(pProj->currentProject()));
}

void MainWindow::adjustLayouts()
{
    //ui->previewWidget->adjustItems(QSize(ui->splitterH->sizes().at(0), 0));
    //int w = ui->scrollAreaWork->verticalScrollBar()->isVisible()
    //        ? ui->scrollAreaWork->verticalScrollBar()->width() + ui->splitterH->handleWidth()
    //        : ui->splitterH->handleWidth();
    //ui->workingWidget->adjustItems(QSize(ui->splitterV->sizes().at(0) - w, ui->splitterH->sizes().at(0)));

    //w=ui->scrollAreaTabLine->verticalScrollBar()->isVisible()
    //        ? ui->scrollAreaTabLine->verticalScrollBar()->width() + ui->splitterV->handleWidth()
    //        : ui->splitterV->handleWidth();
    //qDebug() << "At(0)=" << ui->splitterV->sizes().at(0) << " At(1)=" << ui->splitterV->sizes().at(1);
    //ui->tabLineWidget->adjustItems(QSize(ui->splitterV->sizes().at(1) - w, ui->splitterV->sizes().at(0)));

    //qDebug() << "adjustLayouts";
    TAB_INST->notifyModelChanged();
}

void MainWindow::on_action_Open_project_triggered()
{
    QString strProj = QFileDialog::getOpenFileName(this,
        tr("Open Project"), QDir::currentPath(), tr("Tab Project Files (*.tpf)"));
    TAB_INST->openProject(strProj);
    project2UI();
}

void MainWindow::on_action_New_Project_triggered()
{
    //ui->tabLineWidget->reset();
    ui->workingList->reset();
    ui->previewList->reset();
    TAB_INST->createPorject(false);
    setWindowTitle(tr("New project*"));
}

void MainWindow::on_action_Save_Project_triggered()
{
    QString strProj = TAB_INST->currentProject();
    if (strProj.isEmpty())
    {
        strProj = QFileDialog::getSaveFileName(this,
                      tr("Save Project"), QDir::currentPath(), tr("Tab Project Files (*.tpf)"));
        if (QFileUtil::getFileExt(strProj).toLower() != PROJ_FILE_EXT)
            strProj.append(PROJ_FILE_EXT);
    }

    if (strProj.isEmpty())
        return;

    bool bRes = TAB_INST->saveProject(strProj);

    QMessageBox msg;
    msg.setText(bRes ? tr("Project saved!") : tr("Error!"));
    msg.exec();

    setWindowTitle(QFileUtil::getFileName(TAB_INST->currentProject()));
}

void MainWindow::on_action_Exit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_action_Project_Config_triggered()
{
    QProjectConfigDialog dlg(this);
    dlg.setModal(true);
    dlg.show();
    dlg.exec();
}

void MainWindow::on_actionPlay_triggered()
{
    QString tempProj = QDir::tempPath() + "/" + QUuid::createUuid().toString() + ".tpf";
    TAB_INST->saveProject(tempProj, true);

    QString playerPath;
#ifdef _MSC_VER
    playerPath = QApplication::applicationDirPath() + "/" + PLAYER_NAME;
#endif

    QString command("\"");
    command.append(playerPath);
    command.append("\" \"");
    command.append(tempProj);
    command.append("\" -RemoveProjOnExit");

    QProcess proc;
    proc.startDetached(command);
}
