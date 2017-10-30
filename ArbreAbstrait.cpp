#include <stdlib.h>
#include <iostream>
#include <typeinfo>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
    if (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(vector<Noeud*> vectSi, Noeud* sequenceSinon)
: m_vectSi(vectSi), m_sequenceSinon(sequenceSinon){
}

int NoeudInstSiRiche::executer() {
    int i;
    // on cherche la premiere condition vraie
    /*while (i<m_vectSi.size() && ! m_vectSi[i]->executer()) 
        i+=2;
    if (i<m_vectSi.size()) m_vectSi[i]->executer();
    else if (m_sequenceSinon) m_sequenceSinon->executer(); */
    for (i=0; i < m_vectSi.size() ; i++){
        m_vectSi[i]->executer();
    }
    if (i<m_vectSi.size()) m_vectSi[i]->executer();
    else if (m_sequenceSinon) m_sequenceSinon->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSiRiche::traduitEnCPP(ostream cout,unsigned int indentation)const {
    int i = 0;
    cout << setw(4*indentation) << "" << "if (";// Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_vectSi[i]->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage 
    cout << ") {" << endl;// Ecrit ") {" et passe à la ligne 
    m_vectSi[i+1]->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
    while (i <= m_vectSi) {
        i = i+2;
        cout << setw(4*indentation) <<""<<"else if (";// Ecrit "else if (" avec un décalage de 4*indentation espaces 
        m_vectSi[i]->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage 
        cout <<") {"<<endl;// Ecrit ") {" et passe à la ligne 
        m_vectSi[i+1]->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
        cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne
    }
    if (m_sequenceSinon) {
        cout << setw(4*indentation) <<""<<"else {";// Ecrit "else {" avec un décalage de 4*indentation espaces 
        m_sequenceSinon->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
        cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* instruction, Noeud* expression)
: m_instruction(instruction), m_expression(expression) {
}

int NoeudInstRepeter::executer() {
    do
        m_instruction->executer(); while (!m_expression->executer());
    return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstPour::NoeudInstPour(Noeud* init, Noeud* cond, Noeud* incr, Noeud* sequence)
: m_initialisation(init), m_condition(cond), m_incrementation(incr), m_seqInstruction(sequence) {

}

int NoeudInstPour::executer() {
    for ((m_initialisation) ? m_initialisation->executer() : 0;
            m_condition->executer();
            (m_incrementation) ? m_incrementation->executer() : 0
            ) m_seqInstruction->executer();
}


// Classe pour représenter un noeud "ecrire"

NoeudEcrire::NoeudEcrire()
:m_chainesEtExpressions(){

}

int NoeudEcrire::executer(){
    for(auto p : m_chainesEtExpressions){
        // on regarde si l’objet pointé par p est de type SymboleValue et si c’est une chaîne
        if ( (typeid(*p)==typeid(SymboleValue) && *((SymboleValue*)p)== "<CHAINE>" )){
            cout<<((SymboleValue*)p)->getChaine();
        }
        else{
            cout<<p->executer();
        }
    }
    return 0;
}

// Classe pour représenter un noeud "lire"

NoeudLire::NoeudLire()
:m_variables(){

}

int NoeudLire::executer(){
    for(auto p : m_variables){
        int valeur;
        cin>>valeur; //on lit la valeur de le la variable p sur le flux d'entrée
        ((SymboleValue*) p)->setValeur(valeur); //on affecte la valeur à la variable
    }
    return 0;
}



void NoeudInstTantQue::traduitEnCPP(ostream cout, unsigned int indentation) const {
    cout << setw(4*indentation) <<""<<"while (";// Ecrit "while (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage 
    cout <<") {"<<endl;// Ecrit ") {" et passe à la ligne 
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne
}

void NoeudInstRepeter::traduitEnCPP(ostream cout, unsigned int indentation) const {
    cout << setw(4*indentation) <<""<<"for (";// Ecrit "for (" avec un décalage de 4*indentation espaces 
    m_instruction->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage 
    cout <<") {"<<endl;// Ecrit ") {" et passe à la ligne 
    m_expression->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne
}

void NoeudInstPour::traduitEnCPP(ostream cout, unsigned int indentation) const {
    cout << setw(4*indentation) <<""<<"for (";// Ecrit "for (" avec un décalage de 4*indentation espaces 
    m_initialisation->traduitEnCPP(cout,0);// Traduit l'initialisation en C++ sans décalage 
    cout << "; ";
    m_condition->traduitEnCPP(cout, 0);// Traduit la condition en C++ sans décalage
    cout << "; ";
    m_incrementation->traduitEnCPP(cout, 0);// Traduit l'incrémentation en C++ sans décalage
    cout <<") {"<<endl;// Ecrit ") {" et passe à la ligne 
    m_seqInstruction->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne
}

void NoeudEcrire::traduitEnCPP(ostream cout, unsigned int indentation) const {
    
}

void NoeudLire::traduitEnCPP(ostream cout, unsigned int indentation) const {
    
}