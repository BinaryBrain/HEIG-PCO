#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QVector>
#include <stdint.h>

/*
Cette classe est un thread qui ex�cute une partie de la boucle qui se trouvait dans startHacking.
Le constructeur prend en argument toutes les variables n�cessaires � l'ex�cution du code sans
d�pendre de variables externes, ce qui am�liore les performances car il n'y a pas besoin de
g�rer d'acc�s synchronis�s � ces variables.
Chaque Worker effectue nbToCompute / nbThreads it�rations.
La communication avec le Thread Manager est g�r�e par des signals/slots.
Pour controler l'arr�t des threads, le ThreadManager n'as qu'a �mettre un signal sur exitThreadSlot().
*/
class Worker : public QThread
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
