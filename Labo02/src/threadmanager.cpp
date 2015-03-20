#include <QVector>
#include <stdint.h>
#include <cmath>
#include <stdio.h>

#include "worker.h"
#include "threadmanager.h"

/* MESURES OBTENUES
1 Thread: 19847ms
2 Threads: 10297ms
4 Threads: 5176ms
8 Threads: 4115ms
16 Threads: 4138ms

Ces mesures étaient tout à fait prévisibles. Le processeur utilié a 4 coeurs hyperthreadés.
Il est donc normal que jusqu'à 4 threads les performances soient multipliées par le nombre
de threads, puisque chaque thread s'exécute sur un coeur physique.
Il y a encore un légé gain à 8 threads grace à l'hyperthreading, mais largement moins significatif.
La raison pour laquelle 16 threads sont plus lents que 8 est que l'overhead requis pour gérer les
threads devient trop important, et au final seulement 8 threads peuvent s'exécuter en vrai parallélisme.

PROTOCOLE DE TEST
Nous avons simplement lancé le programme plusieures fois avec un nombre de threads différent. 
Pour vérifier que les threads étaient correctement gérés nous avons utilisé le moniteur de ressources
pour afficher l'utilisation du CPU et le nombres de threads lancés par l'application.
*/

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow(
    long long unsigned int number,
    long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
    {
        return 1;
    }

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
    {
        number *= num;
    }

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
QString ThreadManager::startHacking(QString charset, QString salt, QString hash, unsigned int nbChars, unsigned int nbThreads)
{
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
    for (unsigned int i = 0; i < nbThreads; i++)
    {
        /*
         * On initialise le premier mot de passe à tester courant en le remplissant
         * de nbChars fois du premier carcatère de charset
         */
        currentPasswordString.fill(charset.at(std::ceil((charset.size() * i) / (double)nbThreads)), nbChars);
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

    //on attend que les workers se termninent gracieusement
    for (unsigned int i = 0; i < nbThreads; i++)
    {
        workers.at(i)->wait();
    }

    return password;
}

void ThreadManager::incrementProgressBarTransmit(double percentComputed)
{
    emit incrementPercentComputed(percentComputed / nbThreads);
}

void ThreadManager::passwordFound(const QString &password)
{
    this->password = password;
}
