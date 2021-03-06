#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////

class Noeud {
    // Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
    // Remarque : la classe ne contient aucun constructeur
public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
    virtual void traduitEnCPP(ostream & cout, unsigned int indentation) const =0;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudSeqInst : public Noeud {
    // Classe pour représenter un noeud "sequence d'instruction"
    //  qui a autant de fils que d'instructions dans la séquence
public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction); // Ajoute une instruction à la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const;
private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////

class NoeudAffectation : public Noeud {
    // Classe pour représenter un noeud "affectation"
    //  composé de 2 fils : la variable et l'expression qu'on lui affecte
public:
    NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    NoeudAffectation(Noeud* variable, string type); // contruit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitEnCPP(ostream & cout, unsigned int indentation) const;
private:
    Noeud* m_variable;
    Noeud* m_expression;
    string m_type;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudOperateurBinaire : public Noeud {
    // Classe pour représenter un noeud "opération binaire" composé d'un opérateur
    //  et de 2 fils : l'opérande gauche et l'opérande droit
public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit

    ~NoeudOperateurBinaire() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute (évalue) l'opération binaire)
    void traduitEnCPP(ostream & cout, unsigned int indentation) const;
private:
    Symbole m_operateur;
    Noeud* m_operandeGauche;
    Noeud* m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstSi : public Noeud {
    // Classe pour représenter un noeud "instruction si"
    //  et ses 2 fils : la condition du si et la séquence d'instruction associée
public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
    // Construit une "instruction si" avec sa condition et sa séquence d'instruction

    ~NoeudInstSi() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation)const;
private:
    Noeud* m_condition;
    Noeud* m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSiRiche : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSiRiche (vector<Noeud*> vectSi, Noeud* sequenceSinon);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSiRiche() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation) const;
  private:
    vector<Noeud*> m_vectSi;  
    Noeud*  m_sequenceSinon;
};


////////////////////////////////////////////////////////////////////////////////

class NoeudInstTantQue : public Noeud {
    // Classe pour représenter un noeud "instruction tantque"
    //  et ses 2 fils : la condition du tantque et la séquence d'instruction associée
public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
    // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction

    ~NoeudInstTantQue() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation)const;
private:
    Noeud* m_condition;
    Noeud* m_sequence;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstRepeter : public Noeud {
    // Classe pour représenter un noeud "instruction repeter"
public:
    NoeudInstRepeter(Noeud* instruction, Noeud* expression);

    ~NoeudInstRepeter() {
    }
    int executer();
    void traduitEnCPP(ostream & cout,unsigned int indentation)const;
private:
    Noeud* m_instruction;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstPour : public Noeud {
    // Classe pour représenter un noeud "instruction pour"
public:
    NoeudInstPour(Noeud* init, Noeud* cond, Noeud* incr, Noeud* sequence);

    ~NoeudInstPour() {
    }
    int executer();
    void traduitEnCPP(ostream & cout,unsigned int indentation)const;
private:
    Noeud* m_initialisation;
    Noeud* m_condition;
    Noeud* m_incrementation;
    Noeud* m_seqInstruction;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudEcrire : public Noeud {
    // Classe pour représenter un noeud "instruction ecrire"
public:
    NoeudEcrire();

    ~NoeudEcrire() {
    }
    int executer();
    void ajoute(Noeud* parametre) override{
        m_chainesEtExpressions.push_back(parametre);
    }
    void traduitEnCPP(ostream & cout,unsigned int indentation)const;
private:
    vector<Noeud *> m_chainesEtExpressions;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudLire : public Noeud {
    // Classe pour représenter un noeud "instruction lire"
public:
    NoeudLire();

    ~NoeudLire() {
    }
    int executer();
    void ajoute(Noeud* parametre) override{
        m_variables.push_back(parametre);
    }
    void traduitEnCPP(ostream & cout,unsigned int indentation)const;
private:
    vector<Noeud *> m_variables;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstSwitch : public Noeud {
    // Classe pour représenter un noeud "instruction selon"
public:
    NoeudInstSwitch(Noeud* variable, Noeud* instructionsParDefaut, vector<Noeud*> m_vectCasCondition, vector<Noeud*> m_vectCasInstruction);
    ~NoeudInstSwitch() {}
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation) const;
  private:
    vector<Noeud*> m_vectCasCondition;
    vector<Noeud*> m_vectCasInstruction;
    Noeud*  m_variable;
    Noeud*  m_instDefaut;//optionnel
};
#endif /* ARBREABSTRAIT_H */
