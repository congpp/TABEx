#ifndef QTABLINECONFIGDIALOG_H
#define QTABLINECONFIGDIALOG_H

#include <QDialog>
#include "commondefine.h"

namespace Ui {
class QTabLineConfigDialog;
}

enum TabLineConfigMode
{
    TLCM_NEW,
    TLCM_MODIFY,
};

class QTabLineConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QTabLineConfigDialog(QWidget *parent = nullptr, TabLineConfigMode mode = TLCM_MODIFY);
    ~QTabLineConfigDialog();

    void setTabLine(TabLinePtr ptrTabLine);
    void setTabLine(int index);

    int getNewTabLinePosition();
public slots:
    void accept();
    void reject();
    void updateInfo();

protected:
    void initRadio(TabLineConfigMode mode);
    void checkModification();
private slots:
    void on_lineEditSections_textChanged(const QString &arg1);

    void on_pushButtonPrev_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonNext_clicked();

private:
    Ui::QTabLineConfigDialog *ui;

    TabLinePtr m_ptrTabLine;
    TabLinePtr m_tlBefore;
    int        m_tlIndex = -1;

    static double m_lastSetions;

    TabLineConfigMode m_tlcm;
    int m_newTabLinePosition = -1;
};

#endif // QTABLINECONFIGDIALOG_H
