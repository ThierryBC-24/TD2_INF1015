﻿// Programme qui détermine si une année lue du clavier est bissextile.
// file   bissextile.cpp
// author Bédard-Cortey et Li
// date 14 février 2021
// Créé le 29 janvier 2021

#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include "cppitertools/range.hpp"
#include "gsl/span"
using namespace std;
using namespace iter;
using namespace gsl;

void afficherFilm(const Film& Film);

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ajouterFilm(ListeFilms& listeFilms, Film* film) {
	if (listeFilms.capacite <= 0) {
		listeFilms.capacite = 1;
		listeFilms.elements = new Film * [listeFilms.capacite];
	}
	else {
		if (listeFilms.capacite <= listeFilms.nElements) {
			Film** nouveauTableau = new Film * [listeFilms.capacite *= 2];
			for (unsigned i = 0; i < listeFilms.nElements; i++)
				nouveauTableau[i] = listeFilms.elements[i];
			delete[] listeFilms.elements;
			listeFilms.elements = nouveauTableau;
		}
	}
	listeFilms.elements[listeFilms.nElements++] = film;
}

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
void enleverFilm(ListeFilms& listeFilms, Film* film) {
	for (int i = 0; i < listeFilms.nElements; i++) {
		if (listeFilms.elements[i]->titre == film->titre) {
			listeFilms.elements[i] = listeFilms.elements[--listeFilms.nElements];
			listeFilms.elements[--listeFilms.nElements] = nullptr;
			listeFilms.nElements--;
			i = listeFilms.nElements;
		}
	}
}

//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
Acteur* trouverActeur(ListeFilms listeFilms, string nom) {
	Acteur* acteurTrouve = {};
	acteurTrouve = nullptr;
	for (int i : range(listeFilms.nElements)) {
		for (int j : range(listeFilms.elements[i]->acteurs.nElements)) {
			if (listeFilms.elements[i]->acteurs.elements[j]->nom == nom)
				acteurTrouve = listeFilms.elements[i]->acteurs.elements[j];
		}
	}
	return acteurTrouve;
}

//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
Acteur* lireActeur(ListeFilms list, istream& fichier)
{
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	acteur.joueDans.nElements = 0;
	acteur.joueDans.capacite = 0;
	if (trouverActeur(list, acteur.nom) == nullptr) {
		cout << "Nouvel acteur cree: " << acteur.nom << "\n";
		return new Acteur(acteur);
	}
	else
		return trouverActeur(list, acteur.nom); //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}

Film* lireFilm(ListeFilms listeFilms, istream& fichier)
{
	Film film = {};
	film.titre = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = lireUint16(fichier);
	film.recette = lireUint16(fichier);
	film.acteurs.nElements = lireUint8(fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	film.acteurs.elements = new Acteur * [film.acteurs.nElements + 1];
	film.acteurs.capacite = film.acteurs.nElements + 1;
	for (int i = 0; i < film.acteurs.nElements; i++) {
		//TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		Acteur* ptracteur = lireActeur(listeFilms, fichier);
		film.acteurs.elements[i] = ptracteur;

		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		ajouterFilm(ptracteur->joueDans, new Film(film));
		cout << "!";
	}
	return new Film(film); //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);

	//TODO: Créer une liste de films vide.
	ListeFilms listeFilms = {};
	for (int i = 0; i < nElements; i++) {
		//TODO: Ajouter le film à la liste.
		ajouterFilm(listeFilms, lireFilm(listeFilms, fichier));
	}
	return listeFilms; //TODO: Retourner la liste de films.
}

//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.
void detruireFilm(ListeFilms& listeFilms, string titreFilm) {
	int temp = 0;
	for (int i = 0; i < listeFilms.nElements; i++) {
		if (listeFilms.elements[i]->titre == titreFilm) {
			for (int j : range(listeFilms.elements[i]->acteurs.nElements)) {
				int nElementJoueDans = listeFilms.elements[i]->acteurs.elements[j]->joueDans.nElements;
				for (int k : range(nElementJoueDans)) {
					if (listeFilms.elements[i]->acteurs.elements[j]->joueDans.elements[k]->titre == titreFilm) {
						enleverFilm(listeFilms, listeFilms.elements[i]->acteurs.elements[j]->joueDans.elements[k]);
						//delete listeFilms.elements[i]->acteurs.elements[j]->joueDans.elements[k];
						listeFilms.elements[i]->acteurs.elements[j]->joueDans.nElements--;
						if (listeFilms.elements[i]->acteurs.elements[j]->joueDans.nElements == 0) {
							cout << "Acteur détruit: " << listeFilms.elements[i]->acteurs.elements[j]->nom << endl;
							delete listeFilms.elements[i]->acteurs.elements[j];
							listeFilms.elements[i]->acteurs.nElements--;
						}
						break;
					}
				}
			}
			delete listeFilms.elements[i];
			temp = i;
			listeFilms.nElements--;
			i = listeFilms.nElements;
		}
	}
	Film** nouveauTableau = new Film * [listeFilms.capacite];
	for (unsigned i = temp; i < listeFilms.nElements; i++)
		nouveauTableau[i] = listeFilms.elements[i + 1];
	delete[] listeFilms.elements;
	listeFilms.elements = nouveauTableau;
}

//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
void detruireListe(ListeFilms& listeFilms) {
	int nFilms = listeFilms.nElements;
	for (int i : range(nFilms)) {
		detruireFilm(listeFilms, listeFilms.elements[0]->titre);
	}
	delete[] listeFilms.elements;
}

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
void afficherFilm(const Film& Film) {
	cout << Film.titre << endl;
	for (int i : range(Film.acteurs.nElements)) {
		afficherActeur(*Film.acteurs.elements[i]);
	}
}

void afficherListeFilms(const ListeFilms& listeFilms)
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35m==============================\033[0m\n";;
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	span<Film*> monSpan(listeFilms.elements, listeFilms.nElements);
	for (int i = 0; i < monSpan.size(); i++) {
		//TODO: Afficher le film.
		if (monSpan[i] != nullptr) {
			afficherFilm(*(monSpan[i]));
			cout << ligneDeSeparation;
		}
	}
}

void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = trouverActeur(listeFilms, nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms(acteur->joueDans);
}

int main()
{
#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
#endif
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	 //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Detected memory leak" de "4 bytes" affiché dans la "Sortie", qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms = creerListe("films.bin");

	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	afficherFilm(*listeFilms.elements[0]);

	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	afficherListeFilms(listeFilms);
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	Acteur* ptrActeur = trouverActeur(listeFilms, "Benedict Cumberbatch");
	ptrActeur->anneeNaissance = 1976;
	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	afficherFilmographieActeur(listeFilms, "Benedict Cumberbatch");
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	detruireFilm(listeFilms, "Alien");
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	afficherListeFilms(listeFilms);
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	enleverFilm(listeFilms, listeFilms.elements[0]);
	afficherFilmographieActeur(listeFilms, "Tom Hanks");
	//TODO: Détruire tout avant de terminer le programme.  L'objet verifierFuitesAllocations devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	detruireListe(listeFilms);
}