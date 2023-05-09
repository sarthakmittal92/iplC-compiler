#include "symb.hh"

void SymbTab::print() {
    cout << "[" << endl;
    for (auto it = Entries.begin(); it != Entries.end(); ++it) {
        cout << "[" << endl;
        cout << "\"" << it->first << "\", " << endl;
        cout << "\"" << it->second.flag << "\", " << endl;
        cout << "\"" << it->second.scope << "\", " << endl;
        cout << it->second.size <<", "<< endl;
        cout << it->second.offset <<", "<< endl;
        cout << "\"" << it->second.type.print() << "\"" ;
        if (next(it,1) != Entries.end())
            cout << "]," << endl;
    }
    if (!Entries.empty())
        cout << "]" << endl;
    cout << "]"; 
}

void SymbTab::printgst() {
    cout << "[" << endl;
    for (auto it = Entries.begin(); it != Entries.end(); ++it) {
        cout << "[" << endl;
        cout << "\"" << it->first << "\", " << endl;
        cout << "\"" << it->second.flag << "\", " << endl;
        cout << "\"" << it->second.scope << "\", " << endl;
        cout << it->second.size <<", "<< endl;
        if (it->second.flag == "fun")
        {
            cout << it->second.offset <<", "<< endl;
            cout << "\"" << it->second.type.print() << "\"" ;
        }
        else
        {
            cout << "\"-\"," << endl << "\"-\"" << endl;
        }
        if (next(it,1) != Entries.end())
            cout << "]," << endl;
    }
    if (!Entries.empty())
        cout << "]" << endl;
    cout << "]";
}

string typeTree::print() {
    stringstream ss;
    ss<<basetype;
    for (int i=0; i<strcnt; i++) {
        ss << "*";
    }
    for (int i=0; i<arrayspec.size(); i++) {
        ss<<"["<<arrayspec[i]<<"]";
    }
    return ss.str();
}

int typeTree::totpont() {
    return strcnt + arrayspec.size();
}

string typeTree::boiled_print() {
    stringstream ss;
    ss<<basetype;
    int bistrcnt = 0;
    int monostrcnt = strcnt;
    for (int i=0; i<strcnt; i++) {
        ss << "*";
    }
    if (arrayspec.size()==0)
        return ss.str();
    else if (arrayspec.size()==1)
    {
        ss<<"*";
        return ss.str();
    }
    else
    {
        ss<<"(*)";
        for (int i=1; i<arrayspec.size(); i++) {
            ss<<"["<<arrayspec[i]<<"]";
        }
        return ss.str();
    }
}