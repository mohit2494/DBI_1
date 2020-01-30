#include "DBFile.h"
#include "Schema.h"
#include <string.h>
#include <gtest/gtest.h>


class FilePath{
    public:
    // make sure that the file path/dir information below is correct
    const std::string dbfile_dir = "/home/mk/Documents/uf docs/sem 2/Database Implementation/DBI/dbfiles/"; // dir where binary heap files should be stored
    const std::string tpch_dir ="/home/mk/Documents/uf docs/sem 2/Database Implementation/git/tpch-dbgen/"; // dir where dbgen tpch files (extension *.tbl) can be found
    const std::string catalog_path = "catalog"; // full path of the catalog file
};


// creating DBFile at specified location
TEST(DBFileTesting, Create) {
    // creating DBFile for nation table
    FilePath filepath;
    const string schemaSuffix = "nation.bin";
    const string completePath = filepath.dbfile_dir + schemaSuffix;
    DBFile dbfile;
    ASSERT_EQ(1,dbfile.Create(completePath.c_str(),heap,NULL));
    
    // loading file so that open can use it
    Schema nation ("catalog", "nation");
    dbfile.Load(nation, completePath.c_str());
}

TEST(DBFileTesting, Open) {

    // creating DBFile for nation table
    FilePath filepath;
    const string schemaSuffix = "nation.bin";
    const string completePath = filepath.dbfile_dir + schemaSuffix;
    DBFile dbfile;
    ASSERT_EQ(1,dbfile.Open(completePath.c_str()));
}
 

TEST(DBFileTesting, Close) { 
    // creating DBFile for nation table
    FilePath filepath;
    const string schemaSuffix = "nation.bin";
    const string completePath = filepath.dbfile_dir + schemaSuffix;
    DBFile dbfile;
    dbfile.Open(completePath.c_str());
    ASSERT_EQ(1,dbfile.Close());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}