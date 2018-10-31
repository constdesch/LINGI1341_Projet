# LINGI1341_Projet
Projet de LINGI1341 - Reseaux informatiques
@author: Constantin de Schaetzen and Jean Gillain
@date: October 2018

Pour utiliser les programmes contenus dans le repertoire /src
veuillez specifier au moins 2 arguments, a savoir le nom de domaine ou l'adresse IPV6 du receveur et le numero de port UDP.

L'option -f peut etre mentionnée et a un role different en fonction de la fonction qu'elle complete :

@sender: -f filename.txt permet de lire l'information sur le fichier nommé filename.txt
@receiver: -f filename.txt permet d'ecrire l'information recue sur le fichier nommé filename.txt

La commande "make clean" permet de supprimer certains .o après utilisation

Vous pouvez utiliser simplement les fonctions en compilant avec la commande make.
Pour tester le programme, nous vous proposons les deux méthodes suivantes :

1 - Ouvrez deux terminaux et rendez-vous dans le repertoire projet1_deSchaetzen_Gillain dans chaque terminal
2 - Dans le premier terminal, écrivez : make receive
3 - Dans le seconde terminal, écriver : make send
4 - Rendez-vous dans fileout.txt pour lire le contenu transféré depuis file.txt  

deuxième solution :
1 - Ouvrez un terminal et rendez-vous dans le repertoire projet1_deSchaetzen_Gillain
2 - Ecrivez : make test
3 - Rendez-vous dans receiver.log pour voir le résultat

La deuxième solution lancera le script test.sh 

Pour effectuer les tests disponibles dans le dossier /tests:
make test_queue lancera le test des fonctions relatives aux queues et aux noeuds
make test_decode_encode lancera le test des fonctions encode et decode

Enfin, pour lancer valgrind et traquer les memory leaks:

1 - Ouvrez deux terminaux et rendez-vous dans le repertoire projet1_deSchaetzen_Gillain dans chaque terminal
2 - Dans le premier terminal, écrivez : make valgrind_receive
3 - Dans le seconde terminal, écriver : make valgrind_send
