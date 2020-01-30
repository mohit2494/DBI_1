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
#include "fstream"

/*************** Preference Class Definitions ******************/


// BufferMode Preference::getPageBufferMode() {
//     return this->pageBufferMode;
// }

// void Preference::setPageBufferMode(BufferMode pageBufferMode) {
//     this->pageBufferMode = pageBufferMode;
// }

// off_t Preference::getCurrentPage() {
// 		return this->currentPage;
// }

// void Preference::setCurrentPage(off_t currentPage) {
//     this->currentPage = currentPage;
// }

// bool Preference::isPageFull() {
//     return this->isPageFull;
// }

// void Preference::setisPageFull(bool isPageFull) {
//     this->isPageFull = isPageFull;
// }

// int Preference::getCurrentRecordPosition(){
// 		return this->currentRecordPosition;
// }

// void Preference::setCurrentRecordPosition(int currentRecord) {
// 		this->currentRecordPosition = currentRecord;
// }
/**************** End of Preference Class ********************/



// stub file .. replace it with your own DBFile.cc
DBFile::DBFile () {
    // initializing page count
    // holds the number of pages
    // DBFile holds in physical memory
}

DBFile::~DBFile () {
    
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    if (f_type == heap){
            char * fName = "/home/mk/Documents/uf docs/sem 2/Database Implementation/DBI/dbfiles/nation.pref";
            myFile.Open(0,(char *)f_path);
            LoadPreference(fName);            
            return 1;
    }
    return 0;
}

int DBFile::GetPageLocationToWrite() {
    int pageLocation = myFile.GetLength();
    return !pageLocation ? 0 : pageLocation-1;
}

int DBFile::GetPageLocationToRead(BufferMode mode) {
    if (mode == WRITE){
        return myPreference.currentPage-2;
    }
    else if (mode == READ){
        return myPreference.currentPage;
    }
}

int DBFile::GetPageLocationToReWrite(){
    int pageLocation = myFile.GetLength();
    return pageLocation == 2 ? 0 : pageLocation-1; 

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
    called             myPage.EmptyItOut();
            myPage.EmptyItOut();
Next. 
**/ 
void DBFile::Add (Record &rec) {
    
     if (myPreference.pageBufferMode == READ ) {
            if( myPage.getNumRecs() > 0){
                myPage.EmptyItOut();
            }
            myFile.GetPage(&myPage,GetPageLocationToReWrite());
            myPreference.currentPage = GetPageLocationToReWrite();
            myPreference.currentRecordPosition = myPage.getNumRecs();
            myPreference.reWriteFlag = true;

            // open page for write
    }
    myPreference.pageBufferMode = WRITE;
    
    // add record to current page 
    // check if the page is full
    if(!this->myPage.Append(&rec)) {
        
        cout << "DBFile page full, writing to disk ...." << myFile.GetLength() << endl;
    
        // if page is full, then write page to disk
        if (myPreference.reWriteFlag){
            this->myFile.AddPage(&this->myPage,GetPageLocationToReWrite());
            myPreference.reWriteFlag = false;
        }
        else{
            this->myFile.AddPage(&this->myPage,GetPageLocationToWrite());
        }
        
        // empty page
        this->myPage.EmptyItOut();

        // add again to page
        this->myPage.Append(&rec);
    }
    myPreference.allRecordsWritten=false;
}


/**
 * the Load function bulk loads the DBFile instance from a text file,
 * appending new data to it using the SuckNextRecord function from 
 * Record.h. The character string passed to Load is the name of the 
 * data file to bulk load. 
**/
void DBFile::Load (Schema &f_schema, const char *loadpath) {
    Record temp;
    if (myFile.IsFileOpen()){
        if (myPreference.pageBufferMode == READ ) {
            if( myPage.getNumRecs() > 0){
                myPage.EmptyItOut();
            }
            myFile.GetPage(&myPage,GetPageLocationToReWrite());
            myPreference.currentPage = GetPageLocationToReWrite();
            myPreference.currentRecordPosition = myPage.getNumRecs();
            myPreference.reWriteFlag = true;

            // open page for write
        }
        myPreference.pageBufferMode = WRITE;
        FILE *tableFile = fopen (loadpath, "r"); 
        // while there are records, keep adding them to the DBFile
        while(temp.SuckNextRecord(&f_schema, tableFile)==1) {
            Add(temp);
        }
    }
}

int DBFile::Open (const char *f_path) {
    char* fName = "/home/mk/Documents/uf docs/sem 2/Database Implementation/DBI/dbfiles/nation.pref";
    myFile.Open(1,(char *)f_path);
    LoadPreference(fName);
    if(myFile.IsFileOpen()){
        if( myPreference.pageBufferMode == READ){
            myFile.GetPage(&myPage,GetPageLocationToRead(myPreference.pageBufferMode));
            for (int i = 0 ; i < myPreference.currentRecordPosition; i++){
                myPage.GetFirst(&myRecord);
            }
            myPreference.currentPage++;
        }
        else if(!myPreference.isPageFull){
            myFile.GetPage(&myPage,GetPageLocationToRead(myPreference.pageBufferMode));
        }
        return 1;
    }
    return 0;
}

void DBFile::MoveFirst () {
    if (myFile.IsFileOpen()){
        if (myPreference.pageBufferMode == WRITE && myPage.getNumRecs() > 0){
            if(!myPreference.allRecordsWritten){
                myFile.AddPage(&myPage,GetPageLocationToReWrite());
            }
        }
        myPage.EmptyItOut();
        myPreference.pageBufferMode = READ;
        myFile.MoveToFirst();
        myPreference.currentPage = 0;
        myPreference.currentRecordPosition = 0;
    }
}

/**
	Next, Close simply closes the file. The return value is a 1
    on success and a zero on failure.
**/ 
int DBFile::Close () {
    if(myPreference.pageBufferMode == WRITE && myPage.getNumRecs() > 0){
            if(!myPreference.allRecordsWritten){
                if (myPreference.reWriteFlag){
                    myFile.AddPage(&this->myPage,GetPageLocationToReWrite());
                    myPreference.reWriteFlag = false;
                }
                else{
                    myFile.AddPage(&this->myPage,GetPageLocationToWrite());
                }
            }
            myPreference.isPageFull = false;
            myPreference.currentPage = myFile.Close();
            myPreference.allRecordsWritten = true;
            myPreference.currentRecordPosition = myPage.getNumRecs();
    }
    else if(myPreference.pageBufferMode == READ){
            myPreference.currentPage--;
    }
    DumpPreference();
}


int DBFile::GetNext (Record &fetchme) {
    if (myFile.IsFileOpen()){
        if (myPreference.pageBufferMode == WRITE && myPage.getNumRecs() > 0){
            if(!myPreference.allRecordsWritten){
                myFile.AddPage(&myPage,GetPageLocationToReWrite());
            }
            myPage.EmptyItOut();
            myPreference.currentPage = myFile.GetLength();
            myPreference.currentRecordPosition = myPage.getNumRecs();
            return 0;
        }
        myPreference.pageBufferMode = READ;
        if (!myPage.GetFirst(&fetchme)) {
            if (myPreference.currentPage+1 >= myFile.GetLength()){
               return 0;
            }
            else{
                myFile.GetPage(&myPage,GetPageLocationToRead(myPreference.pageBufferMode));
                myPage.GetFirst(&fetchme);
                myPreference.currentPage++;
                myPreference.currentRecordPosition = 0;
            }
        }
        myPreference.currentRecordPosition++;
        // Schema schema ("catalog", "lineitem");
        // fetchme.Print(&schema);
        return 1;
    }
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
        if (myPreference.pageBufferMode == WRITE && myPage.getNumRecs() > 0){
            if(!myPreference.allRecordsWritten){
                myFile.AddPage(&myPage,GetPageLocationToReWrite());
            }
            myPage.EmptyItOut();
            myPreference.currentPage = myFile.GetLength();
            myPreference.currentRecordPosition = myPage.getNumRecs();
            return 0;
        }
        bool readFlag ;
        bool compareFlag;
        do{
            readFlag = GetNext(fetchme);
            compareFlag = myCompEng.Compare (&fetchme, &literal, &cnf);
        }
        while(readFlag && !compareFlag);
        if(!readFlag){
            return 0;
        }
        return 1;

}

void DBFile::LoadPreference(char * newFilePath) {
    ifstream file;
    if (Utilities::checkfileExist(newFilePath)) {
        
        // cout << endl;
        // cout << "----------------------------------------------" << endl;
        // cout << "opening preference file located at :" << endl;
        // cout << newFilePath << endl;
        // cout << "----------------------------------------------" << endl;

        file.open(newFilePath,ios::in);
        if(!file){
            cerr<<"Error in opening file..";
            exit(1);
        }
        file.read((char*)&myPreference,sizeof(Preference));

        // cout << endl;
        // cout << "----------------------------------------------" << endl;
        // cout << "Preference values after read :" << endl;
        // cout << myPreference.preferenceFilePath << endl;
        // cout << "----------------------------------------------" << endl;
        myPreference.preferenceFilePath = (char*)malloc(strlen(newFilePath) + 1); 
        strcpy(myPreference.preferenceFilePath,newFilePath);
    }
    else {
        myPreference.preferenceFilePath = (char*) malloc(strlen(newFilePath) + 1); 
        strcpy(myPreference.preferenceFilePath,newFilePath);
        myPreference.currentPage = 0;
        myPreference.currentRecordPosition = 0;
        myPreference.isPageFull = false;
        myPreference.pageBufferMode = IDLE;
        myPreference.reWriteFlag= false;
    }
}

void DBFile::DumpPreference(){
    ofstream file;
    file.open(myPreference.preferenceFilePath,ios::out);
    if(!file) {
      cerr<<"Error in opening file for writing.."<<endl;
      exit(1);
    }
    cout << endl;
    cout << "----------------------------------------------" << endl;
    cout << "Preference values before writing :" << endl;
    cout << myPreference.preferenceFilePath << endl;
    cout << "----------------------------------------------" << endl;
    file.write((char*)&myPreference,sizeof(Preference));
    file.close();


    // string filename(myPreference.preferenceFilePath);
    // string newFilePath = filename.substr(0,filename.find_last_of('.'))+".bin";
    // const char* somePath = newFilePath.c_str();
    // LoadPreference(somePath);

}
