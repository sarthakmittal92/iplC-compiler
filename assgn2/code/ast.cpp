#include "ast.hh"
#include <cstdarg>
#include <sstream>
using namespace std;

const char* opMap[] = {
    "OR_OP",
    "AND_OP",
    "EQ_OP_INT", "EQ_OP_FLOAT",
    "NE_OP_INT", "NE_OP_FLOAT",
    "LT_OP_INT", "LT_OP_FLOAT",
    "GT_OP_INT", "GT_OP_FLOAT",
    "LE_OP_INT", "LE_OP_FLOAT",
    "GE_OP_INT", "GE_OP_FLOAT",
    "PLUS_INT", "PLUS_FLOAT",
    "MINUS_INT", "MINUS_FLOAT",
    "MULT_INT", "MULT_FLOAT",
    "DIV_INT", "DIV_FLOAT",
    "TO_FLOAT",
    "TO_INT",
    "UMINUS",
    "NOT",
    "ADDRESS",
    "DEREF",
    "PP",
    "EQ_OP", "NE_OP", "LT_OP", "GT_OP", "LE_OP", "GE_OP", "PLUS", "MINUS", "MULT", "DIV"
};

void printAst(const char *astname, const char *fmt...) // fmt is a format string that tells about the type of the arguments.
{   
	typedef vector<abstract_astnode *>* pv;
	va_list args;
	va_start(args, fmt);
	if ((astname != NULL) && (astname[0] != '\0'))
	{
		cout << "{ ";
		cout << "\"" << astname << "\"" << ": ";
	}
	cout << "{" << endl;
	while (*fmt != '\0')
	{
		if (*fmt == 'a')
		{
			char * field = va_arg(args, char *);
			abstract_astnode *a = va_arg(args, abstract_astnode *);
			cout << "\"" << field << "\": " << endl;
			
			a->print(0);
		}
		else if (*fmt == 's')
		{
			char * field = va_arg(args, char *);
			char *str = va_arg(args, char *);
			cout << "\"" << field << "\": ";

			cout << "\"" << str << "\"" << endl;
		}
		else if (*fmt == 'i')
		{
			char * field = va_arg(args, char *);
			int i = va_arg(args, int);
			cout << "\"" << field << "\": ";

			cout << i;
		}
		else if (*fmt == 'f')
		{
			char * field = va_arg(args, char *);
			double f = va_arg(args, double);
			cout << "\"" << field << "\": ";
			cout << f;
		}
		else if (*fmt == 'l')
		{
			char * field = va_arg(args, char *);
			pv f =  va_arg(args, pv);
			cout << "\"" << field << "\": ";
			cout << "[" << endl;
			for (int i = 0; i < (int)f->size(); ++i)
			{
				(*f)[i]->print(0);
				if (i < (int)f->size() - 1)
					cout << "," << endl;
				else
					cout << endl;
			}
			cout << endl;
			cout << "]" << endl;
		}
		++fmt;
		if (*fmt != '\0')
			cout << "," << endl;
	}
	cout << "}" << endl;
	if ((astname != NULL) && (astname[0] != '\0'))
		cout << "}" << endl;
	va_end(args);
}

std::string expTypeTree_t::print() {
	stringstream s;
	s<<basetype;
	for (int i=0; i<strcnt; ++i)
	{
		s<<"*";
	}
	if (sec_strcnt)
	{
		s<<"(*)";
	}
	for (int i=0; i<array_sz.size(); i++) {
        s<<"["<<array_sz[i]<<"]";
    }
	return s.str();
}

int expTypeTree_t::totpont() {
	return strcnt + array_sz.size() + sec_strcnt;
}

std::string expTypeTree_t::boiled_print() {
	stringstream ss;
    ss<<basetype;
    int monostrcnt = strcnt;
    for (int i=0; i<strcnt; i++) {
        ss << "*";
    }
    if (array_sz.size()==0)
        return ss.str();
    else if (sec_strcnt!=0) {
		ss<<"(*)";
		for (int i=0; i<array_sz.size(); i++) {
            ss<<"["<<array_sz[i]<<"]";
        }
		return ss.str();
	}
	else if (array_sz.size()==1) {
		ss<<"*";
		return ss.str();
	}
	else {
		ss<<"(*)";
		for (int i=1; i<array_sz.size(); i++) {
            ss<<"["<<array_sz[i]<<"]";
        }
		return ss.str();
	}
}

expTypeTree_t::expTypeTree_t()
{
	menullptr = false;
}

empty_astnode::empty_astnode () {}

void empty_astnode::print (int blanks) {
    cout << "\"empty\"" << endl;
}

seq_astnode::seq_astnode (vector<statement_astnode*> children) {
    this->children = children;
}

void seq_astnode::print (int blanks) {
    printAst("\0","l","seq",this->children,"\0");
}

assignS_astnode::assignS_astnode (exp_astnode* left, exp_astnode* right) {
    this->left = left;
    this->right = right;
}

void assignS_astnode::print (int blanks) {
    printAst("assignS","aa","left",this->left,"right",this->right,"\0");
}

return_astnode::return_astnode (exp_astnode* retnode) {
    this->retnode = retnode;
}

void return_astnode::print (int blanks) {
    printAst("\0","a","return",this->retnode,"\0");
}

proccall_astnode::proccall_astnode (vector<exp_astnode*> args) {
	this->fname = args[0];
	vector<exp_astnode*> temp;
	for (int i = 1; i < args.size(); i++) {
		temp.push_back(args[i]);
	}
	this->args = temp;
}

void proccall_astnode::print (int blanks) {
	printAst("proccall","al","fname",this->fname,"params",this->args,"\0");
}

if_astnode::if_astnode (exp_astnode* cond, statement_astnode* then, statement_astnode* els) {
    this->cond = cond;
    this->then = then;
    this->els = els;
}

void if_astnode::print (int blanks) {
    printAst("if","aaa","cond",this->cond,"then",this->then,"else",this->els,"\0");
}

while_astnode::while_astnode (exp_astnode* cond, statement_astnode* then) {
    this->cond = cond;
    this->then = then;
}

void while_astnode::print (int blanks) {
    printAst("while","aa","cond",this->cond,"stmt",this->then,"\0");
}

for_astnode::for_astnode (exp_astnode* init, exp_astnode* guard, exp_astnode* step, statement_astnode* then) {
    this->init = init;
    this->guard = guard;
    this->step = step;
    this->then = then;
}

void for_astnode::print (int blanks) {
    printAst("for","aaaa","init",this->init,"guard",this->guard,"step",this->step,"body",this->then,"\0");
}

op_binary_astnode::op_binary_astnode (enum typeExp op, exp_astnode* left, exp_astnode* right) {
    this->astnode_type = op;
    this->left = left;
    this->right = right;
}

void op_binary_astnode::print (int blanks) {
    printAst("op_binary","saa","op",opMap[this->astnode_type],"left",this->left,"right",this->right,"\0");
}

op_unary_astnode::op_unary_astnode (enum typeExp op, exp_astnode* node) {
    this->astnode_type = op;
    this->node = node;
}

void op_unary_astnode::print (int blanks) {
    printAst("op_unary","sa","op",opMap[this->astnode_type],"child",this->node,"\0");
}

assignE_astnode::assignE_astnode (exp_astnode* left, exp_astnode* right) {
    this->left = left;
    this->right = right;
}

void assignE_astnode::print (int blanks) {
    printAst("assignE","aa","left",this->left,"right",this->right,"\0");
}

funcall_astnode::funcall_astnode (vector<exp_astnode*> args) {
	this->fname = args[0];
	vector<exp_astnode*> temp;
    for (int i = 1; i < args.size(); i++) {
		temp.push_back(args[i]);
	}
	this->args = temp;
}

void funcall_astnode::print (int blanks) {
    printAst("funcall","al","fname",this->fname,"params",this->args,"\0");
}

intconst_astnode::intconst_astnode (int x) {
    this->x = x;
}

void intconst_astnode::print (int blanks) {
    printAst("\0","i","intconst",this->x,"\0");
}

floatconst_astnode::floatconst_astnode (float x) {
    this->x = x;
}

void floatconst_astnode::print (int blanks) {
    printAst("\0","f","floatconst",this->x,"\0");
}

string_astnode::string_astnode (string s) {
    this->x = s.substr(1, s.size()-2);
}

void string_astnode::print (int blanks) {
    printAst("\0","s","stringconst",(this->x).c_str(),"\0");
}

identifier_astnode::identifier_astnode (string x) {
    this->x = x;
}

void identifier_astnode::print (int blanks) {
    printAst("\0","s","identifier",(this->x).c_str(),"\0");
}

arrayref_astnode::arrayref_astnode (exp_astnode* left, exp_astnode* right) {
    this->left = left;
    this->right = right;
}

void arrayref_astnode::print (int blanks) {
    printAst("arrayref","aa","array",this->left,"index",this->right,"\0");
}

member_astnode::member_astnode (exp_astnode* val, ref_astnode* id) {
    this->val = val;
    this->id = id;
}

void member_astnode::print (int blanks) {
    printAst("member","aa","struct",this->val,"field",this->id,"\0");
}

arrow_astnode::arrow_astnode (exp_astnode* val, ref_astnode* id) {
    this->val = val;
    this->id = id;
}

void arrow_astnode::print (int blanks) {
    printAst("arrow","aa","pointer",this->val,"field",this->id,"\0");
}
