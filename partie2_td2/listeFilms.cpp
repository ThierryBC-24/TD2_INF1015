#include "structures.hpp"
using namespace std;
using namespace iter;
using namespace gsl;

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
std::string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

void ListeFilms::creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nombreFilms = lireUint16(fichier);

	//TODO: Créer une liste de films vide.

	for (int i = 0; i < nombreFilms; i++) {
		//TODO: Ajouter le film à la liste.
		ajouterFilm(lireFilm(fichier));
	}
}

ListeFilms::ListeFilms() {
	capacite = 0;
	nElements = 0;
}

void ListeFilms::ajouterFilm(Film* film) {
	if (capacite <= 0) {
		capacite = 1;
		elements = new Film * [capacite];
	}
	if (capacite <= nElements) {
		Film** nouveauTableau = new Film * [capacite *= 2];
		for (unsigned i = 0; i < nElements; i++)
			nouveauTableau[i] = elements[i];
		delete[] elements;
		elements = nouveauTableau;
	}
	elements[nElements++] = film;
}
void ListeFilms::enleverFilm(Film* film) {
	for (int i : range(nElements)) {
		if (elements[i] == film) {
			Film* temp = elements[i];
			elements[i] = elements[nElements--];
			elements[nElements] = temp;
		}
	}
}

Acteur* ListeFilms::trouverActeur(string nom) {
	Acteur* acteurTrouve;
	acteurTrouve = nullptr;
	for (int i = 0; i < nElements;i++) {
		for (int j = 0; j < elements[i]->acteurs.nElements;j++ ) {
			if (elements[i]->acteurs.elements[j]->nom == nom)
				acteurTrouve = elements[i]->acteurs.elements[j];
		}
	}
	return acteurTrouve;
}

Acteur* ListeFilms::lireActeur(istream& fichier)
{
	Acteur acteur = {};
	Acteur* prtActeur = nullptr;
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	if (trouverActeur(acteur.nom) == nullptr) {
		cout << "Nouvel acteur cree: " << acteur.nom << "\n";
		prtActeur =  new Acteur (acteur);
	}
	else
		prtActeur = trouverActeur(acteur.nom); //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
	return prtActeur;
}

Film* ListeFilms::lireFilm(istream& fichier)
{
	Film film = {};
	Acteur* ptracteur;
	film.titre			= lireString(fichier);
	film.realisateur	= lireString(fichier);
	film.anneeSortie	= lireUint16(fichier);
	film.recette		= lireUint16(fichier);
	film.acteurs.nElements = lireUint8(fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	film.acteurs.capacite = film.acteurs.nElements + 1;
	film.acteurs.elements = new Acteur * [film.acteurs.capacite];
	Film* ptrFilm = new Film (film);
	for (int i = 0; i < film.acteurs.nElements; i++) {
		//TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		ptracteur =lireActeur(fichier);
		film.acteurs.elements[i] = ptracteur;

		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		ptracteur->joueDans.ajouterFilm(ptrFilm);
	}

	return ptrFilm; //TODO: Retourner le pointeur vers le nouveau film.
}

void ListeFilms::detruireFilm(string film) {
	int temp = 0;
	for (int i = 0; i < nElements;i++ ) {
		if (elements[i]->titre == film) {
			for (int j = 0; j < elements[i]->acteurs.nElements;j++) {
				if (elements[i]->acteurs.elements[j]->joueDans.nElements <= 1) {
					cout << "Acteur détruit: " << elements[i]->acteurs.elements[j]->nom << endl;
					delete[] elements[i]->acteurs.elements[j]->joueDans.elements;
					delete elements[i]->acteurs.elements[j];
				}
				else
				{
					elements[i]->acteurs.elements[j]->joueDans.enleverFilm(elements[i]);
				}
			}
			delete[] elements[i]->acteurs.elements;
			delete elements[i];
			temp = i;
			nElements--;
			break;
		}
	}
	Film** nouveauTableau = new Film * [capacite];
	for (unsigned i = 0; i < temp; i++)
		nouveauTableau[i] = elements[i];
	for (unsigned i = temp; i < nElements; i++)
		nouveauTableau[i] = elements[i + 1];
	delete[] elements;
	elements = nouveauTableau;
}
void ListeFilms::detruireListe() {
	int temp = nElements;
	for (int i = 0; i < temp; i++) {
		if (elements[0] != nullptr) {
			detruireFilm(elements[0]->titre);
		}
	}
	delete[] elements;
}

void ListeFilms::afficherFilm(const string filmNom) const {
	cout << filmNom << endl;
	for (int i = 0; i < nElements;i++) {
		if (elements[i]->titre == filmNom) {
			for (int j=0; j<elements[i]->acteurs.nElements;j++)
				cout << "  " << elements[i]->acteurs.elements[j]->nom << ", " << elements[i]->acteurs.elements[j]->anneeNaissance << " " << elements[i]->acteurs.elements[j]->sexe << endl;
		}
	}
}

void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35m==============================\033[0m\n";
	cout << ligneDeSeparation;
	for (int i = 0; i < nElements; i++) {
		//TODO: Afficher le film.
		if (elements[i] != nullptr) {
			afficherFilm(elements[i]->titre);
			cout << ligneDeSeparation;
		}
	}
}

void ListeFilms::afficherFilmographieActeur(const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	static const string ligneDeSeparation = "\n\033[35m==============================\033[0m\n";
	const Acteur* acteur = trouverActeur(nomActeur);
	if (acteur == nullptr) {
	cout << "Aucun acteur de ce nom" << endl;
	cout << ligneDeSeparation;
	}
	else
		acteur->joueDans.afficherListeFilms();
}