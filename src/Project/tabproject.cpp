#include "tabproject.h"
#include "qtpf.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QUuid>

TABProject::TABProject()
{
}

TABProject::~TABProject()
{
    closeProject();

    //clean up temp dir
    QDir dirTmp(getTempPath(false));
    if (!dirTmp.path().isEmpty())
        dirTmp.removeRecursively();
}

TABProject *TABProject::getInstance()
{
    static TABProject proj;
    return &proj;
}

bool TABProject::createPorject(bool restoreLastProject)
{
    Q_UNUSED(restoreLastProject);

    QDir dir;
    QString strPath = getTempPath();

    //多实例有点复杂，暂不实现
    //if (restoreLastProject && QFile::exists(strPath + PROJ_PATH_JSON))
    //{
    //    if (openJson(strPath + PROJ_PATH_JSON))
    //        return true;
    //}

    //Clean up
    closeProject();
    QDir dir1(strPath);
    dir1.removeRecursively();

    if (!dir.exists(strPath) && !dir.mkdir(strPath))
    {
        qDebug() << "Cannot make dir: " << strPath;
        return false;
    }

    TabProjInfo pi = {};
    pi.strWorkingPath = strPath;

    m_projInfo = pi;
    return true;
}

bool TABProject::openProject(QString strProjFile)
{
    if (strProjFile.isEmpty())
        return false;

    closeProject();

    //TODO unpack project file to temp dir
    QString tempPath = getTempPath();
    QTPF tpf;
    if (!tpf.open(strProjFile, QTPF::OF_READ)
        || !tpf.releaseToDir(tempPath))
        return false;;

    //read config from json
    if (!openJson(tempPath + PROJ_PATH_JSON))
        return false;

    m_projFile = strProjFile;

    if (!m_bIsPreviewProject)
        m_history.add(m_projFile, m_uuid);
    return true;
}

bool TABProject::openJson(QString strJsonFile)
{
    if (strJsonFile.isEmpty())
        return false;

    closeProject();

    //read config from json
    QFile file(strJsonFile);
    file.open(QIODevice::ReadOnly);
    QByteArray arr = file.readAll();
    file.close();

    QJsonDocument jsDoc = QJsonDocument::fromJson(arr);
    if (jsDoc.isNull() || jsDoc.isEmpty())
        return false;

    m_projInfo.strWorkingPath = QFileUtil::getFilePath(strJsonFile);

    QJsonObject jsRoot = jsDoc.object();
    m_nImgID = jsRoot["ImgId"].toInt();
    m_projInfo.strCoverImg = jsRoot["CoverImg"].toString();
    m_projInfo.strMusicInfo = jsRoot["MusicInfo"].toString();
    m_projInfo.strMusicTitle = jsRoot["MusicTitle"].toString();
    m_projInfo.strSingerImg = jsRoot["SingerImg"].toString();
    m_projInfo.strSingerInfo = jsRoot["SingerInfo"].toString();
    m_projInfo.strSingerName = jsRoot["SingerName"].toString();
    m_projInfo.beatPerMinute = jsRoot["BeatPerMinute"].toInt();
    m_projInfo.beatPerSection = jsRoot["BeatPerSection"].toInt();
    m_bIsPreviewProject = jsRoot["Preview"].toBool();

    QJsonArray jsImgs = jsRoot["imgs"].toArray();
    for (auto it : jsImgs)
        loadImage(it.toString());

    QJsonArray jsTabs = jsRoot["tabs"].toArray();
    for (auto it : jsTabs)
    {
        QJsonObject jsTab = it.toObject();
        TabLinePtr tl = json2TabLine(jsTab);
        addTabLine(tl);
    }

    setSingerImg(m_projInfo.strSingerImg);
    setCoverImg(m_projInfo.strCoverImg);
    m_tlModel.notifyChanged(0);
    return true;
}

bool TABProject::saveProject(QString strProjFile, bool bTemp)
{
    if (strProjFile.isEmpty())
        return false;

    //save config to json
    QJsonDocument jsDoc;
    QJsonObject jsRoot;
    jsRoot.insert("ImgId", m_nImgID);
    jsRoot.insert("CoverImg", m_projInfo.strCoverImg);
    jsRoot.insert("MusicInfo", m_projInfo.strMusicInfo);
    jsRoot.insert("MusicTitle", m_projInfo.strMusicTitle);
    jsRoot.insert("SingerImg", m_projInfo.strSingerImg);
    jsRoot.insert("SingerInfo", m_projInfo.strSingerInfo);
    jsRoot.insert("SingerName", m_projInfo.strSingerName);
    jsRoot.insert("BeatPerMinute", m_projInfo.beatPerMinute);
    jsRoot.insert("BeatPerSection", m_projInfo.beatPerSection);
    jsRoot.insert("Preview", bTemp);

    QJsonArray jsImgs;
    for (auto it : m_projInfo.imgList)
        jsImgs.append(it);
    jsRoot.insert("imgs", jsImgs);

    QJsonArray jsTabs;
    for (auto it : m_projInfo.tabLines)
    {
        QJsonObject jsTab = tabLine2Json(it);
        jsTabs.append(jsTab);
    }
    jsRoot.insert("tabs", jsTabs);

    jsDoc.setObject(jsRoot);

    QByteArray arrDoc = jsDoc.toJson(QJsonDocument::Indented);
    QFile file(m_projInfo.strWorkingPath + PROJ_PATH_JSON);
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << arrDoc;
    file.close();

    //pack project dir and save to a single file
    QTPF tpf;
    if (!tpf.open(strProjFile, QTPF::OF_WRITE))
        return false;

    tpf.appendFile(TPFHT_CONFIG, m_projInfo.strWorkingPath + PROJ_PATH_JSON);
    tpf.appendFile(TPFHT_SINGER_IMAGE, m_projInfo.strWorkingPath + PROJ_PATH_SINGER);
    tpf.appendFile(TPFHT_COVER_IMAGE, m_projInfo.strWorkingPath + PROJ_PATH_COVER);
    for (auto it : m_projInfo.imgList)
    {
        tpf.appendImage(m_projInfo.strWorkingPath + it);
    }

    if (!bTemp)
    {
        m_projFile = strProjFile;
        m_history.add(m_projFile, m_uuid);
    }
    return true;
}

bool TABProject::closeProject()
{
    if(!m_projFile.isEmpty() && !m_bIsPreviewProject)
        m_history.add(m_projFile, m_uuid);

    TabProjInfo pi = {};
    m_projInfo = pi;
    m_imgMap.clear();
    m_imgPreviewMap.clear();
    m_projFile.clear();
    m_tlModel.notifyChanged(0);
    return true;
}

QString TABProject::currentProject()
{
    return m_projFile;
}

bool TABProject::addImage(QString strImg, QString* pStrImgInProject)
{
    if (strImg.isEmpty())
        return false;

    //Copy to project dir
    QString strUniqueName = QString::asprintf("%lld", m_nImgID++) + QFileUtil::getFileExt(strImg);
    QString strNewFile = m_projInfo.strWorkingPath + strUniqueName;
    QFile::remove(strNewFile);
    QFile::copy(strImg, strNewFile);

    if (!loadImage(strUniqueName))
        return false;

    if (pStrImgInProject) *pStrImgInProject = strUniqueName;
    m_imgModel.notifyChanged(getImageCount()-1);
    m_prevImgModel.notifyChanged(getImageCount()-1);
    return true;
}

bool TABProject::insertImage(QString strImg, int index, QString *pStrImgInProject)
{
    if (strImg.isEmpty())
        return false;

    //Copy to project dir
    QString strUniqueName = QString::asprintf("%lld", m_nImgID++) + QFileUtil::getFileExt(strImg);
    QString strNewFile = m_projInfo.strWorkingPath + strUniqueName;
    QFile::remove(strNewFile);
    QFile::copy(strImg, strNewFile);

    if (!loadImage(strUniqueName, index))
        return false;

    if (pStrImgInProject) *pStrImgInProject = strUniqueName;
    m_imgModel.notifyChanged(index);
    m_prevImgModel.notifyChanged(index);
    return true;
}

bool TABProject::removeImage(QString strImg)
{
    if (strImg.isEmpty())
        return false;

    auto it = m_imgMap.find(strImg);
    if (it != m_imgMap.end()) m_imgMap.erase(it);

    auto it2 = m_imgPreviewMap.find(strImg);
    if (it2 != m_imgPreviewMap.end()) m_imgPreviewMap.erase(it2);

    for (auto it3 = m_projInfo.imgList.begin(); it3 != m_projInfo.imgList.end();)
    {
        if ((*it3) == strImg)
            it3 = m_projInfo.imgList.erase(it3);
        else
            it3++;
    }

    for (auto it3 = m_projInfo.tabLines.begin(); it3 != m_projInfo.tabLines.end();)
    {
        if ((*it3)->strImg == strImg)
            it3 = m_projInfo.tabLines.erase(it3);
        else
            it3++;
    }

    QFile::remove(strImg);
    m_tlModel.notifyChanged(-1);
    m_imgModel.notifyChanged(-1);
    m_prevImgModel.notifyChanged(-1);
    return true;
}

bool TABProject::removeImage(int index)
{
    if (index >= m_projInfo.imgList.size())
        return false;

    return removeImage(m_projInfo.imgList.at(index));
}

bool TABProject::moveUpImage(QString strImg)
{
    if (strImg.isEmpty())
        return false;

    if (m_projInfo.imgList.size() <= 1 || m_projInfo.imgList.front() == strImg)
        return true;

    auto first = m_projInfo.imgList.begin();
    for (auto second = first+1; second != m_projInfo.imgList.end(); first++, second++)
    {
         if (*second == strImg)
         {
             auto temp = *first;
             *first = *second;
             *second = temp;
             return true;
         }
    }

    return false;
}

bool TABProject::moveDownImage(QString strImg)
{
    if (strImg.isEmpty())
        return false;

    if (m_projInfo.imgList.size() <= 1 || m_projInfo.imgList.back() == strImg)
        return true;

    auto first = m_projInfo.imgList.begin();
    for (auto second = first+1; second != m_projInfo.imgList.end(); first++, second++)
    {
         if (*first == strImg)
         {
             auto temp = *first;
             *first = *second;
             *second = temp;
             return true;
         }
    }

    return false;
}

bool TABProject::moveImage(int indexFrom, int indexTo)
{
    int listSize = m_projInfo.imgList.size();
    if (indexFrom >= listSize)
        return false;

    QString strImg = m_projInfo.imgList.at(indexFrom);
    m_projInfo.imgList.removeAt(indexFrom);
    if (indexTo < 0 || indexTo > listSize)
    {
        m_projInfo.imgList.append(strImg);
        m_imgModel.notifyChanged(indexFrom, listSize - indexFrom);
        m_prevImgModel.notifyChanged(indexFrom, listSize - indexFrom);
    }
    else
    {
        m_projInfo.imgList.insert(indexTo, strImg);
        m_imgModel.notifyChanged(std::min(indexFrom, indexTo), listSize - indexFrom);
        m_prevImgModel.notifyChanged(std::min(indexFrom, indexTo), listSize - indexFrom);
    }
    return true;
}

QString TABProject::getImageName(int idx)
{
    if (idx > m_projInfo.imgList.size())
        return "";

    return m_projInfo.imgList.at(idx);
}

QImagePtr TABProject::getImage(QString strImg)
{
    if (strImg.isEmpty())
        return nullptr;

    auto it = m_imgMap.find(strImg);
    return it == m_imgMap.end() ? nullptr : *it;
}

QImagePtr TABProject::getImage(int idx)
{
    if (idx > m_projInfo.imgList.size())
        return nullptr;

    return getImage(m_projInfo.imgList[idx]);
}

QImagePtr TABProject::getImagePreview(QString strImg)
{
    if (strImg.isEmpty())
        return nullptr;

    auto it = m_imgPreviewMap.find(strImg);
    return it == m_imgPreviewMap.end() ? nullptr : *it;
}

QImagePtr TABProject::getImagePreview(int idx)
{
    if (idx > m_projInfo.imgList.size())
        return nullptr;

    return getImagePreview(m_projInfo.imgList[idx]);
}

QImagePtr TABProject::getSingerImage()
{
    return getImage(PROJ_PATH_SINGER);
}

QImagePtr TABProject::getCoverImage()
{
    return getImage(PROJ_PATH_COVER);
}

QList<QString> TABProject::getAllImage()
{
    return m_projInfo.imgList;
}

int TABProject::getImageCount()
{
    return m_projInfo.imgList.size();
}

bool TABProject::addTabLine(TabLinePtr tl)
{
    if (tl.isNull())
        return false;

    m_projInfo.tabLines.append(tl);
    m_tlModel.notifyChanged(m_projInfo.tabLines.count()-1);
    return true;
}

bool TABProject::addTabLine(TabLineList tl)
{
    if (tl.isEmpty())
        return false;

    int idx = m_projInfo.tabLines.count();
    m_projInfo.tabLines.append(tl);
    m_tlModel.notifyChanged(idx, tl.size());

    return true;
}

TabLinePtr TABProject::getTabLineAt(int index)
{
    if (index < 0 || index >= m_projInfo.tabLines.size())
        return nullptr;

    return m_projInfo.tabLines[index];
}

TabLineList TABProject::getTabLineList()
{
    return m_projInfo.tabLines;
}

int TABProject::getTabLineCount()
{
    return m_projInfo.tabLines.size();
}

bool TABProject::insertTabLine(int idx, TabLinePtr tl)
{
    if (tl.isNull())
        return false;

    m_projInfo.tabLines.insert(idx, tl);
    m_tlModel.notifyChanged(idx);
    return true;
}

bool TABProject::insertTabLine(int idx, TabLineList tl)
{
    if (tl.isEmpty())
        return false;

    int i = idx;
    for (auto it : tl)
        insertTabLine(i++, it);

    m_tlModel.notifyChanged(idx, tl.size());
    return true;
}

bool TABProject::removeTabLine(int idx)
{
    if (idx >= m_projInfo.tabLines.size())
        return false;

    m_projInfo.tabLines.erase(m_projInfo.tabLines.begin() + idx);
    m_tlModel.notifyChanged(idx, m_projInfo.tabLines.size()-idx);
    return true;
}

bool TABProject::removeTabLine(TabLinePtr tl)
{
    for (auto it = 0; it < m_projInfo.tabLines.count(); ++it)
    {
        if (m_projInfo.tabLines[it] == tl)
        {
            m_projInfo.tabLines.erase(m_projInfo.tabLines.begin() + it);
            m_tlModel.notifyChanged(it, m_projInfo.tabLines.size()-it);
            return true;
        }
    }

    return false;
}

bool TABProject::moveLeftTabLine(TabLinePtr tl)
{
    if (m_projInfo.tabLines.size() < 2)
        return true;

    int idx=0;
    auto prev = m_projInfo.tabLines.begin();
    auto curr = prev+1;
    for (; curr != m_projInfo.tabLines.end(); ++curr, ++prev, ++idx)
    {
        if ((*curr) == tl)
        {
            *curr = *prev;
            *prev = tl;
            m_tlModel.notifyChanged(idx);
            return true;
        }
    }

    return false;
}

bool TABProject::moveRightTabLine(TabLinePtr tl)
{
    if (m_projInfo.tabLines.size() < 2)
        return true;

    int idx=0;
    auto curr = m_projInfo.tabLines.begin();
    auto next = curr+1;
    for (; next != m_projInfo.tabLines.end(); ++curr, ++next, ++idx)
    {
        if ((*curr) == tl)
        {
            *curr = *next;
            *next = tl;
            m_tlModel.notifyChanged(idx);
            return true;
        }
    }

    return false;
}

QSize TABProject::getTabLineSizeH(int padding)
{
    int w=0, h=0;
    for (auto tl : m_projInfo.tabLines)
    {
        h = std::max(h, tl->rcPos.height() + tl->rcOffset.top());
        w += tl->rcPos.width() + padding;
    }

    return QSize(w, h);
}

QSize TABProject::getTabLineSizeV(int padding)
{
    int w=0, h=0;
    for (auto tl : m_projInfo.tabLines)
    {
        h += tl->rcPos.height() + tl->rcOffset.top() + padding;
        w = std::max(w, tl->rcPos.width());
    }

    return QSize(w, h);
}

QSize TABProject::getTabLineFixedSizeV()
{
    int w=0, h=INT_MAX;
    for (auto tl : m_projInfo.tabLines)
    {
        int w1 = tl->rcPos.width();
        if (w1 > w)
        {
            w = w1;
            h = tl->rcPos.height();//std::min(h, tl->rcPos.height());
        }
    }

    if (h == INT_MAX)
        h = 0;

    return QSize(w, h);
}

TabLinePtr TABProject::findTabLineByTime(double t)
{
    //一个小节的时间
    double timePerSection = getSecondPerSection();
    //找到最后一个时间小于t的小节，返回
    int section = 0;
    for (auto it : m_projInfo.tabLines)
    {
        if ((section + it->sections) * timePerSection > t)
            return it;

        section += it->sections;
    }
    //nothing found
    return nullptr;
}

void TABProject::setMusicTitle(QString strMusicTitle)
{
    m_projInfo.strMusicTitle = strMusicTitle;
}

QString TABProject::getMusicTitle()
{
    return m_projInfo.strMusicTitle;
}

void TABProject::setMusicInfo(QString strMusicInfo)
{
    m_projInfo.strMusicInfo = strMusicInfo;
}

QString TABProject::getMusicInfo()
{
    return m_projInfo.strMusicInfo;
}

void TABProject::setSingerName(QString strSingerName)
{
    m_projInfo.strSingerName = strSingerName;
}

QString TABProject::getSingerName()
{
    return m_projInfo.strSingerName;
}

bool TABProject::setSingerImg(QString strImg)
{
    if (strImg.isEmpty())
        return false;

    //Copy to project dir
    QString strNewFile;
    QImagePtr pImg;
    if (strImg != PROJ_PATH_SINGER)
    {
        strNewFile = m_projInfo.strWorkingPath + PROJ_PATH_SINGER;
        QFile::remove(strNewFile);
        //QFile::copy(strImg, strNewFile);
        pImg.reset(new QImage(strImg));
        if (pImg->isNull())
            return false;

        pImg->save(strNewFile, "PNG");
    }
    else
    {
        pImg.reset(new QImage(m_projInfo.strWorkingPath + PROJ_PATH_SINGER));
        if (pImg->isNull())
            return false;
    }

    m_projInfo.strSingerImg = PROJ_PATH_SINGER;
    m_imgMap[PROJ_PATH_SINGER] = pImg;
    return true;
}

QString TABProject::getSingerImg()
{
    return m_projInfo.strSingerImg;
}

void TABProject::setSingerInfo(QString strSingerInfo)
{
    m_projInfo.strSingerInfo = strSingerInfo;
}

QString TABProject::getSingerInfo()
{
    return m_projInfo.strSingerInfo;
}

bool TABProject::setCoverImg(QString strImg)
{
    if (strImg.isEmpty())
        return false;

    //Copy to project dir
    QString strNewFile;
    QImagePtr pImg;
    if (strImg != PROJ_PATH_COVER)
    {
        strNewFile = m_projInfo.strWorkingPath + PROJ_PATH_COVER;
        QFile::remove(strNewFile);
        //QFile::copy(strImg, strNewFile);
        pImg.reset(new QImage(strImg));
        if (pImg->isNull())
            return false;

        pImg->save(strNewFile, "PNG");
    }
    else
    {
        pImg.reset(new QImage(m_projInfo.strWorkingPath + PROJ_PATH_COVER));
        if (pImg->isNull())
            return false;
    }

    m_projInfo.strCoverImg = PROJ_PATH_COVER;
    m_imgMap[PROJ_PATH_COVER] = pImg;
    return true;
}

QString TABProject::getCoverImg()
{
    return m_projInfo.strCoverImg;
}

void TABProject::setBeatPerMinute(int beat)
{
    m_projInfo.beatPerMinute = beat;
}

int TABProject::getBeatPerMinute()
{
    return m_projInfo.beatPerMinute;
}

int TABProject::getBeatPerMinuteAdjusted()
{
    return m_projInfo.beatPerMinute + m_adjustedBeat;
}

void TABProject::setBeatPerSection(int beat)
{
    m_projInfo.beatPerSection = beat;
}

int TABProject::getBeatPerSection()
{
    return m_projInfo.beatPerSection;
}

int TABProject::getSectionCount()
{
    int count = 0;
    for (auto it : m_projInfo.tabLines)
    {
        count += it->sections;
    }

    return count;
}

double TABProject::getSecondPerSection()
{
    //已知：一个小节节拍数(4/4、6/8之类的分母)，每分钟节拍数(节拍器的读数)
    //那么可以算出一个小节的时间
    return m_projInfo.beatPerSection * (60.0 / (m_projInfo.beatPerMinute + m_adjustedBeat));
}

double TABProject::getSecondPerBeat()
{
    return (60.0 / m_projInfo.beatPerMinute);
}

double TABProject::getSecondAtTabLine(int iTabLine)
{
    if (iTabLine > m_projInfo.tabLines.size())
        return 0;

    return m_projInfo.tabLines.at(iTabLine)->sections * getSecondPerSection();
}

double TABProject::getSecondOfThisSong()
{
    return getSectionCount() * getSecondPerSection();
}

QJsonObject TABProject::tabLine2Json(TabLinePtr tl)
{
    QJsonObject jsTab;
    jsTab.insert("Blur", QStringUtil::rect2String(tl->rcBlur));
    jsTab.insert("Offset", QStringUtil::rect2String(tl->rcOffset));
    jsTab.insert("Pos", QStringUtil::rect2String(tl->rcPos));
    jsTab.insert("sections", tl->sections);
    jsTab.insert("Img", tl->strImg);
    return jsTab;
}

TabLinePtr TABProject::json2TabLine(QJsonObject jsTab)
{
    TabLinePtr tl(new TabLine);
    tl->strImg = jsTab["Img"].toString();
    if (getImage(tl->strImg).isNull())
        return nullptr;

    tl->rcBlur = QStringUtil::string2Rect(jsTab["Blur"].toString());
    tl->rcOffset = QStringUtil::string2Rect(jsTab["Offset"].toString());
    tl->rcPos = QStringUtil::string2Rect(jsTab["Pos"].toString());
    tl->sections = jsTab["sections"].toInt();
    return tl;
}

QAbstractItemModel *TABProject::getTabLineModel()
{
    return &m_tlModel;
}

QAbstractItemModel *TABProject::getImageModel()
{
    return &m_imgModel;
}

QAbstractItemModel *TABProject::getPreviewImageModel()
{
    return &m_prevImgModel;
}

QAbstractTableModel *TABProject::getHistoryModel()
{
    return static_cast<QAbstractTableModel*>(&m_history);
}

void TABProject::notifyModelChanged()
{
    m_tlModel.notifyChanged(0, getTabLineCount());
    m_imgModel.notifyChanged(0, getImageCount());
    m_prevImgModel.notifyChanged(0, getImageCount());
}

int TABProject::adjustSpeed(int beatAdjusted)
{
    int beat = m_projInfo.beatPerMinute + beatAdjusted;
    if (beat <= 0)
        m_adjustedBeat = -m_projInfo.beatPerMinute;
    else if (beat >= MAX_BEAT_PER_MINUTE)
        m_adjustedBeat = MAX_BEAT_PER_MINUTE - m_projInfo.beatPerMinute;
    else
        m_adjustedBeat = beatAdjusted;

    return m_adjustedBeat;
}

int TABProject::getAdjustSpeed()
{
    return m_adjustedBeat;
}

bool TABProject::loadImage(QString strFileName, int index)
{
    if (m_imgMap.find(strFileName) != m_imgMap.end())
        return true;

    QImagePtr pImg(new QImage(m_projInfo.strWorkingPath + strFileName));
    if (pImg->isNull())
        return false;

    QImagePtr pImgPreview(new QImage(*pImg));
    if (pImgPreview->isNull())
        return false;

    QSize szMaxPreview(MAX_PREVIEW_ITEM_WIDTH, MAX_PREVIEW_ITEM_HEIGHT);
    if (pImgPreview->width() * pImgPreview->height() > szMaxPreview.width() * szMaxPreview.height())
        *pImgPreview = pImgPreview->scaled(szMaxPreview, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_imgPreviewMap[strFileName]=pImgPreview;
    m_imgMap[strFileName]=pImg;
    if (index == -1 || index >= m_projInfo.imgList.size())
        m_projInfo.imgList.append(strFileName);
    else
        m_projInfo.imgList.insert(index, strFileName);
    return true;
}

QString TABProject::getTempPath(bool bCreate)
{
    QString strPath = QDir::homePath() + PROJ_PATH_ROOT;
    if (bCreate)
        m_uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);

    if (m_uuid.isEmpty())
        return "";

    strPath.append(m_uuid);
    strPath.append("/");

    QDir dir;
    if (bCreate && !dir.exists(strPath) && !dir.mkpath(strPath))
    {
        qDebug() << "Cannot make dir: " << strPath;
        return "";
    }
    return strPath;
}

