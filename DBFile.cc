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
    isFileOpen = false;
}

DBFile::~DBFile () {
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    if (Utilities::checkfileExist(f_path)) {
        cout << "file you are about to create already exists!"<<endl;
        return 0;    
    }
    // check if the file type is correct
    if (f_type == heap){

            // changing .bin extension to .pref for storing preferences.
            string s(f_path);
            string news = s.substr(0,s.find_last_of('.'))+".pref";

            //Utilities::Log("Inside DBFile Create, preference file path after replacing extension");
            //Utilities::Log(news);

            char* finalString = new char[news.length()+1];
            strcpy(finalString, news.c_str());

            // opening file with given file extension
            myFile.Open(0,(char *)f_path);
            
            // loading preferences
            //Utilities::Log("File path before calling LoadPreference ");
            //Utilities::Log(string(finalString));
            LoadPreference(finalString);            
            isFileOpen = true;
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
    called Next. 
**/
void DBFile::Add (Record &rec) {
    
    if (!isFileOpen){
        cerr << "Trying to load a file which is not open!";
        exit(1);
    }
    
     // Flush the page data from which you are reading and load the last page to start appending records.
     if (myPreference.pageBufferMode == READ ) {
            if( myPage.getNumRecs() > 0){
                myPage.EmptyItOut();
            }
            // open page the last written page to start rewriting
            myFile.GetPage(&myPage,GetPageLocationToReWrite());
            myPreference.currentPage = GetPageLocationToReWrite();
            myPreference.currentRecordPosition = myPage.getNumRecs();
            myPreference.reWriteFlag = true;

    }

    // set DBFile in write mode
    myPreference.pageBufferMode = WRITE;
    
    // add record to current page 
    // check if the page is full
    if(!this->myPage.Append(&rec)) {
        
//        cout << "DBFile page full, writing to disk ...." << myFile.GetLength() << endl;
    
        // if page is full, then write page to disk. Check if the date needs to rewritten or not
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
    
    if (!isFileOpen){
        cerr << "Trying to load a file which is not open!";
        exit(1);
    }

    Record temp;
    // Flush the page data from which you are reading and load the last page to start appending records.
    if (myPreference.pageBufferMode == READ ) {
        if( myPage.getNumRecs() > 0){
            myPage.EmptyItOut();
        }
        // open page for write
        myFile.GetPage(&myPage,GetPageLocationToReWrite());
        myPreference.currentPage = GetPageLocationToReWrite();
        myPreference.currentRecordPosition = myPage.getNumRecs();
        myPreference.reWriteFlag = true;
    }
    // set DBFile in WRITE Mode
    myPreference.pageBufferMode = WRITE;
    FILE *tableFile = fopen (loadpath, "r"); 
    // while there are records, keep adding them to the DBFile. Reuse Add function.
    while(temp.SuckNextRecord(&f_schema, tableFile)==1) {
        Add(temp);
    }
    
}

int DBFile::Open (const char *f_path) {
    
    if (!Utilities::checkfileExist(f_path)) {
        cout << "Trying to open a file which is not created yet!"<<endl;
        return 0;
    }

    // changing .bin extension to .pref for storing preferences.
    string s(f_path);
    string news = s.substr(0,s.find_last_of('.'))+".pref";
    //Utilities::Log("Inside DBFile Open, preference file path after replacing extension");
    //Utilities::Log(news);
    char* finalString = new char[news.length()+1];
    strcpy(finalString, news.c_str());

    // opening file using given path
    //Utilities::Log("Opening binary file using path :");
    //Utilities::Log(string(f_path));
    myFile.Open(1,(char *)f_path);

    // loading preferences
    //Utilities::Log("File path before calling LoadPreference ");
    //Utilities::Log(string(finalString));
    LoadPreference(finalString);            

    if(myFile.IsFileOpen()){
        isFileOpen = true;
        // Load the last saved state from preference.
        if( myPreference.pageBufferMode == READ){
            myFile.GetPage(&myPage,GetPageLocationToRead(myPreference.pageBufferMode));
            Record myRecord;
            for (int i = 0 ; i < myPreference.currentRecordPosition; i++){
                myPage.GetFirst(&myRecord);
            }
            myPreference.currentPage++;
        }
        else if(myPreference.pageBufferMode == WRITE && !myPreference.isPageFull){
            myFile.GetPage(&myPage,GetPageLocationToRead(myPreference.pageBufferMode));
        }
        return 1;
    }
    isFileOpen = false;
    return 0;
}

void DBFile::MoveFirst () {
    if (myFile.IsFileOpen()){
        isFileOpen = true;
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
    if (!isFileOpen) {
        cout << "trying to close a file which is not open!"<<endl;
        return 0;
    }
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
    else{
        if(myPreference.pageBufferMode == READ){
            myPreference.currentPage--;
        }
        myFile.Close();
    }
    
    isFileOpen = false;
    DumpPreference();
    return 1;
}


int DBFile::GetNext (Record &fetchme) {
    if (myFile.IsFileOpen()){
        // Flush the Page Buffer if the WRITE mode was active.
        if (myPreference.pageBufferMode == WRITE && myPage.getNumRecs() > 0){
            if(!myPreference.allRecordsWritten){
                myFile.AddPage(&myPage,GetPageLocationToReWrite());
            }
            //  Only Write Records if new records were added.
            myPage.EmptyItOut();
            myPreference.currentPage = myFile.GetLength();
            myPreference.currentRecordPosition = myPage.getNumRecs();
            return 0;
        }
        myPreference.pageBufferMode = READ;
        // loop till the page is empty and if empty load the next page to read
        if (!myPage.GetFirst(&fetchme)) {
            // check if all records are read.
            if (myPreference.currentPage+1 >= myFile.GetLength()){
               return 0;
            }
            else{
                // load new page and get its first record.
                myFile.GetPage(&myPage,GetPageLocationToRead(myPreference.pageBufferMode));
                myPage.GetFirst(&fetchme);
                myPreference.currentPage++;
                myPreference.currentRecordPosition = 0;
            }
        }
        // increament counter for each read.
        myPreference.currentRecordPosition++;
        return 1;
    }
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
        // Flush the Page Buffer if the WRITE mode was active.
        if (myPreference.pageBufferMode == WRITE && myPage.getNumRecs() > 0){
            //  Only Write Records if new records were added.
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
        // loop until all records are read or if some record maches the filter CNF
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
        //Utilities::Log("Opening preference file located at : "+ std::string(newFilePath));
        file.open(newFilePath,ios::in); 
        if(!file){
            //Utilities::Log("Error opening preference file at : "+ std::string(newFilePath));
            cerr<<"Error in opening file..";
            exit(1);
        }
        file.read((char*)&myPreference,sizeof(Preference));
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
        myPreference.allRecordsWritten = true;
    }
    //Utilities::Log("Preferences Loaded..!");
}

void DBFile::DumpPreference(){
    //Utilities::Log("Dumping preferences at :"+ std::string(myPreference.preferenceFilePath));
    ofstream file;
    file.open(myPreference.preferenceFilePath,ios::out);
    if(!file) {
        //Utilities::Log("Error in dumping preferences at :"+ std::string(myPreference.preferenceFilePath));
        cerr<<"Error in opening file for writing.."<<endl;
        exit(1);
    }
    //Utilities::Log("Dumping preferences at :"+ std::string(myPreference.preferenceFilePath));
    file.write((char*)&myPreference,sizeof(Preference));
    file.close();
    //Utilities::Log("Preferences Dumped!...");
}
