#ifndef __TILES_H
#define __TILES_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define METHOD_A 1
#define METHOD_B 2


enum CornerType { Empty = 0, Input_Begin, Input_S,
	Assignment_Variable=10, Input_Variable, Input_Literal,
    Assignment_Var1= 20, ComputationBoundary_Var1, Computation_Var1, Computation_Literal1, FinalCheck_Literal1, Computation_S1, FinalCheck_S1,
    Assignment_Var0 = 30, ComputationBoundary_Var0,Computation_Var0, Computation_Literal0, FinalCheck_Literal0, Computation_S0, FinalCheck_S0,
    Computation_Clause=40, Input_Clause,
    Computation_OK=50, FinalCheck_OK,
    Input_C = 60, FinalCheck_C,
    FinalCheck_V=70, Assingment_V, Input_V,
    FinalCheck_E=80, Input_E, ComputationBoundary_E,
    FinalCheck_T=90, FinalCheck_Sucess,
    FinalCheck_F=100
    };

enum TileType { InputBoundary, VariableTile, ClauseTile, Assignment, ComputationBoundary, Computation, FinalCheck, Error};

class Clause
{
	public:

    	int number;
    	bool operator==(Clause c);
		char variables[256];
    	char nots[256];

};

class TileCorner
{
	public:
    	TileCorner(){offset_for_b = data_offset = -1;}
        TileCorner(CornerType t, int d, int d2 = -1);

		CornerType type;
        int data_offset;
        int offset_for_b;

        bool operator== (TileCorner t);
};

class Tile
{
	public:
    	Tile(){}
        Tile(TileType t) { tileType = t; }
    	Tile(TileType, TileCorner, TileCorner,TileCorner,TileCorner);

		TileType tileType;
    	TileCorner topLeft;
	    TileCorner topRight;
    	TileCorner bottomLeft;
	    TileCorner bottomRight;

        bool inUse;
};

int doCornerTypesMatch(CornerType type1, CornerType type2);

#endif