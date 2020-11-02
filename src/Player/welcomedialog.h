#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>

namespace Ui {
class WelcomeDialog;
}

class QHistoryTableDelegate;

class WelcomeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeDialog(QWidget *parent = nullptr);
    ~WelcomeDialog() override;

    QString getSelectedProjFile();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButtonOpen_clicked();
    void slotOnTableClicked(const QModelIndex &idx);

    void on_pushButtonCancel_clicked();

private:
    Ui::WelcomeDialog *ui;
    bool m_dragging = false;
    QPoint m_dragPosition;

    QHistoryTableDelegate* m_pDelegate;
    QString m_strProjFile;
};

#endif // WELCOMEDIALOG_H
