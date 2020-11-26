#include "qlineheightcombobox.h"


QLineHeightComboBox::QLineHeightComboBox(QWidget *parent)
    : QComboBox (parent)
    , m_pValidator(new QIntValidator(50, 500, this))
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnCurrentIndexChanged(int)));
}

QLineHeightComboBox::~QLineHeightComboBox()
{
    if (m_pValidator) delete m_pValidator;
}

void QLineHeightComboBox::setHeight(int h)
{
    if (h == 0)
    {
        this->setCurrentIndex(0);
    }
    else
    {
        if (h < m_pValidator->bottom())
            h = m_pValidator->bottom();
        else if (h > m_pValidator->top())
            h = m_pValidator->top();

        this->lineEdit()->setText(QString::asprintf("%d", h));
    }

    emit signalLineHeightChanged(h);
}

void QLineHeightComboBox::slotOnCurrentIndexChanged(int index)
{
    if (index == 0)
        emit signalLineHeightChanged(0);
    else
        emit signalLineHeightChanged(itemText(index).toInt());
}


void QLineHeightComboBox::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Return || key == Qt::Key_Enter)
    {
        checkEditText();
    }

    QComboBox::keyReleaseEvent(event);
}

void QLineHeightComboBox::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);

    checkEditText();

    QComboBox::focusOutEvent(event);
}

void QLineHeightComboBox::checkEditText()
{
    QLineEdit *pEdit = this->lineEdit();
    if (pEdit)
    {
        QString str = pEdit->text();
        int pos=0;
        int st = m_pValidator->validate(str, pos);
        if (st != QValidator::Acceptable)
            this->setCurrentIndex(0);
        else
            emit signalLineHeightChanged(str.toInt());
    }
}
