/*
 * CS 325 Analysis of Algorithms
 * Oregon State University Spring 2020
 * Homework 3 - shopping
 * Name: Gabrielle Josephson
 * Date: 4/19/2020
 */

#include <iostream>
#include <fstream>
#include <istream>

using namespace std;

struct node {
	int val;
	node* next;
};

bool openFile(ifstream&);
void getMax(int[], int[], int[], int, int);
void deleteLL(node**);

/**
 * Opens an input file with criteria for shopping competition. Calculates
 * the max profit and required items for each family member within the 
 * weight contraints. Results are printed to the console as well as
 * written to an output file
 * named results.txt.
 */
int main()
{
	ifstream inFile;
	ofstream outFile;

	int itemNum = 0;
	int famSize = 0;
	int maxMoney = 0;
	int testCase = 0;

	int* family = NULL;
	int* price = NULL;
	int* total = NULL;
	int* weight = NULL;

	node **famInfo = NULL;

	if (!openFile(inFile)) {										// Open input file named 'shopping.txt'
		cout << "File not found. Program terminating." << endl;
		return -1;
	}

	outFile.open("results.txt");

	// Parse input file data into variables

	inFile >> testCase;

	for (int caseNum = 0; caseNum < testCase; caseNum++) {
		inFile >> itemNum;

		price = new int[itemNum + 1];				// Make price and weight schedules
		weight = new int[itemNum + 1];
		price[0] = 0;
		weight[0] = 0;

		for (int i = 1; i <= itemNum; i++) {
			inFile >> price[i];
			inFile >> weight[i];
		}

		inFile >> famSize;
		family = new int[famSize];

		total = new int[itemNum + 1];

		for (int j = 0; j < famSize; j++) {			// Make family weight constraint schedule
			inFile >> family[j];
		}

		famInfo = new node*[famSize];			// Array of linked lists stores included items for each family member
		for (int i = 0; i < famSize; i++) {
			famInfo[i] = new node;
			famInfo[i]->val = 0;
			famInfo[i]->next = NULL;
		}

		node* cur;

		for (int j = 0; j < famSize; j++) {
			getMax(price, weight, total, family[j], itemNum);	// Calculates value and items for one family member
			int info = 0;

			cur = famInfo[j];
			for (int k = 0; k <= itemNum; k++) {		// Loop to add each members profit to total profit
				if (total[k] == 1) {
					maxMoney += price[k];

					if (cur == famInfo[j] && cur->val == 0) {	// Store each family member's items in linked list
						cur->val = k;
					}
					else {
						cur->next = new node;
						cur = cur->next;
						cur->val = k;
						cur->next = NULL;
					}

					info++;
					total[k] = 0;
				}
			}
		}

		// Output answers to both console and output file

		cout << "Test Case " << caseNum + 1 << endl;
		outFile << "Test Case " << caseNum + 1 << endl;

		cout << "Total Price " << maxMoney << endl;
		outFile << "Total Price " << maxMoney << endl;

		cout << "Member Items" << endl;
		outFile << "Member Items" << endl;

		for (int i = 0; i < famSize; i++) {
			cout << "\t" << i + 1 << ": ";
			outFile << "\t" << i + 1 << ": ";
			cur = famInfo[i];
			while (cur != NULL) {				// Traverse through linked list to output member items
				cout << cur->val << " ";
				outFile << cur->val << " ";
				cur = cur->next;
			}
			cout << endl;
			outFile << endl;
		}

		// Reset variables and deallocate memory

		maxMoney = 0;

		for (int i = 0; i < famSize; i++) {
			deleteLL(&famInfo[i]);
		}
		delete[] famInfo;
		delete[] price;
		delete[] weight;
		delete[] family;
		delete[] total;
	}
	inFile.close();
	outFile.close();
}

/**
 * Opens a file (shopping.txt) containing data to run the program. Returns 'true'
 * if file is successfully opened and 'false' if file fails to open.
 * @param: inFile
 * @return: true if file opens, false if it fails to open
 */
bool openFile(ifstream& inFile) {
	inFile.open("shopping.txt");
	if (!inFile) {
		return false;
	}
	return true;
}

/**
 * Calculates the max profit to be made from a set of items with
 * a price scheudle p[] and weight schedule w[]. The price an items
 * included are coded into a 0/1 array t[], where item i is included
 * if the corresponding array index is 1.
 * @param: p[], w[], t[], f, item
 * @return: Modifies the array t[] by reference to be used in the main function
 */
void getMax(int p[], int w[], int t[], int f, int item) {
	int** maxP = new int*[item + 1];
	int m;
	int n;

	for (int i = 0; i <= item; i++) {
		maxP[i] = new int[f + 1];
	}

	for (int i = 0; i <= f; i++) {
		maxP[0][i] = 0;
	}

	for (int j = 1; j <= item; j++) {				// Consider item j
		maxP[j][0] = 0;
		for (int k = 1; k <= f; k++) {				// Consider item j with weight limit k
			if (w[j] <= k) {
				if (p[j] + maxP[j - 1][k - w[j]] > maxP[j - 1][k]) {		// If the item is within the weight contraints and it increases profit, calculate it into the profit
					maxP[j][k] = p[j] + maxP[j - 1][k - w[j]];
				}
				else {
					maxP[j][k] = maxP[j - 1][k];
				}
			}
			else {
				maxP[j][k] = maxP[j - 1][k];
			}
		}
	}

	m = item;
	n = f;

	while (maxP[m][n] != 0) {					// Loop iterates matrix to determine which items were included
		if (maxP[m - 1][n] >= maxP[m][n]) {
			m--;
		}
		else {
			t[m] = 1;							// If element is uppermost of that value, the item of that row is included
			n = n - w[m];
			m--;
		}
	}

	for (int i = 0; i < item + 1; i++) {
		delete [] maxP[i];
	}
	delete[] maxP;

}

void deleteLL(node** head) {
	node* cur;
	node* next;

	cur = *head;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}

	*head = NULL;
}
