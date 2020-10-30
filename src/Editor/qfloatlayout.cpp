/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "qfloatlayout.h"
//! [1]
QFloatLayout::QFloatLayout(QWidget *parent)
    : QLayout(parent)
{
}


//! [1]

//! [2]
QFloatLayout::~QFloatLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}
//! [2]

//! [3]
void QFloatLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}
//! [3]

//! [4]
//int QFloatLayout::horizontalSpacing() const
//{
//    return 0;
//}

//int QFloatLayout::verticalSpacing() const
//{
//    return 0;
//}
//! [4]

//! [5]
int QFloatLayout::count() const
{
    return itemList.size();
}

QLayoutItem *QFloatLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *QFloatLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return nullptr;
}

void QFloatLayout::setFixedSize(QSize sz)
{
    m_szFixed = sz;
    update();
}
//! [5]

//! [6]
//Qt::Orientations QFloatLayout::expandingDirections() const
//{
//    return 0;
//}
//! [6]

//! [7]
//bool QFloatLayout::hasHeightForWidth() const
//{
//    return true;
//}

//int QFloatLayout::heightForWidth(int width) const
//{
//    int height = doLayout(QRect(0, 0, width, 0), true);
//    return height;
//}
//! [7]

//! [8]
void QFloatLayout::setGeometry(const QRect &rect)
{
    QRect rc=rect;
    rc.setWidth(m_szFixed.width());
    rc.setHeight(m_szFixed.height());
    QLayout::setGeometry(rc);
}

QSize QFloatLayout::sizeHint() const
{
    if (count() == 0)
        return QSize();

    return m_szFixed;
}

QSize QFloatLayout::minimumSize() const
{
    return m_szFixed;
}
//! [8]


