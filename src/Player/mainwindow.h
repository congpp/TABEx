#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userconfig.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    void initUI();

    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    //virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
    //virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    virtual void paintEvent(QPaintEvent *event) override;

    void setWindowTitle(QString strTitle);
public slots:
    bool openProject(QString strProj);

private slots:
    void on_actionOpen_triggered();
    void on_btnPlay_clicked(bool checked);
    void on_progress_valueChanged(int value);
    void on_btnLeft_clicked();
    void on_btnRight_clicked();
    void on_btnSlowDown_clicked();
    void on_btnSpeedUp_clicked();
    void on_btnResetSpeed_clicked();
    void onPlayerTabLineChanged(int iTabLine);
    void on_actionExit_triggered();
    void on_pushButtonClose_clicked();
    void on_pushButtonMax_clicked();
    void on_pushButtonMin_clicked();

private:
    Ui::MainWindow *ui;

    bool m_dragging = false;
    QPoint m_dragPosition;

    UserTpfConfig cfg;
};

#endif // MAINWINDOW_H
