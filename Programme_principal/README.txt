• Outil de scan de système de fichier •

Cet outil permet de scanner tout un système de fichier et émet un rapport sur les fichiers qui semblent être chiffrés en précisant,
si possible, le ransomware par lequel le fichier a été effectivement corrompu.

*********************************
          Compilation
*********************************
La compilation des fichiers nécessite les librairies boost, WpdPack ainsi que g++. Soyez certains de l'avoir en référence dans
votre projet Visual Studio.
Le main se trouve dans "PI_DiskAnalyzer.cpp".

Notes de compilation:
Boost: - mettre le path vers le répertoire du dossier principal de Boost dans Propriétés de configuration -> C/C++ -> Autres répertoires Include
WpdPack: - mettre le path vers le répertoire Include de WpdPack dans Propriétés de configuration -> Répertoires VC++ -> Répertoires Include
         - mettre le path vers le répertoire Lib de WpdPack dans Propriétés de configuration -> Répertoires VC++ -> Répertoires de bibliothèques
         - ajouter la chaîne WPCAP dans Propriétés de configuration -> C/C++ -> Préprocesseur -> Définitions de préprocesseur
         - ajouter la chaîne wpcap.lib dans Propriétés de configuration -> Éditeur de liens -> Dépendances supplémentaires
	 - Installer WINPcap, qui ajoutera la DLL nécessaire dans System 32.
// WinPcap requirements:
// Libray: https://www.winpcap.org/devel.htm
// Doc: https://www.winpcap.org/docs/docs_411/html/group__wpcapsamps.html

*********************************
          Exécution
*********************************
Au lancement, le programme demande de choisir entre deux modes : l'analyse du système de fichier ou la restauration des fichiers.
Notez qu'une analyse doit avoir été faite pour pouvoir lancer une éventuelle restauration (l'interface le précise).

Si vous souhaitez utiliser les fonctions de restauration d'HiddenTear, il vous faut un lien vers les exécutables HiddenTear que vous pouvez générer à partir du code C# fourni dans le dépot. Dans notre cas, nous compilons ces programmes pour le framework .NET 4, compatible avec Windows XP.
