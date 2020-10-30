#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool openProject(QString strProj);
    
protected:
    void initUI();

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
