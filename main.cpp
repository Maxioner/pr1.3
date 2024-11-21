#include "parser.h"
#include "insert.hpp"
#include "delete.h"
#include "select.h"

int main() {
    TableJson json_table;
    Parser(json_table);

    string command;
    while (true) {
        cout << "Enter command: ";
        getline(cin, command);
        if (command == "") {
            continue;
        }
        if (command == "EXIT") {
            return 0;
        }
        else if (command.find("INSERT") == 0) {
            insert(command, json_table);
        }
        else if (command.find("DELETE") == 0) {
            delet(command, json_table);
        }
        else if (command.find("SELECT") == 0) {
            select(command, json_table);
        }
        else {
            cerr << "Incorrect command" << endl;
        }
    }
    return 0;
}
