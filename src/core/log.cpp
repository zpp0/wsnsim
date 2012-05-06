/***********************************************
 *
 * File: log.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 **********************************************/

#include <iostream>

#include "simulator.h"
#include "log.h"
#include "env.h"

QString log::m_logFile="log.txt";
QString log::m_errorLogFile="error.txt";
// log::LogFormat log::m_format;
QHash<QString,QString> log::namesInfo;
QString log::m_currentProjectPath;
QFile* log::m_log;
log::LogFormat log::m_format;
QDataStream* log::m_logDataStream;
QTextStream* log::m_logTextStream;

void log::init(QString currentProjectPath, LogFormat format)
{
    qDebug() << "log path is" << currentProjectPath;
    
    m_currentProjectPath = currentProjectPath;
    
    m_format = format;

    qDebug() << "format file" << ((m_format == log::BINARY) ? "binary" : "text");
    //для начала, удалим файл логов для того, чтобы создать новый
    // удаляем старый файл лога
    m_log = new QFile(m_currentProjectPath + m_logFile);
    m_log->remove();

    // удаляем старый файл ошибок скриптов
    QFile errorFile(m_currentProjectPath + m_errorLogFile);
    errorFile.remove();

    if (!m_log->open(QIODevice::Append)) {
        std::cerr << "Error: cannot open log file " << (m_currentProjectPath + m_logFile).toStdString() << std::endl;
        exit(1);
    }

    if (format == log::BINARY)
        m_logDataStream = new QDataStream(m_log);
    else
        m_logTextStream = new QTextStream(m_log);
    
    // QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
}

void log::uninit()
{
    m_log->close();
}

void log::writeLog(LogEvent &event, Loglevel level)
{
    // m_logStream << event;
    switch (m_format)
    {
    case BINARY:
        (*m_logDataStream) << event;
        break;
    case TEXT:
        (*m_logTextStream) << event;
        break;
    }
}

// записать текст ошибки скриптов в файл ошибок
void log::writeError(nodeEvent* author, QString text)
{
    QFile file (m_currentProjectPath + m_errorLogFile);
    // открываем файл
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        // записываем текст ошибки
        out << text;
        out << "    on node " << author->eventNode->getMAC();
        out << endl;
        // закрываем
        file.close();
    }
}
