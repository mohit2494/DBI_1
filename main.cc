
#include <iostream>
#include "Record.h"
#include <stdlib.h>
using namespace std;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

int main () {

	// try to parse the CNF
	cout << "Enter in your CNF: ";
	if (yyparse() != 0) {
		cout << "Can't parse your CNF.\n";
		exit (1);
	}

	// suck up the schema from the file
	Schema lineitem ("catalog", "lineitem");

	// grow the CNF expression from the parse tree
	CNF myComparison;
	Record literal;
	myComparison.GrowFromParseTree (final, &lineitem, literal);

	// print out the comparison to the screen
	myComparison.Print ();

	// now open up the text file and start procesing it
	FILE *tableFile = fopen ("/home/mk/Documents/uf docs/sem 2/Database Implementation/git/tpch-dbgen/lineitem.tbl", "r");

	Record temp;
	Schema mySchema ("catalog", "lineitem");

	//char *bits = literal.GetBits ();
	//cout << " numbytes in rec " << ((int *) bits)[0] << endl;
	//literal.Print (&supplier);

	// *** Step 1 of assignment ***

	// After you have compiled and tried out main.cc, I would advise you to play with main.cc
	// a bit before you really get started on this assignment, to familiarize yourself with the
 	// code base that you will start from. Try to alter main.cc so that it loads a page up full
	// of records, and then writes that page to disk. Then try to retrieve the page from disk and
	// print the records to the screen. Once you feel fairly comfortable with the code base and 
	// how to use it, then you are ready to implement assignment one. 

	// Step 1. Load a page up full of records
	Page tempPage;

	bool pageFull = false;
	// Step 2. Check while appending whether the page is full or not
	while(temp.SuckNextRecord(&mySchema, tableFile)==1 && (!pageFull)) {
		
		// Step 2. Check while appending whether the page is full or not
		if(!tempPage.Append(&temp))
			pageFull = true;
	}
	
	cout << "Page Full" << pageFull << endl;

	// using this file for 
	File tempFile;
	off_t whichPage = 0;

	// Step 3. Write the page to disk
	tempFile.AddPage(&tempPage,whichPage);

	whichPage = 0;

	// Step 4. Retrieve page from disk
	tempFile.GetPage(&tempPage, whichPage);


	/*** step 1 of assignment complete ***/


	// read in all of the records from the text file and see if they match
	// the CNF expression that was typed in
	int counter = 0;
	ComparisonEngine comp;
	while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
		counter++;
		if (counter % 10000 == 0) {
			cerr << counter << "\n";
		}

		if (comp.Compare (&temp, &literal, &myComparison))
					temp.Print (&mySchema);
	}
}