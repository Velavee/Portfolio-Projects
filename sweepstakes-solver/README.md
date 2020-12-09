
# Shopping Spree Sweepstakes
## About
This program considers families who have won a contest to take whatever items they want from a store. Each family member has a limit for how much weight he or she can carry, and each family member may only take one of each item. Each item has a designated price and weight. The objective of the program is to calculate which items each family member should take in order to maximize the value of items taken for the entire family. The program can solve the problem for multiple families. The implentation uses dynamic programming.
## Input
The program reads numerical data from a text file. The format is:

The number of cases (i.e. the number of families) (1 <= N <= 100)
The number of objects that can be taken from the store (1 <= N <= 100)
N number of lines, each containing two values: P and W. P (1 <= P <= 5000) is the price of the item and W (1 <= W <= 100) is th weight of the item
The number of people in the family
F number of lines of value M (1 <= M <= 200) that correspond with the maximum weight that can be carried by the ith person in the family.
### Sample Input
1  
3  
72 17  
44 23  
31 24  
1  
26  

In this example, there is one test case. Three items can be taken from the store. Item 1 has a price of 72 and a weight of 17. Item 2 has a price of 44 and a weight of 23. Item 3 has a price of 31 and a weight of 24. There is one person in the family who can carray a maximum weight of 26.
## Operating Instructions
To compile the program, use the following command:

g++ -Wall -std=c++0x shopping.cpp -o shopping

The program can then be executed with the command:
./shopping

The program will prompt you for the location of a text file. The text file must be in the format outlined in the section 'Sample Input' above. A sample file, shopping.txt, is included for testing.
## Credits and Acknowledgements
This program was completed as a homework assignment for CS 325, Analysis of Algorithms, at Oregon State University during spring term 2020.
