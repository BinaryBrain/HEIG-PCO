#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QString>

class ThreadManager: public QObject
{
    Q_OBJECT
private:
    unsigned int nbThreads;

public:
    ThreadManager(QObject *parent);
    QString startHacking(QString charset, QString salt, QString hash, unsigned int nbChars, unsigned int nbThreads);

signals:
    void incrementPercentComputed(double percentComputed);

public slots:
    void incrementProgressBarTransmit(double percentComputed);
};

#endif // THREADMANAGER_H
