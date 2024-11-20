#include <iostream>
#include "parser.h"

// Функция для удаления директории и всего её содержимого
void removeDir(const filesystem::path& dirPath) {
  if (filesystem::exists(dirPath)) {  /
    filesystem::remove_all(dirPath);  
  }
}

// Функция для создания файлов и директорий на основе JSON-структуры
void createFiles(const filesystem::path& schemePath, const json& jsonStruct, JsonTable& jstable) {
  Tables* head = nullptr;  
  Tables* tail = nullptr;  

  // Проходим по каждому элементу JSON-структуры
  for (const auto& table : jsonStruct.items()) {
    filesystem::path tableDir = schemePath / table.key();  
    filesystem::create_directory(tableDir);  // Создаем директорию для таблицы
    if (!filesystem::exists(tableDir)) {  
        cerr << "Не удалось создать директорию: " << tableDir << endl;
        return;
    }

    // Создаем новый элемент списка таблиц
    Tables* newTable = new Tables{table.key(), nullptr, nullptr};

    // Если список таблиц пуст, устанавливаем голову и хвост на новый элемент
    if (head == nullptr) {
      head = newTable;
      tail = newTable;
    }
    else {  // Иначе добавляем новый элемент в конец списка
      tail->next = newTable;
      tail = newTable;
    }

    // Создаем файл блокировки для таблицы
    filesystem::path lockDir = tableDir / (table.key() + "_lock.txt");
    ofstream file(lockDir);
    if (!file.is_open()) {  
      cerr << "Не удалось открыть файл." << endl;
      return;
    }
    file << "unlocked";  
    file.close();

    // Создаем первичный ключ для таблицы
    string pkColumn = table.key() + "_pk";
    Node* firstColumn = new Node{pkColumn, nullptr};

    Node* headColumn = firstColumn;  // Указатель на начало списка столбцов
    Node* tailColumn = firstColumn;  // Указатель на конец списка столбцов

    // Создаем CSV-файл для таблицы
    filesystem::path csvDir = tableDir / "1.csv";
    ofstream csvFile(csvDir);
    if (!csvFile.is_open()) {  // Проверяем, успешно ли открыт файл
      cerr << "Не удалось открыть файл." << endl;
      return;
    }
    csvFile << pkColumn << ",";  // Записываем первичный ключ в CSV-файл
    
    const auto& columns = table.value();  // Получаем список столбцов из JSON
    for (size_t i = 0; i < columns.size(); i++) {
      csvFile << columns[i].get<string>();  
      Node* newColumn = new Node{columns[i].get<string>(), nullptr};
      tailColumn->next = newColumn; 
      tailColumn = newColumn;
      if (i < columns.size() - 1) {
        csvFile << ",";  // Добавляем разделитель между столбцами
      }
    }
    csvFile << endl;  
    csvFile.close();

    newTable->column = headColumn;  // Устанавливаем список столбцов для таблицы

    // Создаем файл последовательности для первичного ключа
    filesystem::path sequence = tableDir / (pkColumn + "_sequence.txt");
    ofstream seqFile(sequence);
    if (!seqFile.is_open()) {  
      cerr << "Не удалось открыть файл." << endl;
      return;
    }
    seqFile << "0";  // Записываем начальное значение последовательности
    seqFile.close();
  }
  jstable.head = head;  // Устанавливаем голову списка таблиц в структуре JsonTable
}

// Парсер
void parser(JsonTable& jstab) {
    ifstream file("schema.json");  
    if (!file.is_open()) {  
        cerr << "Не удалось открыть файл: schema.json" << endl;
        return;
    }
    
    json jspars;
    file >> jspars;  // Загружаем JSON-данные из файла
    file.close();

    jstab.scheme = jspars["name"]; 
    filesystem::path schemePath = filesystem::current_path() / jstab.scheme;  // Создаем путь к директории схемы
    removeDir(schemePath);  // Удаляем директорию с прошлого запуска
    filesystem::create_directory(schemePath);  // Создаем новую директорию для схемы
    if (!filesystem::exists(schemePath)) {  
        cerr << "Не удалось создать директорию: " << schemePath << endl;
        return;
    }

    // Если JSON содержит структуру, создаем файлы и директории на основе этой структуры
    if (jspars.contains("structure")) {
        createFiles(schemePath, jspars["structure"], jstab);
    }

    jstab.rowsCount = jspars["tuples_limit"];  // Получаем лимит строк из JSON
}
