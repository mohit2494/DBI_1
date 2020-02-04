#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include <string>

typedef enum {heap, sorted, tree} fType;
typedef enum {READ, WRITE,IDLE} BufferMode;

// class to take care of meta data for each table
class Preference{
public:

	// Variable to store the state in which the page buffer is present;
	// Possible values : WRITE,READ and IDLE(default state when the file is just created )
	BufferMode pageBufferMode;

	// Variable to track Current Page from which we need to READ or WRITE to with the file. May have different value according to the mode.
	off_t currentPage;

	// Variable to trac the Current record during the READ and WRITE to the file.
	int currentRecordPosition;

	// Boolean to check if the page isFull or not.
	bool isPageFull;

	//  Variable to store the file path  to the preference file
	char * preferenceFilePath;

	// Boolean indicating if all the records in the page have been written in the file(disk) or not.
	bool allRecordsWritten;

	// Boolean indicating if the page needs to be rewritten or not.
	bool reWriteFlag;


	// relevant getters and setters
	// void setCurrentPage(off_t currentPage);
	// off_t getCurrentPage();
	//
	// bool isPageFull;
	// void setisPageFull(bool isPageFull);
	//
	// int getCurrentRecordPosition();
	// void setCurrentRecordPosition(int currentRecord);
	//
	// BufferMode getPageBufferMode();
	// void setPageBufferMode(BufferMode pageBufferMode);
};


// stub DBFile header..replace it with your own DBFile.h

class DBFile {
private:
//  Used to read & write page to the disk.
    File myFile;
//  Used as a buffer to read and write data.
    Page myPage;
//  Used to keep track of the state.
    Preference myPreference;
//  Used to keep track of the state.
	ComparisonEngine myCompEng;

public:
	// Constructor and Destructor
	DBFile ();
    ~DBFile ();
	
	// Function to load preference from the disk. This is needed to make read and writes persistent.
	// @input - the file path of the table to be created or opened. Each tbl file has a corresponding .pref
	void LoadPreference(char*f_path);

	// Function to dumpn the preference to the disk. This is needed to make read and writes persistent.
	void DumpPreference();

	// Function to get the next page location to write. i.e. if there are 10 pages written this will give the location to write the 11th page.
	int GetPageLocationToWrite();

	// Function to get the page location from which we need to start the disk read. 
	// @input : Buffer Mode : Will give different locations according to current state of the DBFile.
	int GetPageLocationToRead(BufferMode mode);

	// Function to get the same current page location when we want to rewrite data.
	int GetPageLocationToReWrite();


	//  Function to directly load data from the tbl files.
	/**
		The Load function bulk loads the DBFile instance from a text file, appending new data
		to it using the SuckNextRecord function from Record.h. The character string passed to Load is
		the name of the data file to bulk load.
	**/
	void Load (Schema &myschema, const char *loadpath);
	bool isFileOpen;
	
	/**
		Each DBFile instance has a “pointer” to the current record
		in the file. By default, this pointer is at the first record
		in the file, but it can move in response to record retrievals.
		The following function forces the pointer to correspond to the
		first record in the file.
	**/
	void MoveFirst ();

	/**
	 	In order to add records to the file,
		the function Add is used. In the case of
		the unordered heap file that you are implementing
		in this assignment, this function simply adds the
		new record to the end of the file
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
    int Create (const char *fpath, fType file_type, void *startup);

	/**
		Next, we have Open. This function assumes that the DBFile already exists and has previously
		been created and then closed. The one parameter to this function is simply the physical location
		of the file. If your DBFile needs to know anything else about itself, it should have written this
		to an auxiliary text file that it will also open at startup. The return value is a 1 on success
		and a zero on failure.
	**/
    int Open (const char *fpath);
	/**
		Next, Close simply closes the file. The return value is a 1 on success and a zero on failure.
	**/
    int Close ();
};
#endif
