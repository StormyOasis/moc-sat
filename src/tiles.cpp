#include "tiles.h"

Tile::Tile(TileType type, TileCorner tl, TileCorner tr,TileCorner bl,TileCorner br)
{
	tileType = type;
	topLeft = tl;
	topRight = tr;
    bottomLeft = bl;
	bottomRight = br;
    inUse = true;
}

TileCorner::TileCorner(CornerType t, int d, int d2)
{
	type = t;
    data_offset = d;
    offset_for_b = d2;
}

bool TileCorner::operator== (TileCorner t)
{
	int ct = doCornerTypesMatch(this->type, t.type);
    if(ct == -1)
    	return false;

    if(data_offset != t.data_offset)
    	return false;

    if(offset_for_b != t.offset_for_b)
    	return false;

    /*if(data_offset == -1 && t.data_offset != -1)
    {
		if(ct == 4) //clauses
        {
        	Clause * c1 = &methodA->clauses[this->data_offset];
        	Clause * c2 = &methodA->clauses[t.data_offset];
        	if(*c1 == *c2)
            	return false;
        }
		else if(ct == 1 || ct == 2 || ct == 3) //variables
        {
        	char * c1 = (char*)methodA->variable_list[this->data_offset];
        	char * c2 = (char*)methodA->variable_list[t.data_offset];
        	if(c1[0] != c2[0])
            	return false;
        }
    }
    else if((data_offset != -1 && t.data_offset == -1) || (data_offset == -1 && t.data_offset != -1))
    	return false;   */

    return true;
}

bool Clause::operator==(Clause c)
{
	if(number != c.number)
    	return false;

	for(int i = 0 ; i < 256; i++)
	{
        if(variables[i] != c.variables[i])
           	return false;

    	if(nots[i] != c.nots[i])
          	return false;
    }

	return true;
}

int doCornerTypesMatch(CornerType type1, CornerType type2)
{
	int mode1 = type1 / 10;
	int mode2 = type2 / 10;

	if(mode1 == mode2)
    	return mode1;

    return -1;
}