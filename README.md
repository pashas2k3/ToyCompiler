# ToyCompiler
Toy Compiler implementing a subset of C language.
The program built from the source code here can be used to scan a program
written in the subset of C89 ("the source language") and give MIPS code.
This was done as a part of a course (Course number intentionally not mentioned).

Usage:
    compiler -s type_check [-i <input>] [-o <output>]

USAGE:
 compiler [-i inputFileName] [-o outputFileName]

 default value of
          inputFileName = stdin
          outputFileName = stdout

Test file used and attached with submission:
 src/tokens.txt

Input:
 a. stdin if no input file provided
 b. Input file provided as an argument

Output:
 stdout or outputFile