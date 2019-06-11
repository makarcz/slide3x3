/*
 *----------------------------------------------------------------------------
 * Project: 3x3 Slide Puzzle Solver.
 * Author:  Copyright (C) by Marek Karcz 2002, 2019. All rights reserved.
 * License: Free for personal and non-commercial use.
 * File:    main.cpp
 * Purpose: Main Program Loop Implementation.
 * Notes:
 * To Do:
 *----------------------------------------------------------------------------
 */

#include <iostream>
#include <stdlib.h>
#include <string.h>
#ifndef AIX43
#include <io.h>
#endif
#include "puzlist.h"
#include "puzzle.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void Usage (char *szPrgName)
{
	cout << "Usage:" << endl;
	cout << endl;
	cout << szPrgName;
	cout << " -tbegin T T T T T T T T T -tend [-depth n] [-moves n]" << endl;
	cout << "   [-obegin B B B B -oend] [-thisclose n]" << endl;
	cout << endl;
	cout << "Where:" << endl;
	cout << endl;
	cout << "-tbegin ... -tend - the initial state to solve given as a list of ";
	cout << "9 tiles." << endl;
	cout << "T - the puzzle tile number 1..8 OR 0 for an empty slot." << endl;
	cout << "n - the positive non-zero integer number." << endl;
	cout << "B - the integer number 0..3, where 0,1,2,3 stand for LEFT,UP,";
	cout << "RIGHT,DOWN" << endl;
	cout << "    branches." << endl;
	cout << endl;
	cout << "If any of the options below are used, the heuristic algorithm is ";
	cout << "assumed:" << endl;
	cout << endl;
	cout << "-depth - the depth limit for recursive heuristic algorithm." << endl;
	cout << "-moves - the searching steps limit for recursive heuristic ";
	cout << "algorithm." << endl;
	cout << "-obegin ... -oend - the order of search in the first step of ";
	cout << "recursive" << endl; 
	cout << "                    heuristic algorithm." << endl;
	cout << "-thisclose - the heuristic distance for which the current state ";
	cout << "should be" << endl;
	cout << "             dumped while searching." << endl;
	cout << endl;
	cout << "Otherwise, Breadth-first search (BDF) is used." << endl;
	cout << endl;
	cout << "BDF generates all the states to the linear table which, together ";
	cout << "with" << endl;
	cout << "descendants index represents the tree starting from the root.";
	cout  << endl;
	cout << "The root state is the goal state." << endl;
	cout << "The list of all solutions is saved in an integer encoded form to ";
	cout << "a file" << endl;
	cout << "for later re-use. The solutions cache increases the speed as the ";
	cout << "tree" << endl;
	cout << "doesn't need to be re-generated each time the program runs." << endl;
	cout << "Search starts at the root and ends when initial state is found.";
	cout << endl;
	cout << "With BDF the shortest puzzle solution path is found, while the";
	cout << endl;;
	cout << "heuristic algorithm may need more steps." << endl;
	cout << endl;
}

/*
 * Scan command line arguments:
 * Common section (breadth-first search & heuristics recursive search
 * algorithms):
 * -tbegin - the beginning of the initial state
 * -tend - the end of the initial state
 * Heuristic recursive search algorithm only:
 * -depth - the depth of the recursive heuristic algorithm
 * -moves - the number of moves limit for the recursive heuristic algorithm
 * -obegin - beginning of the search order table in the first step of heuristic
 *					 algorithm
 * -oend - search order table end
 * -thisclose - the heuristic distance for which the program should dump the
 *							state to output
 */
int ScanArg (char **argv, 
			 Box *p, 
			 int *pnDepth, 
			 int *pnMoves, 
			 Order *pOrd, 
			 bool *pbUseHeur, 
			 int *pnThisClose,
			 bool *pbUseHeurAlg)
{
	int i = 1;
	while (argv[i] != NULL)	{
		if (strcmp (argv[i], "-tbegin") == 0) {
			int x = 0, y = 0;
			i++;
			while (strcmp (argv[i], "-tend") != 0 && x < 3)	{
				(*p)[x][y] = atoi (argv[i]);
				if ((*p)[x][y] < 0 || (*p)[x][y] > 8)	{
					cerr << "Invalid tile number (must be 0..8, 0 - empty)." << endl;
					exit (-1);
				}
				y++;
				if (y == 3)	{
					y = 0;
					x++;
				}
				i++;
			}
		}	else if (strcmp (argv[i], "-depth") == 0)	{
			*pbUseHeurAlg = true;
			i++;
			*pnDepth = atoi (argv[i]);
			if (*pnDepth < 1) {
				cerr << "Invalid depth limit value (must be >= 1)." << endl;
				exit (-2);
			}
		}	else if (strcmp (argv[i], "-moves") == 0)	{
			*pbUseHeurAlg = true;
			i++;
			*pnMoves = atoi (argv[i]);
			if (*pnMoves < 1) {
				cerr << "Invalid moves limit value (must be >= 1)." << endl;
				exit (-3);
			}
		}	else if (strcmp (argv[i], "-obegin") == 0) {
			*pbUseHeurAlg = true;
			int x = 0;
			i++;
			while (strcmp (argv[i], "-oend") != 0) {
				(*pOrd)[x] = (ShiftDir) atoi (argv[i]);
				if ((*pOrd)[x] < 0 || (*pOrd)[x] > 3)	{
					cerr << "Invalid order value (must be 0..3)." << endl;
					exit (-4);
				}
				x++;
				i++;
			}
			*pbUseHeur = false;
		}	else if (strcmp (argv[i], "-thisclose") == 0)	{
			*pbUseHeurAlg = true;
			*pnThisClose = atoi (argv[++i]);
			cerr << "Dump data when the heuristic distance to solution is less or ";
			cerr << "equal to " << *pnThisClose << "." << endl;
		}	else {
			cerr << "Unknown parameter:" << argv[i] << endl;
		}
		i++;
	}

	return (i);
}

eSolveStat SolvePuzzle (int nDeep,
												int nMoves,
												int nDump,
												PTree *pBox,
												bool bUseHeurFirst)
{
	Order anOrder = {LEFT, UP, RIGHT, DOWN};
	eSolveStat stat;

	CPuzzle Puzzle (pBox->anBox, anOrder);

	stat = Puzzle.Solve (nDeep, nMoves, nDump, pBox, bUseHeurFirst);
	if (stat == SOLVED)	{
		Puzzle.ShowGoodWay ();
	}

	return (stat);
}

int main (int argc, char *argv[])
{
	Box anStart =
	{
		{ 5, 4, 8},
		{ 7, 0, 3},
		{ 1, 2, 6}
	};
	Order anOrder = {LEFT, UP, RIGHT, DOWN};
	#ifdef LOWRAM
	int nMaxDeep = 40;
	#else
	int nMaxDeep = 100;
	#endif
	int nMaxMoves = 10000;
	bool bUseHeurFirstStep = true;
	int nThisClose = 0;
	bool bUseHeuristicAlg = false;

	cout << endl;
	cout << "Slide puzzle 3x3. Copyright (C) by Marek Karcz 2002, 2019." << endl;
	cout << endl;

	if (argc > 1) {
		ScanArg (argv, &anStart, &nMaxDeep, &nMaxMoves, 
				 			&anOrder, &bUseHeurFirstStep, &nThisClose,
				 			&bUseHeuristicAlg
						);
	}	else {
		Usage (argv[0]);
		exit (0);
	}

	CPuzzle Puzzle ;

	cout << "Starting state:" << endl;
	Puzzle.ShowState (anStart);

	cout << "Goal state:" << endl;
	Puzzle.ShowState (*(Puzzle.GetGoal ()));

	if (bUseHeuristicAlg == true) {

		cout << "Algorithm type: Recursive Heuristic Search." << endl;
		cout << "Algorithm depth limit = " << nMaxDeep;
		cout << ", Visited branches limit = " << nMaxMoves << "." << endl;

		if (bUseHeurFirstStep != true)
			cout << "Suppress heuristics in first step." << endl;
	}	else
		cout << "Algorithm type: Breadth-First Search." << endl;

	try	{
		Stat stat;
		cerr << "Please wait..." << endl;
		if (bUseHeuristicAlg == true)	{
			CPuzzle Puzzle1 (anStart, anOrder);
			int nDeepControl = 10;
		
			while (nDeepControl < nMaxDeep
						 &&
						 (stat = SolvePuzzle (nDeepControl++, 
			                         	  nMaxMoves, 
																  nThisClose, 
																  &(Puzzle1.m_Tree), 
																  bUseHeurFirstStep)) != SOLVED
				  	) /* EMPTY */;
		} else {
			
			// Breadth-First Search
			Puzzle.GenerateTree(anStart);
			stat = Puzzle.Solve2 (anStart);
		}
		
		if (stat != SOLVED) {
			cout << "The solution for this puzzle initial configuration could not ";
			cout << "be found." << endl;
		}
		
	}	catch (eSolveStat e) {
		cerr << endl;
		cerr << "Can't solve the puzzle, reason:" << endl;
		switch (e)
		{
		case MAXMOVES:
			cerr << "The number of moves limit reached." << endl;
			break;
		case WRONGTILE:
			cerr << "Wrong tile number detected in the puzzle box." << endl;
			break;
		case NOMEMORY:
			cerr << "Out of memory error." << endl;
			break;
		case NOTIMPLEMENTED:
			cerr << "Attempt to use method that is not implemented." << endl;
			break;
		case FILEERROR:
			cerr << "File operation error." << endl;
			break;
		default:
			cerr << "Unknown." << endl;
			break;
		}
		exit (-1);
	}	catch (...)	{
		cerr << endl;
		cerr << "Unhandled exception error." << endl;
		exit (-2);
	}

	return (0);
}

