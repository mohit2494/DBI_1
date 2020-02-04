#include "DBFile.h"
#include "Schema.h"
#include <string.h>
#include <gtest/gtest.h>


class FilePath{
    public:
    // make sure that the file path/dir information below is correct
    // dir where binary heap files should be stored
    const std::string dbfile_dir = "dbfiles/"; 
    
    //****************** update the following path as per requirement*************************************/
    // dir where dbgen tpch files (extension *.tbl) can be found
    const std::string tpch_dir ="/home/mk/Documents/uf docs/sem 2/Database Implementation/git/tpch-dbgen/"; 

    const std::string catalog_path = "catalog"; // full path of the catalog file
};


TEST(DBFileTestingCreate, Scenario1_Create) {
    
    // scenario 1: Create a file
    FilePath filepath;
    const string schemaSuffix = "nation.bin";
    const string dbFilePath = filepath.dbfile_dir + schemaSuffix;
    DBFile dbfile;
    ASSERT_EQ(1,dbfile.Create(dbFilePath.c_str(),heap,NULL));
    // Schema nation ("catalog", "nation");
    // dbfile.Load(nation, dbFilePath.c_str());
    dbfile.Close();

}

TEST(DBFileTestingCreate, Scenario2_Create) {
    
    FilePath filepath;
    const string schemaSuffix = "nation.bin";
    const string dbFilePath = filepath.dbfile_dir + schemaSuffix;
    DBFile dbfile;
   
    // scenario 2: Creating a file which already exists
    // creating DBFile for nation table
    ASSERT_EQ(0,dbfile.Create(dbFilePath.c_str(),heap,NULL));
}

TEST(DBFileTesting, Scenario1_Open) {

    // creating DBFile for nation table
    FilePath filepath;
    DBFile dbfile;
    const string existentFile = "nation.bin";
    const string nonExistentFile = "something.bin";
    const string existentPath = filepath.dbfile_dir + existentFile;
    const string nonExistentPath = filepath.dbfile_dir + nonExistentFile;
    
    // scenario 1:
    // Trying to open a file which doesn't exist
    ASSERT_EQ(0,dbfile.Open(nonExistentPath.c_str()));

}

TEST(DBFileTesting, Scenario2_Open) {

    // creating DBFile for nation table
    FilePath filepath;
    DBFile dbfile;
    const string existentFile = "nation.bin";
    const string nonExistentFile = "something.bin";
    const string existentPath = filepath.dbfile_dir + existentFile;
    const string nonExistentPath = filepath.dbfile_dir + nonExistentFile;
    
    // scenario 2
    // Trying to open a file which exists
    ASSERT_EQ(1,dbfile.Open(existentPath.c_str()));

}

TEST(DBFileTesting, Scenario3_Open) {

    // creating DBFile for nation table
    FilePath filepath;
    DBFile dbfile;
    const string existentFile = "nation.bin";
    const string nonExistentFile = "something.bin";
    const string existentPath = filepath.dbfile_dir + existentFile;
    const string nonExistentPath = filepath.dbfile_dir + nonExistentFile;
        
    // scenario 3
    // Trying to open a file which is already open
    ASSERT_EQ(1,dbfile.Open(existentPath.c_str()));
    
    // close file after test cases have run
    dbfile.Close();
}

TEST(DBFileTesting, Close) {

    // scenario 1: trying to close a file which is not open
    FilePath filepath;
    const string schemaSuffix = "nation.bin";
    const string completePath = filepath.dbfile_dir + schemaSuffix;
    DBFile dbfile;
    ASSERT_EQ(0,dbfile.Close());

    // // scenario 2: trying to close an open file
    dbfile.Open(completePath.c_str());
    ASSERT_EQ(1,dbfile.Close());    
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}