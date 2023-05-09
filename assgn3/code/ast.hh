#ifndef AST
#define AST

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

enum typeExp {
    OR_OP,
    AND_OP,
    EQ_OP_INT, EQ_OP_FLOAT,
    NE_OP_INT, NE_OP_FLOAT,
    LT_OP_INT, LT_OP_FLOAT,
    GT_OP_INT, GT_OP_FLOAT,
    LE_OP_INT, LE_OP_FLOAT,
    GE_OP_INT, GE_OP_FLOAT,
    PLUS_INT, PLUS_FLOAT,
    MINUS_INT, MINUS_FLOAT,
    MULT_INT, MULT_FLOAT,
    DIV_INT, DIV_FLOAT,
    TO_FLOAT,
    TO_INT,
    UMINUS,
    NOT,
    ADDRESS,
    DEREF,
    PP,
    EQ_OP, NE_OP, LT_OP, GT_OP, LE_OP, GE_OP, PLUS, MINUS, MULT, DIV
};

class expTypeTree_t {
    public:
        int strcnt;
        int da_lc;
        vector<int> array_sz;
        bool lval;
        int sec_strcnt;
        string basetype;
        bool menullptr;
        std::string print();
        int totpont();
        std::string boiled_print();
        expTypeTree_t();
        int downsize();
};

class abstract_astnode {
    public:
        virtual void print(int blanks) = 0;
        virtual void jaincode() = 0;
        enum typeExp astnode_type;
};

class statement_astnode : public abstract_astnode {
    public:
        virtual void print(int blanks) = 0;
        virtual void jaincode() = 0;
};

class exp_astnode : public abstract_astnode {
    public:
        virtual void print(int blanks) = 0;
        virtual void jaincode() = 0;
        expTypeTree_t ett;
        virtual string fnamer();
};

class ref_astnode : public exp_astnode {
    public:
        virtual void print(int blanks) = 0;
        virtual void jaincode() = 0;
};

class empty_astnode : public statement_astnode {
    public:
        void print(int blanks);
        void jaincode();
        empty_astnode();
};

class seq_astnode: public statement_astnode {
    public:
        vector<statement_astnode*> children;
        void print(int blanks);
        void jaincode();
        seq_astnode(vector<statement_astnode*> children);
};

class assignS_astnode: public statement_astnode {
    public:
        exp_astnode *left, *right;
        void print(int blanks);
        void jaincode();
        assignS_astnode(exp_astnode* left, exp_astnode* right);
};

class return_astnode: public statement_astnode {
    public:
        exp_astnode *retnode;
        void print(int blanks);
        void jaincode();
        return_astnode(exp_astnode* retnode);
};

class proccall_astnode: public statement_astnode {
    public:
        exp_astnode* fname;
        vector<exp_astnode*> args;
        void print(int blanks);
        void jaincode();
        proccall_astnode(vector<exp_astnode*> args);
};

class if_astnode: public statement_astnode {
    public:
        exp_astnode *cond;
        statement_astnode *then, *els;
        void print(int blanks);
        void jaincode();
        if_astnode(exp_astnode* cond, statement_astnode* then, statement_astnode* els);
};

class while_astnode: public statement_astnode {
    public:
        exp_astnode *cond;
        statement_astnode *then;
        void print(int blanks);
        void jaincode();
        while_astnode(exp_astnode* cond, statement_astnode* then);
};

class for_astnode: public statement_astnode {
    public:
        exp_astnode *init, *guard, *step;
        statement_astnode *then;
        void print(int blanks);
        void jaincode();
        for_astnode(exp_astnode* init, exp_astnode* guard, exp_astnode* step, statement_astnode* then);
};

class op_binary_astnode : public exp_astnode {
    public:
        exp_astnode *left, *right;
        void print(int blanks);
        void jaincode();
        op_binary_astnode(enum typeExp op, exp_astnode* left, exp_astnode* right);
};

class op_unary_astnode : public exp_astnode {
    public:
        exp_astnode *node;
        void print(int blanks);
        void jaincode();
        op_unary_astnode(enum typeExp op, exp_astnode* node);
};

class assignE_astnode : public exp_astnode {
    public:
        exp_astnode *left, *right;
        void print(int blanks);
        void jaincode();
        assignE_astnode(exp_astnode* left, exp_astnode* right);
};

class funcall_astnode : public exp_astnode {
    public:
        exp_astnode* fname;
        vector<exp_astnode*> args;
        void print(int blanks);
        void jaincode();
        funcall_astnode(vector<exp_astnode*> args);
};

class intconst_astnode : public exp_astnode {
    int x;
    public:
        void print(int blanks);
        void jaincode();
        intconst_astnode(int x );
};

class floatconst_astnode : public exp_astnode {
    float x;
    public:
        void print(int blanks);
        void jaincode();
        floatconst_astnode(float x);
};

class string_astnode : public exp_astnode {
    string x;
    public:
        void print(int blanks);
        void jaincode();
        string_astnode(string x);
};

class identifier_astnode : public ref_astnode {
    string x;
    public:
        void print(int blanks);
        void jaincode();
        identifier_astnode(string x);
        string fnamer();
};

class arrayref_astnode : public ref_astnode {
    public:
        exp_astnode *left, *right;
        void print(int blanks);
        void jaincode();
        arrayref_astnode(exp_astnode* left, exp_astnode* right);
};

class member_astnode : public ref_astnode {
    public:
        exp_astnode* val;
        ref_astnode* id;
        void print(int blanks);
        void jaincode();
        member_astnode(exp_astnode* val, ref_astnode* id);
};

class arrow_astnode : public ref_astnode {
    public:
        exp_astnode* val;
        ref_astnode* id;
        void print(int blanks);
        void jaincode();
        arrow_astnode(exp_astnode* val, ref_astnode* id);
};

#endif
