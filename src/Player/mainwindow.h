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
    virtual bool event(QEvent *event) override;

    void updateWindowTitle();
    void setWindowState(Qt::WindowStates st);
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

    void on_pushButtonOpen_clicked();

protected:
    void hitTest(QMouseEvent *event);
    void resizeRegion(QMouseEvent *event);
    void updatePlayButton(bool bPlay);
private:
    Ui::MainWindow *ui;

    bool m_dragging = false;
    QPoint m_dragPosition;

    const int DRAG_FRAME_SIZE = 8;
    bool m_resizing = false;
    QPoint m_resizePosition;
    QRect  m_resizeBeginRect;
    enum HitTest
    {
        HT_LT, HT_T, HT_RT,
        HT_L,  HT_C, HT_R,
        HT_LB, HT_B, HT_RB,
    }m_hitTest = HT_C;

    UserTpfConfig cfg;
};

#endif // MAINWINDOW_H
