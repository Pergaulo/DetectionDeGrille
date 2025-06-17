#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

				
bool isSafe(int board[9][9], size_t L, size_t C, int num)
{
        //A function that check if we can put a num in this coordinate L,C,l,c
        for (size_t Li = 0; Li < 9; Li++)
        {
                int actual = board[Li][C];
                if (actual == 0)
                {
                        continue;
                }
                else
                {
                        if (actual == num)
                        {
                                return false;
                        }
                }

        }

        for (size_t Ci = 0; Ci < 9; Ci++)
        {
        
                int actual = board[L][Ci];
                if (actual == 0)
                {
                        continue;
                }
                else
                {
                        if (actual == num)
                        {
                                return false;
                        }
                }
                
        }

        for (size_t li = 0; li < 3; li++)
        {
                for (size_t ci = 0; ci < 3; ci++)
                {
                        int actual = board[(L/3)*3 + li][(C/3)*3 +ci];
                        if (actual == 0)
                        {
                                continue;
                        }
                        else
                        {
                                if (actual == num)
				{
                                        return false;
                                }
                        }
                }
        }

        return true;
}


bool fill_board(int board[9][9], size_t L, size_t C)
{
	//A function that fille the board
	//For each element of the board

	if (L == 9 && C == 0)
	{
		return true;
	}

	if (C==9)
	{
		C = 0;
		if (L == 8)
		{
			return true;
		}
		else
		{
			L++;
		}
	}


	if (board[L][C] != 0)
	{
		return fill_board(board, L, C + 1);
	}


	for (unsigned int i = 1; i < 10; i++)
	{
		if (isSafe(board,L,C,i) == true)
		{
			board[L][C] = i;

			if (fill_board(board,L,C + 1) == true)
			{
				return true;
			}
			
			//undo the assignement
			board[L][C] = 0;
		}
	}

	return false;
}


void solver(const char *input_file)
{
        //Open the file given
        FILE *file = fopen(input_file, "r");

        //Create a board of 4 dimensions to organize the sudoku given
        // Allocate memory for the dynamic 4-dimensional array
        int board[9][9];
        char ligne[13]; //Create a ligne that contain the actual ligne used

        //Parse the file to create the boardboard
        for (size_t L = 0; L < 9; L++)
        {

                for (size_t C = 0; C < 9; C++)
                {
                        board[L][C] = 0;
                }
        }


        //Parse the file to create the board
        for (size_t L = 0; L < 9; L++)
        {
                //Get the next line
                fgets(ligne, sizeof(ligne), file);

                //Send a Error if there is not enought line in the given file
                if(ligne == NULL)
                {
                        errx(1, "An error occured: Not enought Line in the input_file");
                }
                if(L == 3 || L == 6)
                {
                        fgets(ligne, sizeof(ligne), file);
                }

                int is_passed = 0;
                for (size_t C = 0; C < 11; C++)
                {
                        //actual : Actual character to put in the board or skip
                        char actual = ligne[C];
                        
                        if(actual == ' ')
                        {
                                is_passed += 1;
                                continue;
                        }

                        // Actual caracter in the line = l*3 + c
                        if (actual == '.')
                        {
                                board[L][C - is_passed] = 0;
                        }
                        else
                        {
                                board[L][C - is_passed] = actual - 48;
                        }
                }
        }

        fclose(file); //Close the file a the end of the read.

        //Call another function to fill the board
        fill_board(board, 0,0);
        
        FILE *solver;
        
        char infile[100];

        for (size_t i = 0; i < 100; i++)
        {
                infile[i] = 0;
        }

        char to_add[] = ".result";
        strcat(infile, input_file); 
        strcat(infile, to_add);
        solver = fopen(infile, "w");

        for (size_t L = 0; L < 9; L++)
        {

                for (size_t C = 0; C < 9; C++)
                {
                        const char to_a = (char)(board[L][C] + 48);
                        
                        fprintf(solver, "%c", to_a);
                                
                        if(C == 2 || C == 5)
                        {
                                fprintf(solver, " ");
                        }
                        
                }
                fprintf(solver, "\n");
                if (L == 2 || L == 5)
                {
                    fprintf(solver, "\n");    
                }
                
        }

        fclose(solver);


}

int main() 
{
        solver("grid_00");
    	return 0;
}



