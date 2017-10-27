#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
    string nomFich;
    if (argc != 2) {
        cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
        cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
        getline(cin, nomFich);
    } else
        nomFich = argv[1];
    ifstream fichier(nomFich.c_str());
    try {
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        if (!interpreteur.getNbErreurs()) {
            // Si pas d'exception levée, l'analyse syntaxique a réussi
            cout << endl << "================ Syntaxe Correcte" << endl;
            // On exécute le programme            // On affiche le contenu de la table des symboles avant d'exécuter le programme
            cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
            cout << endl << "================ Execution de l'arbre" << endl;
            interpreteur.getArbre()->executer();
            // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
            cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
        } else
            cout << interpreteur.getNbErreurs() << "erreur(s) de syntaxe" << endl;
    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
    }
    return 0;
}