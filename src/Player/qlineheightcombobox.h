#ifndef QLINEHEIGHTCOMBOBOX_H
#define QLINEHEIGHTCOMBOBOX_H
#include <QtWidgets>

class QLineHeightComboBox : public QComboBox
{
    Q_OBJECT
public:
    QLineHeightComboBox(QWidget* parent = nullptr);
    ~QLineHeightComboBox() override;

    void setHeight(int h);
signals:
    void signalLineHeightChanged(int height);
public slots:
    void slotOnCurrentIndexChanged(int index);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    void checkEditText();

private:
    QIntValidator *m_pValidator = nullptr;
};

#endif // QLINEHEIGHTCOMBOBOX_H
