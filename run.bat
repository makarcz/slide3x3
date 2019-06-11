echo off
puzzle3x3 -tbegin 4 8 1 0 6 3 2 7 5 -tend -depth 100 -moves 50000 > puzzle2.trace
notepad puzzle2.trace
pause prompt
