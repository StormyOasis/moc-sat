#ifndef METHODB_H__
#define METHODB_H__

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tiles.h"

class MethodB
{
	public:
    	MethodB();
        ~MethodB();

		bool ParseFormula(char *);
		bool RunMethodB();
		bool RunComputation(void * w, bool value = false, int a=0);
		int CalculateRequiredWidth(int method);
		int CalculateRequiredHeight(int method);
		Tile PickTile(Tile * top, Tile * left);
		void CreateTileLibrary();
        bool isTileOK(TileCorner tl, TileCorner br);

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