• Outil de scan de système de fichier •

Cet outil permet de scanner tout un système de fichier et émet un rapport sur les fichiers qui semblent être chiffrés en précisant,
si possible, le ransomware par lequel le fichier a été effectivement corrompu.

*********************************
          Compilation
*********************************
La compilation des fichiers nécessite la librairie boost ainsi que g++. Soyez certains de l'avoir en référence dans
votre projet Visual Studio.
Le main se trouve dans "PI_DiskAnalyzer.cpp".

*********************************
          Exécution
*********************************
Au lancement, le programme demande de choisir entre deux modes : l'analyse du système de fichier ou la restauration des fichiers.
Notez qu'une analyse doit avoir été faite pour pouvoir lancer une éventuelle restauration (l'interface le précise).
