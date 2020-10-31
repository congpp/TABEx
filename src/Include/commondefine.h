#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H
#include <QList>
#include <QImage>
#include <QSharedPointer>
#include <QString>

static const int MAX_PREVIEW_ITEM_WIDTH  = 320;
static const int MAX_PREVIEW_ITEM_HEIGHT = 480;

static const int MAX_BEAT_PER_MINUTE = 300;

static const QString PROJ_PATH_ROOT("/TABProject/");
static const QString PROJ_PATH_TEMP("temp/");
static const QString PROJ_PATH_IMG_DIR("img/");
static const QString PROJ_PATH_COVER("cover.png");
static const QString PROJ_PATH_SINGER("singer.png");
static const QString PROJ_PATH_JSON("config.json");

static const QString PROJ_FILE_EXT(".tpf");

static const QString EDITOR_NAME("Editor");
static const QString PLAYER_NAME("Player");

static const QColor CLR_FG(0, 162, 232, 128);

//TabLine 乐谱的行，包含一到多个小节
//把一整张乐谱分割成N个TabLine，分割的时候把跳转处理好，就可以从头到尾播放了
struct TabLine
{
    int     sections = 0;   //这行有多少个小节
    QString strImg;     //图片路径
    QRect   rcPos;      //图片区域
    QRect   rcOffset;   //显示时的偏移
    QRect   rcBlur;     //挡住一部分区域，不需要显示，unused
};

typedef QSharedPointer<TabLine> TabLinePtr;
typedef QList<TabLinePtr> TabLineList;

struct TabProjInfo
{
    TabProjInfo()
        : strSingerImg(PROJ_PATH_SINGER)
        , strCoverImg(PROJ_PATH_COVER)
    {
    }
    QString strMusicTitle;  //歌曲标题
    QString strMusicInfo;   //歌曲简介
    QString strSingerName;  //歌手
    QString strSingerImg;   //歌手图片
    QString strSingerInfo;  //歌手简介
    QString strCoverImg;    //封面图片
    int     beatPerMinute = 60;  //播放序列速度，一分钟节拍数，就是节拍器的数字了，e.g. 60
    int     beatPerSection = 4; //一小节多少拍，e.g. 3/4的乐谱就是4

    TabLineList tabLines;   //切割的序列
    QList<QString> imgList; //图片
    QString strWorkingPath; //工程的工作目录
};

typedef QSharedPointer<QImage> QImagePtr;

#endif // COMMONDEFINE_H
