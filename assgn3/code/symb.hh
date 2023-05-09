#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

struct SymbTab;

struct LC {
    string da_str;
    string da_func;
};

struct typeTree {
    int strcnt;
    vector<int> arrayspec;
    string basetype;
    string print();
    int totpont();
    string boiled_print();
};

struct SymbTabEntry {
    string flag;
    string scope;
    int size;
    int offset;
    typeTree type;
    SymbTab *symbtab;
};

struct SymbTab {
    void printgst();
    void print();
    map<string, SymbTabEntry> Entries;
};

struct declaration_list {
};

struct declaration {
    string basetype;
};

struct declarator_list {
    string basetype;
};

struct declarator {
    string basetype;
    int strcnt;
};

struct declarator_arr {
    vector<int> arr_held;
    string identifier;
    int sz_mult;
};