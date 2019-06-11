/*
 *----------------------------------------------------------------------------
 * Project: 3x3 Slide Puzzle Solver.
 * Author:  Copyright (C) by Marek Karcz 2002, 2019. All rights reserved.
 * License: Free for personal and non-commercial use.
 * File:    puzzle.cpp
 * Purpose: Puzzle Solver Class Implementation.
 * Notes:
 * To Do:
 *----------------------------------------------------------------------------
 */

#include <iostream>
#include <memory.h>
#include <stdio.h>
#ifndef AIX43
#include <io.h>
#endif
#include "puzzle.h"
#include "puzlist.h"

#ifdef AIX43
#include <unistd.h>
#define _access access
#endif

// The goal configuration, when you change this, remember to change
// Misplaced () too.
Box ganEnd =
{
	{1, 2, 3},
	{4, 5, 6},
	{7, 8, 0}
};

Box * CPuzzle::GetGoal (void)
{
	return (&ganEnd);
}

CPuzzle::CPuzzle ()
{
	m_lID = 0L;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m_Tree.anBox[i][j] = ganEnd[i][j];
	m_Tree.pDown  = NULL;
	m_Tree.pLeft  = NULL;
	m_Tree.pRight = NULL;
	m_Tree.pUp    = NULL;
	m_Tree.lID    = m_lID++;
	for (int k = 0; k < 4; k++)
		m_Order[k] = (ShiftDir) k;
	m_pTree = new (PTreePtr [TREE_SIZE]);
	if (m_pTree == NULL)
		throw (NOMEMORY);
	memset (m_pTree, 0, sizeof (PTreePtr[TREE_SIZE]));
	m_pDescIndexTbl = new (int [TREE_SIZE]);
	if (m_pDescIndexTbl == NULL)
		throw (NOMEMORY);
	memset (m_pDescIndexTbl, 0, sizeof (int [TREE_SIZE]));
	m_pTree[1] = &m_Tree;
}

CPuzzle::CPuzzle (Box a)
{
	m_lID = 0L;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m_Tree.anBox[i][j] = a[i][j];
	m_Tree.pDown  = NULL;
	m_Tree.pLeft  = NULL;
	m_Tree.pRight = NULL;
	m_Tree.pUp    = NULL;
	m_Tree.lID    = m_lID++;
	for (int k = 0; k < 4; k++)
		m_Order[k] = (ShiftDir) k;
	m_pTree = new (PTreePtr[TREE_SIZE]);
	if (m_pTree == NULL)
		throw (NOMEMORY);
	memset (m_pTree, 0, sizeof (PTreePtr[TREE_SIZE]));
	m_pDescIndexTbl = new (int [TREE_SIZE]);
	if (m_pDescIndexTbl == NULL)
		throw (NOMEMORY);
	memset (m_pDescIndexTbl, 0, sizeof (int [TREE_SIZE]));
	m_pTree[1] = &m_Tree;
}

CPuzzle::CPuzzle (Box a, Order b)
{
	m_lID = 0L;
	memcpy (m_Tree.anBox, a, sizeof (Box));
	m_Tree.pDown  = NULL;
	m_Tree.pLeft  = NULL;
	m_Tree.pRight = NULL;
	m_Tree.pUp    = NULL;
	m_Tree.lID    = m_lID++;
	memcpy (m_Order, b, sizeof (Order));
	m_pTree = NULL;
	m_pDescIndexTbl = NULL;
}

CPuzzle::~CPuzzle ()
{
	DeleteBranch (m_Tree.pDown);
	DeleteBranch (m_Tree.pLeft);
	DeleteBranch (m_Tree.pRight);
	DeleteBranch (m_Tree.pUp);
	if (m_pTree != NULL) {
		delete [] m_pTree;
	}
	if (m_pDescIndexTbl != NULL)
		delete [] m_pDescIndexTbl;
}

void CPuzzle::DeleteBranch (PTree *p)
{
	PTree *ptr = p;

	if (ptr != NULL) {
		DeleteBranch (ptr->pDown);
		DeleteBranch (ptr->pLeft);
		DeleteBranch (ptr->pRight);
		DeleteBranch (ptr->pUp);
		delete ptr;
		ptr = NULL;
    p = NULL;
	}
}

void CPuzzle::GenerateBranches (PTree *p)
{
	for (int i = 0; i < 3; i++)
	   for (int j = 0; j < 3; j++)
	      for (int k = 0; k < 4; k++)
			  	Shift (i, j, (ShiftDir) k, p);
}

void CPuzzle::GenerateTree (Box b)
{
	PTree *p;
	int nCount = 0;
	int nCount2 = 0;
	int nCount3 = 0;
	int nDescIndex;

	if (_access ("tree.bin", 0) != -1)	{
		
		FILE *pFile;
		PTree *pTreeTbl;
		int *pnTreeCoded;
		
		pTreeTbl = new (PTree[MAX_PERM]);
		pnTreeCoded = new (int [MAX_PERM]);
		
		if (pTreeTbl == NULL || pnTreeCoded == NULL)
			throw (NOMEMORY);
			
		cout << "Reading tree from file..." << endl;			
	
		if ((pFile = fopen ("tree.bin", "rb")) != NULL) {
			
			int n = 0, mfrerr = 0;
			
			cout << "File <tree.bin> opened successfully." << endl;

			n = fread (pnTreeCoded, sizeof (int), MAX_PERM, pFile);
			if (n < MAX_PERM) {
				cout << "ERROR: Tree read bytes " << n << " out of " << MAX_PERM;
				cout << endl;
				mfrerr++;
			}
			n = fread (m_pDescIndexTbl, sizeof (int), MAX_PERM, pFile);
			if (n < MAX_PERM) {
				cout << "ERROR: Desc. idx read bytes " << n << " out of " << MAX_PERM;
				cout << endl;
				mfrerr++;
			}			
			fclose (pFile);
		
			if (0 == mfrerr)
				cout << "File read successfull." << endl;
			else {
				free (pnTreeCoded);
				cout << "ERROR: File read errors: " << mfrerr << endl;
				throw (FILEERROR);
			}
			
			for (int i = 0; i < MAX_PERM; i++) {
				
				m_Visited.IntToBox ((Box *) &(pTreeTbl[i].anBox), pnTreeCoded[i]);
				m_pTree[i] = pTreeTbl + i;
			}
			free (pnTreeCoded);
			
		}	else {
			
			free (pnTreeCoded);
			cerr << "Error opening file <tree.bin>" << endl;
			throw (FILEERROR);
		}
		
		return ;
	}
	GenerateTree();
}

/*
 * Generate the tree of all branches leading to the goal state. Save it in the
 * binary file for further reuse.
 */
void CPuzzle::GenerateTree (void)
{
	PTree *p;
	int nCount = 0;
	int nCount2 = 0;
	int nCount3 = 0;
	int i, nDescIndex;

	cout << "Generating all solvable unique states tree to file..." << endl;
	for (i = 1, nDescIndex = i + 1; i < MAX_PERM; i++)
	{
#ifdef DEBUG		
		cout << i << " / " << MAX_PERM << ", " << nDescIndex << "\r";
#endif		
		p = m_pTree[i];
		if (p == NULL) {
			
			nCount3++;
			continue;
		}
		nCount++;
		
		if (m_Visited.IsStateInAssocTbl (p->anBox) != true)
			m_Visited.AddStateToAssocTbl (p->anBox);
			
		GenerateBranches (p);
		
		if (nDescIndex >= MAX_PERM)
			break;
			
		if (p->pLeft != NULL
				&& m_Visited.IsStateInAssocTbl (p->pLeft->anBox) != true)	{
					
			m_pDescIndexTbl[nDescIndex]  	= i;
			m_pTree[nDescIndex++]  				= p->pLeft;
			m_Visited.AddStateToAssocTbl (p->pLeft->anBox);
			nCount++;
		}
		else
			nCount2++;
			
		if (p->pUp != NULL
				&& m_Visited.IsStateInAssocTbl (p->pUp->anBox) != true)	{
					
			m_pDescIndexTbl[nDescIndex]  	= i;
			m_pTree[nDescIndex++]    			= p->pUp;
			m_Visited.AddStateToAssocTbl (p->pUp->anBox);
			nCount++;
		}
		else
			nCount2++;
			
		if (p->pDown != NULL
				&& m_Visited.IsStateInAssocTbl (p->pDown->anBox) != true)	{
					
			m_pDescIndexTbl[nDescIndex]  	= i;
			m_pTree[nDescIndex++]  				= p->pDown;
			m_Visited.AddStateToAssocTbl (p->pDown->anBox);
			nCount++;
		}
		else
			nCount2++;
			
		if (p->pRight != NULL
				&& m_Visited.IsStateInAssocTbl (p->pRight->anBox) != true) {
					
			m_pDescIndexTbl[nDescIndex]  	= i;
			m_pTree[nDescIndex++] 				= p->pRight;
			m_Visited.AddStateToAssocTbl (p->pRight->anBox);
			nCount++;
		}
		else
			nCount2++;
	}
	
	cout << "Tree generated. Index = " << i << ". States generated: ";
	cout << nCount << "." << endl;
	cout << "States repeats: " << nCount2 << ". NULL pointers: ";
	cout << nCount3 << "." << endl;
	cout << "Descendants Index = " << nDescIndex << endl;
	cout << "Assoc. Table size: " << m_Visited.GetAssocTblLength () << endl;	
	
	m_Visited.ReleaseAssocTbl ();
	
	cout << "Saving tree file..." << endl;

	FILE *pFile;
	int *pnTreeCoded;
	pnTreeCoded = new (int [MAX_PERM]);
	
	if (pnTreeCoded == NULL)
		throw (NOMEMORY);

#ifdef DEBUG
  cout << "TEST: # Box BoxToInt IntToBox" << endl;
  cout << "=========================================================" << endl;
#endif  
	for (i = 1; i < MAX_PERM; i++) {
#ifdef DEBUG			
		Box a;
#endif		
		if (NULL != m_pTree[i]) {
			pnTreeCoded[i] = m_Visited.BoxToInt (m_pTree[i]->anBox);
#ifdef DEBUG			
			m_Visited.IntToBox ((Box *) &a, pnTreeCoded[i]);
			cout << i << ": ";
			ShowStateCompact(m_pTree[i]->anBox, true);
			cout << " " << pnTreeCoded[i] << " ";
			ShowStateCompact(a);
#endif			
		}
	}		
#ifdef DEBUG	
	cout << "=========================================================" << endl;
#endif	
	
	if ((pFile = fopen ("tree.bin", "wb")) != NULL)	{
		
		cout << "File tree.bin opened for writing." << endl;
		fwrite (pnTreeCoded, sizeof (int), MAX_PERM, pFile);
		fwrite (m_pDescIndexTbl, sizeof (int), MAX_PERM, pFile);
		fclose (pFile);
		cout << "Saved successfully." << endl;
		
		free (pnTreeCoded);

	}	else {
		
		free (pnTreeCoded);
		cerr << "Error opening file tree.bin" << endl;
		throw (FILEERROR);
	}
}

/*
 * Shift ()
 * Generate the tree branch, attach the new branch to the trunk and return it.
 */

PTree * CPuzzle::Shift (int x, int y, ShiftDir z, PTree *pBox)
{
	PTree *pRet = NULL;

	pRet = new (PTree);

	if (pRet == NULL)	{
		cerr << endl << "Out of memory in Shift ()" << endl;
		throw (NOMEMORY);
	}
	if (NULL != pBox) {
		for (int i=0; i<3; i++) {
			for (int j=0; j<3; j++) {
				pRet->anBox[i][j] = pBox->anBox[i][j];
			}		
		}
	}
	
	pRet->pDown = NULL;
	pRet->pLeft = NULL;
	pRet->pRight = NULL;
	pRet->pUp    = NULL;

	if (NULL != pBox && 0 != pBox->anBox[x][y])	{
	   switch (z)
	   {
		  case LEFT:
			 if (y > 0) {
				if (pBox->anBox[x][y-1] == 0)	{
				   pRet->anBox[x][y-1] = pRet->anBox[x][y];
				   pRet->anBox[x][y] = 0;
				   pRet->lID = m_lID++;
				   pBox->pLeft = pRet;
           return (pRet);
				}
			 }
			 break;
		  case UP:
			 if (x > 0) {
				if (pBox->anBox[x-1][y] == 0)	{
				   pRet->anBox[x-1][y] = pRet->anBox[x][y];
				   pRet->anBox[x][y] = 0;
				   pRet->lID = m_lID++;
				   pBox->pUp = pRet;
           return (pRet);
				}
			 }
			 break;
		  case RIGHT:
			 if (y < 2) {
				if (pBox->anBox[x][y+1] == 0)	{
				   pRet->anBox[x][y+1] = pRet->anBox[x][y];
				   pRet->anBox[x][y] = 0;
				   pRet->lID = m_lID++;
				   pBox->pRight = pRet;
           return (pRet);
				}
			 }
			 break;
		  case DOWN:
			 if (x < 2) {
				if (pBox->anBox[x+1][y] == 0) {
				   pRet->anBox[x+1][y] = pRet->anBox[x][y];
				   pRet->anBox[x][y] = 0;
				   pRet->lID = m_lID++;
				   pBox->pDown = pRet;
           return (pRet);
				}
			 }
			 break;
		  default:
			 break;
	   }
	}

	delete pRet;

  return (NULL);
}

bool CPuzzle::CompareBox (Box x, Box y)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (y[i][j] != x[i][j])
				return (false);

	return (true);
}

bool CPuzzle::IsGoal (Box a)
{
	return (CompareBox (a, ganEnd));
}

// AI starts here :-)

// Try to forecast which branch to take to find an optimal solution.
void CPuzzle::HeuristicEval (PTree *pBox, Order p)
{
    int i;
	ShiftDir DirTbl[4] = {LEFT, UP, RIGHT, DOWN};
	
	if (NULL == pBox)
		return;
	// left up right down
	int HeurVal[4], *pH = HeurVal;
	for (i = 0; i < 4; i++)
	{
		p[i] = DirTbl[i];
		HeurVal[i] = 100;
	}
	if (pBox->pLeft != NULL)
		HeurVal[LEFT] = Misplaced (pBox->pLeft->anBox);
	if (pBox->pUp != NULL)
		HeurVal[UP] = Misplaced (pBox->pUp->anBox);
	if (pBox->pRight != NULL)
		HeurVal[RIGHT] = Misplaced (pBox->pRight->anBox);
	if (pBox->pDown != NULL)
		HeurVal[DOWN] = Misplaced (pBox->pDown->anBox);

	// Based on calculated values, sort the order table to determine the search
	// order.
	SortHeurTbl (p, pH);
}

int CPuzzle::NoSign (int a)
{
	if (a < 0)
		return (-a);

	return (a);
}

// Determine the misplaced tiles and their distance from goal location.
int CPuzzle::Misplaced (Box b)
{
	int nRet = 0;
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			if (ganEnd[i][j] != b[i][j]) {
				nRet++; // yes, it's misplaced
				switch (b[i][j]) // calculate how much misplaced is the tile
				{
					case 1:
						if ((i + j) != 1 || b[0][0] != 0)
						   nRet += (i + j);
						break;
					case 2:
					    if ((i + NoSign (1 - j)) != 1 || b[0][1] != 0)
						   nRet += i + NoSign (1 - j);
						break;
					case 3:
					    if ((i + (2 - j)) != 1 || b[0][2] != 0)
						   nRet += i + (2 - j);
						break;
					case 4:
					    if ((NoSign (1 - i) + j) != 1 || b[1][0] != 0)
						   nRet += NoSign (1 - i) + j;
						break;
					case 5:
					    if ((NoSign (1 - i) + j) != 1 || b[1][1] != 0)
						   nRet += NoSign (1 - i) + NoSign (1 - j);
						break;
					case 6:
					    if ((NoSign (1 - i) + (2 - j)) != 1 || b[1][2] != 0)
						   nRet += NoSign (1 - i) + (2 - j);
						break;
					case 7:
					    if (((2 - i) + j) != 1 || b[2][0] != 0)
						   nRet += (2 - i) + j;
						break;
					case 8:
					    if (((2 - i) + NoSign (1 - j)) != 1 || b[2][1] != 0)
						   nRet += (2 - i) + NoSign (1 - j);
						break;
					case 0:
						nRet += (2 - i) + (2 - j);
						break;
					default:
						throw (WRONGTILE);
						break;
				}
			}

	return (nRet);
}

// Based on calculated values, sort the order table to determine the search
// order.
void CPuzzle::SortHeurTbl (Order p, int *pH)
{
	if (NULL != pH) {
		for (int i = 0; i < 3; i++) {
			for (int j = i + 1; j < 4; j++) {
				if (pH[i] > pH[j]) {
					int nTmp, nTmp2;
	
					nTmp = pH[j];
					nTmp2 = p[j];
					pH[j] = pH[i];
					p[j] = p[i];
					pH[i] = nTmp;
					p[i] = (ShiftDir) nTmp2;
				}
			}
		}
	}
}

/*
 * Breadth-First Search.
 * Search the list generated by GenerateTree() method.
 * The list of states + descendants index array emulate
 * the tree of solvable states with paths to the root.
 */
Stat CPuzzle::Solve2 (Box b)
{
	PTree *p;
	int nStatCount = 0;
	
	for (int i = 1; i < MAX_PERM; i++) {
		p = m_pTree[i];
		if (p == NULL)
			continue;

		nStatCount++;
		
#ifdef DEBUG		
		cout << nStatCount << ":";
		ShowStateCompact(p->anBox);
#endif		

		if (CompareBox (p->anBox, b) == true)	{
			cout << "Puzzle solved!" << endl;
			CreateGoodWayList (i);
			ShowGoodWay (false);
			return (SOLVED);
		}
	}
	cout << "Branches searched: " << nStatCount << "." << endl;

	return (UNSOLVED);
}

void CPuzzle::CreateGoodWayList (int nIndex)
{
	int i = nIndex;
	while (i >= 1) {
		m_GoodWay.AddAtEnd (m_pTree[i]->anBox);
		i = m_pDescIndexTbl[i];
	}
}

/*
 *	Heuristic Recursive Search.
 */
Stat CPuzzle::Solve (int nDeepLimit,
											int nMaxMoves,
											int nDumpThisClose,
											PTree *pBox,
											bool bUseHeurFirstStep)
{
	char szMsg[10];

	memset (szMsg, '\0', sizeof (szMsg));
	if (pBox == NULL)
		return (NULLBRANCH);
	if (IsGoal (pBox->anBox) == true)	{
		cout << "Puzzle solved!" << endl;
		m_GoodWay.AddAtEnd (pBox->anBox);

		return (SOLVED);
	}
	if (nDumpThisClose > 0 && Misplaced (pBox->anBox) <= nDumpThisClose) {
		cout << pBox->anBox << endl;
		sprintf (szMsg, "%s", " *CLOSE* ");
	}	else
		sprintf (szMsg, "%s", "         ");

	if (nDeepLimit == 0)
		return (DEEPLIMIT);

	if (m_Visited.IsStateInAssocTbl (pBox->anBox) == true) {
	   return (WRONGWAY);
	}
	
	m_Visited.AddStateToAssocTbl (pBox->anBox);
	if (m_Visited.GetLength () > nMaxMoves)
	   throw (MAXMOVES);
	// Generate descendants branches
	GenerateBranches (pBox);
	// Find the most promising route
	if ((bUseHeurFirstStep == true && m_Visited.GetLength () == 1 ) ||
		 m_Visited.GetLength () > 1) {
		 	
	   HeuristicEval (pBox, m_Order);
	}
	// Solve puzzle
	PTreePtr pPtrTbl[] = { pBox->pLeft, pBox->pUp, pBox->pRight, pBox->pDown,
												 NULL };
												 
	for (int l = 0; l < 4; l++)	{
		if (pPtrTbl[m_Order[l]] != NULL) {
			if (Solve (nDeepLimit - 1,
								 nMaxMoves,
								 nDumpThisClose,
								 pPtrTbl[m_Order[l]]) == SOLVED) {
								 	
				m_GoodWay.AddAtEnd (pBox->anBox);
				return (SOLVED);
			}
		}
	}
	
	return (ALLCHECKED);
}

bool CPuzzle::SolveNonRecursive (int nDeepLimit)
{
  throw (NOTIMPLEMENTED);
	return (false);
}

void CPuzzle::ShowGoodWay (bool bReverseList)
{
   if (m_GoodWay.GetLength () > 0) {
		if (bReverseList == true)
			m_GoodWay.ReverseOrder ();
    cout << m_GoodWay << endl;
    cout << "In " << (m_GoodWay.GetLength () - 1) << " moves." << endl;
   }
}

void CPuzzle::ShowVisited (void)
{
   if (m_Visited.GetLength () > 0) {
      cout << "Visited " << m_Visited.GetLength () << " branches:" << endl;
	  	cout << m_Visited << endl;
   }
}

void CPuzzle::ShowState (Box state)
{
  cout << "-------" << endl;
  for (int i = 0; i < 3; i++) {
     for (int j = 0; j < 3; j++)
        cout << state[i][j] << " ";
     cout << endl;
  }
  cout << "-------" << endl;
}

void CPuzzle::ShowStateCompact (Box state, bool nocr)
{
  for (int i = 0; i < 3; i++) {
     for (int j = 0; j < 3; j++)
        cout << state[i][j];
  }
  if (false == nocr)
  	cout << endl;
}
