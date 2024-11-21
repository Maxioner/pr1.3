#pragma once
#include <string>
#include <fstream>

using namespace std;

// Структура для колонок таблицы
struct ListNode {
    string column_name;
    ListNode* next;
};

// Структура для таблиц
struct Node {
    string table;
    Node* next;
    ListNode* column;
};

// Структура для схемы и таблиц
struct TableJson {
    int table_size;
    string Name;
    Node* table_head;
};
