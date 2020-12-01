#include "qtablineconfigdialog.h"
#include "ui_qtablineconfigdialog.h"
#include "../Project/tabproject.h"
#include "../Utils/utils.h"
#include "qappdata.h"
#include "qtablinewidget.h"
#include <QValidator>

int QTabLineConfigDialog::m_lastSetions = 4;

QTabLineConfigDialog::QTabLineConfigDialog(QWidget *parent, TabLineConfigMode mode) :
    QDialog(parent),
    ui(new Ui::QTabLineConfigDialog),
    m_tlcm(mode)
{
    ui->setupUi(this);
    ui->lineEditSections->setValidator(new QIntValidator(0, 1024, this));
    ui->lineEditAdditionalBeat->setValidator(new QIntValidator(0, 1024, this));

    connect(ui->widgetTabLine, &QTabLineDetailWidget::signalTabLineChanged, this, &QTabLineConfigDialog::updateInfo);

    initRadio(mode);
}

QTabLineConfigDialog::~QTabLineConfigDialog()
{
    delete ui;
}

void QTabLineConfigDialog::initRadio(TabLineConfigMode mode)
{
    bool bShowRadio = mode == TLCM_NEW;
    ui->widgetRadio->setVisible(bShowRadio);
    if (bShowRadio)
    {
        setWindowTitle(tr("New tabline"));
        QTabLineWidget * pList = static_cast<QTabLineWidget*>(g_appData.findWidget(UI_NAME_TABLINE_LIST));
        int i = pList ? pList->getSelectedCount() : 0;
        ui->radioButtonAddToFront->setVisible(true);
        ui->radioButtonAppend->setVisible(true);
        ui->radioButtonInsertBefore->setVisible(i==1);
        ui->radioButtonInsertAfter->setVisible(i==1);
        //只有一个选中的时候时，默认勾选 “插入选中项后面”
        //没有选中或者选中了多个（这种情况无法预测， selectedIndexes 也不是有序的），因此都只能前后追加
        ui->radioButtonAppend->setChecked(i!=1);
        ui->radioButtonInsertAfter->setChecked(i==1);
    }
    else
    {
        setWindowTitle(tr("Edit tabline"));
    }

}

int QTabLineConfigDialog::checkModification()
{
    int ret = QMessageBox::Discard;
    ui2TabLine();
    if (*m_ptrTabLine != *m_tlBefore)
    {
        ret = QMessageBox::question(this, tr("TPF Editor"), tr("Tab line has been modified.\r\nDo you want to save your changes?"),
                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                              QMessageBox::Cancel);

        if (QMessageBox::Save == ret)
            *m_tlBefore = *m_ptrTabLine;
    }
    return ret;
}

void QTabLineConfigDialog::ui2TabLine()
{
    m_lastSetions = m_ptrTabLine->sections = ui->lineEditSections->text().toInt();
    m_ptrTabLine->additionalBeat = ui->lineEditAdditionalBeat->text().toInt();
}

void QTabLineConfigDialog::setTabLine(TabLinePtr ptrTabLine)
{
    m_tlBefore = ptrTabLine;
    m_ptrTabLine.reset(new TabLine);
    *m_ptrTabLine = *ptrTabLine;
    if (m_ptrTabLine.isNull())
        return;

    //tabline 2 ui
    ui->lineEditSections->setText(QString::asprintf("%d", (m_ptrTabLine->sections == 0) ? m_lastSetions : m_ptrTabLine->sections));
    ui->lineEditAdditionalBeat->setText(QString::asprintf("%d", m_ptrTabLine->additionalBeat));
    ui->widgetTabLine->setTabLineInfo(m_ptrTabLine);

    ui->lineEditBPM->setText(QString::asprintf("%d", TAB_INST->getBeatPerMinute()));
    ui->lineEditBPS->setText(QString::asprintf("%d", TAB_INST->getBeatPerSection()));

    ui->pushButtonPrev->setVisible(m_tlIndex != -1);
    ui->pushButtonSave->setVisible(m_tlIndex != -1);
    ui->pushButtonNext->setVisible(m_tlIndex != -1);
    ui->pushButtonPrev->setDisabled(m_tlIndex == 0);
    ui->pushButtonNext->setDisabled(m_tlIndex >= TAB_INST->getTabLineCount());
    updateInfo();
}

void QTabLineConfigDialog::setTabLine(int index)
{
    TabLinePtr tl = TAB_INST->getTabLineAt(index);
    if (!tl.isNull())
    {
        m_tlIndex = index;
        setTabLine(tl);
    }
}

int QTabLineConfigDialog::getNewTabLinePosition()
{
    return m_newTabLinePosition;
}

void QTabLineConfigDialog::updateInfo()
{
    QString strInfo;
    if (m_ptrTabLine)
    {
        strInfo.append("Image src: ");
        strInfo.append(QStringUtil::rect2String(m_ptrTabLine->rcPos));
        strInfo.append(QString::asprintf(" About %0.2f s", ui->lineEditSections->text().toDouble() * TAB_INST->getSecondPerSection()));
    }
    ui->labelInfo->setText(strInfo);
}

void QTabLineConfigDialog::accept()
{
    on_pushButtonSave_clicked();
    QDialog::accept();
}

void QTabLineConfigDialog::reject()
{
    if (QMessageBox::Cancel == checkModification())
        return;

    QDialog::reject();
}

void QTabLineConfigDialog::on_lineEditSections_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    updateInfo();
}

void QTabLineConfigDialog::on_pushButtonPrev_clicked()
{
    checkModification();
    if (m_tlIndex > 0)
        setTabLine(m_tlIndex-1);
}

void QTabLineConfigDialog::on_pushButtonSave_clicked()
{
    ui2TabLine();
    *m_tlBefore = *m_ptrTabLine;
    if (m_tlcm == TLCM_NEW)
    {
        m_newTabLinePosition = -1;
        if (ui->radioButtonAddToFront->isChecked())
            m_newTabLinePosition = 0;
        else if (ui->radioButtonAppend->isChecked())
            m_newTabLinePosition = -1;
        else if (ui->radioButtonInsertBefore->isChecked())
        {
            QTabLineWidget * pList = static_cast<QTabLineWidget*>(g_appData.findWidget(UI_NAME_TABLINE_LIST));
            m_newTabLinePosition = pList ? pList->getFirstSelectedIndex() : 0;
        }
        else if (ui->radioButtonInsertAfter->isChecked())
        {
            QTabLineWidget * pList = static_cast<QTabLineWidget*>(g_appData.findWidget(UI_NAME_TABLINE_LIST));
            m_newTabLinePosition = pList ? pList->getLastSelectedIndex()+1 : 0;
        }
    }
}

void QTabLineConfigDialog::on_pushButtonNext_clicked()
{
    checkModification();
    if (m_tlIndex < TAB_INST->getTabLineCount())
        setTabLine(m_tlIndex+1);
}
