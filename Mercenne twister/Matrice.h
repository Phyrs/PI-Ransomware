#ifndef MATRICE_H
#define MATRICE_H

#include <string>

using namespace std;

class Matrice
{
    public:
        Matrice();
        Matrice(short tx, short ty);
        Matrice(Matrice const &A);
        Matrice(uint8_t const iElements[], short tx);                          //Matrice colonne
        Matrice& operator=(Matrice const &A);
        void operator*=(Matrice const &A);
        void operator+=(Matrice const &A);
        void set(short x, short y, short i);
        void coller(Matrice const &A, short x1, short y1, short x2, short y2);
        ~Matrice();

        short gTx() const;
        short gTy() const;
        string hexa() const;
        operator string() const;
        Matrice transposee() const;
        uint8_t get(short x, short y) const;
        Matrice inverser(Matrice const &iY) const;                  //Solution de X*A = iY
        Matrice operator*(Matrice const &A) const;
        Matrice operator+(Matrice const &A) const;
        Matrice bloc(short x1, short y1, short x2, short y2) const;

        static Matrice id(short tx);
        static Matrice masque(uint8_t const masque[], short tx);
        static Matrice decalageDroite(short tx, short decalage);
        static Matrice decalageGauche(short tx, short decalage);

    //private:
        short tx;           //Nombre de colonnes
        short ty;           //Nombre de lignes
        uint8_t **elements;
};

//Matrice binaire

#endif //MATRICE_H
