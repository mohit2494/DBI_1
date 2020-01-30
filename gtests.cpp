// basic structure of tests
#include <iostream>
#include "DBFile.h"
#include "test.h"

// make sure that the file path/dir information below is correct
const char *dbfile_dir = ""; // dir where binary heap files should be stored
const char *tpch_dir ="/cise/tmp/dbi_sp11/DATA/10M/"; // dir where dbgen tpch files (extension *.tbl) can be found
const char *catalog_path = "catalog"; // full path of the catalog file



TEST(DBFileTesting, Create) { 
   	DBFile dbfile;
	dbfile.Create(rel->path());
}
 
TEST(DBFileTesting, Open) {
    DBFile dbfile;
	dbfile.Open(rel->path());
}
 
TEST(DBFileTesting, Close) {
	DBFile dbfile;
	dbfile.Open(rel->path());
    DBFile.close();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

