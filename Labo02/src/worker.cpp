#include <QCryptographicHash>
#include <QVector>

#include "worker.h"


Worker::Worker(uint64_t nbToCompute, QString charset, QString hash, QString salt,
               unsigned int nbChars, QString currentPasswordString, QVector<unsigned int> currentPasswordArray)
{
    this->nbToCompute = nbToCompute;
    this->charset = charset;
    this->hash = hash;
    this->salt = salt;
    this->currentPasswordString = currentPasswordString;
    this->currentPasswordArray = currentPasswordArray;
    this->nbChars = nbChars;
}

Worker::~Worker()
{

}

void Worker::run()
{
    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);

    /*
     * Hash du mot de passe à tester courant
     */
    QString currentHash;

    QString result;
    /* ... here is the expensive or blocking operation ... */

    uint64_t nbComputed = 0;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars       = charset.length();

    /*
     * Tant qu'on a pas tout essayé...
     */
    while (nbComputed < nbToCompute) {
        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         */
        if (currentHash == hash)
        { emit passwordFoundSignal(currentPasswordString); }

        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */
        if (!(nbComputed % 1000))
        { emit progressUpdated((double) 1000 / nbToCompute); }

        /*
         * On récupère le mot de pass à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArra" comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */
        unsigned int i = 0;

        while (i < (unsigned int)currentPasswordArray.size())
        {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars)
            {
                currentPasswordArray[i] = 0;
                i++;
            }
            else
            { break; }
        }

        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0; i < nbChars; i++)
        { currentPasswordString[i]  = charset.at(currentPasswordArray.at(i)); }

        nbComputed++;
    }
}

