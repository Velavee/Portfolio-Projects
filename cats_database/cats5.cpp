/*
 * Title: Cat Database - Main File
 * Based on: Portland Community College CS 162 Computer Science II, Assignment 5
 * Author: Gabrielle Josephson
 * Date: 8/7/2020
 * Sources: C++ Programming by D.S. Malik
 *
 * Program Description:
 * This program enables a user to create, maintain, and modify a database of cat breeds.
 * The program can load a pre-existing file of data. For the formatting of this file, each data entry must
 * be on one line, and each data field must be separated by a comma (breed,characteristics,group,weight
 * ,height,country). For example:
 *
 * persian,calm,long-hair,10,11,Iran
 * siamese,intelligent,short-hair,9,Thailand
 *
 * Features include adding a new breed to the database, searching for a breed, deleting a breed, and
 * printing the contents of the database to the console. Upon the user terminating the program, the data
 * is saved the the same file that was initially loaded. The output file is in the same format described above.
 */

#include "cats5Header.h"

using namespace std;

int main()
{
	int count;
	bool found;
	bool success;
	char answer;
	char deleteEntry[MAX_STR];
	char searchEntry[MAX_STR];

	CatTypes cat;

	cout << "Welcome to the cat breed information program." << endl;

	count = cat.loadBreeds();

	if (count > 0) {			// If there is data in the file, print number of entries and print contents to console.
		cout << "Successfully loaded " << count << " cat breeds." << endl;
		cat.listBreeds();
	}
	else {						// Inform the user if the file is empty.
		cout << "\nThis file is empty. You can add entries by selecting 'Add' in the menu." << endl;
	}

	do {	// Loop to select features
		cout << "\nWhat would you like to do? (S)earch for a breed, (A)dd a breed, (L)ist all breeds, (R)emove a breed, or (Q)uit? ";
		cin >> answer;

		if (answer == 'A' || answer == 'a') {		// Add a breed
			count = cat.addBreed();
		}
		else if (answer == 'L' || answer == 'l') {	// List breeds
			cat.listBreeds();
		}
		else if (answer == 'R' || answer == 'r') {	// Remove a breed
			cin.clear();
			cin.ignore(MAX_STR, '\n');

			cout << "What is the name of the breed whose entry you would like to delete? ";
			cin.getline(deleteEntry, MAX_STR);
			toLower(deleteEntry);

			success = cat.removeBreed(deleteEntry);
			if (success == true) {
				cout << deleteEntry << " was successfully deleted from the database." << endl;
			}
			else {
				cout << deleteEntry << " was not found. No changes were made." << endl;
			}
		}
		else if (answer == 'S' || answer == 's') {	// Search for a breed
			cin.clear();
			cin.ignore(MAX_STR, '\n');

			cout << "What cat breed would you like to search for? ";
			cin.getline(searchEntry, MAX_STR);
			toLower(searchEntry);

			found = cat.searchBreeds(searchEntry);
			if (found == false) {
				cout << "\n" << searchEntry << " was not found in the database." << endl;
			}
		}
		else if (answer == 'Q' || answer == 'q') {	// Quit program
			cat.writeBreeds();
			cout << "\nData has been written to "  << cat.getFileName() << ". Terminating program" << endl;
		}
		else {
			cout << "Invalid input. Please try again." << endl;
		}

	} while (answer != 'Q' && answer != 'q');

}