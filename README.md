# Système multi-agents : proie-prédateur

Simulation d'une dynamique proie-prédateur (loups et moutons) où les agents
apprennent leur comportement par apprentissage par renforcement.
Projet développé en C, avec visualisation en temps réel via SDL2.

## Fonctionnalités

- Environnement de simulation avec des loups, des moutons, de l'herbe et des rochers
- Apprentissage d'une politique par espèce (`politique_loup.txt`, `politique_mouton.txt`)
- Visualisation en temps réel avec SDL2

## Compilation

    sudo apt install libsdl2-dev libsdl2-image-dev   (à vérifier)
    make

## Utilisation

    ./train     # entraînement des politiques   (à vérifier)
    ./viewer    # visualisation de la simulation (à vérifier)

## Organisation du code

- `monde.c` : environnement de simulation
- `episode.c` : déroulement d'un épisode
- `politique.c`, `reinforce.c` : politiques et apprentissage
- `rendu.c`, `main_sdl.c` : affichage SDL2
- `main_train.c` : programme d'entraînement
- `sprites/` : images utilisées par l'affichage
- `cahier_de_bord.md` : journal de développement

## Auteure
Amani Trabelsi
