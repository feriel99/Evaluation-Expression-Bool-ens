
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	CONSTANTE,
	OPERATEUR,
	PARENTHESE
}	typeT;

typedef enum
{
	FAUX = 0,
	VRAI = 1,
	NON,
	ET,
	OU,
	IMPLICATION,
	EQUIVALENCE,
	OUVRANTE,
	FERMANTE,
}	valeurT;

struct token
{
	typeT	type;
	valeurT	val;
	struct token* suiv;
};
typedef struct token* liste_token;

struct arbre
{
	typeT	 	 type;
	valeurT	     val;
	struct arbre * 	gauche;
	struct arbre *	droite;
};
typedef struct arbre* arbre_token;

// Macro pour ajouter un élément à la pile
#define PUSH(P, E) ({E->suiv = P; P = E;})

// Macro pour enlever un élément de la pile et le rattacher en fin de liste
#define POP(P, L) ({L->suiv = P; L = L->suiv; P = P->suiv;})


liste_token	allouer_liste_token(typeT type, valeurT val)
{
	liste_token l = (liste_token)malloc(sizeof(struct token));
	if (! l)
	exit(1);
	l->type = type;
	l->val = val;
	l->suiv = NULL;
	return l;
}


void desallouer_liste_token(liste_token liste)
{
	liste_token m;
	while (liste)
	{
		m = liste;
		liste = liste->suiv;
		free(m);
	}
}

// Transformer une chaîne de charactères en liste de token
liste_token	string_to_token(const char *string)
{
	liste_token liste ;
	liste = NULL;
	liste_token m;
	typeT type;
	valeurT val;
	while (*string)
	{
		switch(*string){
			case ' ':   string++; continue; break;
			case '0': type = CONSTANTE; val = FAUX; break;
			case '1': type = CONSTANTE; val = VRAI; break;
			case '+': type = OPERATEUR; val = OU; break;
			case '.': type = OPERATEUR; val = ET; break;
			case '(': type = PARENTHESE; val = 	OUVRANTE; break;
			case  ')': type = PARENTHESE; val = FERMANTE; break;		
			default:
				type = OPERATEUR;
				if (string[0] == '=' && string[1] && string[1] == '>')
					val = IMPLICATION;
				else if (string[0] == '<' && string[1] && string[1] == '=' && string[2] && string[2] == '>')
				{
					val = EQUIVALENCE;
					string++;
				}
				else if (string[0] == 'N' && string[1] && string[1] == 'O' && string[2] && string[2] == 'N')
				{
					val = NON;
					string++;
				}
				else
				{
					desallouer_liste_token(liste); 
					liste = NULL;
					break;
				}
				string++; 		
		}
		if (liste)
		{
			m->suiv = allouer_liste_token(type, val);
			m = m->suiv;
		}
		else
		{
			liste = allouer_liste_token(type, val);
			m = liste; 
		}
		string++;
	}
	return liste;
}
// Vérifie si l'expression appartient au langage
int	expression_appartient_langage(liste_token liste)
{
	int etat = 0, pile = 0;
	
	while (liste)
	{
		if (etat)
		{
			if (liste->val != FERMANTE)
			{
				etat = 0;
				if (liste->type != OPERATEUR || liste->val == NON)
					break;
			}
			else
			{
				pile--;
				if (pile < 0)
					break;
			}
		}
		else
		{
			if (liste->val == OUVRANTE)
				pile++;
			else if (liste->type == CONSTANTE)
				etat = 1;
			else if (liste->val != NON)
				break;
		}
		liste = liste->suiv;
	}
	// vrai si état final et pile vide
	return ! pile && etat;
}
// Transforme une liste de tokens de infixe à postfixe
liste_token	liste_token_postfixe(liste_token liste)
{
	liste_token m, n, f, p;
	
	m = allouer_liste_token(PARENTHESE, OUVRANTE); 
	f = allouer_liste_token(PARENTHESE, OUVRANTE);
	n = m;
	while (liste)
	{
		p = liste->suiv;
		if (liste->type == OPERATEUR) 
		{
			while (f->type == OPERATEUR && liste->val > f->val)
			POP(f, n); // on retire les opérateurs moins prioritaires
			PUSH(f, liste); // on ajoute l'opérateur à la pile
		}
		else if (liste->val == FERMANTE) 
		{
			while (f->val != OUVRANTE)
			POP(f, n); // on récupère les opérateurs
			free(liste);
			liste = f->suiv;
			free(f);
			f = liste;
		}
		else if (liste->type == CONSTANTE) // ajout à l'expression
		{
			n->suiv = liste;
			n = n->suiv;
		}
		else // (l->valeur == GAUCHE)
			PUSH(f, liste); // on ajoute la parenthèse à la pile
		liste = p;
	}
	while (f->val != OUVRANTE)
	POP(f, n); // on vide la pile
	free(f);
	n->suiv = NULL;
	liste = m;
	m= liste->suiv;
	free(liste);
	return m;
}


arbre_token	allouer_arbre_token(liste_token liste)
{
	arbre_token a = (arbre_token)malloc(sizeof(struct arbre));
	if (! a)
	exit(1); 
	a->type = liste->type;
	a->val = liste->val;
	a->gauche = NULL;
	a->droite = NULL;
	return a ;
}


void desallouer_arbre_token(arbre_token a)
{
	if (! a)
		return;
	desallouer_arbre_token(a->gauche);
	desallouer_arbre_token(a->droite);
	free(a);
}

// Calcule la taille maximum de la pile d'arbres à partir de l'expression postfixe
int	liste_taille_max(liste_token liste)
{
	int taille = 0, max = 0;
	
	while (liste != NULL)
	{
		if (liste->type == CONSTANTE)
		{
			if (++taille > max)
				max = taille;
		}
		else if (liste->val != NON)
			taille--;
		liste = liste->suiv;
	}
	return max;
}

// Transforme une liste de tokens en arbre
arbre_token	liste_token_to_arbre_token(liste_token liste)
{
	arbre_token t[liste_taille_max(liste)];
	arbre_token m;
	int i = -1;
	while (liste)
	{
		m = allouer_arbre_token(liste);
		if (m->val == NON)
			m->gauche = t[i];
		else if (m->type == CONSTANTE) 
			i++;
		else // on fusionne 2 arbres avec l'opérateur
		{
			m->droite = t[i];
			m->gauche = t[--i];
		}
		t[i] = m;
		liste = liste->suiv;
	}
	
	return t[0];
}

// Résolution de a op b
int	calculer(valeurT a, valeurT b, valeurT op)
{
	switch(op) {
		case NON: return ! a;
		case OU: return a | b;
		case ET: return a & b;
		case IMPLICATION: return (! a) | b;
		default: return ! a ^ b; 
	}
}

// évaluer l'arbre
int	arbre_to_int(arbre_token arb)
{
	if (! arb)
		return 0;
	else if (arb->type == CONSTANTE)
		return arb->val;
	return calculer(arbre_to_int(arb->gauche), arbre_to_int(arb->droite), arb->val);
}

int	main(int argc, char **argv)
{
	if (argc != 2){
		printf("expression incorrecte\n");
		return 0;
	}				
	
	liste_token liste ; 
	liste  = string_to_token(argv[1]);
	if (! expression_appartient_langage(liste)) // on vérifie si l'expression est valide
	{
		desallouer_liste_token(liste);
		printf("expression incorrecte\n");
		return 0;
	}
	liste = liste_token_postfixe(liste); 
	arbre_token arb;
	arb = liste_token_to_arbre_token(liste); // transformer la liste des tokens en arbre
	desallouer_liste_token(liste);
	
	
	printf((arbre_to_int(arb)) ? "VRAI" : "FAUX"); // on calcule le reseltat de l'arbre 
	                                             // et on affiche vrai ou faux 
	printf("\n");
	desallouer_arbre_token(arb); // on libère l'arbre 
	return 0;
}

