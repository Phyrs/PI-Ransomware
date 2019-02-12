#ifndef MERCENNE_SLAYER_H
#define MERCENNE_SLAYER_H

class MercenneSlayer
{
    public:
        MercenneSlayer(short w, short n, short m, short r, int a, short u, int d, short s, int b, short t, int c, short l);
        void sEtats(int nombresAleatoires[]); //Utilise une grande suite de nombre de 32 bits generees par Mercenne twister pour determiner notre etat interne
        double random();                      //Resultat de random()
        void melanger();                      //Melange les etats, appeles tous les n extractions
        int extraire();                       //Entier de 32 bits donne par l'etat actuel, utilise iExtraire
        ~MercenneSlayer();

        int iExtraire(int nombreInitial) const;                       //Entier de 32 bits correspondant a l'etat nombreInitial
        int etatSuivant(int etats[], int nEtat) const;                //Fonction intermediaire utilisee par la fonction melanger
        int* doubleToEtats(double nombreAleatoire) const;             //53 bits des deux entiers de 32 bits utilises pour creer le flottant par random()
        int inverserExtraction(int nombreAleatoire) const;            //Ieme etat utilise pour creer l'entier de 32 bits
        double iRandom(int nombreInitial1, int nombreInitial2) const; //Resultat de random() de python en partant de deux entiers de 32 bits

        static void tester();

    private:
        int a;
        int d;
        int b;
        int c;
        short w;
        short n;
        short m;
        short r;
        short u;
        short s;
        short t;
        short l;
        int *etats;
        short position;
        int masqueDroit;
        int masqueGauche;
};

#endif //MERCENNE_SLAYER_H
