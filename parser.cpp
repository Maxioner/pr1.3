#include "parser.h"

// Удаление директории
void DelDirectory(const fs::path& PathDirectory){
    if (fs::exists(PathDirectory)){
        fs::remove_all(PathDirectory);
    }
}

// Создание директории / файлов
void Create_Dir_Files(const fs::path& PathSheme, const json& structure, TableJson& json_table){
    Node* TableHead = nullptr;
    Node* TableTail = nullptr;

    for (const auto& table: structure.items()){
        fs::path tablePath = PathSheme / table.key(); // Создается путь к директории
        if (!fs::create_directories(tablePath)){
            cerr << "Cant delete this directory: " << tablePath << endl;
            return;
        }
        cout << "Directory has been created: " << tablePath << endl;
        
        // Создание таблицы
        Node* newTable = new Node{table.key(), nullptr, nullptr};

        // Создание файла блокировки
        string LockFileName = table.key() + "_lock.txt";
        ofstream LockFile(tablePath/ LockFileName);
        if (!LockFile.is_open()){
            cerr << "Cant open the file: " << LockFileName << endl;
            return;
        }
        LockFile << "Status: Unlocked";
        LockFile.close();

        // Добавление таблицы в список
        if (TableHead == nullptr){
            TableHead = newTable;
            TableTail = newTable;
        }
        else {
            TableTail->next = newTable;
            TableTail = newTable;
        }

        // Создание колонки pk
        string KeyColumn = table.key() + "_pk";
        ListNode* Column_PK = new ListNode{KeyColumn, nullptr};
        newTable->column = Column_PK;

        // Создание csv файла
        fs::path CSVFilePath = tablePath / "1.csv";
        ofstream File_CSV(CSVFilePath);
        if (!File_CSV.is_open()){
            cerr << "Cant create the file: " << CSVFilePath << endl;
            return;
        }
        File_CSV << KeyColumn << ","; // Записывается имя колонки первичного ключа в csv файл

        // Запись названий колонок в csv и добавление их в список
        const auto& columns = table.value();
        for (size_t i = 0; i < columns.size(); i++){
            string Column_Name = columns[i].get<string>();
            File_CSV << Column_Name;

            // Добавление колонки в список
            ListNode* newcolumn = new ListNode{Column_Name, nullptr};

            if (newTable->column == nullptr){
                newTable->column = newcolumn;
            }
            else {
                ListNode* Last_Column = newTable->column;
                while (Last_Column->next != nullptr){
                    Last_Column = Last_Column->next;
                }
                Last_Column->next = newcolumn;
            }

            // Если последняя колонка - запятую не ставим
            if (i < columns.size() - 1){
                File_CSV << ",";
            }
        }

        File_CSV << endl;
        File_CSV.close();
        cout << "File created: " << CSVFilePath << endl;

        // Создание файла для первичного ключа
        string PK_FileName = KeyColumn + "_sequence.txt";
        ofstream PK_File(tablePath/ PK_FileName);
        if (!PK_File.is_open()){
            cerr << "Cant open the file: " << PK_FileName << endl;
            return;
        }
        PK_File << "0"; // Начальный первичный ключ
        PK_File.close();
    }
    json_table.table_head = TableHead;
}

// Парсер схемы
void Parser(TableJson& json_table){
    string fileName = "schema.json";
    ifstream file(fileName);
    if (!file.is_open()){
        cerr << "Cant open the file: " << fileName << endl;
        return;
    }

    string json_include;
    string line;
    while (getline(file, line)) {
        json_include += line;
    }
    file.close();

    json parser_Json = json::parse(json_include);

    if (parser_Json.contains("name")) {
        json_table.Name = parser_Json["name"];
    } else {
        cerr << "Error: 'name' not searched in shema" << endl;
        return;
    }
fs::path schemePath = fs::current_path() / json_table.Name; // Путь к директории схемы
    cout << "Current directory: " << fs::current_path() << endl;
    cout << "Name of shema: " << json_table.Name << endl;

    // Если схема существует, удаляем её
    if (fs::exists(schemePath)) {
        cout << "Deleting old directory: " << schemePath << endl;
        DelDirectory(schemePath);
    }

    // Создаем директорию для схемы
    if (!fs::create_directory(schemePath)) {
        cerr << "Cannot create directory: " << schemePath << endl;
        cout << "Error: " << strerror(errno) << endl;
        return;
    }
    cout << "Directory has been created: " << schemePath << endl;

    if (parser_Json.contains("structure")) {
        Create_Dir_Files(schemePath, parser_Json["structure"], json_table);
    }

    if (parser_Json.contains("tuples_limit")) {
        json_table.table_size = parser_Json["tuples_limit"];
    } else {
        cerr << "Error: 'tuples_limit' not searched in shema" << endl;
        return;
    }
}
