#include <iostream>
#include "delete.h"

// Функция для удаления строк из таблицы на основе переданной команды.
void deleteRows(string command, JsonTable& jstab) {
    istringstream iss(command);
    string message;

    // Извлекаем первое слово команды
    iss >> message;
    iss >> message;

    // Проверяем, содержит ли команда ключевое слово "FROM"
    if (message != "FROM") {
        cerr << "Неверный ввод данных." << endl;
        return;
    }

    // Считываем имя таблицы
    iss >> message;
    string table = message;

    // Проверяем, существует ли таблица
    if (!tableExist(table, jstab.head)) {
        cerr << "Таблицы: " << table << " не существует" << endl;
        return;
    }

    // Проверяем, не заблокирована ли таблица для редактирования
    if (isLocked(table, jstab.scheme)) {
        cerr << "Файл заблокирован для редактирования" << endl;
        return;
    }

    // Ожидаем дальнейшую команду от пользователя
    string command2;
    cout << "<< ";
    getline(cin, command2);
    istringstream iss2(command2);
    string message2;

    // Проверяем наличие ключевого слова "WHERE" в команде
    iss2 >> message2;
    if (message2 != "WHERE") {
        cerr << "Неверный ввод данных." << endl;
        return;
    }

    // Считываем условие для удаления
    iss2 >> message2;
    string table2;
    string column;

    // Разделяем таблицу и колонку через точку
    separateDot(message2, table2, column);

    // Проверяем, относится ли указанная колонка к текущей таблице
    if (table2 != table) {
        cerr << "Неверный ввод данных." << endl;
        return;
    }

    // Проверяем, существует ли указанная колонка
    if (!columnExist(table, column, jstab.head)) {
        cerr << "Данной колонки нет в таблице." << endl;
        return;
    }

    // Проверяем наличие оператора равенства
    iss2 >> message2;
    if (message2 != "=") {
        cerr << "Неверный ввод данных." << endl;
        return;
    }

    // Считываем значение условия
    iss2 >> message2;
    if (message2.front() != '\'' || message2.back() != '\'') {
        cerr << "Неверный ввод данных." << endl;
        return;
    }

    // Извлекаем значение без кавычек
    string value;
    for (size_t i = 1; i < message2.size() - 1; i++) {
        value += message2[i];
    }

    // Блокируем таблицу для исключения параллельного редактирования
    locker(table, jstab.scheme);

    // Подсчет количества файлов CSV, связанных с таблицей
    size_t csvCount = 1;
    while (true) {
        filesystem::path csvPath = filesystem::current_path() / jstab.scheme / table / (to_string(csvCount) + ".csv");
        ifstream file(csvPath);
        if (!file.is_open()) {
            break;
        }
        file.close();
        csvCount++;
    }

    // Обработка каждого CSV-файла
    for (size_t i = 1; i < csvCount; i++) {
        filesystem::path csvPath = filesystem::current_path() / jstab.scheme / table / (to_string(i) + ".csv");
        rapidcsv::Document csvDoc(csvPath.string());

        // Получаем индекс колонки
        int index = csvDoc.GetColumnIdx(column);
        size_t countRows = csvDoc.GetRowCount();

        // Удаляем строки, соответствующие условию
        for (size_t j = 0; j < countRows; j++) {
            if (csvDoc.GetCell<string>(index, j) == value) {
                csvDoc.RemoveRow(j);
                countRows--; // Уменьшаем количество строк
                j--;         // Возвращаемся к той же строке
            }
        }

        // Сохраняем изменения в файл
        csvDoc.Save(csvPath.string());
    }

    // Разблокируем таблицу
    locker(table, jstab.scheme);
}
