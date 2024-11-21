#pragma once
#include <iostream>
#include <filesystem>
#include "rapidcsv.hpp" 
#include "Node.h"

using namespace std;
namespace fs = filesystem;

bool TableExist(const string& TableName, Node* TableHead);
bool isloker(const string& TableName, const string& SchemaName);
void copyNameColonk(const string& from_file, const string& to_file);
void loker(const string& TableName, const string& SchemaName);
int findCsvFileCount(const TableJson& json_table, const string& TableName);
void createNewCsvFile(const std::string& baseDir, const std::string& TableName, int& csvNumber, const TableJson& tableJson);
void insert(const string& command, TableJson& json_table);
