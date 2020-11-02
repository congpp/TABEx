#ifndef TABPROJECT_H
#define TABPROJECT_H
#include "commondefine.h"
#include "../Utils/utils.h"
#include "tablinemodel.h"
#include "imagemodel.h"
#include "projecthistory.h"
#include <QMap>

class TABProject
{

public:
    TABProject();
    ~TABProject();
    //////////////////////////////////////////////////////
    static TABProject* getInstance();

    //////////////////////////////////////////////////////
    //新建工程（在临时目录新建一个）
    bool createPorject(bool restoreLastProject = false);
    //打开工程，释放到临时目录
    bool openProject(QString strProjFile);
    bool openJson(QString strJsonFile);
    //打包、保存工程
    bool saveProject(QString strProjFile, bool bTemp=false);
    //关闭工程
    bool closeProject();

    QString currentProject();
    //////////////////////////////////////////////////////
    //图片相关接口

    //添加一个图片
    bool addImage(QString strImg, QString* pStrImgInProject);
    bool insertImage(QString strImg, int index, QString* pStrImgInProject);
    bool removeImage(QString strImg);
    bool removeImage(int index);
    //图片上移
    bool moveUpImage(QString strImg);
    bool moveDownImage(QString strImg);
    bool moveImage(int indexFrom, int indexTo);
    //获取图片
    QString   getImageName(int idx);
    QImagePtr getImage(QString strImg);
    QImagePtr getImage(int idx);
    QImagePtr getImagePreview(QString strImg);
    QImagePtr getImagePreview(int idx);
    QImagePtr getSingerImage();
    QImagePtr getCoverImage();

    QList<QString> getAllImage();
    int getImageCount();

    //////////////////////////////////////////////////////
    //tabline相关接口
    //增删查改
    bool addTabLine(TabLinePtr tl);
    bool addTabLine(TabLineList tl);
    TabLinePtr getTabLineAt(int index);
    TabLineList getTabLineList();
    int getTabLineCount();
    bool insertTabLine(int idx, TabLinePtr tl);
    bool insertTabLine(int idx, TabLineList tl);
    bool removeTabLine(int idx);
    bool removeTabLine(TabLinePtr tl);
    bool moveLeftTabLine(TabLinePtr tl);
    bool moveRightTabLine(TabLinePtr tl);

    //获取TabLine 水平/垂直拼接构成的总图片的大小
    QSize getTabLineSizeH(int padding=0);
    QSize getTabLineSizeV(int padding=0);

    //@param time second, e.g. 1.2
    TabLinePtr findTabLineByTime(double time);

    //不需要更新接口，因为是shared_ptr，直接同步了
    //bool updateTabLine(TabLinePtr tl, int index);


    //////////////////////////////////////////////////////
    //基本信息
    void setMusicTitle(QString strMusicTitle);
    QString getMusicTitle();

    void setMusicInfo(QString strMusicInfo);
    QString getMusicInfo();

    void setSingerName(QString strSingerName);
    QString getSingerName();

    bool setSingerImg(QString strSingerImg);
    QString getSingerImg();

    void setSingerInfo(QString strSingerInfo);
    QString getSingerInfo();

    bool setCoverImg(QString strCorverImg);
    QString getCoverImg();

    void setBeatPerMinute(int beat);
    int getBeatPerMinute();
    int getBeatPerMinuteAdjusted();

    void setBeatPerSection(int beat);
    int getBeatPerSection();

    //////////////////////////////////////////////////////
    //辅助接口
    int getSectionCount();

    double getSecondPerSection();
    double getSecondPerBeat();
    double getSecondAtTabLine(int iTabLine);
    double getSecondOfThisSong();

    QJsonObject tabLine2Json(TabLinePtr tl);
    TabLinePtr  json2TabLine(QJsonObject js);

    QAbstractItemModel *getTabLineModel();
    QAbstractItemModel *getImageModel();
    QAbstractItemModel *getPreviewImageModel();
    QAbstractTableModel *getHistoryModel();

    void notifyModelChanged();

    //通过调整节拍器的每分钟节拍数来调整整首歌的速度
    int adjustSpeed(int beatAdjusted);
    int getAdjustSpeed();
protected:
    bool loadImage(QString strFileName, int index = -1);

    QString getTempPath(bool bCreate=true);
private:
    QString m_uuid;
    QString m_projFile;
    TabProjInfo m_projInfo;
    QMap<QString, QImagePtr> m_imgMap;
    QMap<QString, QImagePtr> m_imgPreviewMap;
    int64_t m_nImgID;

    TabLineModel    m_tlModel;
    ImageModel      m_imgModel;
    PreviewImageModel m_prevImgModel;
    ProjectHistory  m_history;

    int m_adjustedBeat = 0; //用来暂时调速
};

#define TAB_INST TABProject::getInstance()

#endif // TABPROJECT_H
