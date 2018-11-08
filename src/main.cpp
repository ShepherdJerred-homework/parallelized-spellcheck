#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <thread>

using std::lock_guard;
using std::mutex;
using std::thread;
using std::string;
using std::ifstream;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::transform;

void add(mutex &m, vector<string> &v, string s) {
    lock_guard l(m);
    v.push_back(s);
    cout << "Added " << s << endl;
}

// https://stackoverflow.com/questions/3450860/check-if-a-stdvector-contains-a-certain-object
void search(mutex &m, vector<string> &v, string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    lock_guard l(m);
    if (std::find(v.begin(), v.end(), s) != v.end()) {
        cout << s << " is correct" << endl;
    } else {
        cout << s << " is incorrect" << endl;
    }
}

void closeThreads(vector<thread> &v) {
    for (auto &thread : v) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

int main(int argc, char **argv) {
    string dictionaryFileName(argv[1]);
    ifstream dictionaryFile(dictionaryFileName);

    mutex m;
    vector<string> dictionary;
    string word;
    while (dictionaryFile >> word) {
        // https://stackoverflow.com/questions/313970/how-to-convert-stdstring-to-lower-case
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        dictionary.push_back(word);
    }

    cout << "Waiting for command..." << endl;

    vector<thread> threads;

    while (true) {
        string command;
        cin >> command;

        closeThreads(threads);

        if (command == "end") {
            closeThreads(threads);
            exit(0);
        } else if (command.rfind("add", 0) == 0) {
            cin >> word;
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            threads.emplace_back(thread(add, ref(m), ref(dictionary), word));
        } else {
            threads.emplace_back(thread(search, ref(m), ref(dictionary), command));
        }
    }
}
