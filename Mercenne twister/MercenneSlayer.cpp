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

    return (idW+Matrice::decalageDroite(w, l))
          *(idW+Matrice::masque(intToChar(c), w)*Matrice::decalageGauche(w, t))
          *(idW+Matrice::masque(intToChar(b), w)*Matrice::decalageGauche(w, s))
          *(idW+Matrice::masque(intToChar(d), w)*Matrice::decalageDroite(w, u));
}

Matrice MercenneSlayer::matriceGeneraleCrypy() const
{
    //On construit la premiere partie de la matrice
    cout << "Creation de la matrice en cours : 0%" << endl;

    short const taille = n*w;
    Matrice res(taille, taille);
    Matrice E = matriceExtraction().bloc(0, 0, w, 8);

    for (short i=0; i<n/96; i++)
    {
        for (short j=0; j<16; j++)
        {
            short const nEquation = (i*16+j)*8;
            short const nEtat = (i*96+64+j*2)*w;
            res.coller(E, nEtat, nEquation, nEtat+w, nEquation+8);
        }
    }

    //On construit les autres morceaux de la matrice
    Matrice droite = Matrice::masque(intToChar(masqueDroit), w);
    Matrice gauche = Matrice::masque(intToChar(masqueGauche), w);

    Matrice iSuivant(w*2, w);
    iSuivant.coller(gauche, 0, 0, w, w);
    iSuivant.coller(droite, w, 0, w*2, w);

    Matrice A(w, w);
    A.coller(Matrice::id(w-1), 0, 1, w-1, w);
    A.coller(Matrice(intToChar(a), w), w-1, 0, w, w);

    iSuivant = A*iSuivant;

    short const txSuivant1 = (m+1)*w;
    Matrice suivant1(txSuivant1, w);
    Matrice idW = Matrice::id(w);

    suivant1.coller(iSuivant, 0, 0, w*2, w);
    suivant1.coller(idW, w*m, 0, txSuivant1, w);

    Matrice suivants[n*4/16*96];

    for (short i=0; i<n-m; i++)
    {
        suivants[i] = Matrice(taille, w);
        suivants[i].coller(suivant1, i*w, 0, i*w+txSuivant1, w);
    }
    
    cout << "Creation de la matrice en cours : 4%" << endl;

    for (short i=n-m; i<n-1; i++)
    {
        suivants[i] = suivants[i-(n-m)];

        Matrice coucheActuelle(taille, w);
        coucheActuelle.coller(iSuivant, i*w, 0, (i+2)*w, w);

        suivants[i] += coucheActuelle;
    }

    cout << "Creation de la matrice en cours : 8%" << endl;

    suivants[n-1] = suivants[m-1];
    suivants[n-1].coller(A*gauche, taille-w, 0, taille, w);
    suivants[n-1] += A*(droite*suivants[0]);

    for (short i=n; i<n*4/16*96; i++)
    {
        if (i%50 == 0) cout << "Creation de la matrice en cours : " << 100.0*(i+n)/(n*4/16*96) << "%" << endl;

        suivants[i] = A*(gauche*suivants[i-n]+droite*suivants[i+1-n]) + suivants[i-n+m];
    }

    //On assemble les morceaux selon l'utilisation de randint par crypy : 32o cle - 16o IV - 32o cle - ... 
    cout << "Creation de la matrice en cours : assemblage" << endl;

    for (short i=n/96; i<n*4/16; i++)
    {
        for (short j=0; j<16; j++)
        {
            short const nEtat = i*96-n+64+j*2;
            short const nEquation = (i*16+j)*8;
            res.coller(E*suivants[nEtat], 0, nEquation, taille, nEquation+8);
        }
    }

    cout << "Creation de la matrice terminee" << endl;

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

    //Tests mercenne twister
    cout << 0x12345678 << " = " << mercenneSlayer.inverserExtraction(mercenneSlayer.iExtraire(0x12345678)) << endl;
    cout << 0x848465 << " = " << mercenneSlayer.inverserExtraction(mercenneSlayer.iExtraire(0x848465)) << endl;

    cout << "random : " << mercenneSlayer.random(1711569972, 742869257) << ", extrait1 : " << mercenneSlayer.iExtraire(1711569972) << ", extrait2 : " << mercenneSlayer.iExtraire(742869257) 
         << ", extrait1 retrouve : " << mercenneSlayer.doubleToW(0.023451428093100413)[0] << ", extrait2 retrouve : " << mercenneSlayer.doubleToW(0.023451428093100413)[1] << endl;

    //cout << "etat1 : " << hex << 1711569972 << endl;
    //cout << "etat1 retrouve : " << hex << mercenneSlayer.inverserExtraction(mercenneSlayer.doubleToW(0.023451428093100413)[0]) << endl;


    //Tests randint
    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");
    cout << "randints : " << mercenneSlayer.randint(0, 255) << " " << mercenneSlayer.randint(0, 255) << " " << mercenneSlayer.randint(0, 255) << endl;
    cout << "randints : " << mercenneSlayer.randint(710524559L, 2078043453L, 0, 255) << " " << mercenneSlayer.randint(1742245849L, 688828953L, 0, 255) << " "
         << mercenneSlayer.randint(17639039L, 1893008952L, 0, 255) << endl << endl;

    cout << "On recupere les 8 premiers bits : " << endl;

    printf("randint 1 : ref %x %x, trouve %x %x\n", mercenneSlayer.iExtraire(710524559L), mercenneSlayer.iExtraire(2078043453L), mercenneSlayer.intToW(102, 0, 255)[0],
           mercenneSlayer.intToW(102, 0, 255)[1]);

    printf("randint 2 : ref %x %x, trouve %x %x\n", mercenneSlayer.iExtraire(1742245849L), mercenneSlayer.iExtraire(688828953L), mercenneSlayer.intToW(231, 0, 255)[0],
           mercenneSlayer.intToW(231, 0, 255)[1]);

    printf("randint 3 : ref %x %x, trouve %x %x\n\n", mercenneSlayer.iExtraire(17639039L), mercenneSlayer.iExtraire(1893008952L), mercenneSlayer.intToW(36, 0, 255)[0],
           mercenneSlayer.intToW(36, 0, 255)[1]);

    printf("etat depart : %x\nextraction : %x\n", 2078043453L, mercenneSlayer.iExtraire(2078043453L));


    //Tests matrice d'extraction et pivot de Gausse
    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");

    Matrice B = mercenneSlayer.matriceExtraction();
    Matrice etat(intToChar(2078043453L), 32);

    cout << "etat matrice : " << etat.hexa() << "extraction matrice : " << (B*etat).hexa();
    cout << "extraction inversee : " << B.inverser(B*etat).hexa() << endl;


    //Tests matrice generale randint
    mercenneSlayer.randint(0, 255);
    Matrice etats(intToChars(mercenneSlayer.etats, 624), 624*32);
    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");

    uint8_t randints[624*4]; //Suite des ivs generes par CryPy
    
    for (short i=0; i<624*4/16; i++)
    {
        for (short j=0; j<32; j++) mercenneSlayer.randint(0, 255);
        for (short j=0; j<16; j++) randints[i*16+j] = mercenneSlayer.randint(0, 255);
    }
 
    cout << "randint" << endl << Matrice(randints, 624*32).hexa() << endl;
    cout << "etats" << endl << etats.hexa() << endl;

    Matrice matriceCrypy("matriceCrypy");
    if (!matriceCrypy.isOk())
    {
        cout << "So we need to rebuild it" << endl;
        matriceCrypy = mercenneSlayer.matriceGeneraleCrypy();
        matriceCrypy.exporter("matriceCrypy");
    }

    Matrice randintsMatrice = (matriceCrypy*etats);
    Matrice randintsMatriceT = randintsMatrice.transposee();

    cout << "randint trouves avec matrice " << endl << randintsMatriceT.hexa() << endl;

    short i;
    for (i=0; i<624*4; i++) if (randintsMatriceT.get8(i, 0) != randints[i]) break;

    if (i < 624*4)
    {
        cout << "egalite jusque : " << i << " : " << hex << (short)randintsMatriceT.get8(i, 0) << hex << (short)randintsMatriceT.get8(i+1, 0) << " != "
                                                  << hex << (short)randints[i] << hex << (short)randints[i+1] << endl;
    }
    else cout << "matriceCrypy correcte" << endl;

    Matrice etatsRetrouves = matriceCrypy.inverser(randintsMatrice);
    cout << "etats retrouves :" << endl << etatsRetrouves.hexa() << endl;
    
    Matrice randintRetrouvesT = (matriceCrypy*etatsRetrouves).transposee();
    cout << "randint avec etat retrouves" << endl << randintRetrouvesT.hexa() << endl;

    for (i=0; i<624*4; i++) if (randintRetrouvesT.get8(i, 0) != randintsMatriceT.get8(i, 0)) break;

    if (i < 624*4)
    {
        cout << "egalite jusque : " << i << " : " << hex << (short)randintRetrouvesT.get8(i, 0) << hex << (short)randintRetrouvesT.get8(i+1, 0) << " != "
                                                  << hex << (short)randintsMatriceT.get8(i, 0) << hex << (short)randintsMatriceT.get8(i+1, 0) << endl;
    }
    else cout << "inversion correcte" << endl;
}
