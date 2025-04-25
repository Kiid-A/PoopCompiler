#!/bin/bash
flex -o lex.cpp lex.l
bison -d -o yacc.cpp yacc.y