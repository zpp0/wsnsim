/***********************************************
 *
 * File: log.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/ 

#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QHash>

#include "nodeEvent.h"
#include "logevent.h"

/*
  класс предназначен для записи файла логов
 */

class log
{
public:
    
    // уровни подробности вывода лога
    enum Loglevel {
        MAIN = 1,
        IMPORTANT,
        DEBUG,
        ALL
    };

    enum LogFormat {
        BINARY = 1,
        TEXT,
    };
    
    // очищает устаревшие файлы с логами, ошибками и чем-то еще
    static void init(QString currentProjectPath, LogFormat format);
    static void uninit();
    
    // записать текст в файл лога
    // принимает указатель на объект, в котором реализованы методы записи в лог,
    // строку с текстом
    // и опционально уровень подробности вывода, на котором будет выводится эта запись
    static void writeLog(LogEvent &event, Loglevel level = log::MAIN);
    
    // записать текст ошибки скриптов в файл ошибок
    // принимает событие узла и текст ошибки
    static void writeError(nodeEvent* author, QString text);

    // static void unInit();
    
    //соответствие между кодами объектов и их именами для отображения пользователю  в логе
    static QHash<QString,QString> namesInfo;

private:
    //имя файла журнала
    static QString m_logFile;
    // имя файла ошибок
    static QString m_errorLogFile;

    static QFile* m_log;

    static QDataStream* m_logDataStream;
    static QTextStream* m_logTextStream;
    static LogFormat m_format;
    
    static QString m_currentProjectPath;

    //УСТАНОВКА УРОВНЯ ОТЛАДКИ
    const static int debugLevel = 2;
};

#endif // LOG_H
