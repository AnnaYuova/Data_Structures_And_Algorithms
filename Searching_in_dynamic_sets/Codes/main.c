#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "splay.h"
#include <time.h>
//#include "chain.h"
#include "open.h"
#define VELKOST 13

int main()
{
	clock_t start, end;
	STROM* binary_tree = NULL;
	splay_tree *t = new_splay_tree();
	
	//zaciatok casu a medzi start a end sa vykona nejaka fukncia 
	start = clock();
	printf("Zaciatok casu = %ld\n" , start);
	int a;
	for (a=1; a<1000000; a++)
	{
		binary_tree = insert(binary_tree, a);
	}

	//koniec casu
    end = clock();
    printf("Koniec casu = %ld\n" , end);
    printf("%.4lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	
	//splay_tree *t = new_splay_tree();
	//insertSplay(t, new_node(a));
	
	//tabulka_init();
	//insertChain(a);
	//serachChain(z);
	return 0;
}


