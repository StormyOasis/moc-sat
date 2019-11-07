#include <math.h>
#include "resltswn.h"
#include "layoutwn.h"

#include "methodb.h"
#include "win.h"

TWin * win;

MethodB::MethodB()
{
	clauses = NULL;
	num_clauses = 0;
	num_variables = 0;
	variable_list = NULL;
	last_width = 0;
	last_height = 0;
	library_count = 0;
	current_input = NULL;
	assignments = 0;
	current = 0;
	tiles = NULL;
	tileLibrary = NULL;
}

MethodB::~MethodB()
{
	if(clauses)
    	free(clauses);
    clauses = NULL;

    if(variable_list)
    	free(variable_list);
    variable_list = NULL;

    if(current_input)
    	free(current_input);
    current_input = NULL;

    if(tileLibrary)
    	free(tileLibrary);
    tileLibrary = NULL;

    if(tiles)
    {
	    for(int i = 0; i < last_height; i++)
        	free(tiles[i]);
        free(tiles);
        tiles = NULL;
    }

   	/*for(int i = 0; i < numValidInputs; i++)
       	free(valid_inputs[i]);
   	for(int i = 0; i < numBadInputs; i++)
       	free(bad_inputs[i]);      */
}

bool MethodB::ParseFormula(char * string)
{
	int len = strlen(string);
	int a = 0;
    int pos = 0;
    bool mod = false;
    bool found;

    if(len == 0)
    	return false;

    while(pos < len)
    {
        if(string[pos] == '!' || string[pos] == '~')
        	mod = true;
		else if(isalpha(string[pos]))
        {
        	clauses[num_clauses-1].variables[a] = string[pos];
         	clauses[num_clauses-1].nots[a] = mod;
         	clauses[num_clauses-1].number = num_clauses-1;
            mod = false;
            a++;

            if(variable_list == NULL)
            {
            	variable_list = (char*)malloc(1);
            	num_variables = 1;
                variable_list[0] = string[pos];
            }
            else
            {
            	found = false;
	            for(int i = 0; i < num_variables; i++)
                {
                	if(variable_list[i] == string[pos])
                    {
                    	found = true;
                        break;
                    }
                }

                if(!found)
                {
                    variable_list[num_variables] = string[pos];
                    num_variables++;
                	variable_list = (char*)realloc(variable_list, 3*num_variables*sizeof(char));
                }
        	}
        }
        else if(string[pos] == '(')
        {
        	clauses = (Clause*)realloc(clauses, 2*sizeof(Clause)*(num_clauses+1));
            memset(clauses[num_clauses].nots, 0, 256);
            memset(clauses[num_clauses].variables, 0, 256);
        	num_clauses++;
            a = 0;
        }

    	pos++;
    }

	return true;
}

bool MethodB::RunComputation(void * w, bool value, int a)
{
	bool res;

    win = reinterpret_cast<TWin*>(w);

    TResultsWin *res_win = TYPESAFE_DOWNCAST(win->Parent, TLayoutWin)->res_win;
    TRect rect;
    rect.top = 5;
    rect.left = 0;
    rect.right = res_win->GetClientRect().Width();
    rect.bottom = 150;

    if(!value)
	    CreateTileLibrary();

    bool sat = false;

    double n = pow(2, num_variables);

    for(int i = 0; (i < n); i++)
    {
        if(tiles != NULL)
		{
    		for(int j = 0; j < last_height; j++)
			 	free(tiles[j]);
			 free(tiles);

            if(current_input)
	            free(current_input);
	    }

        win->percent = 100*(i / n);

        current = num_variables-1;

        //if(i > (num_variables))
	    //    assignments = ~(i - num_variables);
       // else
        if(value)
	        assignments = a;
        else
		    assignments = i;

        current_input = (bool *)malloc(num_variables * sizeof(bool));

        for(int j = num_variables-1, k = 0; j >= 0; j--, k++)
          	current_input[j] = assignments & (1<<k);

     	res = RunMethodB();

        if(value)
        	break;

        if(res)
        {
        	sat = true;
    	    /*valid_inputs[numValidInputs] = (char*)malloc(num_variables * sizeof(char)); //inputs that satisfy
	        for(int j = num_variables-1, k = 0; j >= 0; j--, k++)
            	valid_inputs[numValidInputs][j] = (current_input[j] == true) ? '1' : '0';
	        numValidInputs++;*/

            char * tmp = (char *)malloc(num_variables * sizeof(char));
	        for(int j = num_variables-1, k = 0; j >= 0; j--, k++)
            	tmp[j] = (current_input[j] == true) ? '1' : '0';

            win->AddStringToResultList(ConstructResultString(tmp), 0);

            free(tmp);
        }
        else
        {
    	    /*bad_inputs[numBadInputs] = (char*)malloc(num_variables * sizeof(char)); //inputs that satisfy
	        for(int j = num_variables-1, k = 0; j >= 0; j--, k++)
            	bad_inputs[numBadInputs][j] = (current_input[j] == true) ? '1' : '0';
	        numBadInputs++;  */

            char * tmp = (char *)malloc(num_variables * sizeof(char));
	        for(int j = num_variables-1, k = 0; j >= 0; j--, k++)
            	tmp[j] = (current_input[j] == true) ? '1' : '0';

            win->AddStringToResultList(ConstructResultString(tmp), 1);

            free(tmp);
        }
        //win->Invalidate();
        win->InvalidateRect(TRect(0, 0, win->GetClientRect().Width(), 100), false);
        win->InvalidateRect(TRect(50,175 ,300, 300), false);
//        win->InvalidateRect(TRect(50, 175, 250, 50), false);
        res_win->InvalidateRect(rect, false);
        win->GetApplication()->PumpWaitingMessages();
    }

    win->drawTiles(last_width, last_height);

    return sat;
}

void MethodB::CreateTileLibrary()
{
    int m = num_variables;
    int loc = 0;

	if(tileLibrary)
    	free(tileLibrary);

    /* Assignment tiles 0...m (Same as in Method A)*/
    for(int i = 0; i < m; i++, loc++)
    {
    	TileCorner tr = TileCorner(Assingment_V, -1);
    	TileCorner br = TileCorner(Assignment_Var1, i);
    	TileCorner bl = TileCorner(Assingment_V, -1);
    	TileCorner tl = TileCorner(Assignment_Variable, i);
        tileLibrary = (Tile*)realloc(tileLibrary, (loc+2) * sizeof(Tile));
    	tileLibrary[loc] = Tile(Assignment, tl, tr, bl, br);
        loc++;

    	tr = TileCorner(Assingment_V, -1);
    	br = TileCorner(Assignment_Var0, i);
    	bl = TileCorner(Assingment_V, -1);
    	tl = TileCorner(Assignment_Variable, i);
    	tileLibrary[loc] = Tile(Assignment, tl, tr, bl, br);
    }

    /* Computation Boundary Tiles, (Also Same as Method A) */
    for(int i = 0; i < m; i++, loc++)
    {
    	TileCorner tr = TileCorner(ComputationBoundary_E, -1);
    	TileCorner br = TileCorner(Empty, -1);
    	TileCorner bl = TileCorner(ComputationBoundary_E, -1);
    	TileCorner tl = TileCorner(ComputationBoundary_Var0, i);
        tileLibrary = (Tile*)realloc(tileLibrary, (loc+2) * sizeof(Tile));
    	tileLibrary[loc] = Tile(ComputationBoundary, tl, tr, bl, br);
        loc++;

    	tr = TileCorner(ComputationBoundary_E, -1);
    	br = TileCorner(Empty, -1);
    	bl = TileCorner(ComputationBoundary_E, -1);
    	tl = TileCorner(ComputationBoundary_Var1, i);
    	tileLibrary[loc] = Tile(ComputationBoundary, tl, tr, bl, br);
    }

    /* Computation tiles */

    for(int i = 0; i < m; i++, loc++)
    {
    	for(int j = 0; j < m; j++) //number of literals
        {
			TileCorner tr = TileCorner(Computation_Literal0, j);
            if(i == j)
            	tr = TileCorner(Computation_OK, -1);

			TileCorner tl = TileCorner(Computation_Var1, i);//var
			TileCorner bl = TileCorner(Computation_Literal0, j);
			TileCorner br = TileCorner(Computation_Var1, i); //var
			tileLibrary = (Tile*)realloc(tileLibrary, (loc+2) * sizeof(Tile));
   			tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
            loc++;

			tr = TileCorner(Computation_Literal1, j);
            bl = TileCorner(Computation_Literal1, j);
            if(i == j)
            	tr = TileCorner(Computation_OK, -1);
   			tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
            loc++;
        }
        TileCorner tr = TileCorner(Computation_OK, -1);
		TileCorner tl = TileCorner(Computation_Var0, i);//var
		TileCorner bl = TileCorner(Computation_OK, -1);
		TileCorner br = TileCorner(Computation_Var0, i); //var
		tileLibrary = (Tile*)realloc(tileLibrary, (loc+4) * sizeof(Tile));
		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
        loc++;
        tr = TileCorner(Computation_OK, -1);
		tl = TileCorner(Computation_Var1, i);//var
		bl = TileCorner(Computation_OK, -1);
		br = TileCorner(Computation_Var1, i); //var
		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
        loc++;
        tr = TileCorner(Computation_S1, -1);
		tl = TileCorner(Computation_Var1, i);//var
		bl = TileCorner(Computation_S1, -1);
		br = TileCorner(Computation_Var1, i); //var
		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
        loc++;
        tr = TileCorner(Computation_S0, -1);
		tl = TileCorner(Computation_Var0, i);//var
		bl = TileCorner(Computation_S0, -1);
		br = TileCorner(Computation_Var0, i); //var
		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    }

    /* Final tiles.  Different from Method A */
    for(int i = 0; i < num_clauses; i++)
    {
    	for(int j = 0; j < 256; j++, loc++)
        {
			TileCorner tr = TileCorner(Empty, -1);
			TileCorner tl = TileCorner(FinalCheck_T, -1);
			TileCorner bl = TileCorner(FinalCheck_Literal1, i, j);
			TileCorner br = TileCorner(FinalCheck_T, -1);
        	tileLibrary = (Tile*)realloc(tileLibrary, (loc+4) * sizeof(Tile));
    		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
	        loc++;

			tr = TileCorner(Empty, -1);
			tl = TileCorner(FinalCheck_T, -1);
			bl = TileCorner(FinalCheck_Literal0, i, j);
			br = TileCorner(FinalCheck_T, -1);
    		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
	        loc++ ;

			tr = TileCorner(Empty, -1);
			tl = TileCorner(FinalCheck_F, -1);
	   		bl = TileCorner(FinalCheck_Literal1, i, j);
			br = TileCorner(FinalCheck_F, -1);
    		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
	        loc++;

			tr = TileCorner(Empty, -1);
			tl = TileCorner(FinalCheck_F, -1);
			bl = TileCorner(FinalCheck_Literal0, i, j);
			br = TileCorner(FinalCheck_F, -1);
    		tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
        }
    }
    /* DO the 5 remaining final tiles */
    tileLibrary = (Tile*)realloc(tileLibrary, (loc+6) * sizeof(Tile));

	TileCorner tr = TileCorner(Empty, -1);
	TileCorner tl = TileCorner(FinalCheck_C, -1);
	TileCorner bl = TileCorner(FinalCheck_V, -1);
	TileCorner br = TileCorner(FinalCheck_F, -1);
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;

	tr = TileCorner(Empty, -1);
	tl = TileCorner(FinalCheck_F, -1);
	bl = TileCorner(FinalCheck_OK, -1);
	br = TileCorner(FinalCheck_T, -1);
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;

	tr = TileCorner(Empty, -1);
	tl = TileCorner(FinalCheck_T, -1);
	bl = TileCorner(FinalCheck_OK, -1);
	br = TileCorner(FinalCheck_T, -1);
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;
	tr = TileCorner(Empty, -1);
	tl = TileCorner(FinalCheck_T, -1);
	bl = TileCorner(FinalCheck_S0, -1);
	br = TileCorner(FinalCheck_F, -1);
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;
	tr = TileCorner(Empty, -1);
	tl = TileCorner(FinalCheck_T, -1);
	bl = TileCorner(FinalCheck_S1, -1);
	br = TileCorner(FinalCheck_F, -1);
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;
	tr = TileCorner(FinalCheck_Sucess, -1);
	tl = TileCorner(FinalCheck_F, -1);
	bl = TileCorner(FinalCheck_E, -1);
	br = TileCorner(FinalCheck_Sucess, -1);
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    library_count = loc+1;
}

bool MethodB::RunMethodB()
{
    int width = CalculateRequiredWidth(METHOD_B);
  	int height = CalculateRequiredHeight(METHOD_B);

    last_width = width;
    last_height = height;

    tiles = (Tile**)malloc(height * sizeof(Tile *));
    for(int j = 0; j < height; j++)
    	tiles[j] = (Tile*)malloc(width * sizeof(Tile));


    for(int j = 0; j < height; j++)
    	for(int i = 0; i < width; i++)
	    	tiles[j][i].inUse = false;

    win->tile_count += (width + height);

     /* Begin tile */
  	TileCorner tr = TileCorner(Input_Begin, -1);
  	TileCorner tl = TileCorner(Input_Begin, -1);
  	TileCorner br = TileCorner(Input_Begin, -1);
  	TileCorner bl = TileCorner(Input_Begin, -1);
	tiles[0][0] = Tile(InputBoundary, tl,tr, bl, br);

    for(int i = 0; i < num_variables; i++)
    {
	  	TileCorner t = TileCorner(Input_Variable, i);
		tiles[0][i+1] = Tile(InputBoundary, t,t,t,t);
    }
  	tl = TileCorner(Input_C, -1);
	tiles[0][num_variables+1] = Tile(InputBoundary, tl,tl, tl, tl);
  	bl = TileCorner(Input_V, -1);
	tiles[1][0] = Tile(InputBoundary, bl,bl, bl, bl);

    /* Do the literals now */
    int loc = 2;
    for(int i = 0; i < num_clauses; i++)
    {
    	for(int j = 0; j < 256; j++)
        {
			if(clauses[i].variables[j] != 0)
            {
            	char not = clauses[i].nots[j];

                if(not)
			  		bl = TileCorner(Computation_Literal0, i, j);
                else
					bl = TileCorner(Computation_Literal1, i, j);

				tiles[loc][0] = Tile(InputBoundary, bl,bl, bl, bl);
            	loc++;
            }
        }

        /* put separator here */
  		bl = TileCorner(Input_S, -1);
		tiles[loc][0] = Tile(InputBoundary, bl,bl, bl, bl);
        loc++;
    }

  	bl = TileCorner(Input_E, -1);
	tiles[loc][0] = Tile(InputBoundary, bl,bl, bl, bl);

    int y, x;
    bool found = true, error = false;

   	while(found && !error)
    {
    	found = false;

	    for(int i = 1; i < height; i++)
    	{
    		y = i;
	    	for(int j = 1; j < width; j++)
            {
	    		x = j;
    			if(tiles[y][x].inUse == false)
	        	{
                	Tile tmp = PickTile(&tiles[y-1][x], &tiles[y][x-1]);

                    if(tmp.tileType == Error)
                    {
                    	found = false;
                        error = true;
                        break;
                    }

                    win->tile_count++;

                    tiles[y][x] = tmp;
                    tiles[y][x].inUse = true;
        	        found = true;

                    //win->drawTiles(width, height);

                    if(tmp.tileType == FinalCheck && tmp.topRight.type == FinalCheck_Sucess)
                     	return true;

                    break;
	    	    }
            }

            if(error || !found)
            	break;
        }
    }

    return false;
}

int MethodB::CalculateRequiredWidth(int)
{
	int width;
	width = num_variables + 2;// + num_clauses + 4;
    return width;
}

int MethodB::CalculateRequiredHeight(int)
{
	int height, num_literals = 0;

    for(int i = 0; i < num_clauses; i++)
    {
    	for(int j = 0; j < 256; j++)
        {
        	if(clauses == NULL)
            	return 0;
                
			if(clauses[i].variables[j] != 0)
            	num_literals++;
        }
    }

    height = num_literals + 3 + num_clauses;
    return height;
}

bool MethodB::isTileOK(TileCorner tl, TileCorner bl)
{
	/* Bl is a literal, tl is a variable */

    char var_tl = variable_list[tl.data_offset];
    bool not = false;
    int type = (int)(tl.type / 10);
    if(type == 3)
    	not = true;

    Clause * cb = (Clause*)&clauses[bl.data_offset];
    char var_bl = cb->variables[bl.offset_for_b];
    char not_bl = cb->nots[bl.offset_for_b];

    if(var_tl != var_bl)
    	return false;

    if(not_bl != not)
    	return false;

    return true;
}

Tile MethodB::PickTile(Tile * top, Tile * left)
{
	Tile newTile;
    int type = (int)(left->topRight.type / 10);

   	if((type == 2 || type == 3) && top->tileType != InputBoundary && top->tileType != FinalCheck)
    {
    	if(top->bottomRight.data_offset == -1) //error..can't continue.  Not SATisfiable
        	return Tile(Error);

	    TileCorner var_corner = top->bottomRight;   //variable
  	    TileCorner literal_corner = left->topRight;   //literal
        TileCorner tl,br,bl,tr;
		br = tl = var_corner;

        if(literal_corner.type == Computation_S1 || literal_corner.type == Computation_S0) //not sure about this...
        	tr = bl = literal_corner;
		else
        {
            tr = bl = literal_corner;
	        if(isTileOK(tl,bl))
    	    	tr = TileCorner(Computation_OK, -1,-1);
        }

        newTile = Tile(Computation, tl,tr,bl,br);

        return newTile;
    }
    else if(left->topRight.type == Input_S && top->tileType != FinalCheck)
    {
    	if(top->bottomRight.data_offset == -1) //error..can't continue.  Not SATisfiable
        	return Tile(Error);

	    TileCorner var_corner = top->bottomRight;   //variable
  	    TileCorner literal_corner = left->topRight;   //literal
        TileCorner tl,br,bl,tr;
		br = tl = var_corner;

        if((int)(var_corner.type / 10) == 2)
			tr = bl = TileCorner(Computation_S1, -1, -1);
        else
			tr = bl = TileCorner(Computation_S0, -1, -1);

        newTile = Tile(Computation, tl,tr,bl,br);

    	return newTile;
    }
    else if(left->topRight.type == Computation_OK && top->tileType != FinalCheck)
    {
    	if(top->bottomRight.data_offset == -1) //error..can't continue.  Not SATisfiable
        	return Tile(Error);
	    TileCorner var_corner = top->bottomRight;   //variable
  	    TileCorner literal_corner = left->topRight;   //literal
        TileCorner tl,br,bl,tr;
		br = tl = var_corner;
		tr = bl = literal_corner;
        newTile = Tile(Computation, tl,tr,bl,br);
        return newTile;
    }

    if(left->topRight.type == Computation_S1 || left->topRight.type == Computation_S0)
    {
    	if(top->bottomRight.type == FinalCheck_F)
        	return Tile(Error);
    }

    for(int i = 0; i < library_count; i++) //compare each against the tile library
    {
        Tile tile = tileLibrary[i];
    	int mode1 = doCornerTypesMatch(top->bottomRight.type, tile.topLeft.type);
        if(mode1 == -1)
        	continue;

        int mode2 = doCornerTypesMatch(left->topRight.type, tile.bottomLeft.type);
        if(mode2 == -1)
        	continue;

		  	if(!(top->bottomRight == tile.topLeft) || !(left->topRight == tile.bottomLeft))
            	continue;

        if(tile.tileType == ComputationBoundary)
        {
			if(!(top->bottomRight == tile.topLeft))
            	continue;
        }
        else if(tile.tileType == FinalCheck)
        {
		  	if(!(top->bottomRight == tile.topLeft) || !(left->topRight == tile.bottomLeft))
            	continue;

            newTile = tile;
            break;
        }

        /* If the new tile is an assignment tile, modify the tile so that
        * the input can encoded here.  Use this to change the tile corners into
        * the desired settings.
        */
        if(tile.tileType == Assignment)
        {

		  	if(!(top->bottomRight == tile.topLeft) || !(left->topRight == tile.bottomLeft) || top->topLeft.type != Input_Variable)
            	continue;

        	/* First get the variable to be assigned */
	        //char var = variable_list[top->bottomRight.data_offset];
            tile.bottomRight.data_offset = top->bottomRight.data_offset;
            tile.topLeft.data_offset = top->bottomRight.data_offset;

            //if(current >= 0)
            {
	            bool value = assignments & (1<<current);
    	        current--;

        	    if(value) //+
					tile.bottomRight.type = Assignment_Var1;
	            else //-
					tile.bottomRight.type = Assignment_Var0;
        	}
            newTile = tile;
            break;
        }

        newTile = tile;
    }

    return newTile;
}

char * MethodB::ConstructResultString(char *inputs)
{
	char * buf = (char *)malloc(128 * sizeof(char));
    int k;

    memset(buf, 0, 128);
    k = 0;
    buf[k] = '(';

  	for(int j = 0; j < num_variables; j++)
    {
       	if(j != 0)
   	        buf[++k] = ',';

     	buf[++k] = inputs[j];
    }
    buf[++k] = ')';

    return buf;
}
