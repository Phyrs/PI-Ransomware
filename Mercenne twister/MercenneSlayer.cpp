#include <fstream>
#include <iostream>
#include "Matrice.h"
#include "MercenneSlayer.h"

double randomPython(uint32_t nombreInitial1, uint32_t nombreInitial2)
{
    return ((nombreInitial1 >> 5)*67108864.0+(nombreInitial2 >> 6))*(1.0/9007199254740992.0); 
}

uint8_t* intToChar(uint32_t valeur)
{
    uint8_t *res = new uint8_t[4];
    res[0] = (valeur&0xFF000000) >> 24;
    res[1] = (valeur&0x00FF0000) >> 16;
    res[2] = (valeur&0x0000FF00) >> 8;
    res[3] = (valeur&0x000000FF);

    return res;
}

uint8_t* intToChars(uint32_t valeurs[], short taille)
{
    uint8_t *res = new uint8_t[taille*4];

    for (short i=0; i<taille; i++)
    {
        uint8_t *iRes = intToChar(valeurs[i]);

        for (short j=0; j<4; j++) res[i*4+j] = iRes[j];
        delete[] iRes;
    }

    return res;
}

MercenneSlayer::MercenneSlayer(short w, short n, short m, short r, W a, short u, W d, short s, W b, short t, W c, short l)
 : a(a), d(d), b(b), c(c), w(w), n(n), m(m), r(r), u(u), s(s), t(t), l(l), etats(new W[n]), masqueDroit((1<<r)-1), masqueGauche(~masqueDroit)
{
}

void MercenneSlayer::melanger()
{
    for (short i=0; i<n; i++) etats[i] = etatSuivant(etats, i);
    position = 0;
}

void MercenneSlayer::inverserMelange()
{
    for (short i=n-1; i>=0; i++) etats[i] = etatPrecedent(etats, i);
    position = n;
}

W MercenneSlayer::extraire()
{
    if (position == n) melanger();

    W res = iExtraire(etats[position]);
    position++;

    return res;
}

W MercenneSlayer::extrairePrecedent()
{
    position--;
    W res = iExtraire(etats[position]);

    if (position == 0) inverserMelange();

    return res;
}

double MercenneSlayer::random()
{
    W nombreInitial1 = extraire();
    W nombreInitial2 = extraire();
    
    return randomPython(nombreInitial1, nombreInitial2);
}

int MercenneSlayer::randint(int a, int b)
{
    return a+random()*(b+1-a); 
}

double MercenneSlayer::randomPrecedent()
{
    W nombreInitial2 = extrairePrecedent();
    W nombreInitial1 = extrairePrecedent();
    
    return randomPython(nombreInitial1, nombreInitial2);
}

void MercenneSlayer::sEtatsSEtats(string pathEtats)
{
    ifstream fEtats;
	fEtats.open(pathEtats);
	
	if (!fEtats)
	{
	    cerr << "Error while opening file " << pathEtats << endl;
	    fEtats.close();
	    return;
	}

	fEtats.seekg(0, ios::end);
    long const tailleFEtats = fEtats.tellg();
    fEtats.seekg(0, ios::beg);

    int tailleChaine = n*((w/3)+3)+1; //ln(10)/ln(2) = 3.32
    
    if (tailleFEtats < tailleChaine)
    {
        cerr << "Pas assez d'états pour déterminer l'état interne" << endl;
        fEtats.close();
        return;
    }

    char chaine[tailleChaine];
    
    if (fEtats.read(chaine, tailleChaine))
    {
        long j = 0;

        for (short i=0; i<n; i++)
        {
            W etat = 0;

            while (chaine[j] != 'L')
            {
                etat = etat*10 + (chaine[j]-'0');
                j++;
            }
            j += 3;
            
            etats[i] = etat;
        }
        
        position = n;
    }
    else cerr << "Error while reading file " << pathEtats << endl;

    fEtats.close();
}

void MercenneSlayer::sEtats(W nombresAleatoires[])
{
    for (short i=0; i<n; i++) etats[i] = inverserExtraction(nombresAleatoires[i]);
    position = n;
}

void MercenneSlayer::sEtats(double nombresAleatoires[])
{
    for (short i=0; i<n/2; i++)
    {
        W *wAleatoires = doubleToW(nombresAleatoires[i]);
        etats[i*2] = wAleatoires[0];
        etats[i*2+1] = wAleatoires[1];
        delete[] wAleatoires;
    }

    //brute force

    position = n;
}

//https://en.wikipedia.org/wiki/Mersenne_Twister
W MercenneSlayer::etatSuivant(W etats[], short nEtat) const 
{
    W const x = (etats[nEtat] & masqueGauche) | (etats[(nEtat+1)%n] & masqueDroit);
    W xA = x >> 1;

    if (x%2) xA ^= a;
                                        
    return etats[(nEtat+m)%n] ^ xA;
}


W MercenneSlayer::etatPrecedent(W etats[], short nEtat) const 
{/*
    //On recupere la partie gauche de l'etat
    W xA = etats[nEtat] ^ etats[(nEtat+m)%n];
    W const droiteX = (etats[(nEtat+1)%n] & masqueDroit);

    if ((droiteX >> 1)%2) xA ^= a;

    W const gaucheEtat = (xA << 1) & masqueGauche;

    //On recupere la partie droite de l'etat
    //W const dernierBitEtat = (~etats[(nEtat+n-m)%n]) & 1;

    W const etatAv = etats[(nEtats+n-1)%n];
    W xAAv = etatAv ^ etats[(nEtat-1+m)%n];

    if (bitImportantEtat) xAAv ^= a;
    
    
    W const droiteXAv = (etats[nEtat%n] & masqueDroit);
    
    if ((droiteXAv >> 1)%2) xA ^= a;
    
    W const gaucheEtat = (xA << 1) & masqueGauche;

    W res = gaucheEtat | droiteEtat;

    return res;*/
    return 0;
}

//https://github.com/python/cpython/blob/master/Modules/_randommodule.c
double MercenneSlayer::random(W etat1, W etat2) const
{
    return randomPython(iExtraire(etat1), iExtraire(etat2)); 
}

int MercenneSlayer::randint(W etat1, W etat2, int a, int b) const
{
    return a+randomPython(iExtraire(etat1), iExtraire(etat2))*(b+1-a); 
}

Matrice MercenneSlayer::matriceExtraction() const
{
    Matrice idW = Matrice::id(w);

    return (idW+Matrice::decalageDroite(w, u)*Matrice::masque(intToChar(d), w))
          *(idW+Matrice::decalageGauche(w, s)*Matrice::masque(intToChar(b), w))
          *(idW+Matrice::decalageGauche(w, t)*Matrice::masque(intToChar(c), w))
          *(idW+Matrice::decalageDroite(w, l));
}

Matrice MercenneSlayer::matriceGeneraleRandint() const
{
    short const taille = n*w;
    Matrice res(taille, taille);
    Matrice E = matriceExtraction().bloc(0, 0, 8, w);

    for (short i=0; i<n; i++) res.coller(E, i*8, i*w, (i+1)*8, (i+1)*w);

    Matrice droite = Matrice::masque(intToChar(masqueDroit), w);
    Matrice gauche = Matrice::masque(intToChar(masqueGauche), w);

    Matrice iSuivant(w, w*2);
    iSuivant.coller(gauche, 0, 0, w, w);
    iSuivant.coller(droite, 0, w, w, w*2);

    Matrice A(w, w);
    A.coller(Matrice::id(w-1), 1, 0, w, w-1);
    A.coller(Matrice(intToChar(a), w), 0, w-1, w, w);

    iSuivant *= A;

    short const tailleSuivant1 = (m+1)*w;
    Matrice suivant1(w, tailleSuivant1);
    Matrice idW = Matrice::id(w);

    suivant1.coller(iSuivant, 0, 0, w, w*2);
    suivant1.coller(idW, 0, w*m, w, tailleSuivant1);

    short const tailleSuivant2 = (n-m+2)*w;
    Matrice suivant2(w, tailleSuivant2);

    suivant2.coller(idW, 0, 0, w, w);
    suivant2.coller(iSuivant, 0, tailleSuivant2-w*2, w, tailleSuivant2);

    Matrice suivant(w, taille);
    //suivant.coller(suivant1, 0, w, w, tailleSuivant1+w);
    suivant.coller(suivant2, 0, w, w, tailleSuivant2+w);
    
    Matrice h = suivant;

    cout << (string)iSuivant << endl;

    return suivant;

    /*
    iSuivant *= A*E;

    short const tailleSuivant1 = w*(m+1);
    Matrice suivant1(8, tailleSuivant1);
    Matrice idW = Matrice::id(w);

    suivant1.coller(iSuivant, 0, 0, 8, w*2);
    suivant1.coller(E, 0, w*m, 8, tailleSuivant1);

    cout << (string)suivant1 << endl;
*/

    return res;
}

W MercenneSlayer::iExtraire(W nombreInitial) const
{
    nombreInitial ^= (nombreInitial >> u) & d;
    nombreInitial ^= (nombreInitial << s) & b;
    nombreInitial ^= (nombreInitial << t) & c;
    nombreInitial ^= nombreInitial >> l;

    return nombreInitial;
}

W* MercenneSlayer::doubleToW(double nombreAleatoire) const
{
    W *res = new W[2];
    long const iRes = (nombreAleatoire*9007199254740992.0+0.5);

    res[1] = iRes%67108864 << 6;
    res[0] = ((iRes-(iRes%67108864))/67108864) << 5;

    return res;
}

W* MercenneSlayer::intToW(int nombreAleatoire, int a, int b) const
{
    return doubleToW((static_cast<double>(nombreAleatoire)-a)/(b-a));
}

W MercenneSlayer::inverserExtraction(W nombreAleatoire) const
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

    cout << mercenneSlayer.random(1711569972, 742869257) << " " << mercenneSlayer.iExtraire(1711569972) << " " << mercenneSlayer.iExtraire(742869257) << " " << mercenneSlayer.doubleToW(0.023451428093100413)[0] << " " << mercenneSlayer.doubleToW(0.023451428093100413)[1] << endl;

    cout << mercenneSlayer.inverserExtraction(mercenneSlayer.doubleToW(0.023451428093100413)[0]) << endl;

    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");
    cout << mercenneSlayer.randint(0, 255) << " " << mercenneSlayer.randint(0, 255) << " " << mercenneSlayer.randint(0, 255) << endl;
    cout << mercenneSlayer.randint(710524559L, 2078043453L, 0, 255) << " " << mercenneSlayer.randint(1742245849L, 688828953L, 0, 255) << " "
         << mercenneSlayer.randint(17639039L, 1893008952L, 0, 255) << endl;

    cout << "On recupere les 8 premiers bits : " << endl;
    printf("%x %x %x %x\n", mercenneSlayer.iExtraire(710524559L), mercenneSlayer.iExtraire(2078043453L), mercenneSlayer.intToW(102, 0, 255)[0], mercenneSlayer.intToW(102, 0, 255)[1]);
    printf("%x %x %x %x\n", mercenneSlayer.iExtraire(1742245849L), mercenneSlayer.iExtraire(688828953L), mercenneSlayer.intToW(231, 0, 255)[0], mercenneSlayer.intToW(231, 0, 255)[1]);
    printf("%x %x %x %x\n", mercenneSlayer.iExtraire(17639039L), mercenneSlayer.iExtraire(1893008952L), mercenneSlayer.intToW(36, 0, 255)[0], mercenneSlayer.intToW(36, 0, 255)[1]);

    printf("%x %x\n", 710524559L, mercenneSlayer.iExtraire(710524559L));


    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");

    Matrice B = mercenneSlayer.matriceExtraction();
    Matrice etat(intToChar(710524559L), 32);

    cout << etat.hexa() << (etat*B).hexa() << endl;
    cout << (string)B << endl;
    
    cout << (string)B.bloc(0, 0, 8, 32);
    
    Matrice E(24, 32);
    E.coller(B.bloc(0, 0, 8, 32), 3, 0, 3+8, 32);

    Matrice etats(intToChars(mercenneSlayer.etats, 624), 624*32);

printf("%x %x %x %x\n", mercenneSlayer.etats[0], mercenneSlayer.etats[1], mercenneSlayer.etats[622], mercenneSlayer.etats[623]);
    cout << etats.hexa() << endl;
    
    Matrice suivant = mercenneSlayer.matriceGeneraleRandint();
    Matrice test = etats*suivant;

    printf("%x\n", mercenneSlayer.etatSuivant(mercenneSlayer.etats, 1));
    cout << test.hexa() << endl;
}
