#include <QtDebug>
#include "filevolumeziparchive.h"

FileVolumeZipArchive::FileVolumeZipArchive(QObject* parent, QString zippath)
    : IFileVolume(parent)
    , m_reader(zippath)
    , m_zipfilepath(zippath)
{
    m_cnt = 0;
    m_current = "";

    if(!m_reader.exists())
        return;

    foreach(const QZipReader::FileInfo& info, m_reader.fileInfoList()) {
        if(info.isFile)
            m_filelist.append(info.filePath);
    }
    qSort(m_filelist);
    m_current = m_filelist[m_cnt];
}


bool FileVolumeZipArchive::nextFile()
{
    if(m_filelist.size() <= m_cnt+1)
        return false;
    m_current = m_filelist[++m_cnt];
    return true;
}

bool FileVolumeZipArchive::prevFile()
{
    if(m_cnt-1 < 0)
        return false;
    m_current = m_filelist[--m_cnt];
    return true;
}

bool FileVolumeZipArchive::findImageByIndex(int idx)
{
    if(idx < 0 || idx >= m_filelist.size())
        return false;
    m_cnt = idx;
    m_current = m_filelist[m_cnt];
    return true;
}

bool FileVolumeZipArchive::findImageByName(QString name)
{
    for(int i = 0; i < m_filelist.size(); i++) {
        if(name == m_filelist[i]) {
            m_cnt = i;
            m_current = name;
            return true;
        }
    }
    return false;
}

QPixmap FileVolumeZipArchive::loadImageByName(QString name)
{
    QPixmap ret = QPixmap();
    foreach(const QString& e, m_filelist) {
        if(name == e) {
            m_mutex.lock();
            QByteArray bytes = m_reader.fileData(name);
            m_mutex.unlock();

            ret.loadFromData(bytes);
            return ret;
        }
    }
    return ret;
}

int FileVolumeZipArchive::size()
{
    return m_filelist.size();
}

QString FileVolumeZipArchive::currentPath()
{
    return m_current;
}

QPixmap FileVolumeZipArchive::currentImage()
{
    if(m_cachedPath == currentPath()) {
        return m_cachedImage;
    }
    if(!m_currentCache.isFinished())
        m_currentCache.waitForFinished();

    return m_cachedImage = m_currentCache;

//    QByteArray bytes = m_reader.fileData(currentPath());
//    m_cachedImage = QPixmap();
//    m_cachedImage.loadFromData(bytes);
////    qDebug() << m_cachedImage << m_cachedImage.size();
//    return m_cachedImage;
}
