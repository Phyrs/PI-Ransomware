#ifndef MERCENNE_SLAYER_H
#define MERCENNE_SLAYER_H

class MercenneSlayer
{
    public:
        MercenneSlayer(short w, short n, short m, short r, int a, short u, int d, short s, int b, short t, int c, short l);
        void sEtats(int nombresAleatoires[]);
        double random();
        void melanger();
        int extraire();
        ~MercenneSlayer();

        int iExtraire(int nombreInitial) const;
        int etatSuivant(int etats[], int nEtat) const;
        int* doubleToEtats(double nombreAleatoire) const;
        int inverserExtraction(int nombreAleatoire) const;
        double iRandom(int nombreInitial1, int nombreInitial2) const;

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
