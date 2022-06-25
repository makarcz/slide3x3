#!/usr/bin/env perl
# 3x3 Slide Puzzle Solver.
# Marek Karcz 2022.

use strict;
use warnings;

my $DBG = 0;
my $DBG1 = 0;
my $DBG2 = 0;
my ($root) = "12345678_"; # state of solved puzzle (goal)
my $scrambled;  # scrambled puzzle, state to be solved
my $empty_space = '_';  # empty space placeholder
my %seen = ();   # hash of visited states
my @moves = ();  # array of good moves, in reverse order, used by DFS

sub InitVars
{
   %seen = ();
   @moves = ();
}

# If shift left is possible, execute it, return resulting state.
# Otherwise, return empty string.
# Takes current puzzle state as argument.

sub ShiftLeft
{
   
   my ($state) = @_;
   my $ret = "";
   my $opening_idx = index($state, $empty_space);
   if ($opening_idx == 2 || $opening_idx == 5 || $opening_idx == 8) {
      # shift left not possible
      return $ret;
   }
   my $char_on_right = substr($state, $opening_idx+1, 1);
   my ($begin, $end) = (0, 9);
   for (my $i = $begin; $i < $end; $i++) {
      if (substr($state, $i, 1) eq $empty_space) {
         $ret = $ret . $char_on_right;
      } elsif ($i == $opening_idx+1) {
         $ret = $ret . $empty_space;
      } else {
         $ret = $ret . substr($state, $i, 1);
      }
   }
   return $ret;
}

# If shift up is possible, execute it, return resulting state.
# Otherwise, return empty string.
# Takes current puzzle state as argument.

sub ShiftUp
{

   my ($state) = @_;
   my $ret = "";
   my $opening_idx = index($state, $empty_space);
   if ($opening_idx > 5) {
      # shift up not possible
      return $ret;
   }
   my $char_below = substr($state, $opening_idx+3, 1);
   my ($begin, $end) = (0, 9);
   for (my $i = $begin; $i < $end; $i++) {
      if (substr($state, $i, 1) eq $empty_space) {
         $ret = $ret . $char_below;
      } elsif ($i == $opening_idx+3) {
         $ret = $ret . $empty_space;
      } else {
         $ret = $ret . substr($state, $i, 1);
      }
   }
   return $ret;
}

# If shift right is possible, execute it, return resulting state.
# Otherwise, return empty string.
# Takes current puzzle state as argument.

sub ShiftRight
{

   my ($state) = @_;
   my $ret = "";
   my $opening_idx = index($state, $empty_space);
   if ($opening_idx == 0 || $opening_idx == 3 || $opening_idx == 6) {
      # shift right not possible
      return $ret;
   }
   my $char_on_left = substr($state, $opening_idx-1, 1);
   my ($begin, $end) = (0, 9);
   for (my $i = $begin; $i < $end; $i++) {
      if (substr($state, $i, 1) eq $empty_space) {
         $ret = $ret . $char_on_left;
      } elsif ($i == $opening_idx-1) {
         $ret = $ret . $empty_space;
      } else {
         $ret = $ret . substr($state, $i, 1);
      }
   }
   return $ret;
}

# If shift down is possible, execute it, return resulting state.
# Otherwise, return empty string.
# Takes current puzzle state as argument.

sub ShiftDown
{
   my ($state) = @_;
   print "DBG: ShiftDown($state)\n" if ($DBG);
   my $ret = "";
   my $opening_idx = index($state, $empty_space);
   if ($opening_idx < 3) {
      # shift down not possible
      print "DBG: ShiftDown Not Possible\n" if ($DBG);
      return $ret;
   }
   my $char_above = substr($state, $opening_idx-3, 1);

   print "DBG: opening_idx = $opening_idx\n" if ($DBG);
   print "DBG: char_above = $char_above\n" if ($DBG);

   my ($begin, $end) = (0, 9);
   for (my $i = $begin; $i < $end; $i++) {
      if (substr($state, $i, 1) eq $empty_space) {
         $ret = $ret . $char_above;
      } elsif ($i == $opening_idx-3) {
         $ret = $ret . $empty_space;
      } else {
         $ret = $ret . substr($state, $i, 1);
      }
   }
   print "DBG: ShiftDown, ret = $ret\n" if ($DBG);
   return $ret;
}

# Display puzzle state in 3x3 format.
# Takes single line string state representation as argument.

sub DispState
{
   my ($state) = @_;
   for(my $i = 0; $i < 9; $i++) {
      print "\n" if ($i > 0 && $i%3 == 0);
      print substr($state, $i, 1) . " ";
   }
}

# Check if 1st argument (string) begins with 2nd argument (string).
# Return true if true, otherwise false.

sub BeginsWith
{
   my ($str, $subs) = @_;
   return substr($str, 0, length($subs)) eq $subs; 
}

# Show list of moves leading from scrambled state to goal / root state.

sub ShowMoves
{
   my @reversed = reverse @moves;
   my $solution_length = 0;
   for (my $i = 0; $i < scalar(@reversed); $i++) {
      if (BeginsWith($reversed[$i], "Shift")) {
         print $reversed[$i] . "\n";
         $solution_length++;
      } else {
         DispState($reversed[$i]);
         print "\n-----\n";
      }
   }
   print "Solved in $solution_length moves.\n";
}

# Solve the 3x3 slide puzzle using breadth-first search (BFS).
# Iterative.
# Takes the scrambled state as argument.

sub Slide3x3BFSIter
{
   my ($scrambled) = @_;
   my $begin = $root; # generate all states from root / solved state
   $seen{$begin} = $begin; # hash holding all visited states
   my @all_puzzle_states = (); # flat (1D) array of valid puzzle states
   my @all_puzzle_index = (); # index for backtracking the steps
   my $idx = -1; # index of the parent of scrambled state

   print "DBG2: Slide3x3($begin)\n" if ($DBG2);
   push(@all_puzzle_states, $begin);
   push(@all_puzzle_index, 0);

   for (my ($i, $state) = (0, $begin);
        $i < 181440; # 9!/2
        $i++, $state = $all_puzzle_states[$i]) {

      print "DBG2: Slide3x3, state = $state\n" if ($DBG2);
      $idx = -1;

      my ($left_shift)     = ShiftLeft($state);
      my ($up_shift)       = ShiftUp($state);
      my ($right_shift)    = ShiftRight($state);
      my ($down_shift)     = ShiftDown($state);

      if ($left_shift ne "" and not defined($seen{$left_shift})) {

         $seen{$left_shift} = $left_shift;
         push(@all_puzzle_states, $left_shift);
         push(@all_puzzle_index, $i);

         if ($left_shift eq $scrambled) {
            $idx = $i;
            last;
         }
      }

      if ($up_shift ne "" and not defined($seen{$up_shift})) {

         $seen{$up_shift} = $up_shift;
         push(@all_puzzle_states, $up_shift);
         push(@all_puzzle_index , $i);

         if ($up_shift eq $scrambled) {
            $idx = $i;
            last;
         }
      }

      if ($right_shift ne "" and not defined($seen{$right_shift})) {

         $seen{$right_shift} = $right_shift;
         push(@all_puzzle_states, $right_shift);
         push(@all_puzzle_index, $i);

         if ($right_shift eq $scrambled) {
            $idx = $i;
            last;
         }
      }

      if ($down_shift ne "" and not defined($seen{$down_shift})) {

         $seen{$down_shift} = $down_shift;
         push(@all_puzzle_states, $down_shift);
         push(@all_puzzle_index, $i);

         if ($down_shift eq $scrambled) {
            $idx = $i;
            last;
         }
      }
   }
   # Show the moves leading from scrambled to goal / root state.
   # Backtrack using index array.

   my $solution_length = 1;
   print "Scrambled puzzle:\n";
   DispState($scrambled);
   print "\n";
   print "Goal state:\n";
   DispState($root);
   print "\n";
   print "Searched " . scalar(@all_puzzle_states) . " states.\n";
   if ($idx >= 0) {

      print "Solved!\n";
      print "-----\n";
      for (my $i = $idx; $i > 0; ) {
         DispState($all_puzzle_states[$i]);
         print "\n-----\n";
         $i = $all_puzzle_index[$i];
         $solution_length++;
      }
      DispState($root);
      print "\n-----\n";
      print "Solved in $solution_length moves.\n";

   } else {

      print "Sorry! No solution was found for this puzzle configuration.\n";
   }
}

# Solve the 3x3 slide puzzle using depth-first search (DFS).
# Recursive.
# Takes the state, depth and depth limit as arguments.
# Use within iterative deepening loop to find solution.

sub Slide3x3DFSRecursive
{
   my ($state, $depth, $dlimit) = @_;

   print "DBG2: Slide3x3($state, $depth)\n" if ($DBG2);

   if ($state eq "") {
      print "DBG2: Slide3x3, NULL state.\n" if ($DBG2);
      return "";
   }

   if ($depth > $dlimit) {
      print "DBG2: Slide3x3, depth exceeded.\n" if ($DBG2);
      return "";
   }

   $seen{$state} = $state; # hash holding all visited states
   push(@moves, $state); # push puzzle state to stack

   my ($left_shift)     = ShiftLeft($state);
   my ($up_shift)       = ShiftUp($state);
   my ($right_shift)    = ShiftRight($state);
   my ($down_shift)     = ShiftDown($state);

   print "DBG2: Slide3x3, left_shift  = $left_shift\n" if ($DBG2);
   print "DBG2: Slide3x3, up_shift    = $up_shift\n" if ($DBG2);
   print "DBG2: Slide3x3, right_shift = $right_shift\n" if ($DBG2);
   print "DBG2: Slide3x3, down_shift  = $down_shift\n" if ($DBG2);

   # rinse and repeat for all legal moves
   #
   if ($left_shift ne "" and not defined($seen{$left_shift})) {

      $seen{$left_shift} = $left_shift;

      push(@moves, "Shift RIGHT"); # push move to stack

      if ($left_shift eq $scrambled) {
         return $scrambled;
      } else {
         $state = Slide3x3DFSRecursive($left_shift, $depth+1, $dlimit);
         if ($state eq $scrambled) { return $state; }
      }
      pop(@moves); # that didn't work, pop the move from stack
   }

   if ($up_shift ne "" and not defined($seen{$up_shift})) {

      $seen{$up_shift} = $up_shift;

      push(@moves, "Shift DOWN");

      if ($up_shift eq $scrambled) {
         return $scrambled;
      } else {
         $state = Slide3x3DFSRecursive($up_shift, $depth+1, $dlimit);
         if ($state eq $scrambled) { return $state; }
      }
      pop(@moves);
   }

   if ($right_shift ne "" and not defined($seen{$right_shift})) {

      $seen{$right_shift} = $right_shift;

      push(@moves, "Shift LEFT");

      if ($right_shift eq $scrambled) {
         return $scrambled;
      } else {
         $state = Slide3x3DFSRecursive($right_shift, $depth+1, $dlimit);
         if ($state eq $scrambled) { return $state; }
      }
      pop(@moves);
   }

   if ($down_shift ne "" and not defined($seen{$down_shift})) {

      $seen{$down_shift} = $down_shift;

      push(@moves, "Shift UP");

      if ($down_shift eq $scrambled) {
         return $scrambled;
      } else {
         $state = Slide3x3DFSRecursive($down_shift, $depth+1, $dlimit);
         if ($state eq $scrambled) { return $state; }
      }
      pop(@moves);
   }

   pop(@moves); # that didn't work, pop the state from stack

   return "";
}

# Test DFS solutions with different descent depths, find the shortest
# solution path and present it.
# Takes test_from_depth and descent_limit as arguments.
# (Iterative deepening)

sub DFSIterativeDeepening
{
   my ($test_from_depth, $descent_limit) = @_;
   my $best_solution_len = 1000; # initialize with some arbitrary large number
   my $best_sol_depth = $descent_limit;
   my $solution_length = 0;
   for (my $i = $test_from_depth; $i < $descent_limit; $i++) {
      print "Depth $i ... ";
      InitVars;
      Slide3x3DFSRecursive($root, 0, $i); # start from solved state, make way to the scrambled
      $solution_length = scalar(@moves) / 2;
      if (0 < $solution_length && $solution_length < $best_solution_len) {
         $best_solution_len = $solution_length; # found shorter solution than before
         $best_sol_depth = $i; # remember the depth of best solution so far
      }
      print "solution steps: $solution_length\n";
   }
   # Present the best (shortest) found solution
   InitVars;
   $descent_limit = $best_sol_depth;
   print "DBG2: Main, Testing complete, descent_limit = $descent_limit\n" if ($DBG2);
   Slide3x3DFSRecursive($root, 0, $descent_limit); # from solved ($root) to scrambled
   $solution_length = scalar(@moves) / 2; # @moves has states and moves, so length / 2
   print "DBG2: Main, solution_length = $solution_length\n" if ($DBG2);
   if ($solution_length > 0) {
      print "Solved!\n";
      ShowMoves;
   } else {
      print "Sorry! No solution was found. Try adjusting depth-limit or adding\n";
      print "test-from-depth argument. Alternatively, use BFS method to solve this puzzle.\n";
   }
}

# Depth-First-Search, main sub

sub MainDFS
{
   my $test_from_depth = 10;
   my $descent_limit = 97;
   ($scrambled, $descent_limit, $test_from_depth) = @_;
   die "ERROR: Provide scrambled puzzle state!\n" if (not defined($scrambled));
   $scrambled ne $root or die "Nothing to solve, scramble the puzzle first!";
   if (not defined($descent_limit) or $descent_limit <= 0) {
      $descent_limit = 97;
   }
   if (not defined($test_from_depth) or $test_from_depth >= $descent_limit) {
      $test_from_depth = $descent_limit;
   }
   print "3x3 Slide Puzzle Solver, Recursive DFS with iterative deepening method.\n";
   print "Depth limit: $descent_limit\n";
   print "Test solution from depth: $test_from_depth\n";
   print "Looking to solve: $scrambled, OR\n";
   DispState($scrambled);
   print "\n";
   print "Solution:\n";
   DispState($root);
   print "\n";
   DFSIterativeDeepening($test_from_depth, $descent_limit);
}

# Scramble puzzle from the root / solved state by application of random
# amount of valid puzzle moves / steps between 0 and 181439. (9!/2 - 1)
# Takes the number of scrambled states as argument. (if not provided,
# defaults to 1)
# Return $number of scrambled states.

sub ScramblePuzzle
{
   my ($number) = @_;
   my $begin = $root;
   my $rnd = rand(181440); # 9!/2
   my @all_puzzle_states = ();
   my @ret = ();

   if (not defined($number) or $number <= 0) {

      $number = 1;
   }
   $seen{$begin} = $begin;
   push(@all_puzzle_states, $begin);

   # Produce all valid scrambled states
   
   for (my ($i, $state) = (0, $begin);
        $i < 181440;
        $i++, $state = $all_puzzle_states[$i]) {

      my ($left_shift)     = ShiftLeft($state);
      my ($up_shift)       = ShiftUp($state);
      my ($right_shift)    = ShiftRight($state);
      my ($down_shift)     = ShiftDown($state);

      if ($left_shift ne "" and not defined($seen{$left_shift})) {

         $seen{$left_shift} = $left_shift;
         push(@all_puzzle_states, $left_shift);
      }

      if ($up_shift ne "" and not defined($seen{$up_shift})) {

         $seen{$up_shift} = $up_shift;
         push(@all_puzzle_states, $up_shift);
      }

      if ($right_shift ne "" and not defined($seen{$right_shift})) {

         $seen{$right_shift} = $right_shift;
         push(@all_puzzle_states, $right_shift);
      }

      if ($down_shift ne "" and not defined($seen{$down_shift})) {

         $seen{$down_shift} = $down_shift;
         push(@all_puzzle_states, $down_shift);
      }
   }

   # Generate array with $number of random scrambled states
   
   for (my $i = 0; $i < $number; $i++) {

      $rnd = rand(181440);
      push(@ret, $all_puzzle_states[$rnd]);
   }

   return @ret;
}

# Breadth-First-Search, main sub

sub MainBFS
{
   my $number = 1;
   ($scrambled, $number) = @_;
   $scrambled ne $root or die "Nothing to solve, scramble the puzzle first!";
   if ($scrambled eq "-s") {

      for my $state (ScramblePuzzle($number)) {

         print $state . "\n";
      }

   } elsif ($scrambled eq "-d") {

      for my $state (ScramblePuzzle($number)) {

         print DispState($state);
         print "\n-----\n"
      }

   } else {

      print "3x3 Slide Puzzle Solver, Iterative BFS method.\n";
      InitVars;
      Slide3x3BFSIter($scrambled);
   }
}

sub ShowHelp
{
   print "3x3 Slide Puzzle Solver.\n";
   print "Root (solved) state:\n";
   DispState($root);
   print "\n";
   print "Usage:\n";
   print "   $0 [[-s|d [number]] | state [depth-limit [test-from-depth]]]\n";
   print "Where:\n";
   print "   -s - scramble puzzle, display state in single line string format,\n";
   print "   -d - scramble puzzle, display state in 3x3 format,\n";
   print "   number - number of scrambled states to produce, default = 1,\n";
   print "   state - state of the puzzle, in the form of a single line string - 9\n";
   print "           characters long, that consists of numbers 1..8 that represent\n";
   print "           tiles  and a single underscore symbol '_', which represents\n";
   print "           an empty space.\n";
   print "           To see example of a state, run '$0 -s'.\n";
   print "   depth-limit - depth limit for recursive DFS,\n";
   print "   test-from-depth - perform DFS for different depths, starting with\n";
   print "                     test-from-depth and ending at depth-limit\n";
   print "                     (iterative deepening)";
   print "\n";
   print "NOTE: * If no depth-limit, test-from-depth are provided (state only) as\n";
   print "        arguments, BFS search is performed, otherwise DFS is used.\n";
   print "\n";
   print "      * Not any combination of numbers and empty space are valid puzzle\n";
   print "        states. Puzzle must be scrambled from the root state (a.k.a. solved\n";
   print "        state) by application of valid moves to produce a valid scrambled\n";
   print "        combination.\n";
   print "\n";
}

if (not scalar(@ARGV)) {

   ShowHelp;
   exit;

} elsif (scalar(@ARGV) > 1) {

   if ($ARGV[0] ne "-s" and $ARGV[0] ne "-d") {

      MainDFS @ARGV;

   } else {

      MainBFS @ARGV;
   }
} else {

   MainBFS @ARGV;
}

