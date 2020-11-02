#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "qhistorytabledelegate.h"
#include "../Project/tabproject.h"
#include "mainwindow.h"

#include <QtWidgets>

WelcomeDialog::WelcomeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/resource/logo.ico"));

    this->setWindowFlag(Qt::FramelessWindowHint);
    //this->setAttribute(Qt::WA_TranslucentBackground);

    m_pDelegate = new QHistoryTableDelegate(this);
    ui->tableView->setModel(TAB_INST->getHistoryModel());
    ui->tableView->setItemDelegate(m_pDelegate);
    ui->tableView->verticalHeader()->setEnabled(false);
    ui->tableView->clearFocus();

    connect(ui->tableView, &QTableView::clicked, this, &WelcomeDialog::slotOnTableClicked);
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
    delete m_pDelegate;
}

QString WelcomeDialog::getSelectedProjFile()
{
    return m_strProjFile;
}

void WelcomeDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QRect rc = this->rect();
        rc.setBottom(rc.top() + 32);
        if (rc.contains(event->pos()))
        {
            m_dragging = true;
            m_dragPosition = event->globalPos() - this->pos();
            event->accept();
        }
    }
}

void WelcomeDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() == Qt::LeftButton))
    {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void WelcomeDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging)
    {
        event->accept();
        m_dragging=false;
    }
}

void WelcomeDialog::on_pushButtonOpen_clicked()
{
    QString strProj = QFileDialog::getOpenFileName(this,
        tr("Open Project"), QDir::currentPath(), tr("Tab Project Files (*.tpf)"));

    if (strProj.isEmpty())
        return;

    m_strProjFile = strProj;
    emit signalOpenProject(m_strProjFile);
    close();
}

void WelcomeDialog::slotOnTableClicked(const QModelIndex &idx)
{
    QVariant var = idx.model()->data(idx);
    QStringList sl = var.toStringList();
    if (sl.size() == 2)
    {
        m_strProjFile = sl.at(0);
        emit signalOpenProject(m_strProjFile);
        close();
    }
}
