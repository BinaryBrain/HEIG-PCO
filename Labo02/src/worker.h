#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QVector>
#include <stdint.h>

class Worker: public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE;

public:
    Worker(uint64_t nbToCompute, QString charset, QString hash, QString salt, unsigned int nbChars, QString currentPasswordString, QVector<unsigned int> currentPasswordArray);
    ~Worker();

private:
    uint64_t nbToCompute;
    QString charset;
    QString hash;
    QString salt;
    unsigned int nbChars;
    QString currentPasswordString;
    QVector<unsigned int> currentPasswordArray;

signals:
    void progressUpdated(double newValue);
    void passwordFoundSignal(const QString &password);
};

#endif // WORKER_H
