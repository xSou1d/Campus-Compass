#include <iostream>
#include <string>
#include "CampusCompass.h"

using namespace std;

int main() {
    CampusCompass compass;

    // Load CSV data
    if (!compass.ParseCSV("data/edges.csv", "data/classes.csv")) {
        cout << "unsuccessful" << endl;
        return 0;
    }

    int no_of_lines;
    cin >> no_of_lines;
    cin.ignore();

    string command;
    for (int i = 0; i < no_of_lines; i++) {
        getline(cin, command);
        compass.ParseCommand(command);
    }

    return 0;
}