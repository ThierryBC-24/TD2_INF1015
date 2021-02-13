#pragma once
// Structures mémoires pour une collection de films.

#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.


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

typedef uint8_t UInt8;
typedef uint16_t UInt16;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.
struct ListeActeurs;


UInt8 lireUint8(std::istream& fichier);
UInt16 lireUint16(std::istream& fichier);
std::string lireString(std::istream& fichier);

class ListeFilms {
private:
	int capacite, nElements;
	Film** elements; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.

public:
	ListeFilms();
	void creerListe(std::string nom);
	void ajouterFilm(Film* film);
	void enleverFilm(Film* film);
	Acteur* trouverActeur(std::string nom);
	Acteur* lireActeur(std::istream& fichier);
	Film* lireFilm(std::istream& fichier);
	void detruireFilm(std::string film);
	void detruireListe();
	void afficherFilm(const std::string filmNom)const;
	void afficherListeFilms()const;
	void afficherFilmographieActeur(const std::string& nomActeur);
};


struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};


