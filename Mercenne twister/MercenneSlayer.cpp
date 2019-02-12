#include "MercenneSlayer.h"
#include <iostream>

using namespace std;

int inverserGauche(int x, int masque, short decalage)
{
    for (short i=0; i<32-decalage; i++)
    {
        if (masque & (1 << (i+decalage))) x = x ^ ((x & (1 << i)) << decalage);
    }
    
    return x;
} 

int inverserDroite(int x, int masque, short decalage)
{
    for (short i=31-decalage; i >= 0; i--)
    {
        if (masque & (1 << (i+decalage))) x = x ^ ((x & (1 << (i+decalage))) >> decalage);
    }
    
    return x;
}

MercenneSlayer::MercenneSlayer(short w, short n, short m, short r, int a, short u, int d, short s, int b, short t, int c, short l)
 : a(a), d(d), b(b), c(c), w(w), n(n), m(m), r(r), u(u), s(s), t(t), l(l), etats(new int[n]), masqueDroit((1<<r)-1), masqueGauche(~masqueDroit)
{
}

void MercenneSlayer::melanger()
{
    for (short i=0; i<n; i++) etats[i] = etatSuivant(etats, i);
}

int MercenneSlayer::extraire()
{
    position++;
    if (position == n)
    {
        melanger();
        position = 0;
    }
    
    return iExtraire(etats[position]);
}

double MercenneSlayer::random()
{
    return iRandom(extraire(), extraire());
}

void MercenneSlayer::sEtats(int nombresAleatoires[])
{
    short const nbIEtats = n*2;
    int iEtats[nbIEtats];
    
    for (short i=0; i<nbIEtats; i++) iEtats[i] = inverserExtraction(nombresAleatoires[i]);

    short i;
    for (i=0; i<n; i++)
    {
        if (etatSuivant(iEtats+i, m) == iEtats[i+n+m]) break;
    }
    
    for (short j=0; i<nbIEtats; j++) etats[j] = iEtats[i+j];
}

//https://en.wikipedia.org/wiki/Mersenne_Twister
int MercenneSlayer::etatSuivant(int etats[], int nEtat) const 
{
    int const x = (etats[nEtat] & masqueGauche) | (etats[(nEtat+1)%n] & masqueDroit);
    int xA = x >> 1;
    
    if (x%2) xA ^= a;
    
    return etats[nEtat+m] ^ xA;
}

//https://github.com/python/cpython/blob/master/Modules/_randommodule.c
double MercenneSlayer::iRandom(int nombreInitial1, int nombreInitial2) const
{
    return ((iExtraire(nombreInitial1) >> 5)*67108864.0+(iExtraire(nombreInitial2) >> 6))*(1.0/9007199254740992.0); 
}

int MercenneSlayer::iExtraire(int nombreInitial) const
{
    nombreInitial ^= (nombreInitial >> u) & d;
    nombreInitial ^= (nombreInitial << s) & b;
    nombreInitial ^= (nombreInitial << t) & c;
    nombreInitial ^= nombreInitial >> l;

    return nombreInitial;
}

int* MercenneSlayer::doubleToEtats(double nombreAleatoire) const
{
    int *res = new int[2];
    long const iRes = (nombreAleatoire*9007199254740992.0+0.5);

    res[1] = iRes%67108864 << 6;
    res[0] = ((iRes-(iRes%67108864))/67108864) << 5;

    return res;
}

int MercenneSlayer::inverserExtraction(int nombreAleatoire) const
{
    nombreAleatoire = nombreAleatoire ^ (nombreAleatoire >> l);
    nombreAleatoire = nombreAleatoire ^ ((nombreAleatoire << t) & c);
    nombreAleatoire = inverserGauche(nombreAleatoire, b, s);
    nombreAleatoire = inverserDroite(nombreAleatoire, d, u);

    return nombreAleatoire;
}

MercenneSlayer::~MercenneSlayer()
{
    delete[] etats;
}

//Fonctions statiques
void MercenneSlayer::tester()
{
    MercenneSlayer mercenneSlayer(32, 624, 397, 31, 0x9908B0DF, 11, 0xFFFFFFFF, 7, 0x9D2C5680, 15, 0xEFC60000, 18);
    
    cout << 0x12345678 << " = " << mercenneSlayer.inverserExtraction(mercenneSlayer.iExtraire(0x12345678)) << endl;
    cout << 0x848465 << " = " << mercenneSlayer.inverserExtraction(mercenneSlayer.iExtraire(0x848465)) << endl;
    
    cout << mercenneSlayer.iRandom(1711569972, 742869257) << " " << mercenneSlayer.iExtraire(1711569972) << " " << mercenneSlayer.iExtraire(742869257) << " " << mercenneSlayer.doubleToEtats(0.023451428093100413)[0] << " " << mercenneSlayer.doubleToEtats(0.023451428093100413)[1] << endl;
    
    cout << mercenneSlayer.inverserExtraction(mercenneSlayer.doubleToEtats(0.023451428093100413)[0]) << endl;
}
