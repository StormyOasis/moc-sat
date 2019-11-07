#ifndef METHODA_H__
#define METHODA_H__

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tiles.h"

class MethodA
{
	public:
    	MethodA();
        ~MethodA();

		bool ParseFormula(char *);
		bool RunMethodA();
		bool RunComputation(void * w, bool value = false, int a=0);
		int CalculateRequiredWidth(int method);
		int CalculateRequiredHeight(int method);
		Tile PickTile(Tile * top, Tile * left);
		void CreateTileLibrary();
        char * ConstructResultString(char *);

    public:
		Clause *clauses;
		int num_clauses;
		int num_variables;
		char *variable_list;
		int last_width;
		int last_height;
		int library_count;
		bool * current_input;
		unsigned int assignments;
		int current;
		Tile ** tiles;
		Tile * tileLibrary;
};

#endif