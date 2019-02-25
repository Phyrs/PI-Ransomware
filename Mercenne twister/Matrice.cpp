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

Matrice::Matrice(uint8_t const iElements[], short tx) : tx(tx), ty(1), elements(new uint8_t*[1])
{
    short const iTx = (tx+7)/8;
    elements[0] = new uint8_t[iTx];

    for (short x=0; x<iTx; x++) elements[0][x] = iElements[x];
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

    uint8_t const apres = 0xFF >> decalageDroite;
    uint8_t const avant = ~apres;
    uint8_t const droite = (1<<decalageDroite)-1;

    for (short y=y1; y<y2; y++)
    {
        elements[y][iX1] = (elements[y][iX1] & avant) | (A.elements[y-y1][0]>>decalageDroite);

        for (short x=iX1+1; x<iX2; x++) elements[y][x] = ((A.elements[y-y1][x-iX1-1]&droite) << decalageGauche) | (A.elements[y-y1][x-iX1] >> decalageDroite);
        
        if (decalageDroite) elements[y][iX2] = ((A.elements[y-y1][iX2-iX1-1]&droite) << decalageGauche) | (elements[y][iX2] & apres);
    }
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

void Matrice::set(short x, short y, short i)
{
    short const iX = x%8;
    elements[y][x/8] = (elements[y][x/8] & ~(0x80 >> iX)) | (i << (7-iX));
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
    string res = "";
    short const iTx = tx/8;

    for (short y=0; y<ty; y++)
    {
        for (short x=0; x<iTx; x++)
        {
            stringstream iRes;
            iRes << hex << (short)elements[y][x];
            res += iRes.str();
        }
        res += "\n";
    }

    return res;
}

Matrice::operator string() const
{
    string res = "";

    for (short y=0; y<ty; y++)
    {
        for (short x=0; x<tx; x++) res += '0'+get(x, y);
        res += "\n";
    }

    return res;
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

Matrice Matrice::decalageDroite(short tx, short decalage)
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

Matrice Matrice::decalageGauche(short tx, short decalage)
{
    return decalageDroite(tx, decalage).transposee();
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

/*
0000 (4, 2)
0000

000
000 (3, 4)
000
000

->

0000
0000
0000

000 (3, 2)
000
*/
