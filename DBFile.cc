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

void DBFile::Load (Schema &f_schema, const char *loadpath) {

}

int DBFile::Open (const char *f_path) {
    if (!myPreference.FindFilePath(f_path)){
        cerr << "Cannot Open DBFile before creating it.\n";
        exit(1);
    }
    myFile.Open(1,f_path);
    if(myFile.IsFileOpen()){
        return 1;
    }
    return 0;
    
}

void DBFile::MoveFirst () {
    myFile.MoveToFirst();
}

int DBFile::Close () {
    myPreference.Dumps();
    
}

void DBFile::Add (Record &rec) {

}

int DBFile::GetNext (Record &fetchme) {

}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    
}
