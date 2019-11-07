#include <math.h>

#include "resltswn.h"
#include "layoutwn.h"
#include "methoda.h"
#include "win.h"

TWin * win;

MethodA::MethodA()
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

MethodA::~MethodA()
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
       	free(bad_inputs[i]); */
}

bool MethodA::ParseFormula(char * string)
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
            	variable_list = (char*)malloc(1*sizeof(char));
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
        	clauses = (Clause*)realloc(clauses, 3*sizeof(Clause)*(num_clauses+1));
        	num_clauses++;
            a = 0;
        }

    	pos++;
    }

	return true;
}

bool MethodA::RunComputation(void * w, bool value, int a)
{
	bool res;
    bool sat = false;
    double n = pow(2, num_variables);   

    win = (TWin*)w;

    TResultsWin *res_win = TYPESAFE_DOWNCAST(win->Parent, TLayoutWin)->res_win;
    TRect rect;
    rect.top = 5;
    rect.left = 0;
    rect.right = res_win->GetClientRect().Width();
    rect.bottom = 150;

    if(!value)
	    CreateTileLibrary();

    for(int i = 0; i < n; i++)
    {
        if(tiles != NULL)
		{
    		for(int j = 0; j < last_height; j++)
			 	free(tiles[j]);
			 free(tiles);

            free(current_input);
	    }

        current = num_variables-1;

        win->percent = 100*(i / n);

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

     	res = RunMethodA();

        if(value)
        	break;

        if(res)
        {
        	sat = true;
    	   /* valid_inputs[numValidInputs] = (char*)malloc(num_variables * sizeof(char)); //inputs that satisfy
	        for(int j = num_variables-1, k = 0; j >= 0; j--, k++)
            	valid_inputs[numValidInputs][j] = (current_input[j] == true) ? '1' : '0';
	        numValidInputs++; */
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
	        numBadInputs++;*/
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

void MethodA::CreateTileLibrary()
{
	if(tileLibrary)
    	free(tileLibrary);
    tileLibrary = NULL;

    int m = num_variables;
    //int n = ceil((4/3)*m*(m-1)*(m-2));
    int n = num_clauses;
    //int n = ceil(6*m +(2*m*n)+3);
    //library_count = m + n + 4 + 2*m + 2*m + 2*m + 2*m*n + 3;

    //tileLibrary = new Tile[count];

    int loc = 0;

    /* Assignment tiles 0...m */
    for(int i = 0; i < m; i++, loc++)
    {
    	TileCorner tr = TileCorner(Assingment_V, -1);
    	TileCorner br = TileCorner(Assignment_Var1, i);
    	TileCorner bl = TileCorner(Assingment_V, -1);
    	TileCorner tl = TileCorner(Assignment_Variable, i);

        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));

    	tileLibrary[i] = Tile(Assignment, tl, tr, bl, br);
    }

    /* Assignment tiles m...2m */
    for(int i = 0; i < m; i++, loc++)
    {
    	TileCorner tr = TileCorner(Assingment_V, -1);
    	TileCorner br = TileCorner(Assignment_Var0, i);
    	TileCorner bl = TileCorner(Assingment_V, -1);
    	TileCorner tl = TileCorner(Assignment_Variable, i);

        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
    	tileLibrary[loc] = Tile(Assignment, tl, tr, bl, br);
    }

    /* Computation boundary tiles 0...m */
    for(int i = 0; i < m; i++, loc++)
    {
    	TileCorner tr = TileCorner(ComputationBoundary_E, -1);
    	TileCorner br = TileCorner(Empty, -1);
    	TileCorner bl = TileCorner(ComputationBoundary_E, -1);
    	TileCorner tl = TileCorner(ComputationBoundary_Var1, i);

        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
    	tileLibrary[loc] = Tile(ComputationBoundary, tl, tr, bl, br);
    }

    /* Computation boundary tiles m...2m */
    for(int i = 0; i < m; i++, loc++)
    {
    	TileCorner tr = TileCorner(ComputationBoundary_E, -1);
    	TileCorner br = TileCorner(Empty, -1);
    	TileCorner bl = TileCorner(ComputationBoundary_E, -1);
    	TileCorner tl = TileCorner(ComputationBoundary_Var0, i);

        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
    	tileLibrary[loc] = Tile(ComputationBoundary, tl, tr, bl, br);
    }

    /* computation tiles */
    for(int i = 0; i < m; i++) //for each variable
    {
		for(int j = 0; j < n; j++) //for each clause
        {
    		TileCorner tr = TileCorner(Computation_Clause, j);
    		TileCorner tl = TileCorner(Computation_Var1, i);
    		TileCorner bl = TileCorner(Computation_Clause, j);
    		TileCorner br = TileCorner(Computation_Var1, i);
	        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
	    	tileLibrary[loc] = Tile(Computation, tl, tr, bl, br);
        	loc++;

    		tr = TileCorner(Computation_Clause, j);
    		tl = TileCorner(Computation_Var0, i);
    		bl = TileCorner(Computation_Clause, j);
    		br = TileCorner(Computation_Var0, i);
            tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
	    	tileLibrary[loc] = Tile(Computation, tl, tr, bl, br);
        	loc++;

            //do the tiles that have a clauses and an OK
    		tr = TileCorner(Computation_OK, -1);
    		tl = TileCorner(Computation_Var0, i);
    		bl = TileCorner(Computation_Clause, j);
    		br = TileCorner(Computation_Var0, i);
	        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
	    	tileLibrary[loc] = Tile(Computation, tl, tr, bl, br);
        	loc++;

    		tr = TileCorner(Computation_OK, -1);
    		tl = TileCorner(Computation_Var1, i);
    		bl = TileCorner(Computation_Clause, j);
    		br = TileCorner(Computation_Var1, i);
	        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
	    	tileLibrary[loc] = Tile(Computation, tl, tr, bl, br);
            loc++;
        }

        //each variable has two tiles with 2 OK's, one for + and one for -
   		TileCorner tr = TileCorner(Computation_OK, -1);
   		TileCorner tl = TileCorner(Computation_Var1, i);
   		TileCorner bl = TileCorner(Computation_OK, -1);
   		TileCorner br = TileCorner(Computation_Var1, i);
        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
    	tileLibrary[loc] = Tile(Computation, tl, tr, bl, br);
       	loc++;
   		tr = TileCorner(Computation_OK, -1);
   		tl = TileCorner(Computation_Var0, i);
   		bl = TileCorner(Computation_OK, -1);
   		br = TileCorner(Computation_Var0, i);
        tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
    	tileLibrary[loc] = Tile(Computation, tl, tr, bl, br);
        loc++;
    }

    //do the 3 final tiles
	TileCorner tr = TileCorner(Empty, -1);
	TileCorner tl = TileCorner(FinalCheck_C, -1);
	TileCorner bl = TileCorner(FinalCheck_V, -1);
	TileCorner br = TileCorner(FinalCheck_T, -1);
    tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;
	tr = TileCorner(Empty, -1);
	tl = TileCorner(FinalCheck_T, -1);
	bl = TileCorner(FinalCheck_OK, -1);
	br = TileCorner(FinalCheck_T, -1);
    tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    loc++;
	tr = TileCorner(FinalCheck_Sucess, -1);
	tl = TileCorner(FinalCheck_T, -1);
	bl = TileCorner(FinalCheck_E, -1);
	br = TileCorner(FinalCheck_Sucess, -1);
    tileLibrary = (Tile*)realloc(tileLibrary, (loc+1) * sizeof(Tile));
   	tileLibrary[loc] = Tile(FinalCheck, tl, tr, bl, br);
    library_count = loc+1;
}

bool MethodA::RunMethodA()
{
    int width = CalculateRequiredWidth(METHOD_A);
  	int height = CalculateRequiredHeight(METHOD_A);

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

    /* Clauses */
  	bl = TileCorner(Input_V, -1);
	tiles[1][0] = Tile(InputBoundary, bl,bl, bl, bl);

    for(int i = 0; i < num_clauses; i++)
    {
	  	TileCorner t = TileCorner(Input_Clause, i);
		tiles[i+2][0] = Tile(InputBoundary, t,t,t,t);
    }

  	tl = TileCorner(Input_E, -1);
	tiles[num_clauses+2][0] = Tile(InputBoundary, tl,tl, tl, tl);

    //win->drawTiles(last_width, last_height);

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

Tile MethodA::PickTile(Tile * top, Tile * left)
{
    Tile newTile;

	if((int)(left->topRight.type / 10)  == 4)
    {
    	if(top->bottomRight.data_offset == -1) //error..can't continue.  Not SATisfiable
        	return Tile(Error);

      	Clause *c = &clauses[left->topRight.data_offset];
        char var = variable_list[top->bottomRight.data_offset];
	    CornerType type = top->bottomRight.type;

        bool not = false, found = false;
        //Solve the clause...
        for(int i = 0; i < 256; i++)
        {
        	if(c->variables[i] == var)
            {
                found = true;
				if(c->nots[i] == 1)
                	not = true;
                break;
            }
        }

        bool sat = false;
    	int var0_or_1 = type / 10;

        if(var0_or_1 == 2) //Var1 = 1
        {
        	if(not)
            	sat = false;
            else
            	sat = true;
        }
        else if(var0_or_1 == 3)//Var0 = 0
        {
        	if(not)
            	sat = true;
            else
            	sat = false;
        }
        else
        	return Tile(Error);

        //Build the new tile.
        //if sat is true, then put in an ok. otherweise, propagate the clause

        TileCorner tl = top->bottomRight;
        TileCorner bl = left->topRight;
        TileCorner br = tl;
        TileCorner tr = bl;

        if(sat && found)
        	tr = TileCorner(Computation_OK, -1);

        newTile = Tile(Computation, tl, tr, bl, br);
        return newTile;
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

        if(mode1 ==  2 || mode1 == 3) //variables
        {
        	if(!(top->bottomRight == tile.topLeft))
            	continue;
        } 

        if(mode2 == 4) //clauses
        {
        	if(!(left->topLeft == tile.bottomLeft))
            	continue;
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

           // if(current >= 0)
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

int MethodA::CalculateRequiredWidth(int)
{
	int width;
	width = num_variables + 2;// + num_clauses + 4;

    return width;
}

int MethodA::CalculateRequiredHeight(int)
{
	int height;
    height = num_clauses + 3;    //???

    return height;
}

char * MethodA::ConstructResultString(char *inputs)
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
