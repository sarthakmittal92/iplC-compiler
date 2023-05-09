#include "ast.hh"
#include <cstdarg>
#include <sstream>
#include "symb.hh"
using namespace std;

extern std::map<string, int> bs_size;
string dis_func;
extern SymbTab gst;
int label_index = 1;

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

int expTypeTree_t::downsize() {
	if (sec_strcnt!=0) {
		int base_sz = (strcnt==0?(bs_size[basetype]):(4));
		for (int i=0; i<array_sz.size(); ++i) {
			base_sz*=array_sz[i];
		}
		return base_sz;
	}
	else if (!array_sz.empty()) {
		int base_sz = (strcnt==0?(bs_size[basetype]):(4));
		for (int i=1; i<array_sz.size(); ++i) {
			base_sz*=array_sz[i];
		}
		return base_sz;
	}
	else {
		int base_sz = (strcnt==1?(bs_size[basetype]):(4));
		return base_sz;
	}
}

expTypeTree_t::expTypeTree_t()
{
	menullptr = false;
	da_lc = -1;
}

string exp_astnode::fnamer() {return "";}

empty_astnode::empty_astnode () {}

void empty_astnode::print (int blanks) {
    cout << "\"empty\"" << endl;
}

void empty_astnode::jaincode() {}

seq_astnode::seq_astnode (vector<statement_astnode*> children) {
    this->children = children;
}

void seq_astnode::print (int blanks) {
    printAst("\0","l","seq",this->children,"\0");
}

void seq_astnode::jaincode() {
	for (int i=0; i<children.size(); ++i)
	{
		children[i]->jaincode();
	}
}

assignS_astnode::assignS_astnode (exp_astnode* left, exp_astnode* right) {
    this->left = left;
    this->right = right;
}

void assignS_astnode::print (int blanks) {
    printAst("assignS","aa","left",this->left,"right",this->right,"\0");
}

void assignS_astnode::jaincode() {
	expTypeTree_t mytemp = left->ett;
	right->jaincode();
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tpushl\t\%eax" << endl;
	else {
		cout << "\tsubl\t$"<< bs_size[mytemp.basetype] << ", \%esp" << endl;
		for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
		{
			cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
			cout << "\tmovl\t\%ebx, " << i << "(\%esp)" << endl;
		}
	}
	left->jaincode();
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tpopl\t\%eax" << endl;
	else
		cout << "\tmovl\t\%esp, \%eax" << endl;
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tmovl\t\%eax, (\%ecx)" << endl;
	else {
		for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
		{
			cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
			cout << "\tmovl\t\%ebx, " << i << "(\%ecx)" << endl;
		}
		cout << "\taddl\t$" << bs_size[mytemp.basetype] << ", \%esp" << endl;
	}
}

return_astnode::return_astnode (exp_astnode* retnode) {
    this->retnode = retnode;
}

void return_astnode::print (int blanks) {
    printAst("\0","a","return",this->retnode,"\0");
}

void return_astnode::jaincode() {
	if (dis_func == "main") {
		retnode->jaincode();
		cout << "\tleave" << endl;
		cout << "\tret" << endl;
		return;
	}
	retnode->jaincode();
	expTypeTree_t mytemp = retnode->ett;
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tmovl\t\%eax, " << gst.Entries[dis_func].offset << "(\%ebp)" << endl;
	else {
		for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
		{
			cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
			cout << "\tmovl\t\%ebx, " << gst.Entries[dis_func].offset + i << "(\%ebp)" << endl;
		}
	}
	cout << "\tleave" << endl;
	cout << "\tret" << endl;
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

void proccall_astnode::jaincode() {
	int toskip = 0;
	if (fname->fnamer()==string("printf")) {
		for (int i = args.size()-1; i >= 0; i--) {
			args[i]->jaincode();
			expTypeTree_t mytemp = args[i]->ett;
			if (mytemp.print() == "char*") {
				cout << "\tpushl\t$.LC" << mytemp.da_lc << endl;
				toskip += 4;
			}
			else if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct")) {
				cout << "\tpushl\t\%eax" << endl;
				toskip+=4;
			}
			else {
				cout << "\tsubl\t$"<< bs_size[mytemp.basetype] << ", \%esp" << endl;
				for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
				{
					cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
					cout << "\tmovl\t\%ebx, " << i << "(\%esp)" << endl;
				}
				toskip+=bs_size[mytemp.basetype];
			}
		}
		cout << "\tcall printf" << endl;
		cout << "\taddl\t$" << toskip << ", \%esp" << endl;
		return;
	}
	if (gst.Entries[fname->fnamer()].type.basetype!="void") {
		cout << "\tsubl\t$" << bs_size[gst.Entries[fname->fnamer()].type.basetype] << ", \%esp" << endl;
		toskip += bs_size[gst.Entries[fname->fnamer()].type.basetype];
	}
	for (int i = 0; i < args.size(); i++) {
		args[i]->jaincode();
		expTypeTree_t mytemp = args[i]->ett;
		if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct")) {
			cout << "\tpushl\t\%eax" << endl;
			toskip += 4;
		}
		else {
			cout << "\tsubl\t$"<< bs_size[mytemp.basetype] << ", \%esp" << endl;
			for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
			{
				cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
				cout << "\tmovl\t\%ebx, " << i << "(\%esp)" << endl;
			}
			toskip += bs_size[mytemp.basetype];
		}
	}
	cout << "\tsubl\t$4, \%esp" << endl;
	cout << "\tcall " << fname->fnamer() << endl;
	cout << "\taddl\t$" << toskip << ", \%esp" << endl;
	cout << "\taddl\t$4, \%esp" << endl;
}

if_astnode::if_astnode (exp_astnode* cond, statement_astnode* then, statement_astnode* els) {
    this->cond = cond;
    this->then = then;
    this->els = els;
}

void if_astnode::print (int blanks) {
    printAst("if","aaa","cond",this->cond,"then",this->then,"else",this->els,"\0");
}

void if_astnode::jaincode() {
	cond->jaincode();
	cout << "\tcmpl\t$0, \%eax" << endl;
	int temp_label_1 = label_index;
	++label_index;
	cout << "\tje\t.L"<<temp_label_1 << endl;
	then->jaincode();
	int temp_label_2 = label_index;
	++label_index;
	cout << "\tjmp\t.L"<<temp_label_2<<endl;
	cout << ".L"<<temp_label_1<<":"<<endl;
	els->jaincode();
	cout << ".L"<<temp_label_2<<":"<<endl;
}

while_astnode::while_astnode (exp_astnode* cond, statement_astnode* then) {
    this->cond = cond;
    this->then = then;
}

void while_astnode::print (int blanks) {
    printAst("while","aa","cond",this->cond,"stmt",this->then,"\0");
}

void while_astnode::jaincode() {
	int temp_label_1 = label_index;
	++label_index;
	cout << ".L" << temp_label_1 << ":" << endl;
	cond->jaincode();
	cout << "\tcmpl\t$0, \%eax" << endl;
	int temp_label_2 = label_index;
	++label_index;
	cout << "\tje\t.L"<<temp_label_2 << endl;
	then->jaincode();
	cout << "\tjmp\t.L" << temp_label_1 << endl;
	cout << ".L" << temp_label_2 << ":" << endl;
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

void for_astnode::jaincode() {
	init->jaincode();
	int temp_label_1 = label_index;
	++label_index;
	cout << ".L" << temp_label_1 << ":" << endl;
	guard->jaincode();
	cout << "\tcmpl\t$0, \%eax" << endl;
	int temp_label_2 = label_index;
	++label_index;
	cout << "\tje\t.L"<<temp_label_2 << endl;
	then->jaincode();
	step->jaincode();
	cout << "\tjmp\t.L" << temp_label_1 << endl;
	cout << ".L" << temp_label_2 << ":" << endl;
}

op_binary_astnode::op_binary_astnode (enum typeExp op, exp_astnode* left, exp_astnode* right) {
    this->astnode_type = op;
    this->left = left;
    this->right = right;
}

void op_binary_astnode::print (int blanks) {
    printAst("op_binary","saa","op",opMap[this->astnode_type],"left",this->left,"right",this->right,"\0");
}

void op_binary_astnode::jaincode() {
	switch (astnode_type)
	{
		case OR_OP:
		{
			int temp_label_1 = label_index;
			++label_index;
			left->jaincode();
			cout << "\tcmpl\t$0, \%eax" << endl;
			cout << "\tjne\t.L" << temp_label_1 << endl;
			right->jaincode();
			cout << ".L" << temp_label_1 << ":\tcmpl\t$0, \%eax" << endl;
			int temp_label_2 = label_index;
			++label_index;
			cout << "\tje\t.L" << temp_label_2 << endl;
			cout << "\tmovl\t$1, \%eax" << endl;
			cout << ".L" << temp_label_2 << ":" << endl;
			break;
		}
		case AND_OP:
		{
			int temp_label_1 = label_index;
			++label_index;
			left->jaincode();
			cout << "\tcmpl\t$0, \%eax" << endl;
			cout << "\tje\t.L" << temp_label_1 << endl;
			right->jaincode();
			cout << ".L" << temp_label_1 << ":\tcmpl\t$0, \%eax" << endl;
			int temp_label_2 = label_index;
			++label_index;
			cout << "\tje\t.L" << temp_label_2 << endl;
			cout << "\tmovl $1, \%eax" << endl;
			cout << ".L" << temp_label_2 << ":" << endl;
			break;
		}
		case NE_OP_INT:
		{
			left->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			right->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tsubl\t\%ebx, \%eax" << endl;
			cout << "\tcmpl\t$0, \%eax" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tje\t.L" << temp_label_1 << endl;
			cout << "\tmovl $1, \%eax" << endl;
			cout << ".L" << temp_label_1 << ":" << endl;
			break;
		}
		case EQ_OP_INT:
		{
			left->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			right->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tsubl\t\%eax, \%ebx" << endl;
			cout << "\tcmpl\t$0, \%ebx" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tje\t.L" << temp_label_1 << endl;
			cout << "\tmovl $1, \%ebx" << endl;
			cout << ".L" << temp_label_1 << ":\tmovl\t$1, \%eax" << endl;
			cout << "\tsubl\t\%ebx, \%eax" << endl;
			break;
		}
		case LT_OP_INT:
		{
			left->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			right->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tcmpl\t\%ebx, \%eax" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tjg\t.L"<<temp_label_1 << endl;
			cout << "\tmovl\t$0, \%eax" << endl;
			int temp_label_2 = label_index;
			++label_index;
			cout << "\tjmp\t.L" << temp_label_2 << endl;
			cout << ".L"<<temp_label_1<<":\tmovl\t$1, \%eax" << endl;
			cout << ".L"<<temp_label_2<<":"<<endl;
			break;
		}
		case GT_OP_INT:
		{
			left->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			right->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tcmpl\t\%ebx, \%eax" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tjl\t.L"<<temp_label_1 << endl;
			cout << "\tmovl\t$0, \%eax" << endl;
			int temp_label_2 = label_index;
			++label_index;
			cout << "\tjmp\t.L" << temp_label_2 << endl;
			cout << ".L"<<temp_label_1<<":\tmovl\t$1, \%eax" << endl;
			cout << ".L"<<temp_label_2<<":"<<endl;
			break;
		}
		case LE_OP_INT:
		{
			left->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			right->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tcmpl\t\%ebx, \%eax" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tjge\t.L"<<temp_label_1 << endl;
			cout << "\tmovl\t$0, \%eax" << endl;
			int temp_label_2 = label_index;
			++label_index;
			cout << "\tjmp\t.L" << temp_label_2 << endl;
			cout << ".L"<<temp_label_1<<":\tmovl\t$1, \%eax" << endl;
			cout << ".L"<<temp_label_2<<":"<<endl;
			break;
		}
		case GE_OP_INT:
		{
			left->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			right->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tcmpl\t\%ebx, \%eax" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tjle\t.L"<<temp_label_1 << endl;
			cout << "\tmovl\t$0, \%eax" << endl;
			int temp_label_2 = label_index;
			++label_index;
			cout << "\tjmp\t.L" << temp_label_2 << endl;
			cout << ".L"<<temp_label_1<<":\tmovl\t$1, \%eax" << endl;
			cout << ".L"<<temp_label_2<<":"<<endl;
			break;
		}
		case PLUS_INT:
		{
			expTypeTree_t lhst = left->ett;
			expTypeTree_t rhst = right->ett;
			if (lhst.totpont()!=0)
			{
				right->jaincode();
				cout << "\timul\t$" << lhst.downsize() << ", \%eax" << endl;
				cout << "\tpushl\t\%eax"<<endl;
				left->jaincode();
				cout << "\tpopl\t\%ebx" << endl;
				cout << "\taddl\t\%ebx, \%eax" << endl;
			}
			else if (rhst.totpont()!=0)
			{
				left->jaincode();
				cout << "\timul\t$" << rhst.downsize() << ", \%eax" << endl;
				cout << "\tpushl\t\%eax"<<endl;
				right->jaincode();
				cout << "\tpopl\t\%ebx" << endl;
				cout << "\taddl\t\%ebx, \%eax" << endl;
			}
			else
			{
				left->jaincode();
				cout << "\tpushl\t\%eax" << endl;
				right->jaincode();
				cout << "\tpopl\t\%ebx" << endl;
				cout << "\taddl\t\%ebx, \%eax" << endl;
			}
			break;
		}
		case MINUS_INT:
		{
			expTypeTree_t lhst = left->ett;
			expTypeTree_t rhst = right->ett;
			if (rhst.totpont()!=0)
			{
				right->jaincode();
				cout << "\tpushl\t\%eax" << endl;
				left->jaincode();
				cout << "\tpopl\t\%ebx" << endl;
				cout << "\taddl\t\%ebx, \%eax" << endl;
				cout << "\tcltd" << endl;
				cout << "\tmovl\t$" << rhst.downsize() << ", \%ebx" << endl;
				cout << "\tidiv\t\%ebx" << endl;
			}
			else if (lhst.totpont()!=0)
			{
				right->jaincode();
				cout << "\timul\t$" << lhst.downsize() << ", \%eax" << endl;
				cout << "\tpushl\t\%eax"<<endl;
				left->jaincode();
				cout << "\tpopl\t\%ebx" << endl;
				cout << "\tsubl\t\%ebx, \%eax" << endl;
			}
			else
			{
				right->jaincode();
				cout << "\tpushl\t\%eax" << endl;
				left->jaincode();
				cout << "\tpopl\t\%ebx" << endl;
				cout << "\tsubl\t\%ebx, \%eax" << endl;
			}
			break;
		}
		case MULT_INT:
		{
			right->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			left->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\timul\t\%ebx, \%eax" << endl;
			break;
		}
		case DIV_INT:
		{
			right->jaincode();
			cout << "\tpushl\t\%eax" << endl;
			left->jaincode();
			cout << "\tpopl\t\%ebx" << endl;
			cout << "\tcltd" << endl;
			cout << "\tidiv\t\%ebx" << endl;
			break;
		}
	}
}

op_unary_astnode::op_unary_astnode (enum typeExp op, exp_astnode* node) {
    this->astnode_type = op;
    this->node = node;
}

void op_unary_astnode::print (int blanks) {
    printAst("op_unary","sa","op",opMap[this->astnode_type],"child",this->node,"\0");
}

void op_unary_astnode::jaincode() {
	switch (astnode_type)
	{
		case UMINUS:
		{
			node->jaincode();
			cout << "\tnegl\t\%eax" << endl;
			break;
		}
		case NOT:
		{
			node->jaincode();
			cout << "\tcmpl\t$0, \%eax" << endl;
			int temp_label_1 = label_index;
			++label_index;
			cout << "\tje\t.L"<<temp_label_1<<endl;
			cout << "\tmovl\t$1, \%eax" << endl;
			cout << ".L"<<temp_label_1<<":\tmovl\t\%eax, \%ebx" << endl;
			cout << "\tmovl\t$1, \%eax" << endl;
			cout << "\tsubl\t\%ebx, \%eax" << endl;
			break;
		}
		case ADDRESS:
		{
			node->jaincode();
			cout << "\tmovl\t\%ecx, \%eax" << endl;
			break;
		}
		case DEREF:
		{
			node->jaincode();
			expTypeTree_t mytemp = node->ett;
			if (mytemp.sec_strcnt!=0) {
				cout << "\tmovl\t\%eax, \%ecx" << endl;
			}
			else if (mytemp.array_sz.size()>1) {
				cout << "\tmovl\t\%eax, \%ecx" << endl;
			}
			else if (mytemp.basetype.substr(0,6)=="struct" && mytemp.totpont() == 1){
				cout << "\tmovl\t\%eax, \%ecx" << endl;
			}
			else {
				cout << "\tmovl\t\%eax, \%ecx" << endl;
				cout << "\tmovl\t(\%eax), \%eax" << endl;
			}
			break;
		}
		case PP:
		{
			node->jaincode();
			cout << "\taddl\t$1, (\%ecx)" << endl;
			break;
		}
	}
}

assignE_astnode::assignE_astnode (exp_astnode* left, exp_astnode* right) {
    this->left = left;
    this->right = right;
}

void assignE_astnode::print (int blanks) {
    printAst("assignE","aa","left",this->left,"right",this->right,"\0");
}

void assignE_astnode::jaincode() {
	expTypeTree_t mytemp = left->ett;
	right->jaincode();
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tpushl\t\%eax" << endl;
	else {
		cout << "\tsubl\t$"<< bs_size[mytemp.basetype] << ", \%esp" << endl;
		for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
		{
			cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
			cout << "\tmovl\t\%ebx, " << i << "(\%esp)" << endl;
		}
	}
	left->jaincode();
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tpopl\t\%eax" << endl;
	else
		cout << "\tmovl\t\%esp, \%eax" << endl;
	if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
		cout << "\tmovl\t\%eax, (\%ecx)" << endl;
	else {
		for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
		{
			cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
			cout << "\tmovl\t\%ebx, " << i << "(\%ecx)" << endl;
		}
		cout << "\taddl\t$" << bs_size[mytemp.basetype] << ", \%esp" << endl;
	}
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

void funcall_astnode::jaincode() {
	int toskip = 0;
	if (fname->fnamer()==string("printf")) {
		for (int i = args.size()-1; i >= 0; i--) {
			args[i]->jaincode();
			expTypeTree_t mytemp = args[i]->ett;
			if (mytemp.print() == "char*") {
				cout << "\tpushl\t$.LC" << mytemp.da_lc << endl;
				toskip += 4;
			}
			else if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct")) {
				cout << "\tpushl\t\%eax" << endl;
				toskip+=4;
			}
			else {
				cout << "\tsubl\t$"<< bs_size[mytemp.basetype] << ", \%esp" << endl;
				for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
				{
					cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
					cout << "\tmovl\t\%ebx, " << i << "(\%esp)" << endl;
				}
				toskip+=bs_size[mytemp.basetype];
			}
		}
		cout << "\tcall printf" << endl;
		cout << "\taddl\t$" << toskip << ", \%esp" << endl;
		return;
	}
	if (gst.Entries[fname->fnamer()].type.basetype!="void") {
		cout << "\tsubl\t$" << bs_size[gst.Entries[fname->fnamer()].type.basetype] << ", \%esp" << endl;
		toskip += bs_size[gst.Entries[fname->fnamer()].type.basetype];
	}
	for (int i = 0; i < args.size(); i++) {
		args[i]->jaincode();
		expTypeTree_t mytemp = args[i]->ett;
		if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct")) {
			cout << "\tpushl\t\%eax" << endl;
			toskip += 4;
		}
		else {
			cout << "\tsubl\t$"<< bs_size[mytemp.basetype] << ", \%esp" << endl;
			for (int i=0; i<bs_size[mytemp.basetype]; i+=4)
			{
				cout << "\tmovl\t" << i << "(\%eax), \%ebx" << endl;
				cout << "\tmovl\t\%ebx, " << i << "(\%esp)" << endl;
			}
			toskip += bs_size[mytemp.basetype];
		}
	}
	cout << "\tsubl\t$4, \%esp" << endl;
	cout << "\tcall\t" << fname->fnamer() << endl;
	cout << "\taddl\t$" << toskip << ", \%esp" << endl;
	cout << "\taddl\t$4, \%esp" << endl;
	cout << "\tleal\t-" << bs_size[gst.Entries[fname->fnamer()].type.basetype] << "(\%esp), \%eax" << endl;
	if (gst.Entries[fname->fnamer()].type.basetype.substr(0,6)!="struct")
		cout << "\tmovl\t(\%eax), \%eax" << endl;
}

intconst_astnode::intconst_astnode (int x) {
    this->x = x;
}

void intconst_astnode::print (int blanks) {
    printAst("\0","i","intconst",this->x,"\0");
}

void intconst_astnode:: jaincode() {
	cout << "\tmovl\t$" << x << ", \%eax" << endl;
}

floatconst_astnode::floatconst_astnode (float x) {
    this->x = x;
}

void floatconst_astnode::print (int blanks) {
    printAst("\0","f","floatconst",this->x,"\0");
}

void floatconst_astnode::jaincode() {}

string_astnode::string_astnode (string s) {
    this->x = s.substr(1, s.size()-2);
}

void string_astnode::print (int blanks) {
    printAst("\0","s","stringconst",(this->x).c_str(),"\0");
}

void string_astnode::jaincode() {}

identifier_astnode::identifier_astnode (string x) {
    this->x = x;
}

void identifier_astnode::print (int blanks) {
    printAst("\0","s","identifier",(this->x).c_str(),"\0");
}

void identifier_astnode::jaincode() {
	cout << "\tleal\t" << gst.Entries[dis_func].symbtab->Entries[x].offset << "(\%ebp), \%ecx" << endl;
	if (gst.Entries[dis_func].symbtab->Entries[x].offset>0 && !ett.array_sz.empty())
	{
		cout << "\tmovl\t(\%ecx), \%ecx" << endl;
	}
	expTypeTree_t mytemp = ett;
	if (mytemp.array_sz.size()!=0) {
		cout << "\tmovl\t\%ecx, \%eax" << endl;
	}
	else if (mytemp.basetype.substr(0,6)=="struct" && mytemp.totpont() == 0){
		cout << "\tmovl\t\%ecx, \%eax" << endl;
	}
	else {
		cout << "\tmovl\t(\%ecx), \%eax" << endl;
	}
}

string identifier_astnode::fnamer() {
	return x;
}

arrayref_astnode::arrayref_astnode (exp_astnode* left, exp_astnode* right) {
    this->left = left;
    this->right = right;
}

void arrayref_astnode::print (int blanks) {
    printAst("arrayref","aa","array",this->left,"index",this->right,"\0");
}

void arrayref_astnode::jaincode() {
	right->jaincode();
	expTypeTree_t lhst = left->ett;
	cout << "\timul\t$" << lhst.downsize() << ", \%eax" << endl;
	cout << "\tpushl\t\%eax"<<endl;
	left->jaincode();
	cout << "\tpopl\t\%ebx" << endl;
	cout << "\taddl\t\%ebx, \%eax" << endl;
	expTypeTree_t mytemp = left->ett;
	if (mytemp.sec_strcnt!=0) {
		cout << "\tmovl\t\%eax, \%ecx" << endl;
	}
	else if (mytemp.array_sz.size()>1) {
		cout << "\tmovl\t\%eax, \%ecx" << endl;
	}
	else if (mytemp.basetype.substr(0,6)=="struct" && mytemp.totpont() == 1){
		cout << "\tmovl\t\%eax, \%ecx" << endl;
	}
	else {
		cout << "\tmovl\t\%eax, \%ecx" << endl;
		cout << "\tmovl\t(\%eax), \%eax" << endl;
	}
}

member_astnode::member_astnode (exp_astnode* val, ref_astnode* id) {
    this->val = val;
    this->id = id;
}

void member_astnode::print (int blanks) {
    printAst("member","aa","struct",this->val,"field",this->id,"\0");
}

void member_astnode::jaincode() {
	val->jaincode();
	cout << "\tleal\t" << gst.Entries[val->ett.basetype].symbtab->Entries[id->fnamer()].offset << "(\%eax), \%ecx" << endl;
	expTypeTree_t mytemp = ett;
	if (mytemp.array_sz.size()!=0) {
		cout << "\tmovl\t\%ecx, \%eax" << endl;
	}
	else if (mytemp.basetype.substr(0,6)=="struct" && mytemp.totpont() == 0){
		cout << "\tmovl\t\%ecx, \%eax" << endl;
	}
	else {
		cout << "\tmovl\t(\%ecx), \%eax" << endl;
	}
}

arrow_astnode::arrow_astnode (exp_astnode* val, ref_astnode* id) {
    this->val = val;
    this->id = id;
}

void arrow_astnode::print (int blanks) {
    printAst("arrow","aa","pointer",this->val,"field",this->id,"\0");
}

void arrow_astnode::jaincode() {
	val->jaincode();
	cout << "\tleal\t" << gst.Entries[val->ett.basetype].symbtab->Entries[id->fnamer()].offset << "(\%eax), \%ecx" << endl;
	expTypeTree_t mytemp = ett;
	if (mytemp.array_sz.size()!=0) {
		cout << "\tmovl\t\%ecx, \%eax" << endl;
	}
	else if (mytemp.basetype.substr(0,6)=="struct" && mytemp.totpont() == 0){
		cout << "\tmovl\t\%ecx, \%eax" << endl;
	}
	else {
		cout << "\tmovl\t(\%ecx), \%eax" << endl;
	}
}
