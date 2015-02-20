/*
 * sudoku.cpp
 * Name: Matt Hoendorf <hoendomd@mail.uc.edu>
 *
 * This program solves a sudoku board, given as a filename on the command line.
 * The text file contains 9 lines of numbers, with a period denoting an unknown
 * number.
 *
 *
 * Dec 2, 2014
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

using namespace std;

// Declare the following functions, to be defined later
struct sudoku_t;
vector<sudoku_t> succ(const sudoku_t &board);
bool solution(const sudoku_t &board);
bool hasDupes(string a);
bool valid(const sudoku_t &board);


// Below are general DFS functions, templated of course!

// State struct to signal back if a solution was found
template <class T>
struct state_t{
  T state;
  bool valid;
};

// Print all boards
template <class T>
void print(const vector<T> boards){
  cout << boards.size() << " variants\n";
  for(unsigned i = 0; i < boards.size(); i++){
    print(boards[i]);
    cout << endl;
  }
  cout << "--------\n";
}

template <class T>
void print(const state_t<T> &state){
  cout << "solution? " << state.valid << endl;
  print(state.state);
}

// The Depth First Search function.  Given a state, this will
// return a solution state, or not if one couldn't be found.
// solution and succ must be defined
template <class T>
state_t<T> dfs(const T &state){
  if( solution(state)){   // Do we have a solution?
    state_t<T> good;    // Pack the solution in a state_t to be send back up
    good.state = state;
    good.valid = true;  // This is how dfs knows it's a solution!
    return good;
  }
  vector<T> options = succ(state);  // Get all possible successor states
  if( options.size() == 0){ // If no successors, return false
    state_t<T> bad;
    bad.state.spots = ""; 
    bad.valid = false;      // Not a solution
    return bad;
  }
  // Here we choose to pick the order arbitrarily.  An alternate method would be
  // to explore the the 'best' one using a heuristic.
  for(unsigned i = 0; i < options.size(); i++){
    // For each successor state, go deeper
    state_t<T> possible = dfs(options[i]);
    if(possible.valid){ // Found a solution deeper down, return it!
      return possible;
    }
  }
  // didn't return yet so no solution was found. :-(
  state_t<T> bad;
  bad.state.spots = "";
  bad.valid = false;
  return bad;
}


// sudoku specific code

// This encodes the state of the board
struct sudoku_t{
  string spots;  
  unsigned next_spot; 
};          


// How to print a board
void print(const sudoku_t &board){
  for(int i = 0; i < board.spots.size(); i++){
    if(i % 9 == 0){
      cout << endl << " ";
    }else if(i % 3 == 0){
      cout << "| ";
    }
    if(i % 27 == 0){
      for(int j = 2; j < 23; j++){
        if(j % 8 == 0){
          cout << "+";
        }else{
          cout << "-";
        }
      }
      cout << endl << " ";
    }
    cout << board.spots[i] << " " ;
  }
  cout << endl << " ";
  for(int j = 2; j < 23; j++){
    if(j % 8 == 0){
      cout << "+";
    }else{
      cout << "-";
    }
  }
  cout << endl << endl;
}

// Set the sent board
void init(sudoku_t &board, string state){
  board.spots = state;
}


// given a board, generate all possible valid successors to it
vector<sudoku_t> succ(const sudoku_t &board){
  vector<sudoku_t> ret;
  // find blank
  unsigned blank = 99;
  for(unsigned i = 0; i < board.spots.size(); i++){
    if(board.spots[i] == '.'){
      blank = i;
      break;
    }
  }
  if(blank == 99){
    return ret;
  }
  // Now create 9 new boards
  for(int i = 1; i <= 9; i++){
    sudoku_t temp = board;
    temp.spots[blank] = '0' + i;
    if( valid(temp)){
      ret.push_back(temp);
    }
  }
  return ret;
}

// Is it a solution board?
bool solution(const sudoku_t &board){
  // A solution will have no '.' characters
  for(int i = 0; i < board.spots.size(); i++){
    if(board.spots[i] == '.'){
      return false;
    }
  }
  // Ok so far, is it valid?  Should always be, but just to be safe
  return valid(board);
}

bool hasDupes(string a){
  int count = 1;
  char test;
  for(int i = 0; i < 9; i++){
    test = count + '0';
    if(a.find(test) != a.rfind(test)){ return true; }
    count++;
  }
  return false;
}

// A valid board is one which does not have any number violations
bool valid(const sudoku_t &board){
  int current = 0;
  string temp = "";
  // Check if rows are valid
  for(int h = 0; h < 9; h++){
    for(int w = 0; w < 9; w++){
      temp.push_back(board.spots[current]);
      current++;
    }
    if(hasDupes(temp) == true){ return false; }
    temp = "";
  }
  // Check if columns are valid
  for(int h = 0; h < 9; h++){
    current = h;
    for(int w = 0; w < 9; w++){
      temp.push_back(board.spots[current]);
      current += 9;
    }
    if(hasDupes(temp) == true){ return false; }
    temp = "";
  }
  // Check if squares are valid
  for(int h = 0; h < 9; h++){   
    if(h >= 3 && h < 6){ current = 3 * h + 18;}
    else if(h >= 6){ current = 3 * h + 36; }
    else{ current = 3 * h; }
    for(int w = 0; w < 3; w++){
      temp.push_back(board.spots[current]);
      temp.push_back(board.spots[current+1]);
      temp.push_back(board.spots[current+2]);
      current += 9;
    }
    if(hasDupes(temp) == true){ return false; }
    temp = "";
  }
  return true;
}

int main(int argc, char* argv[]) {
	// Make sure they entered a file on the command-line.
  if(argc != 2){
    cout << "Enter sudoku board file to solve\n";
    return 0;
  }
	// Open file, read in the strings, one per line, appended together.
  ifstream file;
  file.open(argv[1]);
  if(file.fail()){
  	cout << "Can not open " << argv[1] << ".\n";
		return 0;
  }
  string game = "";
  string line;
  while(file >> line){
    game = game + line;
  }
  file.close();
  
  sudoku_t start;
  init(start,game);
  cout << "Starting board: \n";
  print(start);
  
  // Debug line to help you work on valid
  print(succ(start));
  
  state_t<sudoku_t> solution = dfs(start);
  cout << "Solution found: \n";
  print(solution);
}
