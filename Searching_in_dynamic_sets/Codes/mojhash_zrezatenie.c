#include<stdio.h>
#include<stdlib.h>

#define VELKOST 13
#define NASOBENIE 3
int globalnaVelkost = VELKOST;

typedef struct node
{
    int hodnota;
    struct node *dalsi;
}NODE;
struct node *tabulka[VELKOST];


//na zaciatku dam do celej tabulky NULL
void tabulka_init()
{
    int i;
    for(i = 0; i < globalnaVelkost; i++)
        tabulka[i] = NULL;
}


//vytvorim si zakladnu vetvu, do ktorej vlozi hodnotu a nastavi jej dalsiu na null
NODE* create_node(int hodnota)
{
	NODE *newNode = (NODE*)malloc(sizeof(NODE));
	newNode->hodnota = hodnota;
	newNode->dalsi = NULL;
}


/*
vytvorim pomocnu tabulku, ulozim si tam nove hodnoty
pouzivam uz novu (pomocnu) tabulku a tu povodnu zmazem
povodna = pomocna tabulka
funkcia rozsir tabulku
velkost na globalnu premennu 
zadefinovat tabulku malloc
*/
int insertChain(int hodnota)
{
    NODE *newNode = create_node(hodnota);
	
	//vypocitam hash
    int index = hodnota % globalnaVelkost;
	struct node *aktualny = tabulka[index];
		
    //pozriem sa ci je tabulka s danym indexom prazdna a ak ano, vlozim hodnotu
    if(tabulka[index] == NULL)
    {
    	tabulka[index] = newNode;
    	return 0;
	}
	else
	{
		//ak tabulka s danym indexom prazdna, musim prechadzat v zozname kym nenajdem null a tam to vlozim
		while(aktualny->dalsi != NULL)
	    {
	        aktualny = aktualny->dalsi;
		}
	    aktualny->dalsi = newNode;
	}

    return 0;
}

	
int searchChain(int value)
{
    int index = value % globalnaVelkost;
    struct node *aktualny = tabulka[index];
    if (aktualny == NULL)
    {
    	return NULL;
	}
	
    while(aktualny != NULL)
    {
        if(aktualny->hodnota == value)
        {
        	return aktualny->hodnota;
		}
        aktualny = aktualny->dalsi;
    }
    return NULL;
}

int main()
{
    tabulka_init();

    insertChain(30);
    insertChain(10);
    insertChain(20);
    insertChain(5);
    insertChain(70);
    insertChain(12);
    insertChain(140);

    int i;
    for(i = 0; i < globalnaVelkost; i++)
    {
        struct node *akt = tabulka[i];
        printf("tabulka[%d] -> ",i);
        while(akt != NULL)
        {
            printf("%d -> ",akt->hodnota);
            akt = akt->dalsi;
        }
        printf("NULL\n");
    }
	
	int hladany = 10;
    printf("Hladany: %d\n", hladany);

    if(searchChain(hladany))
    {
    	printf("Nasiel\n");
	}
    else
    {
    	printf("Nenasiel\n");
	}
        
    return 0;
}
