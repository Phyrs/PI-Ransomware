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
    //Utiliser la matrice, cf crypy
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
    //Meme principe que pour Crypy
}

void MercenneSlayer::sEtatSCrypy(uint8_t iNombresAleatoires[])
{
    //On reconstruit les nombres aleatoires
    int const tailleIV = 16;
    int const tailleNom = 36;
    int const tailleCle = 32;
    int const nbBitsEtatIV = 8;
    int const nbBitsEtatNom = 5;
    int const nbBitsIV = tailleIV*nbBitsEtatIV;
    int const nbBitsNom = tailleNom*nbBitsEtatNom;
    int const nbBitsCycle = nbBitsIV+nbBitsNom;
    int const tailleCycle = tailleIV;//tailleNom;
    int const nbFichiers = n*w/nbBitsCycle;

    int nbCycles = 1+tailleIV*2;//+tailleNom*2;
    int tailleCycles[nbCycles];
    int nbBitsCycles[nbCycles];

    tailleCycles[0] = tailleCle*2+tailleNom*2;
    nbBitsCycles[0] = 0;

    for (int i=1; i<nbCycles; i++)
    {
        tailleCycles[i] = 1;
    }
/*
    for (int i=0; i<tailleNom*2; i++)
    {
        nbBitsCycles[i] = nbBitsEtatNom;
    }*/

    for (int i=1; i<nbCycles; i++)
    {
        nbBitsCycles[i] = nbBitsEtatIV;
    }

    //cout << "randintCry" << endl << Matrice(iNombresAleatoires, (nbFichiers+1)*tailleCycle*8).hexa() << endl;

    Matrice nombresAleatoires(iNombresAleatoires, n*w);//(1, n*w);
    
    cout << nombresAleatoires.hexa() << endl;
/*
    for (short i=0; i<nbFichiers; i++)
    {
        nombresAleatoires.coller(Matrice(iNombresAleatoires+i*tailleCycle, nbBitsEtatNom), 0, nbBitsCycle*i, 1, nbBitsCycle*i+nbBitsEtatNom);
        nombresAleatoires.coller(Matrice(iNombresAleatoires+i*tailleCycle+tailleNom, nbBitsEtatIV), 0, nbBitsCycle*i+nbBitsEtatNom, 1, nbBitsCycle*(i+1));
    }

    int const nbBitsRestants = n*w-nbFichiers*nbBitsCycle;
    
    if (nbBitsRestants > 0)
    {
        nombresAleatoires.coller(Matrice(iNombresAleatoires+nbFichiers*tailleCycle, nbBitsEtatNom), 0, nbFichiers*nbBitsCycle, 1, min(nbFichiers*nbBitsCycle+nbBitsNom, n*w));
        
        if (nbBitsRestants > nbBitsNom) nombresAleatoires.coller(Matrice(iNombresAleatoires+nbFichiers*tailleCycle+tailleNom, nbBitsEtatNom), 0, nbFichiers*nbBitsCycle+nbBitsEtatNom, 1, n*w);
    }
*/
    cout << nombresAleatoires.hexa() << endl;

    //On ajoute des contraintes
    int const finReferences = 623*32;
    Matrice contraintes(finReferences+1, 624*32);
    contraintes.coller(Matrice::id(finReferences), 0, 0, finReferences, finReferences); //Les n-1 premiers etats servent de reference

    Matrice A(w, w);
    A.coller(Matrice::id(w-1), 0, 1, w-1, w);
    A.coller(Matrice(intToChar(a), w), w-1, 0, w, w);

    Matrice iPassage(623*32+1, 32);
    iPassage.coller(Matrice::id(31), 1, 1, 32, 32);
    iPassage.set(623*32, 0, 1);
    
    contraintes.coller(A*iPassage, 0, finReferences, finReferences+1, 624*32);
    contraintes.coller(Matrice::id(32), (397-1)*32, finReferences, 397*32, 624*32);

    //On recuperes nos etats
    Matrice matriceCrypy = matriceGeneraleCrypy(nbBitsCycles, tailleCycles, nbCycles);
    Matrice matriceCrypyContrainte("matrices/matriceCrypyContrainte_"+to_string(0)+"_"+to_string(33)+".ms");

    if (!matriceCrypyContrainte.isOk())
    {
        cout << "Creation de la matrice contrainte" << endl;

        matriceCrypyContrainte = matriceCrypy*contraintes;
        matriceCrypyContrainte.exporter("matrices/matriceCrypyContrainte_"+to_string(0)+"_"+to_string(33)+".ms");
    }

    Matrice iEtats = (contraintes*matriceCrypyContrainte.inverser(nombresAleatoires)).transposee();

    cout << iEtats.hexa() << endl;

    for (short i=0; i<n*4; i+=4) etats[i/4] = (iEtats.get8(i, 0) << 24) | (iEtats.get8(i+1, 0) << 16) | (iEtats.get8(i+2, 0) << 8) | iEtats.get8(i+3, 0);
    
    position = 0;
}

//https://en.wikipedia.org/wiki/Mersenne_Twister
W MercenneSlayer::etatSuivant(W etats[], short nEtat) const 
{
    W const x = (etats[nEtat] & masqueGauche) | (etats[(nEtat+1)%n] & masqueDroit);
    W xA = x >> 1;

    if (x%2) xA ^= a;
                                        
    return etats[(nEtat+m)%n] ^ xA;
}

//https://github.com/python/cpython/blob/master/Modules/_randommodule.c
double MercenneSlayer::random(W etat1, W etat2) const
{
    W nombreInitial1 = iExtraire(etat1);
    W nombreInitial2 = iExtraire(etat2);
    
    return randomPython(iExtraire(etat1), iExtraire(etat2)); 
}

int MercenneSlayer::randint(W etat1, W etat2, int a, int b) const
{
    return a+random(etat1, etat2)*(b+1-a); 
}

Matrice MercenneSlayer::matriceExtraction() const
{
    Matrice idW = Matrice::id(w);

    return (idW+Matrice::decalageDroite(w, l))
          *(idW+Matrice::masque(intToChar(c), w)*Matrice::decalageGauche(w, t))
          *(idW+Matrice::masque(intToChar(b), w)*Matrice::decalageGauche(w, s))
          *(idW+Matrice::masque(intToChar(d), w)*Matrice::decalageDroite(w, u));
}

Matrice MercenneSlayer::matriceGeneraleCrypy(int nbBitsCycles[], int tailleCycles[], int nbCycles) const
{
    int const ty = n*w;
    int const tx = n*w;
    int const decalage = 0;    //Nombre d'extraction entre l'etat interne originel et le debut de Crypy. Doit etre un multiple de 2 a cause de random()
    int tailleGrandCycle = 0;
    int nbBitsUtilesCycle = 0;

    for (int i=1; i<nbCycles; i+=2) nbBitsUtilesCycle += nbBitsCycles[i]*tailleCycles[i];
    for (int i=0; i<nbCycles; i++) tailleGrandCycle += tailleCycles[i];

    int const nbGrandsCycles = n*w/nbBitsUtilesCycle+1;
    string const nomRes = "matrices/matriceCrypy_"+to_string(decalage)+"_"+to_string(nbGrandsCycles)+"_"+to_string(nbBitsUtilesCycle)+".ms";

    Matrice matriceCrypy(nomRes);
    if (matriceCrypy.isOk()) return matriceCrypy;

    cout << "Création de " << nomRes << endl;

    //On construit la premiere partie de la matrice
    cout << "Creation de la matrice en cours : 0%" << endl;

    Matrice res(tx, ty);
    Matrice E = matriceExtraction();
/*
    Matrice E1 = matriceExtraction().bloc(0, 0, w, nbBitsEtatPetitCycle1);
    Matrice E2 = matriceExtraction().bloc(0, 0, w, nbBitsEtatPetitCycle2);
*/

    for (int i=0; i<(n-decalage+tailleGrandCycle-1)/tailleGrandCycle; i++)
    {
        int debutEtat = 0;
        int debutEquation = 0;

        for (int j=0; j<nbCycles; j++)
        {
            if (j%2)
            {
                int const nbBitsPetitCycle = nbBitsCycles[j];

                for (int k=0; k<tailleCycles[j]; k++)
                {
                    int const nEquation = i*nbBitsUtilesCycle+debutEquation;
                    int const nBitEtat = (i*tailleGrandCycle+debutEtat+k+decalage)*w;

                    if (nBitEtat < tx) res.coller(E, nBitEtat, nEquation, nBitEtat+w, nEquation+nbBitsPetitCycle);
                    debutEquation += nbBitsPetitCycle;
                }
            }

            debutEtat += tailleCycles[j];
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

    int const txSuivant1 = (m+1)*w;
    Matrice suivant1(txSuivant1, w);
    Matrice idW = Matrice::id(w);

    suivant1.coller(iSuivant, 0, 0, w*2, w);
    suivant1.coller(idW, w*m, 0, txSuivant1, w);

    int const nbSuivants = nbGrandsCycles*tailleGrandCycle+decalage-n;
    Matrice suivants[nbSuivants];

    for (int i=0; i<n-m; i++)
    {
        suivants[i] = Matrice(tx, w);
        suivants[i].coller(suivant1, i*w, 0, i*w+txSuivant1, w);
    }

    cout << "Creation de la matrice en cours : " << 100.0*(n-m)/nbSuivants << "%" << endl;

    for (int i=n-m; i<n-1; i++)
    {
        suivants[i] = suivants[i-(n-m)];

        Matrice coucheActuelle(tx, w);
        coucheActuelle.coller(iSuivant, i*w, 0, (i+2)*w, w);

        suivants[i] += coucheActuelle;
    }

    cout << "Creation de la matrice en cours : " << 100.0*n/nbSuivants << "%" << endl;

    suivants[n-1] = suivants[m-1];
    suivants[n-1].coller(A*gauche, tx-w, 0, tx, w);
    suivants[n-1] += A*(droite*suivants[0]);

    for (int i=n; i<nbSuivants; i++)
    {
        if (i%50 == 0) cout << "Creation de la matrice en cours : " << 100.0*i/nbSuivants << "%" << endl;

        string nomEtat = "etats/etat_"+to_string(i);
        suivants[i] = Matrice(nomEtat);

        if (!suivants[i].isOk())
        {
            suivants[i] = A*(gauche*suivants[i-n]+droite*suivants[i+1-n]) + suivants[i-n+m];
            suivants[i].exporter(nomEtat);
        }
    }

    //On assemble les morceaux selon l'utilisation de randint par crypy : 32o cle - 16o IV - 32o cle - ... 
    cout << "Creation de la matrice en cours : assemblage" << endl;

    for (int i=max(0, (n-decalage)/tailleGrandCycle); i<nbGrandsCycles; i++)
    {
        int debutEtat = 0;
        int debutEquation = 0;

        for (int j=0; j<nbCycles; j++)
        {
            if (j%2) 
            {
                int const nbBitsPetitCycle = nbBitsCycles[j];

                for (int k=0; k<tailleCycles[j]; k++)
                {
                    int const nEquation = i*nbBitsUtilesCycle+debutEquation;
                    int const nEtat = decalage-n+i*tailleGrandCycle+debutEtat+k;

                    if (nEquation < ty && nEtat >= 0) res.coller(E*suivants[nEtat], 0, nEquation, tx, min(nEquation+nbBitsPetitCycle, ty));
                    debutEquation += nbBitsPetitCycle;
                }
            }

            debutEtat += tailleCycles[j];
        }
    }

    res.exporter(nomRes);
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
    Matrice B = mercenneSlayer.matriceExtraction();
    Matrice etat(intToChar(2078043453L), 32);

    cout << "etat matrice : " << etat.hexa() << "extraction matrice : " << (B*etat).hexa();
    cout << "extraction inversee : " << B.inverser(B*etat).hexa() << endl;


    //Tests matrice generale crypy
    int const w = 32;
    int const n = 624;
    int const decalage = 4;//25*n;
    int const tailleIV = 16;
    int const tailleNom = 36;
    int const tailleCle = 32;
    int const nbBitsEtatIV = 8;
    int const nbBitsEtatNom = 5;
    int const nbBitsIV = tailleIV*nbBitsEtatIV;
    int const nbBitsNom = tailleNom*nbBitsEtatNom;

    int const nbBitsCycle = nbBitsIV;//+nbBitsNom;
    int const tailleCycle = tailleIV;//+tailleNom;
    int const nbFichiers = n*w/nbBitsCycle+1;
    int const nbNombresAleatoires = nbFichiers*tailleCycle;
    int const nbOctetsAleatoires = n*w/8;
    
    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");
    mercenneSlayer.randint(0, 255);

    int nbCycles = 1+tailleIV*2;//+tailleNom*2;
    int tailleCycles[nbCycles];
    int nbBitsCycles[nbCycles];

    tailleCycles[0] = tailleCle*2+tailleNom*2;
    nbBitsCycles[0] = 0;

    for (int i=1; i<nbCycles; i++)
    {
        tailleCycles[i] = 1;
    }
/*
    for (int i=0; i<tailleNom*2; i++)
    {
        nbBitsCycles[i] = nbBitsEtatNom;
    }*/

    for (int i=1; i<nbCycles; i++)
    {
        nbBitsCycles[i] = nbBitsEtatIV;
    }

    W *iEtats = mercenneSlayer.etats;
    Matrice etats(intToChars(mercenneSlayer.etats, 624), 624*32);
    mercenneSlayer.sEtatsSEtats("tests/exempleEtat");

    //Construction du tableau intermediaire
    uint8_t iNombresAleatoires[nbNombresAleatoires]; //Suite des ivs generes par CryPy

    for (short j=0; j<decalage; j++) mercenneSlayer.extraire();

    for (short i=0; i<nbNombresAleatoires/tailleCycle; i++)
    {
        for (short j=0; j<32; j++) mercenneSlayer.randint(0, 255);
        for (short j=0; j<36; j++)
        {
            //iNombresAleatoires[i*tailleCycle+j] = (mercenneSlayer.extraire() >> 24) & 0xF8;
            mercenneSlayer.extraire();
            mercenneSlayer.extraire();
        }
        for (short j=0; j<16; j++) iNombresAleatoires[i*tailleCycle+j] = mercenneSlayer.randint(0, 255);
    }

    cout << "iNombresAleatoires" << endl << Matrice(iNombresAleatoires, nbNombresAleatoires*8).hexa() << endl;
return;
    //Construction de la matrice de nombre aleatoires

    Matrice nombresAleatoires(iNombresAleatoires, n*w);//(1, 624*32);

/*
    for (short i=0; i<nbFichiers; i++)
    {
        for (short j=0; j<tailleNom; j++)
        {
            if (i*nbBitsCycle+j*nbBitsEtatNom < n*w)
            {
                nombresAleatoires.coller(Matrice(iNombresAleatoires+i*tailleCycle+j, nbBitsEtatNom), 0, i*nbBitsCycle+j*nbBitsEtatNom, 1, min(i*nbBitsCycle+(j+1)*nbBitsEtatNom, n*w));
            }
        }

        for (short j=0; j<tailleIV; j++)
        {
            if (i*nbBitsCycle+nbBitsNom+j*nbBitsEtatIV <= n*w)
            {
                nombresAleatoires.coller(Matrice(iNombresAleatoires+i*tailleCycle+tailleNom+j, nbBitsEtatIV), 0, i*nbBitsCycle+nbBitsNom+j*nbBitsEtatIV,
                                                                                                              1, min(i*nbBitsCycle+nbBitsNom+(j+1)*nbBitsEtatIV, n*w));
            }
        }
    }*/

    cout << "nombreAleatoires" << endl << nombresAleatoires.hexa() << endl;
    cout << "etats" << endl << etats.hexa() << endl;

    short i;
    Matrice matriceCrypy = mercenneSlayer.matriceGeneraleCrypy(nbBitsCycles, tailleCycles, nbCycles);
    Matrice randintsMatrice = matriceCrypy*etats;
    Matrice randintsMatriceT = randintsMatrice.transposee();
    Matrice nombresAleatoiresT = nombresAleatoires.transposee();

    cout << "randint trouves avec matrice " << endl << randintsMatriceT.hexa() << endl;

    for (i=0; i<nbOctetsAleatoires; i++) if (randintsMatriceT.get8(i, 0) != nombresAleatoiresT.get8(i, 0)) break;

    if (i < nbOctetsAleatoires)
    {
        cout << "egalite jusque : " << i << " : " << hex << (short)randintsMatriceT.get8(i, 0) << hex << (short)randintsMatriceT.get8(i+1, 0) << " != "
                                                  << hex << (short)nombresAleatoiresT.get8(i, 0) << hex << (short)nombresAleatoiresT.get8(i+1, 0) << endl;
    }
    else cout << "matriceCrypy correcte" << endl << endl;

    //On ajoute des contraintes
    int const finReferences = 623*32;
    Matrice contraintes(finReferences+1, 624*32);
    contraintes.coller(Matrice::id(finReferences), 0, 0, finReferences, finReferences); //Les n-1 premiers etats servent de reference

    Matrice A(mercenneSlayer.w, mercenneSlayer.w);
    A.coller(Matrice::id(mercenneSlayer.w-1), 0, 1, mercenneSlayer.w-1, mercenneSlayer.w);
    A.coller(Matrice(intToChar(mercenneSlayer.a), mercenneSlayer.w), mercenneSlayer.w-1, 0, mercenneSlayer.w, mercenneSlayer.w);

    Matrice iPassage(623*32+1, 32);
    iPassage.coller(Matrice::id(31), 1, 1, 32, 32);
    iPassage.set(623*32, 0, 1);
    
    contraintes.coller(A*iPassage, 0, finReferences, finReferences+1, 624*32);
    contraintes.coller(Matrice::id(32), (397-1)*32, finReferences, 397*32, 624*32);

    //On verifie que les contraintes sont justes
    Matrice sousEtats(1, finReferences+1);
    for (short i=0; i<finReferences; i++) sousEtats.set(0, i, etats.get(0, i));
    sousEtats.set(0, finReferences, 0); //Inconnu, a modifier pour les tests

    Matrice transposeeEtatsContraint = (contraintes*sousEtats).transposee();
    Matrice transposeeEtats = etats.transposee();
    Matrice sousEtatsT = sousEtats.transposee();

    cout << "etat a partir de sous etat" << endl << transposeeEtatsContraint.hexa() << endl;


    for (i=0; i<624*4; i++) if (transposeeEtatsContraint.get8(i, 0) != transposeeEtats.get8(i, 0)) break;

    if (i < 624*4)
    {
        cout << "egalite jusque : " << i << " : " << hex << (short)transposeeEtatsContraint.get8(i, 0) << hex << (short)transposeeEtatsContraint.get8(i+1, 0) << " != "
                                                  << hex << (short)transposeeEtats.get8(i, 0) << hex << (short)transposeeEtats.get8(i+1, 0) << endl;
    }
    else cout << "contraintes correcte" << endl;


    //Test de l'inversion
    Matrice matriceCrypyContrainte("matrices/matriceCrypyContrainte_"+to_string(decalage)+"_"+to_string(nbCycles)+".ms");

    if (!matriceCrypyContrainte.isOk())
    {
        matriceCrypyContrainte = matriceCrypy*contraintes;
        matriceCrypyContrainte.exporter("matrices/matriceCrypyContrainte_"+to_string(decalage)+"_"+to_string(nbCycles)+".ms");
    }

    Matrice etatsRetrouves = contraintes*(matriceCrypyContrainte.inverser(randintsMatrice));
    cout << "etats retrouves :" << endl << etatsRetrouves.hexa() << endl;

    Matrice randintRetrouvesT = (matriceCrypy*etatsRetrouves).transposee();
    cout << "randint avec etat retrouves" << endl << randintRetrouvesT.hexa() << endl;

    for (i=0; i<nbOctetsAleatoires; i++) if (randintRetrouvesT.get8(i, 0) != randintsMatriceT.get8(i, 0)) break;

    if (i < nbOctetsAleatoires)
    {
        cout << "egalite jusque : " << i << " : " << hex << (short)randintRetrouvesT.get8(i, 0) << hex << (short)randintRetrouvesT.get8(i+1, 0) << " != "
                                                  << hex << (short)randintsMatriceT.get8(i, 0) << hex << (short)randintsMatriceT.get8(i+1, 0) << endl;
    }
    else cout << "inversion correcte" << endl;

    cout << "fin" << endl;
}
