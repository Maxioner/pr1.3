#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;


struct Node {
    string data; //Название столбца
    Node* next;
};

struct Tables {
    string name; //Название таблицы
    Node* column;
    Tables* next; // Следующая таблица
};

struct JsonTable { //Структура таблицы с json
    int rowsCount; // количество строк
    string scheme; // имя схемы
    Tables* head;
};

void removeDir(const filesystem::path& dirPath);
void createFiles(const filesystem::path& schemePath, const json& jsonStruct, JsonTable& jstable);
void parser(JsonTable& jstab);
