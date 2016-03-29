//
// Created by 王 on 16/3/13.
//
#include <vector>
#include <string>
#include <stdlib.h>
using namespace std;
void strToVector(char* buff, const string &delimiter, vector<int>* result) {
    string buffer = buff;
    size_t lastDelimiter = 0;
    size_t nextDelimiter = buffer.find_first_of(delimiter,lastDelimiter);
    while (nextDelimiter != string::npos) {
        result->push_back(atoi(buffer.substr(lastDelimiter, nextDelimiter-lastDelimiter).c_str()));
        lastDelimiter = nextDelimiter + 1;
        nextDelimiter = buffer.find_first_of(delimiter,lastDelimiter);
    }
    if (buffer.length() > lastDelimiter) {
        result->push_back(atoi(buffer.substr(lastDelimiter, buffer.length()-lastDelimiter).c_str()));
    }
}