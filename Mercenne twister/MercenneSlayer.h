#ifndef MERCENNE_SLAYER_H
#define MERCENNE_SLAYER_H

#include <string>

class Matrice;

typedef uint32_t W;

using namespace std;

class MercenneSlayer
{
    public:
        MercenneSlayer(short w, short n, short m, short r, W a, short u, W d, short s, W b, short t, W c, short l);
        void sEtats(double nombresAleatoires[]);
        void sEtatSCrypy(uint8_t randints[]);
        void sEtatsSEtats(string pathEtats);
        void sEtats(W nombresAleatoires[]);      //Utilise n nombre generees par Mercenne twister pour determiner notre etat interne
        int randint(int a, int b);
        double randomPrecedent();
        void inverserMelange();
        W extrairePrecedent();
        double random();                         //Resultat de random()
        void melanger();                         //Melange les etats, appeles tous les n extractions
        W extraire();                            //Entier de W bits donne par l'etat actuel, utilise iExtraire
        ~MercenneSlayer();

        Matrice matriceExtraction() const;                  //Matrice w x w representant la fonction d'extraction
        W iExtraire(W nombreInitial) const;                 //Entier de w bits correspondant a l'etat nombreInitial
        Matrice matriceGeneraleCrypy() const;               //Matrice M (n*w) x (n*w) telle que : M*etatInterne = (IV generes par Crypy mis bout a bout)
        double random(W etat1, W etat2) const;              //Resultat de random() de python en partant de deux etat
        W* doubleToW(double nombreAleatoire) const;         //53 bits des deux entiers de 32 bits utilises pour creer le flottant par random()
        W etatSuivant(W etats[], short nEtat) const;        //Fonction intermediaire utilisee par la fonction melanger
        W inverserExtraction(W nombreAleatoire) const;      //Ieme etat utilise pour creer l'entier de w bits
        int randint(W etat1, W etat2, int a, int b) const;
        W* intToW(int nombreAleatoire, int a, int b) const;

        static void tester();

    private:
        W a;
        W d;
        W b;
        W c;
        short w;
        short n;
        short m;
        short r;
        short u;
        short s;
        short t;
        short l;
        W *etats;
        W masqueDroit;
        W masqueGauche;
        short position;
};

template<typename T>
T inverserGauche(T x, T masque, short decalage)
{
    short const fin = sizeof(T)*8-decalage;
    
    for (short i=0; i<fin; i++)
    {
        if (masque & (1 << (i+decalage))) x = x ^ ((x & (1 << i)) << decalage);
    }
    
    return x;
} 

template<typename T>
T inverserDroite(T x, T masque, short decalage)
{
    short const debut = sizeof(T)*8-decalage;
    
    for (short i=debut; i >= 0; i--)
    {
        if (masque & (1 << (i+decalage))) x = x ^ ((x & (1 << (i+decalage))) >> decalage);
    }
    
    return x;
}

#endif //MERCENNE_SLAYER_H
