#pragma once
#include "Node.h" // структура таблиц
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem> // директории
#include "json.hpp" // json

using namespace std;
using json = nlohmann::json; 
namespace fs = filesystem;


void DelDirectory(const fs::path& directoryPath); // удаление директории
void Create_Dir_Files(const fs::path& ShemePath, const json& structure, TableJson& json_table); // создание полной директории и файлов
void Parser(TableJson& json_table); // парсинг схемы
