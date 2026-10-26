# Cahier de bord du projet – Groupe

---

## Semaine 1

### Lundi
Nous avons commencé la prise en main de SDL au sein du groupe. Nous avons exécuté les exemples fournis en cours afin de comprendre le fonctionnement des fenêtres, leur ouverture et leur fermeture, ainsi que les bases des boucles événementielles.

Le travail était principalement individuel, chacun réalisant des tests pour mieux comprendre le fonctionnement global. Nous avons réussi à finaliser la partie sur la gestion des fenêtres, même si nous étions déjà légèrement en retard sur le planning initial. Après les heures obligatoires, nous avons poursuivi les tests pour approfondir notre compréhension.

---

### Mardi
Nous avons travaillé sur les objets géométriques à partir des programmes fournis (rectangles, cercles, lignes, couleurs). Nous avons tenté de réaliser une animation avec un carré rebondissant dans la fenêtre.

La gestion des collisions avec les bords a posé problème : le programme compilait mais le comportement restait incorrect. Malgré plusieurs essais, nous n’avons pas obtenu un résultat satisfaisant et avons décidé de mettre ce point en pause.

Nous avons ensuite travaillé sur les textures et les images : chargement, création de textures, affichage et animation. Nous avons découvert les spritesheets et leur utilisation pour les animations.

Nous avons principalement insisté sur la compréhension des sprites, puis réalisé qu’il fallait également approfondir la notion de fond défilant demandée dans l’exercice.

---

### Mercredi – Jeudi
Nous avons commencé le projet multi-agents autour du jeu de la vie. Une grande partie du temps a été consacrée à la réflexion et à la modélisation avant l’implémentation.

Nous avons organisé le projet en modules :
- Amani et moi : logique du monde, agents, maître du jeu  
- Meryem : interface graphique SDL  

Nous avons modélisé le monde sous forme de grille où chaque case représente une cellule vivante ou morte. Une structure de perception a été définie, contenant l’état de la cellule et le nombre de voisins vivants.

Une fonction permet de construire ces perceptions à partir de l’état global du monde.

Nous avons implémenté une initialisation aléatoire de la grille.

Nous avons également défini le rôle du maître du jeu : transmettre les perceptions, récupérer les décisions des agents et appliquer les actions de manière synchrone.

Sur la partie graphique, nous avons rencontré des difficultés sur le zoom, le déplacement de la vue et les interactions utilisateur (clic sur cellule). Plusieurs tests et recherches ont été nécessaires.

Nous avons réfléchi à l’optimisation de l’affichage en ne mettant à jour que les cellules modifiées.

---

### Vendredi
Première soutenance du projet. Nous avons constaté un manque de préparation à l’oral malgré une partie fonctionnelle du projet.

Après la soutenance, nous avons analysé les problèmes : manque de structuration et importance sous-estimée de certains éléments (carnet de bord, diagramme de Gantt).

Nous avons ensuite commencé la réflexion sur le projet suivant : un système prédateur-proie (loups et moutons). Plusieurs idées ont été discutées et confrontées.

Le professeur nous a conseillé de stabiliser une modélisation solide avant d’ajouter des interactions complexes.

---

## Week-end
Nous avons travaillé sur la modélisation du système loups-moutons. Après plusieurs discussions, nous avons trouvé un consensus et organisé le projet en modules.

Nous avons commencé l’implémentation et repris la préparation de la soutenance de rattrapage. Nous avons ajouté les éléments manquants (diagramme de Gantt, répartition des tâches, structuration de la présentation).

Nous avons également travaillé la préparation orale pour améliorer la fluidité et la coordination.

---

## Semaine 2

### Lundi – Mardi
Nous avons réfléchi à l’ajout d’un mécanisme d’apprentissage dans le système loups-moutons.

Objectifs définis :
- Les moutons doivent survivre plus longtemps en évitant les loups et en recherchant de la nourriture  
- Les loups doivent améliorer leurs stratégies de chasse  

Nous avons travaillé sur les perceptions, actions et récompenses afin de garder un modèle cohérent.

Nous avons ajouté une idée importante : la chasse coopérative chez les loups. Une capture nécessite plusieurs loups et la récompense est partagée entre eux.

Le professeur a validé l’approche et proposé une amélioration : découper la grille en zones afin de calculer des densités (herbe pour les moutons, proies pour les loups), afin d’apporter une vision globale aux agents.

---

### Mercredi – Jeudi
Nous avons intégré le mécanisme d’apprentissage dans le système.

- Meryem : intégration de la partie apprentissage et comportements  
- Moi : adaptation de la modélisation et du système global  
- Amani : parallélisation et optimisation des traitements  

Nous avons travaillé sur la cohérence globale du système et la stabilisation du code.

---

### Jeudi soir
Nous avons préparé la soutenance finale :
- création des slides  
- répartition des rôles à l’oral  
- mise à jour du cahier de charge et du diagramme de Gantt  
- répétitions de la présentation  

---