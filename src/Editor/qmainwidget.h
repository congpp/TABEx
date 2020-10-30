#ifndef QMAINWIDGET_H
#define QMAINWIDGET_H

#include <QWidget>

class QMainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMainWidget(QWidget *parent = nullptr);

signals:

public slots:

public:
    //void setGeometry(int x, int y, int w, int h) override;
};

#endif // QMAINWIDGET_H
