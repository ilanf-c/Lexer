# Lexer
Regular expression -> NFA -> DFA -> minDFA -> lexer program(cpp) 
# Regular Expression to DFA Converter

## Overview

This project is a C++ implementation of a regular expression to DFA (Deterministic Finite Automaton) converter. It takes a regular expression as input and generates a DFA that recognizes the same language.

## Requirements

* C++ compiler (e.g. GCC)
* `iostream`, `vector`, `unordered_map`, `stack`, and `algorithm` libraries

## Usage

1. Compile the code using `g++`: `g++ lab2.cpp -o lab2`
2. Run the program: `./lab2`
3. Enter a regular expression when prompted
4. The program will generate a DFA and output it in DOT language format
5. Use a tool like Graphviz to visualize the DFA

## Features

* Supports basic regular expression syntax (e.g. `*`, `+`, `?`, `|`, `(`, `)`)
* Generates a DFA that recognizes the same language as the input regular expression
* Outputs the DFA in DOT language format for visualization

## Notes

* This implementation uses a recursive descent parser to parse the regular expression
* The DFA is generated using the powerset construction algorithm
* The output DFA is in DOT language format, which can be visualized using tools like Graphviz
* This project is for educational purposes only and may not be suitable for production use

## Files

* `lab2.cpp`: The main implementation file
* `test.txt`: A sample input file containing regular expressions
* `nfa.dot`, `dfa.dot`, `mindfa.dot`: Sample output files containing the generated DFA in DOT language format

## Author

ilanf willow
