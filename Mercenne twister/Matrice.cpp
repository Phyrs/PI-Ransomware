#include <fstream>
#include <iostream>
#include "Matrice.h"
#include <bits/stdc++.h> 

uint8_t xorChar(uint8_t x)
{
    uint8_t res = 0;
    for (short i=0; i<8; i++) res ^= (x & (1<<i)) >> i;

    return res;
}

Matrice::Matrice() : tx(1), ty(1), elements(new uint8_t*[1])
{
    elements[0] = new uint8_t[1];
}

Matrice::Matrice(Matrice const &A) : tx(1), ty(1), elements(new uint8_t*[1])
{
    elements[0] = new uint8_t[1];
    *this = A;
}

Matrice::Matrice(short tx, short ty) : tx(tx), ty(ty), elements(new uint8_t*[ty])
{
    short const iTx = (tx+7)/8;

    for (short y=0; y<ty; y++)
    {
        elements[y] = new uint8_t[iTx];
        for (short x=0; x<iTx; x++) elements[y][x] = 0;
    }
}

Matrice::Matrice(uint8_t const iElements[], short ty) : tx(1), ty(ty), elements(new uint8_t*[ty])
{
    for (short y=0; y<ty; y++)
    {
        elements[y] = new uint8_t[1];
        elements[y][0] = (iElements[y/8] & (0x80>>(y%8))) << (y%8);
    }
}

Matrice& Matrice::operator=(Matrice const &A)
{
    if (this == &A) return *this;

    for (short y=0; y<ty; y++) delete[] elements[y];
    delete[] elements;

    tx = A.tx;
    ty = A.ty;
    short const iTx = (tx+7)/8;
    elements = new uint8_t*[ty];

    for (short y=0; y<ty; y++)
    {
        elements[y] = new uint8_t[iTx];
        for (short x=0; x<iTx; x++) elements[y][x] = A.elements[y][x];
    }

    return *this;
}

void Matrice::operator*=(Matrice const &A)
{
    *this = *this*A;
}

void Matrice::operator+=(Matrice const &A)
{
    *this = *this+A;
}

void Matrice::coller(Matrice const &A, short x1, short y1, short x2, short y2)
{
    short const iX1 = x1/8;
    short const iX2 = x2/8;
    short const decalageDroite = x1%8;
    short const decalageGauche = 8-decalageDroite;

    uint8_t const avant = 0xFF << decalageGauche;
    uint8_t const apres1 = 0xFF << (8-(x2%8));
    uint8_t const apres2 = ~apres1;

    bool const isMorceauApres = x2%8;
    bool const is2MorceauxApres = (x2-x1)%8;

    for (short y=y1; y<y2; y++)
    {
        elements[y][iX1] = (elements[y][iX1] & avant) | (A.elements[y-y1][0] >> decalageDroite);

        for (short x=iX1+1; x<iX2; x++) elements[y][x] = (A.elements[y-y1][x-iX1-1] << decalageGauche) | (A.elements[y-y1][x-iX1] >> decalageDroite);

        if (isMorceauApres) 
        {
            elements[y][iX2] = ((A.elements[y-y1][iX2-iX1-1] << decalageGauche) & apres1) | (elements[y][iX2] & apres2);
            if (is2MorceauxApres) elements[y][iX2] |= (A.elements[y-y1][iX2-iX1] >> decalageDroite) & apres1;
        }
    }
}

void Matrice::set(short x, short y, short i)
{
    short const iX = x%8;
    elements[y][x/8] = (elements[y][x/8] & ~(0x80 >> iX)) | (i << (7-iX));
}

short Matrice::gTx() const
{
    return tx;
}

short Matrice::gTy() const
{
    return ty;
}

Matrice Matrice::operator*(Matrice const &A) const
{
    Matrice res(A.tx, ty);
    Matrice iA = A.transposee();

    short const iTm = (tx+7)/8;
    short const iTx = (A.tx+7)/8;

    for (short y=0; y<ty; y++)
    {
        for (short x=0; x<iTx; x++)
        {
            res.elements[y][x] = 0;

            for (short k=0; k<8; k++)
            {
                uint8_t iRes = 0;
                short const iX = x*8+k;

                if (iX < A.tx)
                {
                    for (short m=0; m<iTm; m++) iRes ^= elements[y][m] & iA.elements[iX][m];
                    res.elements[y][x] |= xorChar(iRes) << (7-k);
                }

            }
        }
    }

    return res;
}

Matrice Matrice::operator+(Matrice const &A) const
{
    Matrice res(tx, ty);
    short const iTx = (tx+7)/8;

    for (short y=0; y<ty; y++) for (short x=0; x<iTx; x++) res.elements[y][x] = this->elements[y][x]^A.elements[y][x];
    
    return res;
}

uint8_t Matrice::get(short x, short y) const
{
    short const iX = x%8;
    return (elements[y][x/8] & (0x80 >> iX)) >> (7-iX);
}

Matrice Matrice::transposee() const
{
    Matrice res(ty, tx);
    short const iTy = (ty+7)/8;

    for (short y=0; y<iTy; y++)
    {
        for (short x=0; x<tx; x++)
        {
            short const iX = x%8;
            res.elements[x][y] = 0;

            for (short i=0; i<8; i++)
            {
                short const iY = y*8+i;
                if (iY<ty) res.elements[x][y] |= ((elements[iY][x/8] & (0x80 >> iX)) << iX) >> i;
            }
        }
    }

    return res;
}

Matrice Matrice::inverser(Matrice const &iY) const
{
    int n = tx;
    Matrice Y = iY;
    Matrice X(1, n);
    Matrice matrice = *this;

    //On triangularise le système
    for (int i=0; i<n; i++) //On parcours les variables
    {
        if (matrice.get(i, i) == 0)
        {
            //On trouve une ligne ou le coefficient est non nul
            int j;
            for (j=i+1; j<n; j++)
            {
                if (matrice.get(i, j))
                {
                    //On inverse les lignes i et j
                    for (int k=0; k<n/8; k++)
                    {
                        uint8_t const inter = matrice.elements[i][k];
                        matrice.elements[i][k] = matrice.elements[j][k];    
                        matrice.elements[j][k] = inter;
                    }

                    uint8_t const inter = Y.elements[i][0];
                    Y.elements[i][0] = Y.elements[j][0];
                    Y.elements[j][0] = inter;
                    break;
                }
            }
            if (j==n) continue;
        }

        //On annule la ieme variable dans toutes les lignes en-dessous
        for (int j=i+1; j<n; j++)
        {
            if (matrice.get(i, j))
            {
                for (int k=0; k<n/8; k++) matrice.elements[j][k] ^= matrice.elements[i][k];
                Y.elements[j][0] ^= Y.elements[i][0];
            }
        }
    }

    //On resoud le systeme triangulaire
    for (int i=n-1; i>=0; i--)
    {
        X.elements[i][0] = Y.elements[i][0];
        for (int j=n-1; j>i; j--) if (X.elements[j][0]) X.elements[i][0] ^= matrice.get(j, i) << 7;
    }

    return X;
}

Matrice Matrice::bloc(short x1, short y1, short x2, short y2) const
{
    short const iX1 = (x1+7)/8;
    short const iX2 = (x2+7)/8;
    Matrice res(x2-x1, y2-y1);

    for (short y=y1; y<y2; y++) for (short x=iX1; x<iX2; x++) res.elements[y-y1][x-iX1] = elements[y][x];

    return res;
}

string Matrice::hexa() const
{
    if (tx == 1 && ty > 1) return "t_"+transposee().hexa();
    else
    {
        string res = "";
        short const iTx = tx/8;

        for (short y=0; y<ty; y++)
        {
            for (short x=0; x<iTx; x++)
            {
                stringstream iRes;
                iRes << hex << (short)elements[y][x];
                
                if (iRes.str().length() == 1) res += "0";
                res += iRes.str();
            }
            res += "\n";
        }

        return res;
    }
}

Matrice::operator string() const
{
    if (tx == 1 && ty > 1) return "t_"+static_cast<string>(transposee());
    else
    {
        string res = "";

        for (short y=0; y<ty; y++)
        {
            for (short x=0; x<tx; x++) res += '0'+get(x, y);
            res += "\n";
        }

        return res;
    }
}

Matrice::~Matrice()
{
    for (short y=0; y<ty; y++) delete[] elements[y];
    delete[] elements;
}


//Fonctions statiques
Matrice Matrice::id(short tx)
{
    Matrice res(tx, tx);
    short const iTx = (tx+7)/8;

    for (short y=0; y<tx; y++)
    {
        res.elements[y][y/8] = 0x80 >> (y%8);
        for (short x=0; x<y/8; x++) res.elements[y][x] = 0;
        for (short x=y/8+1; x<iTx; x++) res.elements[y][x] = 0;
    }

    return res;
}

Matrice Matrice::decalageGauche(short tx, short decalage)
{
    Matrice res(tx, tx);
    short const iTx = (tx+7)/8;
    
    for (short y=0; y<tx; y++)
    {
        short const d = y+decalage;
        res.elements[y][d/8] = 0x80 >> (d%8);
        
        for (short x=0; x<d/8; x++) res.elements[y][x] = 0;
        for (short x=d/8+1; x<iTx; x++) res.elements[y][x] = 0;
    }
    
    return res;
}

Matrice Matrice::decalageDroite(short tx, short decalage)
{
    return decalageGauche(tx, decalage).transposee();
}

Matrice Matrice::masque(uint8_t const masque[], short tx)
{
    Matrice res(tx, tx);
    short const iTx = (tx+7)/8;

    for (short y=0; y<tx; y++)
    {
        uint8_t const position = masque[y/8] & (0x80 >> (y%8));
        if (position) res.elements[y][y/8] = position;
        else res.elements[y][y/8] = 0;

        for (short x=0; x<y/8; x++) res.elements[y][x] = 0;
        for (short x=y/8+1; x<iTx; x++) res.elements[y][x] = 0;
    }

    return res;
}
