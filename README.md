
I am sure many people know this simple toy.
There are 8 numbered 1..8 squares in a 3 x 3 array, so that one space is empty.
The squares neighboring the empty space can slide into it.
By sliding the squares back and forth from the scrambled state, player should
try to arrange the numbered squares from 1 to 8 starting at the top left.

	
			  +---+---+---+
			  | 1 | 2 | 3 |
			  +---+---+---+
			  | 4 | 5 | 6 |
			  +---+---+---+
			  | 7 | 8 |   |
			  +---+---+---+

		Figure 1: 3x3 sliding puzzle in
		          solved state

I always liked this puzzle. It's not very challenging, just a simple relaxing
game. This was one of my first programming projects while learning C++, so
please be gentle. I found this code recently in the forgotten archive and
started playing with it. It turned out it had many problems and bugs.
After correcting whatever problems I found I decided to share this program with
the github community.

Enjoy!

Marek Karcz 6/10/2019

------------------------------------------------------------------------------

Theory of operation
-------------------

The tree of all permutations is the structure of states with four descendants
each. Each descendant represents move in one direction in the puzzle. For many
states only two moves are valid, for some all four. Invalid moves may be
replaced by the NULL pointer in the branch. Also, when the move is about to
repeat the state that is already generated in one of the previous branches, it
should also be considered illegal.


                               GoalState R0
                          ------------------------						
                          Left   Up   Down   Right
                           /     |     |       \
                          /     NULL   |       NULL
                         /             |
                 State1 LB0            |
           ------------------------    |		
           Left   Up   Down   Right    |
            /    NULL  DB1    NULL     |
           /                       State2 DB2									  
        State LB1           ------------------------						
                            Left   Up   Down   Right
                             /     |     |       \
                            /     NULL   |       NULL
                           /             |
                   State3 LB3            |						 
             ------------------------    |					
             Left   Up   Down   Right    |
                                         |
                                    State4 DB3									
                              ------------------------						
                              Left   Up   Down   Right


		Figure 2: The tree structure for sliding puzzle

Breadth-first Search
--------------------

The approach to solve the puzzle is to generate all possible descendants of
the goal state in the form of a tree. For 3x3 puzzle we have 9! permutations.
Each configuration has exactly 9!/2 solutions.

Data is represented in the linear tables couple:

Index:

0 1  2   3   4   5   6   7   ...

Tree:

X R0 LB0 DB2 LB1 DB1 LB3 DB3 ...  

Parent state index table:

X 0  1   1   2   2   3   3 ...


First row represents the index in the table. The second row represents the 
states of the puzzle. The index #0 is not used. The Parent state index table 
is used to create the sequence of the moves once the initial state is 
generated.

The tree array keeps the values of pointers to the PTree structure:

struct PuzzleTree

{

	Box anBox; // represents the state

	// descendants

	PuzzleTree *pUp;

	PuzzleTree *pDown;

	PuzzleTree *pRight;

	PuzzleTree *pLeft;

	// internal use

	long lID;

};


typedef struct PuzzleTree PTree;

typedef PTree *PTreePtr;


The InitialState R0 is the goal state.

Recursive Heuristic Evaluation Search
-------------------------------------

Another way to find a solution is to sort of "crawl" on the tree of possible
states trying to find a path from scrambled state to goal state. When using
this method, we don't generate all solvable states. Instead we generate states
on the fly by evaluating which next move is the best. The direction of search
is determined by analysis of the next state and calculating it's heuristic
"distance" from the goal state by assigning it the score. After all possible
states from the current state are evaluated this way, the state with the best
score wins and the search goes first into that branch.
This search is recursive. If the search returns from that branch without
finding a solution, then it tries the next branch with the next best score,
and so on until the solution is found or the search is exhausted.

