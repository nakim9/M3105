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
    for (unsigned int i = 0; i < m_instructions.size(); i++) {
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    for (auto inst : m_instructions) {
        inst->traduitEnCPP(cout, indentation);
        if (typeid (*inst) == typeid (NoeudAffectation) || typeid (*inst) == typeid (NoeudInstRepeter) || typeid (*inst) == typeid (NoeudLire) || typeid (*inst) == typeid (NoeudEcrire)) {
            cout << ";";
        }
        cout << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

NoeudAffectation::NoeudAffectation(Noeud* variable, string type)
: m_variable(variable), m_type(type) {
}

int NoeudAffectation::executer() {
    if (m_expression != nullptr) {
        int valeur = m_expression->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
        return 0; // La valeur renvoyée ne représente rien !
    } else if (m_expression == nullptr && m_type == "postIncr") {
        Noeud* exp = m_variable;
        int valeur = exp->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(valeur++); // On affecte la variable
        return 0; // La valeur renvoyée ne représente rien !
    } else if (m_expression == nullptr && m_type == "postDecr") {
        Noeud* exp = m_variable;
        int valeur = exp->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(valeur--); // On affecte la variable
        return 0; // La valeur renvoyée ne représente rien !
    } else if (m_expression == nullptr && m_type == "preIncr") {
        Noeud* exp = m_variable;
        int valeur = exp->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(++valeur); // On affecte la variable
        return 0; // La valeur renvoyée ne représente rien !
    } else if (m_expression == nullptr && m_type == "preDecr") {
        Noeud* exp = m_variable;
        int valeur = exp->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(--valeur); // On affecte la variable
        return 0; // La valeur renvoyée ne représente rien !
    }
}

void NoeudAffectation::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    if (m_expression != nullptr) {
        m_variable->traduitEnCPP(cout, indentation);
        cout << " = ";
        m_expression->traduitEnCPP(cout, 0);
    } else if (m_expression == nullptr && m_type == "postIncr") {
        m_variable->traduitEnCPP(cout, indentation);
        cout << "" << "++";
    } else if (m_expression == nullptr && m_type == "postDecr") {
        m_variable->traduitEnCPP(cout, indentation);
        cout << "" << "--";
    } else if (m_expression == nullptr && m_type == "preIncr") {
        cout << setw(4 * indentation) << "++";
        m_variable->traduitEnCPP(cout, 0);
    } else if (m_expression == nullptr && m_type == "preDecr") {
        cout << setw(4 * indentation) << "--";
        m_variable->traduitEnCPP(cout, 0);
    }
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

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    string operateur;
    if (this->m_operateur == "+") operateur = "+";
    else if (this->m_operateur == "-") operateur = "-";
    else if (this->m_operateur == "*") operateur = "*";
    else if (this->m_operateur == "==") operateur = "==";
    else if (this->m_operateur == "!=") operateur = "!=";
    else if (this->m_operateur == "<") operateur = "<";
    else if (this->m_operateur == ">") operateur = ">";
    else if (this->m_operateur == "<=") operateur = "<=";
    else if (this->m_operateur == ">=") operateur = ">=";
    else if (this->m_operateur == "et") operateur = "&&";
    else if (this->m_operateur == "ou") operateur = "||";
    else if (this->m_operateur == "non") operateur = "!";
    else if (this->m_operateur == "/") {
        operateur = "/";
    }
    m_operandeGauche->traduitEnCPP(cout, indentation);
    cout << " " << operateur << " ";
    m_operandeDroit->traduitEnCPP(cout, 0);
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

void NoeudInstSi::traduitEnCPP(ostream & cout, unsigned int indentation)const {
    cout << "("; // Ecrit "(" 
    m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage 
    cout << ") {" << endl; // Ecrit ") {" et passe à la ligne
    m_sequence->traduitEnCPP(cout, indentation); // Traduit en C++ la séquence avec indentation augmentée 
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(vector<Noeud*> vectSi, Noeud* sequenceSinon)
: m_vectSi(vectSi), m_sequenceSinon(sequenceSinon) {
}

int NoeudInstSiRiche::executer() {
    int i;
    // on cherche la premiere condition vraie
    /*while (i<m_vectSi.size() && ! m_vectSi[i]->executer()) 
        i+=2;
    if (i<m_vectSi.size()) m_vectSi[i]->executer();
    else if (m_sequenceSinon) m_sequenceSinon->executer(); */
    for (i = 0; i < m_vectSi.size(); i++) {
        m_vectSi[i]->executer();
    }
    if (i < m_vectSi.size()) m_vectSi[i]->executer();
    else if (m_sequenceSinon) m_sequenceSinon->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSiRiche::traduitEnCPP(ostream & cout, unsigned int indentation)const {
    int i = 0;
    cout << setw(4 * indentation) << "" << "if ";
    m_vectSi[i]->traduitEnCPP(cout, indentation + 1); // Traduit la condition en C++ sans décalage 
    cout << setw(4 * indentation) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne
    i++;
    while (i < m_vectSi.size()) {
        cout << setw(4 * indentation) << "" << "else if "; // Ecrit "else if (" avec un décalage de 4*indentation espaces 
        m_vectSi[i]->traduitEnCPP(cout, indentation + 1); // Traduit la condition en C++ sans décalage
        cout << setw(4 * indentation) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne
        i++;
    }
    if (m_sequenceSinon) {
        cout << setw(4 * indentation) << "" << "else {" << endl; // Ecrit "else {" avec un décalage de 4*indentation espaces 
        m_sequenceSinon->traduitEnCPP(cout, indentation + 1); // Traduit en C++ la séquence avec indentation augmentée 
        cout << setw(4 * indentation) << "" << "}"; // Ecrit "}" avec l'indentation initiale et passe à la ligne 
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

void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "while ("; // Ecrit "while (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage 
    cout << ") {" << endl; // Ecrit ") {" et passe à la ligne 
    m_sequence->traduitEnCPP(cout, indentation + 1); // Traduit en C++ la séquence avec indentation augmentée
    cout << setw(4 * indentation) << "" << "}"; // Ecrit "}" avec l'indentation initiale et passe à la ligne
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

void NoeudInstRepeter::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "do " << endl; // Ecrit "do " avec un décalage 
    m_instruction->traduitEnCPP(cout, indentation + 1); // Traduit la condition en C++ sans décalage 
    cout << setw(4 * indentation) << "" << "while ("; // écrit "while (" avec un décalage
    m_expression->traduitEnCPP(cout, 0); // Traduit en C++ la séquence avec indentation augmentée
    cout << ");"; // Ecrit "}" avec l'indentation initiale et passe à la ligne
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* init, Noeud* cond, Noeud* incr, Noeud* sequence)
: m_initialisation(init), m_condition(cond), m_incrementation(incr), m_seqInstruction(sequence) {

}

int NoeudInstPour::executer() {
    for ((m_initialisation) ? m_initialisation->executer() : 0;
            m_condition->executer();
            (m_incrementation) ? m_incrementation->executer() : 0
            ) m_seqInstruction->executer();
}

void NoeudInstPour::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    if(m_initialisation != nullptr && m_incrementation != nullptr){
        cout << setw(4 * indentation) << "" << "for ("; // Ecrit "for (" avec un décalage de 4*indentation espaces 
        m_initialisation->traduitEnCPP(cout, 0); // Traduit l'initialisation en C++ sans décalage 
        cout << "; ";
        m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage
        cout << "; ";
        m_incrementation->traduitEnCPP(cout, 0); // Traduit l'incrémentation en C++ sans décalage
        cout << ") {" << endl; // Ecrit ") {" et passe à la ligne 
        m_seqInstruction->traduitEnCPP(cout, indentation + 1); // Traduit en C++ la séquence avec indentation augmentée
        cout << setw(4 * indentation) << "" << "}"; // Ecrit "}" avec l'indentation initiale et passe à la ligne
    }else{
        if(m_initialisation != nullptr){
            m_initialisation->traduitEnCPP(cout, indentation);
            cout<<";"<<endl;
        }
        cout << setw(4 * indentation) << "" << "while ("; // Ecrit "for (" avec un décalage de 4*indentation espaces 
        m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage
        cout << ") {" << endl; // Ecrit ") {" et passe à la ligne 
        m_seqInstruction->traduitEnCPP(cout, indentation + 1); // Traduit en C++ la séquence avec indentation augmentée
        if(m_incrementation != nullptr){
            m_incrementation->traduitEnCPP(cout, indentation+1);
            cout<<";"<<endl;
        }             
        cout << setw(4 * indentation) << "" << "}"; // Ecrit "}" avec l'indentation initiale et passe à la ligne           
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudEcrire::NoeudEcrire()
: m_chainesEtExpressions() {

}

int NoeudEcrire::executer() {
    for (auto p : m_chainesEtExpressions) {
        // on regarde si l’objet pointé par p est de type SymboleValue et si c’est une chaîne
        if ((typeid (*p) == typeid (SymboleValue) && *((SymboleValue*) p) == "<CHAINE>")) {
            string chaine = ((SymboleValue*) p)->getChaine();
            int i = 1;
            string str = "";
            while (chaine[i] != '"') {
                str.push_back(chaine[i]);
                i++;
            }
            cout << str;
        } else {
            cout << p->executer();
        }
    }
    return 0;
}

void NoeudEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "cout";
    for (auto p : m_chainesEtExpressions) {
        // on regarde si l’objet pointé par p est de type SymboleValue et si c’est une chaîne
        if ((typeid (*p) == typeid (SymboleValue) && *((SymboleValue*) p) == "<CHAINE>")) {
            cout << " << " << ((SymboleValue*) p)->getChaine();
        } else {
            cout << " << ";
            p->traduitEnCPP(cout, 0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudLire::NoeudLire()
: m_variables() {

}

int NoeudLire::executer() {
    for (auto p : m_variables) {
        int valeur;
        cin>>valeur; //on lit la valeur de le la variable p sur le flux d'entrée
        ((SymboleValue*) p)->setValeur(valeur); //on affecte la valeur à la variable
    }
    return 0;
}

void NoeudLire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "cin";
    for (auto p : m_variables) {
        cout << " >> ";
        p->traduitEnCPP(cout, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSwitch
////////////////////////////////////////////////////////////////////////////////

NoeudInstSwitch::NoeudInstSwitch(Noeud* variable, Noeud* instructionsParDefaut, vector <Noeud*> vectCasCondition, vector <Noeud*> vectCasInstruction)
: m_variable(variable), m_instDefaut(instructionsParDefaut), m_vectCasCondition(vectCasCondition), m_vectCasInstruction(vectCasInstruction) {
}

int NoeudInstSwitch::executer() {
    int taille = m_vectCasCondition.size();
    bool execDefault = true;
    for (int i = 0; i < taille; i++) {
        if (m_variable->executer() == m_vectCasCondition[i]->executer()) {
            m_vectCasInstruction[i]->executer();
            execDefault = false; //on n'execute pas defaut si une condition est correcte
        }
    }
    if (execDefault && m_instDefaut != nullptr) {
        m_instDefaut->executer();
    }
}

void NoeudInstSwitch::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "switch (";
    m_variable->traduitEnCPP(cout, 0);
    cout << ") {";
    for (int i = 0; i < m_vectCasCondition.size(); i++) {
        cout << endl << setw(4 * (indentation + 1)) << "" << "case ";
        m_vectCasCondition[i]->traduitEnCPP(cout, 0);
        cout << "" << ":" << endl;
        m_vectCasInstruction[i]->traduitEnCPP(cout, indentation + 2);
        cout << setw(4 * (indentation + 2)) << "" << "break;";
    }
    if (m_instDefaut != nullptr) {
        cout << endl << setw(4 * (indentation + 1)) << "" << "default :" << endl;
        m_instDefaut->traduitEnCPP(cout, indentation + 2);
    }
    cout << setw(4 * indentation) << "" << "}";
}
