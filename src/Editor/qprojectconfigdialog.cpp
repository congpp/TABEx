#include "qprojectconfigdialog.h"
#include "ui_qprojectconfigdialog.h"
#include "../Project/tabproject.h"
#include "qimagebutton.h"
#include <QFileDialog>
#include <QCloseEvent>
#include <QValidator>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

QProjectConfigDialog::QProjectConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QProjectConfigDialog)
{
    ui->setupUi(this);
    connect(ui->btnSingerImg, SIGNAL(clicked(bool)), this, SLOT(slotSingerImgBtnClicked(bool)));
    connect(ui->btnCoverImg, SIGNAL(clicked(bool)), this, SLOT(slotCoverImgBtnClicked(bool)));
    ui->textEditBeatPerSection->setValidator(new QIntValidator(1,32,this));
    ui->textEditBeatPerMinute->setValidator(new QIntValidator(1,240,this));
    projectInfo2UI();
}

QProjectConfigDialog::~QProjectConfigDialog()
{
    delete ui;
}

bool QProjectConfigDialog::projectInfo2UI()
{
    ui->textEditDesc->setPlainText(TAB_INST->getMusicInfo());
    ui->textEditTitle->setText(TAB_INST->getMusicTitle());
    ui->textEditSinger->setText(TAB_INST->getSingerName());
    ui->textEditSingerInfo->setPlainText(TAB_INST->getSingerInfo());
    ui->textEditBeatPerSection->setText(QString::asprintf("%d", TAB_INST->getBeatPerSection()));
    ui->textEditBeatPerMinute->setText(QString::asprintf("%d", TAB_INST->getBeatPerMinute()));
    //ui->btnCover->setStyleSheet();
    ui->btnSingerImg->setImage(TAB_INST->getSingerImg());
    ui->btnCoverImg->setImage(TAB_INST->getCoverImg());
    return true;
}

bool QProjectConfigDialog::UI2ProjectInfo()
{
    TAB_INST->setMusicInfo(ui->textEditDesc->toPlainText());
    TAB_INST->setMusicTitle(ui->textEditTitle->text());
    TAB_INST->setSingerName(ui->textEditSinger->text());
    TAB_INST->setSingerInfo(ui->textEditSingerInfo->toPlainText());
    TAB_INST->setBeatPerSection(ui->textEditBeatPerSection->text().toInt());
    TAB_INST->setBeatPerMinute(ui->textEditBeatPerMinute->text().toInt());

    TAB_INST->setCoverImg(ui->btnCoverImg->getImagePath());
    TAB_INST->setSingerImg(ui->btnSingerImg->getImagePath());
    return true;
}

void QProjectConfigDialog::accept()
{
    UI2ProjectInfo();
    QDialog::accept();
}

void QProjectConfigDialog::slotSingerImgBtnClicked(bool b)
{
    Q_UNUSED(b);
    QString strImg = QFileDialog::getOpenFileName(this,
        tr("Select singer image:"), QDir::currentPath(), tr("Image Files (*.bmp *.jpg *.png)"));
    if (!strImg.isEmpty())
    {
        //TAB_INST->setSingerImg(strImg);
        ui->btnSingerImg->setImage(strImg);
    }
}

void QProjectConfigDialog::slotCoverImgBtnClicked(bool b)
{
    Q_UNUSED(b);
    QString strImg = QFileDialog::getOpenFileName(this,
        tr("Select conver image:"), QDir::currentPath(), tr("Image Files (*.bmp *.jpg *.png)"));
    if (!strImg.isEmpty())
    {
        //TAB_INST->setCoverImg(strImg);
        ui->btnCoverImg->setImage(strImg);
    }
}

void QProjectConfigDialog::on_pushButtonSearchCover_clicked()
{
    QString music163("https://music.163.com/#/search/m/?id=1344897943&s=%1&type=1");
    QString title = ui->textEditTitle->text();
    if (title.isEmpty())
        return;

    QUrl url(music163.arg(title));
    QNetworkAccessManager m_qnam;
    QNetworkRequest qnr(url);
    QNetworkReply* reply = m_qnam.get(qnr);
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    QByteArray replyData = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QVariant redirectAttr = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error())
        qDebug() << "Error open url: " << reply->errorString();
    else if ((statusCode >= 300 && statusCode < 399) || !redirectAttr.isNull())
        qDebug()<<"Error redirect";
    else
        qDebug() << QString(replyData);

    reply->deleteLater();
    reply = nullptr;
}

void QProjectConfigDialog::on_pushButtonSearchSinger_clicked()
{

}
