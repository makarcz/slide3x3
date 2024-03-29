/*
 *----------------------------------------------------------------------------
 * Project: 3x3 Slide Puzzle Solver.
 * Author:  Copyright (C) by Marek Karcz 2002, 2019. All rights reserved.
 * License: Free for personal and non-commercial use.
 * File:    puzlist.h
 * Purpose: Class Prototype For List and State Assoc. Table.
 * Notes:
 * To Do:
 *----------------------------------------------------------------------------
 */
#ifndef _PUZLIST_H_
#define _PUZLIST_H_

#include <iostream>
#include <set>

using namespace std;

// puzzle state
typedef int Box[3][3];

enum eSolveStat
{
	ZERO = 0, SOLVED, MAXMOVES, WRONGWAY, NULLBRANCH, DEEPLIMIT, 
		ALLCHECKED, WRONGTILE, NOMEMORY, NOTIMPLEMENTED, UNSOLVED,
		FILEERROR
};

typedef eSolveStat Stat;

struct ListItem
{
	Box anData;
	ListItem *pNext;
	ListItem *pPrev;
};

typedef ListItem *PList;
typedef std::set<int> BoxSet;

class CPuzzleList
{
	BoxSet AssocTbl;
	int m_nLength;
	int m_nIndex;
	PList m_pEnd;

	public:
	   PList m_pData;
     CPuzzleList ();
	   ~CPuzzleList ();

	   int AddAtEnd (Box anData);
	   int RemoveFromEnd ();
	   int FreeMem (PList p);
     int RemoveFromAt (int nIndex);
	   int GetLength ();
	   int InsertAt (int nIndex);
	   int FindItem (Box anItem);
	   void ReverseOrder (void);
	   void AddStateToAssocTbl (Box anData);
	   bool IsStateInAssocTbl (Box anData);
	   void ReleaseAssocTbl (void);
	   int GetAssocTblLength (void);
	   int BoxToInt (Box anData);
	   void IntToBox (Box *pBox, int nBoxCode);

     CPuzzleList operator= (CPuzzleList x)
     {
		  	PList ptr = m_pData;
      	if (m_pData != NULL)
        	FreeMem (m_pData);
        m_nLength = x.m_nLength;
        m_nIndex = x.m_nIndex;
		  	for (PList p = x.m_pData; p != NULL; p = p->pNext) {
					ptr = new (ListItem);
					if (ptr == NULL)
						throw (NOMEMORY);
					memcpy (ptr->anData, p->anData, sizeof (Box));
					ptr = ptr->pNext;
		  	}

        return (*this);
    }
     
		friend ostream & operator<< (ostream &os, CPuzzleList &data)
    {
    	os << "LIST:" << endl;
      for (PList p = data.m_pData; p != NULL; p = p->pNext) {
     		os << "-------" << endl;
        for (int i=0; i<3; i++) {
        	for (int j=0; j<3; j++)	{
          	os << p->anData[i][j] << " ";
					}
					os << endl;
				}
				os << "-------" << endl;
        os << "NEXT..." << endl;
      }
      os << "END OF LIST." << endl;
      
			return (os);
   }
};

#endif //_PUZLIST_H_
