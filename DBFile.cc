#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include "Utilities.h"

void Preference :: Loads() {
    
}

void Preference :: Dumps(){
    
}

bool Preference :: FindFilePath(const char *f_path){
    return true;
}

// stub file .. replace it with your own DBFile.cc
DBFile::DBFile () {
    myPreference.Loads();

    // initializing page count
    // holds the number of pages
    // DBFile holds in physical memory
    currentPageCount = -1;
}

DBFile::~DBFile () {
    
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    if (f_type == heap){
        if (!myPreference.FindFilePath(f_path)){
            char *fName = strdup(f_path);
            myFile.Open(0,fName);
            return 1;
        }
    }
    return 0;
}

/**
 * the Load function bulk loads the DBFile instance from a text file,
 * appending new data to it using the SuckNextRecord function from 
 * Record.h. The character string passed to Load is the name of the 
 * data file to bulk load. 
**/
void DBFile::Load (Schema &f_schema, const char *loadpath) {
    Record temp;
 // @todo use bool isOpen();
    FILE *tableFile = fopen(loadpath,"r");
    
    // while there are records, keep adding them to the DBFile
    while(temp.SuckNextRecord(&f_schema, tableFile)==1) {
        Add(temp);
    }
}

int DBFile::Open (const char *f_path) {
    if (!myPreference.FindFilePath(f_path)){
        cerr << "Cannot Open DBFile before creating it.\n";
        exit(1);
    }
    char * fName = strdup(f_path);
    myFile.Open(1,fName);
    if(myFile.IsFileOpen()){
        return 1;
    }
    return 0;
    
}

void DBFile::MoveFirst () {
    myFile.MoveToFirst();
    myPreference.currentPage = 1;
}

/**
	Next, Close simply closes the file. The return value is a 1
    on success and a zero on failure.
**/ 
int DBFile::Close () {
    myPreference.Dumps();
}

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
void DBFile::Add (Record &rec) {
    
    // add record to current page 
    // check if the page is full
    if(!this->myPage.Append(&rec)) {
        
        cout << "DBFile page full, writing to disk ....";
        int fileExists = 1;
        fileExists = Utilities::checkfileExist(this->myFilePath);
    
        // if page is full, then write page to disk
        this->myFile.Open(fileExists, this->myFilePath);
        this->myFile.AddPage(&this->myPage,currentPageCount);
        currentPageCount++;
        
        // empty page
        this->myPage.EmptyItOut();

        // add again to page
        this->myPage.Append(&rec);
    }
}


int DBFile::GetNext (Record &fetchme) {
    if (myFile.IsFileOpen()){
        if (myPreference.pageBufferMode == WRITE){
            myFile.AddPage(&myPage,myFile.GetLength()+1);
            myPreference.currentPage = myFile.GetLength();
            myPage.EmptyItOut();
            return 0;
        }
        if (!myPage.GetFirst(&fetchme)) {
            if (++myPreference.currentPage >myFile.GetLength()){
               return 0;
            }
            else{
                myFile.GetPage(&myPage,myPreference.currentPage);
                myPage.GetFirst(&fetchme);
            }
        }
        myPreference.pageBufferMode = READ;
        return 1;
    }

}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {

    if (myPreference.pageBufferMode == WRITE){
        ComparisonEngine comp;
        do{
            bool readFlag = GetNext(&fetchme);
            bool compareFlag = comp.Compare (&fetchme, &literal, &cnf)
        }
        while(readFlag && !compareFlag);
        if(!readFlag){
            return 0;
        }
        return 1;
    }

}
