#include <QVector>
#include <stdint.h>

#include "worker.h"
#include "threadmanager.h"

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow (
        long long unsigned int number,
        long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{}
/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(QString charset, QString salt, QString hash, unsigned int nbChars, unsigned int nbThreads) {
    this->nbThreads = nbThreads;
    this->password = "";

    // Nous avons corrigé les types
    uint64_t nbToCompute;

    /*
     * Mot de passe à tester courant
     */
    QString currentPasswordString;

    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
    QVector<unsigned int> currentPasswordArray;

    /*
     * Calcul du nombre de hash à générer
     */
    nbToCompute = intPow(charset.length(), nbChars) / nbThreads;

    // Création des threads

    std::vector<Worker*> workers;

    for (unsigned int i = 0; i < nbThreads; i++) {
        /*
         * On initialise le premier mot de passe à tester courant en le remplissant
         * de nbChars fois du premier carcatère de charset
         */
        currentPasswordString.fill(charset.at((charset.size() * i) / nbThreads), nbChars);
        currentPasswordArray.fill((charset.size() * i) / nbThreads, nbChars);

        Worker* worker = new Worker(nbToCompute, charset, hash, salt, nbChars, currentPasswordString, currentPasswordArray);
        workers.push_back(worker);

        connect(
                    worker,
                    SIGNAL(progressUpdated(double)),
                    this,
                    SLOT(incrementProgressBarTransmit(double))
                    );

        connect(
                    worker,
                    SIGNAL(passwordFoundSignal(const QString)),
                    this,
                    SLOT(passwordFound(const QString))
                    );

        worker->start();
    }

    for (unsigned int i = 0; i < nbThreads; i++) {
        workers.at(i)->wait();
    }

    return password;
}

void ThreadManager::incrementProgressBarTransmit(double percentComputed) {
    emit incrementPercentComputed(percentComputed / nbThreads);
}

void ThreadManager::passwordFound(const QString &password) {
    this->password = password;
}
