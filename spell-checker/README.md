# Spell Checker

## About
This program takes a user-inputted word and determines whether it has been spelt correctly. If it has not been spelt correctly, the program suggests the five closest words to it. Words in a dictionary (in the file dictionary.txt) are placed into a hash map so that they can be found efficiently. The program also implements Levenstein's algorithm to find the five closest words to a misspelt word.
## Operating Instructions
A makefile is included to compile the program. You can simply input the command:  
make spellChecker  

To execute the program, use the following command:  
./spellChecker  
## Credits and Acknowledgements
This program was completed as a homework assignment for CS 261, Data Structures, at Oregon State University during winter term of 2020. 
