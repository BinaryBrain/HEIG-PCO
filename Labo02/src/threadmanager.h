#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>

#include "worker.h"

class ThreadManager: public QObject
{
    Q_OBJECT
private:
    unsigned int nbThreads;
    QString password;
    std::vector<Worker*> workers;

public:
    ThreadManager(QObject *parent);
    QString startHacking(QString charset, QString salt, QString hash, unsigned int nbChars, unsigned int nbThreads);

signals:
    void incrementPercentComputed(double percentComputed);

public slots:
    void incrementProgressBarTransmit(double percentComputed);
    void passwordFound(const QString &password);
};

#endif // THREADMANAGER_H
