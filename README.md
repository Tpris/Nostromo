# Nostromo

Lors de l'exécution de Nostromo.c, 8 passagers sont générés.
Chaque passager a un nom spécifique et une race (humain ou alien).
Parmi ces passagers doivent figurer exactement un Alien et une humaine appelée Ellen Ripley.

Toutes les 3 secondes le processus père envoie un signal SIGUSR1 à l'un des passagers et l'indique dans le terminal.(1)  
Lors de la réception du signal :

- Si le passager est humain et n'est pas Ellen Ripley, il doit écrire dans un fichier [pid].txt(2) :  "Je suis [nom](3) et ma fin est proche." puis se terminer.
- Si le passager est Ellen Ripley, il ne se passe rien.
- Si le passager est l'alien, il doit afficher sa mort(4) dans le terminal et envoyer un signal à son père.

Lors de la réception du signal de l'alien par le père, celui-ci :

- indique que c'est la fin de la partie(5)
- affiche dans le terminal le nombre de survivants humains
- termine tous ses fils restant encore en vie

avant de conclure par : "Dans l’espace personne ne vous entend crier."  
  
(1) leurs pid devront donc être stockés dans un tableau  
(2) remplacez [pid] par le pid du passager en chaîne de caractères  
(3) remplacez [nom] par le nom du passager  
(4) une épitaphe / un onomatopée d'alien qui meurt / etc.  
(5) "L'alien n'était pas un imposteur" / "GAME!" / "Victoire" / etc.  
