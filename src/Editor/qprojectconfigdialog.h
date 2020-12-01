#ifndef QPROJECTCONFIGDIALOG_H
#define QPROJECTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class QProjectConfigDialog;
}

class QProjectConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QProjectConfigDialog(QWidget *parent = nullptr);
    ~QProjectConfigDialog() override;

private:
    Ui::QProjectConfigDialog *ui;

protected:
    bool projectInfo2UI();
    bool UI2ProjectInfo();

public slots:
    void accept();
    void slotSingerImgBtnClicked(bool b);
    void slotCoverImgBtnClicked(bool b);
private slots:
    void on_pushButtonSearchCover_clicked();
    void on_pushButtonSearchSinger_clicked();
};

#endif // QPROJECTCONFIGDIALOG_H
