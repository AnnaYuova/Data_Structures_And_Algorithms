#include <stdio.h>
#include <stdlib.h>

typedef struct strom 
{
  int kluc;
  int vyska;
  struct strom* lavy;
  struct strom* pravy;
} STROM;

STROM* init_tree(int kluc)
{
    STROM* tree = (STROM*)(malloc(sizeof(STROM)));
    tree->kluc = kluc;
    tree->vyska = 1;
    tree->lavy = NULL;
    tree->pravy = NULL;
    return tree;
}

int vyska(STROM* akt) 
{ 
    if (akt == NULL) 
        return 0; 
    return akt->vyska; 
}  

//ak mam vacsi lavypodstrom musim tocit dolava
//funkcia vrati novy koren
STROM *otocPravy(STROM* koren) 
{ 
	//ulozim si kam ukazuje koren vlavo a odtial vpravo
    STROM *novyKoren = koren->lavy; 
    STROM *presun = novyKoren->pravy; 
  
    //novy koren (lave dieta povodneho korena)
    novyKoren->pravy = koren; 
    //do povodneho korena dam podstrom, ktory bol predtym nalavo cize je mensi, preto ja teraz na lavej strane
    koren->lavy = presun; 
  
  	//pomocne vypocty na zistenie novej vysky povodneho korena a noveho korena
  	int vysledok=0;
    int pom = (vyska(koren->lavy));
    int pom2 = (vyska(koren->pravy))+1;
    if (pom > pom2)
    {
    	vysledok = pom;
	}
	else
	{
		vysledok = pom2;
	}
    koren->vyska = vysledok; 
    
    int vysledok1=0;
    int vyskaLavy = (vyska(novyKoren->lavy));
    int vyskaPravy = (vyska(novyKoren->pravy))+1;
    if (vyskaLavy > vyskaPravy)
    {
    	vysledok1 = vyskaLavy;
	}
	else
	{
		vysledok1 = vyskaPravy;
	}
    novyKoren->vyska = vysledok1; 
  
    return novyKoren; 
} 
  
 
STROM *otocLavy(STROM *koren) 
{ 
   	STROM *novyKoren = koren->pravy; 
    STROM *presun = novyKoren->lavy; 
  
     
    novyKoren->lavy = koren; 
    koren->pravy = presun; 
  
    int vysledok=0;
    int pom = (vyska(koren->lavy));
    int pom2 = (vyska(koren->pravy))+1;
    if (pom > pom2)
    {
    	vysledok = pom;
	}
	else
	{
		vysledok = pom2;
	}
    koren->vyska = vysledok; 
    
    int vysledok1=0;
    int vyskaLavy = (vyska(novyKoren->lavy));
    int vyskaPravy = (vyska(novyKoren->pravy))+1;
    if (vyskaLavy > vyskaPravy)
    {
    	vysledok1 = vyskaLavy;
	}
	else
	{
		vysledok1 = vyskaPravy;
	}
    
    novyKoren->vyska = vysledok; 
    return novyKoren; 
} 

STROM* insert(STROM* tree, int kluc)
{
    STROM* current = tree;
 
 	//vlozim do stromu klasicky ako binarny strom
    if (current == NULL)
    {
        return (init_tree(kluc));
    }
 	
 	if (kluc == current->kluc)
	{
		return current;
	}
	
	//ak je dane cislo mensie vkladam na lavu stranu
    if (kluc < current->kluc)
    {
        current->lavy  = insert(current->lavy, kluc); 
    } 
    
    //ak je dane cislo vacsie vkladam na pravu stranu
	if (kluc > current->kluc) 
	{
		current->pravy = insert(current->pravy, kluc); 
	}
	
	
	//pomocne vypocty na zistenie praveho a laveho podstromu (vysky), vyska daneho cisla a vypocet faktoru vyvazenia
	int pomocna = 1;
	int pom1 = vyska(current->lavy);
	int pom2 = vyska(current->pravy);
	if (pom1 > pom2 )
	{
		pomocna += pom1;
	}
	else
	{
		pomocna += pom2;
	}
    current->vyska = pomocna;
    
    //vypocet balance faktor
    int faktor = pom1 - pom2;
    
    //ak fakt >1 alebo <-1, tak strom treba vyvazit
    //mam vacsi lavy podstrom, preto musim tocit doprava
    if (faktor > 1 )
	{
		//vlavo vlavo
		if (kluc < current->lavy->kluc)
		{
			return otocPravy(current); 
		}
		//vlavo vpravo
		if (kluc > current->lavy->kluc)
		{
			current->lavy =  otocLavy(current->lavy); 
        	return otocPravy(current);
		}
 	}
 	
 	//mam vacsi pravy podstrom, preto musim tocit dolava
 	if (faktor < -1)
 	{
 		//vpravo vpravo
 		if (kluc > current->pravy->kluc)
 		{
 			return otocLavy(current); 
		}
		//vpravo vlavo
		if (kluc < current->pravy->kluc)
		{
			current->pravy = otocPravy(current->pravy); 
        	return otocLavy(current); 
		}
	}
  
    return current;
}

STROM* search(STROM* tree, int hodnota) 
{
	STROM* aktualny = tree;
	
	if (aktualny->kluc == hodnota)
	{
		return aktualny->kluc;
	}
	
	if (hodnota < aktualny->kluc && aktualny->kluc != NULL)
	{
		return search(aktualny->lavy,hodnota);
	}
	
	if (hodnota > aktualny->kluc && aktualny->kluc != NULL)
	{
		return search(aktualny->pravy, hodnota);
	}
	return NULL;
}

//vypise strom inorder
void vypis(STROM *miesto) 
{
  if(miesto != NULL) 
  {
    vypis(miesto->lavy);
    printf("%d\n", miesto->kluc);
    vypis(miesto->pravy);
  }
}

int main() {
	STROM* binary_tree = NULL;
	/*binary_tree = insert(binary_tree, 10); 
    binary_tree = insert(binary_tree, 20); 
    binary_tree = insert(binary_tree, 30); 
    binary_tree = insert(binary_tree, 40); 
   	binary_tree = insert(binary_tree, 50); 
    binary_tree = insert(binary_tree, 25); 
	binary_tree = insert(binary_tree, 70);
	binary_tree = insert(binary_tree, 60); */
	
	int a;
	for (a=1; a<1000; a++)
	{
		binary_tree = insert(binary_tree, a);
	}
    
  	vypis(binary_tree); 
  	
  	STROM* hladany = search(binary_tree, 500);
  	if (hladany == NULL)
  	{
  		printf("\nTakyto prvok sa v strome nenachadza\n");
	}
	else
	{
		printf("hladany: %d\n", hladany);
	}
  	//printf("\nsearch: %d\n" ,hladany);
	return 0;
}