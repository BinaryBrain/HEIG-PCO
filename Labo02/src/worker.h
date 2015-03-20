#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QVector>
#include <stdint.h>

/*
Cette classe est un thread qui exécute une partie de la boucle qui se trouvait dans startHacking.
Le constructeur prend en argument toutes les variables nécessaires à l'exécution du code sans
dépendre de variables externes, ce qui améliore les performances car il n'y a pas besoin de
gérer d'accès synchronisés à ces variables.
Chaque Worker effectue nbToCompute / nbThreads itérations.
La communication avec le Thread Manager est gérée par des signals/slots.
Pour controler l'arrêt des threads, le ThreadManager n'as qu'a émettre un signal sur exitThreadSlot().
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
