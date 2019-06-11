echo off
rem puzzle3x3 -tbegin 8 6 7 2 5 4 3 0 1 -tend -depth 34 -moves 80000 > puzzle2.trace
rem puzzle3x3 -tbegin 8 6 7 2 5 4 3 0 1 -tend > puzzle2.trace
rem notepad puzzle2.trace
puzzle3x3 -tbegin 8 6 7 2 5 4 3 0 1 -tend
pause prompt
