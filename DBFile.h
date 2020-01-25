#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"

typedef enum {heap, sorted, tree} fType;

// stub DBFile header..replace it with your own DBFile.h 

class DBFile {

public:
	DBFile (); 

	int Create (const char *fpath, fType file_type, void *startup);
	int Open (const char *fpath);
	int Close ();

	void Load (Schema &myschema, const char *loadpath);

	/**
		In order to add records to the file, 
		the function Add is used. In the case of 
		the unordered heap file that you are implementing 
		in this assignment, this function simply adds the 
		new record to the end of the file
	**/
	void MoveFirst ();




	/**
		Note that this function should actually consume addMe, 
		so that after addMe has been put into the file, it cannot
		be used again. There are then two functions that allow 
		for record retrieval from a DBFile instance; all are 
		called GetNext. 
	**/ 
	void Add (Record &addme);

	/**
		The first version of GetNext simply gets
		the next record from the file and returns it to the user,
		where “next” is defined to be relative to the current 
		location of the pointer.After the function call returns,
		the pointer into the file is incremented, so a subsequent
		call to GetNext won’t return the same record twice. The
		return value is an integer whose value is zero if and 
		only if there is not a valid record returned from the 
		function call (which will be the case, for example, if 
		the last record in the file has already been returned).
	**/
	int GetNext (Record &fetchme);

	/**
		The next version of GetNext also accepts a selection predicate
		(this is a conjunctive normal form expression). It returns the
		next record in the file that is accepted by the selection predicate.
		The literal record is used to check the selection predicate, and is
		created when the parse tree for the CNF is processed.
	**/
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

	/** 
		Next, there is a function that is used to actually create the file,
		called Create. The first parameter to this function is a text string
		that tells you where the binary data is physically to be located – 
		you should store the actual database data using the File class from 
		File.h. If you need to store any meta-data so that you can open it 
		up again in the future (such as the type of the file when you re-open it)
		as I indicated above, you can store this in an associated text file – just
		take name and append some extension to it, such as .header, and write your
		meta-data to that file.

		The second parameter to the Create function tells you the type of the file.
		In DBFile.h, you should define an enumeration called myType with three possible
		values: heap, sorted, and tree. When the DBFile is created, one of these tree
		values is passed to Create to tell the file what type it will be. In this 
		assignment, you obviously only have to deal with the heap case. Finally, the
		last parameter to Create is a dummy parameter that you won’t use for this assignment,
		but you will use for assignment two. The return value from Create is a 1 on success
		and a zero on failure. 
	**/
	int Create (char *name, fType myType, void *startup); 

	/**
		Next, we have Open. This function assumes that the DBFile already exists and has previously
		been created and then closed. The one parameter to this function is simply the physical location
		of the file. If your DBFile needs to know anything else about itself, it should have written this
		to an auxiliary text file that it will also open at startup. The return value is a 1 on success
		and a zero on failure. 
	**/
	int Open(char *name);

	/**
		Next, Close simply closes the file. The return value is a 1 on success and a zero on failure.
	**/ 
	int Close();

	/**
		Finally, the Load function bulk loads the DBFile instance from a text file, appending new data
		to it using the SuckNextRecord function from Record.h. The character string passed to Load is 
		the name of the data file to bulk load. 
	**/
	void Load (Schema &mySchema, char *loadMe); 
};
#endif
