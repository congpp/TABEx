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
    ~MainWindow() override;

    
public slots:
    //Spliter slot
    void onSpliterMoved(int, int);

    //Preview layout slot
    void onPreviewItemChecked(int index);

    void slotAddTabLine(QString strImgPath, QRect rc);
    void slotTabLineItemClicked(bool);
private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

protected:
    void project2UI();
    void adjustLayouts();
private slots:
    void on_action_Open_project_triggered();
    void on_action_New_Project_triggered();
    void on_action_Save_Project_triggered();
    void on_action_Exit_triggered();
    void on_action_Project_Config_triggered();
    void on_actionPlay_triggered();
};

#endif // MAINWINDOW_H
