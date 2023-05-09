%skeleton "lalr1.cc"
%require  "3.0.1"

%defines 
%define api.namespace {IPL}
%define api.parser.class {Parser}

%define parse.trace

%code requires{
   namespace IPL {
      class Scanner;
   }
   #include "symb.hh"
   #include "ast.hh"

  // # ifndef YY_NULLPTR
  // #  if defined __cplusplus && 201103L <= __cplusplus
  // #   define YY_NULLPTR nullptr
  // #  else
  // #   define YY_NULLPTR 0
  // #  endif
  // # endif

}

%printer { std::cerr << $$; } STRUCT
%printer { std::cerr << $$; } IDENTIFIER
%printer { std::cerr << $$; } VOID
%printer { std::cerr << $$; } INT
%printer { std::cerr << $$; } FLOAT
%printer { std::cerr << $$; } INT_CONSTANT
%printer { std::cerr << $$; } FLOAT_CONSTANT
%printer { std::cerr << $$; } WHILE
%printer { std::cerr << $$; } FOR
%printer { std::cerr << $$; } OR_OP
%printer { std::cerr << $$; } AND_OP
%printer { std::cerr << $$; } RETURN
%printer { std::cerr << $$; } EQ_OP
%printer { std::cerr << $$; } NE_OP
%printer { std::cerr << $$; } LE_OP
%printer { std::cerr << $$; } GE_OP
%printer { std::cerr << $$; } IF
%printer { std::cerr << $$; } ELSE
%printer { std::cerr << $$; } INC_OP
%printer { std::cerr << $$; } PTR_OP
%printer { std::cerr << $$; } STRING_LITERAL
%printer { std::cerr << $$; } OTHERS


%parse-param { Scanner  &scanner  }
%locations
%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <map>
   #include <string>
   #include "scanner.hh"
   std::map<string,abstract_astnode*> ast;
   extern SymbTab gst;
   string basetype;
   int strcnt=0;
   int offset=0;
   std::map<string, int> bs_size {
      {"int", 4},
      {"float", 4},
      {"pointer", 4}
   };
   string flag;
   SymbTab *curst = nullptr;
   string globname;
   std::map<string, std::vector<typeTree> *> func_params;
   std::vector<typeTree> *curpl = nullptr;
   std::string globtypespec;

#undef yylex
#define yylex IPL::Parser::scanner.yylex

}




%define api.value.type variant
%define parse.assert

%start translation_unit

%token <std::string> STRUCT
%token <std::string> VOID
%token <std::string> INT
%token <std::string> FLOAT
%token <std::string> WHILE
%token <std::string> FOR
%token <std::string> IF
%token <std::string> ELSE
%token <std::string> INT_CONSTANT
%token <std::string> FLOAT_CONSTANT
%token <typeExp> OR_OP
%token <typeExp> AND_OP
%token <std::string> RETURN
%token <typeExp> EQ_OP
%token <typeExp> NE_OP
%token <typeExp> LE_OP
%token <typeExp> GE_OP
%token <typeExp> INC_OP
%token <std::string> PTR_OP
%token <std::string> STRING_LITERAL
%token <std::string> IDENTIFIER
%token <std::string> OTHERS

%token ';' '{' '}' '=' '(' ')' '<' '>' '[' ']' ',' '-' '!' '&' '+' '*' '/' '.'

%nterm <int> translation_unit struct_specifier function_definition
fun_declarator parameter_declaration parameter_list
/* %nterm <std::vector<typeTree>> parameter_list */

%nterm <abstract_astnode*> compound_statement
%nterm <statement_astnode*> statement selection_statement iteration_statement
%nterm <seq_astnode*> statement_list
%nterm <assignS_astnode*> assignment_statement
%nterm <proccall_astnode*> procedure_call

%nterm <exp_astnode*> expression unary_expression postfix_expression
primary_expression logical_and_expression equality_expression
relational_expression additive_expression multiplicative_expression
%nterm <funcall_astnode*> expression_list
%nterm <assignE_astnode*> assignment_expression

%nterm <typeExp> unary_operator

%nterm <struct declaration_list> declaration_list
%nterm <struct declaration> declaration
%nterm <struct declarator_list> declarator_list
%nterm <struct declarator> declarator
%nterm <struct declarator_arr> declarator_arr
%nterm <std::string> type_specifier


%%
translation_unit: 
struct_specifier
{
   
}
| function_definition
{
   
}
| translation_unit struct_specifier
{
   
}
| translation_unit function_definition
{
   
}

struct_specifier:
STRUCT IDENTIFIER '{' {
   flag = "struct";
   offset = 0;
   curst = new SymbTab();
   curst->Entries.clear();
   globname = string("struct ") + $2;
} declaration_list '}' ';'
{
   SymbTabEntry ste;
   ste.flag = "struct";
   ste.scope = "global";
   ste.size = -offset;
   ste.offset = 0;
   for (auto it=curst->Entries.begin(); it!=curst->Entries.end(); it++) {
      it->second.offset = -(it->second.offset+it->second.size);
   }
   ste.symbtab = curst;
   curst = nullptr;
   gst.Entries.insert({globname, ste});
   bs_size.insert({string("struct ")+$2, -offset});
}

function_definition:
type_specifier {
   if ($1.substr(0,6) == string("struct")) {
      if (gst.Entries.find($1) == gst.Entries.end()) {
         error(@$, string("return type ") + $1 + string("not defined"));
      }
   }
   flag = "fun";
   offset = 4;
   curst = new SymbTab();
   curst->Entries.clear();
   curpl = new vector<typeTree>();
   curpl->clear();
} fun_declarator {
   for (auto it=curst->Entries.begin(); it!=curst->Entries.end(); it++) {
      it->second.offset += (12 - offset);
      it->second.scope = "param";
   }
   func_params.insert({globname, curpl});
   curpl = nullptr;
   offset = 0;
   globtypespec = $1;
} compound_statement
{
   SymbTabEntry ste;
   ste.flag = "fun";
   ste.scope = "global";
   ste.size = 0;
   ste.offset = 0;
   ste.type.basetype = $1;
   ste.type.strcnt = 0;
   ste.type.arrayspec = vector<int>();
   ste.symbtab = curst;
   curst = nullptr;
   gst.Entries.insert({globname, ste});
   abstract_astnode* node = $5;
   ast.insert({globname,node});
}

type_specifier:
VOID
{
   basetype = "void";
   $$ = basetype;
}
| INT
{
   basetype = "int";
   $$ = basetype;
}
| FLOAT
{
   basetype = "float";
   $$ = basetype;
}
| STRUCT IDENTIFIER
{
   basetype = string("struct ") + $2;
   $$ = basetype;
}

fun_declarator:
IDENTIFIER '(' parameter_list ')'
{
   globname = $1;
}
| IDENTIFIER '(' ')'
{
   globname = $1;
}

parameter_list:
parameter_declaration
{
   strcnt = 0;
}
| parameter_list ',' parameter_declaration
{
   strcnt = 0;
}

parameter_declaration:
type_specifier declarator
{
   
}

declarator_arr:
IDENTIFIER
{
   $$.identifier = $1;
   $$.arr_held = vector<int>();
   $$.sz_mult = 1;
}
| declarator_arr '[' INT_CONSTANT ']'
{
   $$.identifier = $1.identifier;
   $$.arr_held = $1.arr_held;
   $$.arr_held.push_back(std::stoi($3));
   $$.sz_mult = $1.sz_mult * std::stoi($3);
}

declarator:
declarator_arr
{
   SymbTabEntry ste;
   ste.flag = "var";
   ste.scope = "local";
   ste.size = $1.sz_mult*bs_size[strcnt==0?basetype:"pointer"];
   offset -= ste.size;
   ste.offset = offset;
   ste.type.strcnt = strcnt;
   ste.type.arrayspec = $1.arr_held;
   ste.type.basetype = basetype;
   if (ste.type.basetype == "void" && ste.type.strcnt == 0)
      error(@$, string("cannot declare variable of type: ") + ste.type.print());
   if (curst->Entries.find($1.identifier) != curst->Entries.end())
      error(@$, string("identifier of name ") + $1.identifier + string(" already defined."));
   if (gst.Entries.find($1.identifier) != gst.Entries.end())
      error(@$, $1.identifier + string(" already names a function"));
   if (ste.type.basetype.substr(0,6) == string("struct") && gst.Entries.find(ste.type.basetype) == gst.Entries.end())
   {
      if (ste.type.totpont()!=0 && ste.type.basetype==globname)
         ;
      else
         error(@$, ste.type.basetype + string(" not defined"));
   }
   curst->Entries.insert({$1.identifier, ste});
   if (curpl!=nullptr)
      curpl->push_back(ste.type);
}
| '*' {
   ++strcnt;
} declarator
{
   --strcnt;
}

compound_statement:
'{' '}'
{
   std::vector<statement_astnode*> children;
   seq_astnode* node = new seq_astnode(children);
   $$ = node;
}
| '{' statement_list '}'
{
   $$ = $2;
}
| '{' declaration_list '}'
{
   std::vector<statement_astnode*> children;
   seq_astnode* node = new seq_astnode(children);
   $$ = node;
}
| '{' declaration_list statement_list '}'
{
   $$ = $3;
}

statement_list:
statement
{
   std::vector<statement_astnode*> children;
   children.push_back($1);
   seq_astnode* node = new seq_astnode(children);
   $$ = node;
}
| statement_list statement
{
   std::vector<statement_astnode*> children = ($1)->children;
   children.push_back($2);
   seq_astnode* node = new seq_astnode(children);
   $$ = node;
}

statement:
';'
{
   empty_astnode* node = new empty_astnode();
   $$ = node;
}
| '{' statement_list '}'
{
   $$ = $2;
}
| selection_statement
{
   $$ = $1;
}
| iteration_statement
{
   $$ = $1;
}
| assignment_statement
{
   $$ = $1;
}
| procedure_call
{
   $$ = $1;
}
| RETURN expression ';'
{
   
   expTypeTree_t rhst = $2->ett;
   expTypeTree_t lhst = $2->ett;
   lhst.strcnt = lhst.sec_strcnt = 0;
   lhst.lval = true;
   lhst.array_sz.clear();
   lhst.basetype = globtypespec;
   exp_astnode *rtnode = $2;
   if (lhst.totpont() != rhst.totpont() && lhst.print()!="void*" && rhst.print()!="void*") {
      if (lhst.totpont()!=0 && rhst.menullptr)
         ;
      else
         error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
   }
   if (lhst.totpont()!=0 && rhst.menullptr)
      ;
   else if (lhst.totpont()==0 && rhst.totpont()==0)
   {
      if (lhst.basetype != rhst.basetype && !(lhst.basetype == "int" && rhst.basetype == "float" || rhst.basetype == "int" && lhst.basetype == "float"))
         error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
      if (lhst.basetype == "int" && rhst.basetype == "float")
      {
         op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_INT, rtnode);
         rtnode = temp;
      }
      if (rhst.basetype == "int" && lhst.basetype == "float")
      {
         op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_FLOAT, rtnode);
         rtnode = temp;
      }
   }
   else
   {
      if (lhst.print()==string("void*") || rhst.print()==string("void*")) {
         if (lhst.totpont()==0 || rhst.totpont()==0)
            error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
      }
      else if (lhst.boiled_print() != rhst.boiled_print())
         error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
   }   
   return_astnode* node = new return_astnode(rtnode);
   $$ = node;
}

assignment_expression:
unary_expression '=' expression
{
   expTypeTree_t rhst = $3->ett;
   expTypeTree_t lhst = $1->ett;
   exp_astnode *rtnode = $3;
   if (!lhst.lval)
      error(@$, string("need lval on left side of assignment, got rval of type ") + $1->ett.print());
   if (!lhst.array_sz.empty())
      error(@$, string("need modifiable lval on left side of assignment, got array-kind of type ") + $1->ett.print());
   if (lhst.totpont() != rhst.totpont() && lhst.print()!="void*" && rhst.print()!="void*"){
      if (lhst.totpont()!=0 && rhst.menullptr)
         ;
      else
         error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
   }
   if (lhst.totpont()!=0 && rhst.menullptr)
      ;
   else if (lhst.totpont()==0 && rhst.totpont()==0)
   {
      if (lhst.basetype != rhst.basetype && !(lhst.basetype == "int" && rhst.basetype == "float" || rhst.basetype == "int" && lhst.basetype == "float"))
         error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
      if (lhst.basetype == "int" && rhst.basetype == "float")
      {
         op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_INT, rtnode);
         rtnode = temp;
      }
      if (rhst.basetype == "int" && lhst.basetype == "float")
      {
         op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_FLOAT, rtnode);
         rtnode = temp;
      }
   }
   else
   {
      if (lhst.print()==string("void*") || rhst.print()==string("void*")) {
         if (lhst.totpont()==0 || rhst.totpont()==0)
            error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
      }
      else if (lhst.boiled_print() != rhst.boiled_print())
         error(@$, string("incompatible types for assignment: ") + lhst.print() + " and " + rhst.print());
   }
   assignE_astnode* node = new assignE_astnode($1,rtnode);
   $$ = node;
}

assignment_statement:
assignment_expression ';'
{
   assignS_astnode* node = new assignS_astnode(($1)->left,($1)->right);
   $$ = node;
}

procedure_call:
IDENTIFIER '(' ')' ';'
{
   identifier_astnode* temp = new identifier_astnode($1);
   std::vector<exp_astnode*> args;
   args.push_back(temp);
   if (func_params.find($1) == func_params.end() && $1!="printf" && $1!="scanf")
      error(@$, string("function with name ") + $1 + string(" not defined"));
   if ($1 == "printf" || $1 == "scanf") {
      proccall_astnode* node = new proccall_astnode(args);
      $$ = node;
   }
   else {
      vector<typeTree> vtt = *func_params[$1];
      if (!vtt.empty())
         error(@$, string("too many parameters to function ") + $1);
      proccall_astnode* node = new proccall_astnode(args);
      $$ = node;
   }
}
| IDENTIFIER '(' expression_list ')' ';'
{
   identifier_astnode* temp = new identifier_astnode($1);
   std::vector<exp_astnode*> args;
   args.push_back(temp);
   args.insert(args.end(),(($3)->args).begin(),(($3)->args).end());
   if (func_params.find($1) == func_params.end() && $1!="printf" && $1!="scanf")
      error(@$, string("function with name ") + $1 + string(" not defined"));
   if ($1 == "printf" || $1 == "scanf") {
      proccall_astnode* node = new proccall_astnode(args);
      $$ = node;
   }
   else{
      vector<typeTree> vtt = *func_params[$1];
      if (vtt.size() > args.size()-1)
         error(@$, string("too few parameters to function ") + $1);
      if (vtt.size() < args.size()-1)
         error(@$, string("too many parameters to function ") + $1);
      for (int i=0; i<vtt.size(); ++i)
      {
         expTypeTree_t rhst = args[i+1]->ett;
         typeTree lhst = vtt[i];
         if (lhst.totpont() != rhst.totpont() && lhst.print()!=string("void*") && rhst.print()!=string("void*")) {
            if (lhst.totpont()!=0 && rhst.menullptr)
               ;
            else
               error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
         }
         if (lhst.totpont()!=0 && rhst.menullptr)
            ;
         else if (lhst.totpont()==0 && rhst.totpont()==0)
         {
            if (lhst.basetype != rhst.basetype && !(lhst.basetype == "int" && rhst.basetype == "float" || rhst.basetype == "int" && lhst.basetype == "float"))
               error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
            if (lhst.basetype == "int" && rhst.basetype == "float")
            {
               op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_INT, args[i+1]);
               args[i+1] = temp;
            }
            if (rhst.basetype == "int" && lhst.basetype == "float")
            {
               op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_FLOAT, args[i+1]);
               args[i+1] = temp;
            }
         }
         else
         {
            if (lhst.print()==string("void*") || rhst.print()==string("void*")) {
               if (lhst.totpont()==0 || rhst.totpont()==0)
                  error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
            }
            else if (lhst.boiled_print() != rhst.boiled_print())
               error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
         }
      }
      proccall_astnode* node = new proccall_astnode(args);
      $$ = node;
   }
}

//--MJW

expression:
logical_and_expression
{
   $$ = $1;
}
| expression OR_OP logical_and_expression
{
   op_binary_astnode* node;
   if ($1->ett.totpont()==0 && $1->ett.basetype!="float" && $1->ett.basetype!="int")
         error(@$, string("argument to || cannot be of type ") + $1->ett.print());
   if ($3->ett.totpont()==0 && $3->ett.basetype!="float" && $3->ett.basetype!="int")
         error(@$, string("argument to || cannot be of type ") + $3->ett.print());
   node = new op_binary_astnode(typeExp::OR_OP,$1,$3);
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}

logical_and_expression:
equality_expression
{
   $$ = $1;
}
| logical_and_expression AND_OP equality_expression
{
   op_binary_astnode* node;
   if ($1->ett.totpont()==0 && $1->ett.basetype!="float" && $1->ett.basetype!="int")
         error(@$, string("argument to && cannot be of type ") + $1->ett.print());
   if ($3->ett.totpont()==0 && $3->ett.basetype!="float" && $3->ett.basetype!="int")
         error(@$, string("argument to && cannot be of type ") + $3->ett.print());
   node = new op_binary_astnode(typeExp::AND_OP,$1,$3);
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}

equality_expression:
relational_expression
{
   $$ = $1;
}
| equality_expression EQ_OP relational_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::EQ_OP_INT,$1,$3);
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::EQ_OP_FLOAT,tempynode,$3);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::EQ_OP_FLOAT,$1,tempynode);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::EQ_OP_FLOAT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::EQ_OP_INT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $3->ett.print() == "void*" || $3->ett.totpont()!=0 && $1->ett.print() == "void*")
   {
      node = new op_binary_astnode(typeExp::EQ_OP_INT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $3->ett.menullptr)
   {
      node = new op_binary_astnode(typeExp::EQ_OP_INT,$1,$3);
   }
   else
      error(@$, string("invalid operand types to == operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett = $1->ett;
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}
| equality_expression NE_OP relational_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::NE_OP_INT,$1,$3);
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::NE_OP_FLOAT,tempynode,$3);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::NE_OP_FLOAT,$1,tempynode);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::NE_OP_FLOAT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::NE_OP_INT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $3->ett.print() == "void*" || $3->ett.totpont()!=0 && $1->ett.print() == "void*")
   {
      node = new op_binary_astnode(typeExp::NE_OP_INT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $3->ett.menullptr)
   {
      node = new op_binary_astnode(typeExp::EQ_OP_INT,$1,$3);
   }
   else
      error(@$, string("invalid operand types to != operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett = $1->ett;
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}

relational_expression:
additive_expression
{
   $$ = $1;
}
| relational_expression '<' additive_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::LT_OP_INT,$1,$3);
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::LT_OP_FLOAT,tempynode,$3);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::LT_OP_FLOAT,$1,tempynode);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::LT_OP_FLOAT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::LT_OP_INT,$1,$3);
   }
   else
      error(@$, string("invalid operand types to < operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett = $1->ett;
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}
| relational_expression '>' additive_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::GT_OP_INT,$1,$3);
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::GT_OP_FLOAT,tempynode,$3);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::GT_OP_FLOAT,$1,tempynode);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::GT_OP_FLOAT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::GT_OP_INT,$1,$3);
   }
   else
      error(@$, string("invalid operand types to > operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett = $1->ett;
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}
| relational_expression LE_OP additive_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::LE_OP_INT,$1,$3);
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::LE_OP_FLOAT,tempynode,$3);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::LE_OP_FLOAT,$1,tempynode);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::LE_OP_FLOAT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::LE_OP_INT,$1,$3);
   }
   else
      error(@$, string("invalid operand types to <= operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett = $1->ett;
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}
| relational_expression GE_OP additive_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::GE_OP_INT,$1,$3);
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::GE_OP_FLOAT,tempynode,$3);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::GE_OP_FLOAT,$1,tempynode);
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::GE_OP_FLOAT,$1,$3);
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::GE_OP_INT,$1,$3);
   }
   else
      error(@$, string("invalid operand types to >= operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett = $1->ett;
   node->ett.lval = false;
   node->ett.basetype = "int";
   node->ett.sec_strcnt = node->ett.strcnt = 0;
   node->ett.array_sz.clear();
   $$ = node;
}

additive_expression:
multiplicative_expression
{
   $$ = $1;
}
| additive_expression '+' multiplicative_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::PLUS_INT,$1,$3);
      node->ett = $1->ett;
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::PLUS_FLOAT,tempynode,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::PLUS_FLOAT,$1,tempynode);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::PLUS_FLOAT,$1,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.totpont()!=0 && $3->ett.print()==string("int"))
   {
      node = new op_binary_astnode(typeExp::PLUS_INT,$1,$3);
      node->ett = $1->ett;
   }
   else if ($3->ett.totpont()!=0 && $1->ett.print()==string("int"))
   {
      node = new op_binary_astnode(typeExp::PLUS_INT,$1,$3);
      node->ett = $3->ett;
   }
   else
      error(@$, string("invalid operand types to + operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett.lval = false;
   $$ = node;
}
| additive_expression '-' multiplicative_expression
{
   op_binary_astnode* node;
   if ($1->ett.print() == "int" && $3->ett.print() == "int")
   {
      node = new op_binary_astnode(typeExp::MINUS_INT,$1,$3);
      node->ett = $1->ett;
   }
   else if ($1->ett.print() == "int" && $3->ett.print() == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::MINUS_FLOAT,tempynode,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::MINUS_FLOAT,$1,tempynode);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.print() == "float" && $3->ett.print() == "float")
   {
      node = new op_binary_astnode(typeExp::MINUS_FLOAT,$1,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.totpont()!=0 && $3->ett.print()==string("int"))
   {
      node = new op_binary_astnode(typeExp::MINUS_INT,$1,$3);
      node->ett = $1->ett;
   }
   else if ($1->ett.totpont()!=0 && $1->ett.boiled_print() == $3->ett.boiled_print())
   {
      node = new op_binary_astnode(typeExp::MINUS_INT,$1,$3);
      node->ett = $1->ett;
      node->ett.lval = false;
      node->ett.basetype = "int";
      node->ett.sec_strcnt = node->ett.strcnt = 0;
      node->ett.array_sz.clear();
   }
   else
      error(@$, string("invalid operand types to - operator: ") + $1->ett.print() + string(" and ") + $3->ett.print());
   node->ett.lval = false;
   $$ = node;
}

unary_expression:
postfix_expression
{
   $$ = $1;
}
| unary_operator unary_expression
{
   op_unary_astnode* node = new op_unary_astnode($1,$2);
   if ($1 == typeExp::UMINUS)
   {
      if ($2->ett.strcnt!=0 || $2->ett.sec_strcnt!=0 || !$2->ett.array_sz.empty() || ($2->ett.basetype!="float" && $2->ett.basetype!="int"))
         error(@$, string("argument to - cannot be of type ") + $2->ett.print());
      node->ett = $2->ett;
   }
   else if ($1 == typeExp::NOT)
   {
      if ($2->ett.totpont()==0 && $2->ett.basetype!="float" && $2->ett.basetype!="int")
         error(@$, string("argument to ! cannot be of type ") + $2->ett.print());
      node->ett.lval = false;
      node->ett.basetype = "int";
      node->ett.sec_strcnt = node->ett.strcnt = 0;
      node->ett.array_sz.clear();
   }
   else if ($1 == typeExp::ADDRESS)
   {
      node->ett = $2->ett;
      if (!node->ett.lval)
         error(@$, string("argument to & needs to be an lval"));
      node->ett.lval = false;
      if (node->ett.array_sz.empty())
         ++node->ett.strcnt;
      else
         ++node->ett.sec_strcnt;
   }
   else if ($1 == typeExp::DEREF)
   {
      node->ett = $2->ett;
      if (node->ett.sec_strcnt)
      {
         node->ett.sec_strcnt = false;
      }
      else if (!node->ett.array_sz.empty())
      {
         node->ett.array_sz.erase(node->ett.array_sz.begin());
      }
      else if (node->ett.strcnt!=0)
      {
         node->ett.strcnt-=1;
      }
      else 
         error(@$, string("cannot derefence type ") + node->ett.print());
      if (node->ett.print()==string("void"))
         error(@$, string("cannot derefence void*"));
      node->ett.lval = true;
   }
   $$ = node;
}

multiplicative_expression:
unary_expression
{
   $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
   op_binary_astnode* node;
   if ($1->ett.strcnt!=0 || $1->ett.sec_strcnt!=0 || !$1->ett.array_sz.empty() || ($1->ett.basetype!="float" && $1->ett.basetype!="int"))
      error(@$, string("lhs to * cannot be of type ") + $1->ett.print());
   if ($3->ett.strcnt!=0 || $3->ett.sec_strcnt!=0 || !$3->ett.array_sz.empty() || ($3->ett.basetype!="float" && $3->ett.basetype!="int"))
      error(@$, string("rhs to * cannot be of type ") + $3->ett.print());
   if ($1->ett.basetype == "int" && $3->ett.basetype == "int")
   {
      node = new op_binary_astnode(typeExp::MULT_INT,$1,$3);
      node->ett = $1->ett;
   }
   else if ($1->ett.basetype == "int" && $3->ett.basetype == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::MULT_FLOAT,tempynode,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.basetype == "float" && $3->ett.basetype == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::MULT_FLOAT,$1,tempynode);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else
   {
      node = new op_binary_astnode(typeExp::MULT_FLOAT,$1,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   node->ett.lval = false;
   $$ = node;
}
| multiplicative_expression '/' unary_expression
{
   op_binary_astnode* node;
   if ($1->ett.strcnt!=0 || $1->ett.sec_strcnt!=0 || !$1->ett.array_sz.empty() || ($1->ett.basetype!="float" && $1->ett.basetype!="int"))
      error(@$, string("lhs to / cannot be of type ") + $1->ett.print());
   if ($3->ett.strcnt!=0 || $3->ett.sec_strcnt!=0 || !$3->ett.array_sz.empty() || ($3->ett.basetype!="float" && $3->ett.basetype!="int"))
      error(@$, string("rhs to / cannot be of type ") + $3->ett.print());
   if ($1->ett.basetype == "int" && $3->ett.basetype == "int")
   {
      node = new op_binary_astnode(typeExp::DIV_INT,$1,$3);
      node->ett = $1->ett;
   }
   else if ($1->ett.basetype == "int" && $3->ett.basetype == "float")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$1);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::DIV_FLOAT,tempynode,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else if ($1->ett.basetype == "float" && $3->ett.basetype == "int")
   {
      op_unary_astnode *tempynode = new op_unary_astnode(typeExp::TO_FLOAT,$3);
      tempynode->ett = $1->ett;
      tempynode->ett.basetype = "float";
      node = new op_binary_astnode(typeExp::DIV_FLOAT,$1,tempynode);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   else
   {
      node = new op_binary_astnode(typeExp::DIV_FLOAT,$1,$3);
      node->ett = $1->ett;
      node->ett.basetype = "float";
   }
   node->ett.lval = false;
   $$ = node;
}

postfix_expression:
primary_expression
{
   $$ = $1;
}
| postfix_expression '[' expression ']'
{
   arrayref_astnode* node = new arrayref_astnode($1,$3);
   node->ett = $1->ett;
   if (node->ett.sec_strcnt)
      --node->ett.sec_strcnt;
   else if (!node->ett.array_sz.empty())
      node->ett.array_sz.erase(node->ett.array_sz.begin());
   else if (node->ett.strcnt > 0)
      node->ett.strcnt-=1;
   else
      error(@$, string("Cannot index type ") + node->ett.print());
   if (node->ett.print()==string("void"))
         error(@$, "cannot derefence void*");
   if ($3->ett.print()!="int")
      error(@$, string("Cannot use type ") + $3->ett.print() + string(" to index"));
   node->ett.lval = true;
   $$ = node;
}
| IDENTIFIER '(' ')'
{
   identifier_astnode* temp = new identifier_astnode($1);
   std::vector<exp_astnode*> args;
   args.push_back(temp);
   if (func_params.find($1) == func_params.end())
      error(@$, string("function with name ") + $1 + string(" not defined"));
   vector<typeTree> vtt = *func_params[$1];
   if (!vtt.empty())
      error(@$, string("too many parameters to function ") + $1);
   funcall_astnode* node = new funcall_astnode(args);
   typeTree tt = gst.Entries[$1].type;
   node->ett.strcnt = tt.strcnt;
   node->ett.array_sz = tt.arrayspec;
   node->ett.lval = false;
   node->ett.sec_strcnt = 0 ;
   node->ett.basetype = tt.basetype;
   $$ = node;
}
| IDENTIFIER '(' expression_list ')'
{
   identifier_astnode* temp = new identifier_astnode($1);
   std::vector<exp_astnode*> args;
   args.push_back(temp);
   args.insert(args.end(),(($3)->args).begin(),(($3)->args).end());
   if (func_params.find($1) == func_params.end())
      error(@$, string("function with name ") + $1 + string(" not defined"));
   vector<typeTree> vtt = *func_params[$1];
   if (vtt.size() > args.size()-1)
      error(@$, string("too few parameters to function ") + $1);
   if (vtt.size() < args.size()-1)
      error(@$, string("too many parameters to function ") + $1);
   for (int i=0; i<vtt.size(); ++i)
   {
      expTypeTree_t rhst = args[i+1]->ett;
      typeTree lhst = vtt[i];
      if (lhst.totpont() != rhst.totpont() && lhst.print()!="void*" && rhst.print()!="void*") {
         if (lhst.totpont()!=0 && rhst.menullptr)
            ;
         else
            error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
      }
      if (lhst.totpont()!=0 && rhst.menullptr)
         ;
      else if (lhst.totpont()==0 && rhst.totpont()==0)
      {
         if (lhst.basetype != rhst.basetype && !(lhst.basetype == "int" && rhst.basetype == "float" || rhst.basetype == "int" && lhst.basetype == "float"))
            error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
         if (lhst.basetype == "int" && rhst.basetype == "float")
         {
            op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_INT, args[i+1]);
            args[i+1] = temp;
         }
         if (rhst.basetype == "int" && lhst.basetype == "float")
         {
            op_unary_astnode *temp = new op_unary_astnode(typeExp::TO_FLOAT, args[i+1]);
            args[i+1] = temp;
         }
      }
      else
      {
         if (lhst.print()==string("void*") || rhst.print()==string("void*")) {
            if (lhst.totpont()==0 || rhst.totpont()==0)
               error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
         }
         else if (lhst.boiled_print() != rhst.boiled_print())
            error(@$, string("incompatible types for argument and parameter: ") + lhst.print() + " and " + rhst.print());
      }
   }
   typeTree tt = gst.Entries[$1].type;
   funcall_astnode* node = new funcall_astnode(args);
   node->ett.strcnt = tt.strcnt;
   node->ett.array_sz = tt.arrayspec;
   node->ett.lval = false;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = tt.basetype;
   $$ = node;
}
| postfix_expression '.' IDENTIFIER
{
   identifier_astnode* temp = new identifier_astnode($3);
   member_astnode* node = new member_astnode($1,temp);
   expTypeTree_t mytemp= $1->ett;
   if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
      error(@$, string("cannot use . operator on type ") + $1->ett.print());
   if (gst.Entries.find(mytemp.print()) == gst.Entries.end())
      error(@$, string("type ") + mytemp.print() + string("not defnied"));
   if (gst.Entries[mytemp.print()].symbtab->Entries.find($3) == gst.Entries[mytemp.print()].symbtab->Entries.end())
      error(@$, mytemp.print() + string(" does not have a member named ") + $3);
   typeTree tt = gst.Entries[mytemp.print()].symbtab->Entries[$3].type;
   node->ett.strcnt = tt.strcnt;
   node->ett.array_sz = tt.arrayspec;
   node->ett.lval = true;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = tt.basetype;
   $$ = node;
}
| postfix_expression PTR_OP IDENTIFIER
{
   identifier_astnode* temp = new identifier_astnode($3);
   arrow_astnode* node = new arrow_astnode($1,temp);
   expTypeTree_t mytemp= $1->ett;
   if (mytemp.sec_strcnt)
      --mytemp.sec_strcnt;
   else if (!mytemp.array_sz.empty())
      mytemp.array_sz.erase(mytemp.array_sz.begin());
   else  if (mytemp.strcnt)
      --mytemp.strcnt;
   else
      error(@$, string("cannot use -> operator on type ") + $1->ett.print());
   if (mytemp.strcnt!=0 || !mytemp.array_sz.empty() || mytemp.basetype.substr(0,6)!=string("struct"))
      error(@$, string("cannot use -> operator on type ") + $1->ett.print());
   if (gst.Entries.find(mytemp.print()) == gst.Entries.end())
      error(@$, string("type ") + mytemp.print() + string("not defnied"));
   if (gst.Entries[mytemp.print()].symbtab->Entries.find($3) == gst.Entries[mytemp.print()].symbtab->Entries.end())
      error(@$, mytemp.print() + string(" does not have a member named ") + $3);
   typeTree tt = gst.Entries[mytemp.print()].symbtab->Entries[$3].type;
   node->ett.strcnt = tt.strcnt;
   node->ett.array_sz = tt.arrayspec;
   node->ett.lval = true;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = tt.basetype;
   $$ = node;
}
| postfix_expression INC_OP
{
   op_unary_astnode* node = new op_unary_astnode(typeExp::PP,$1);
   node->ett = $1->ett;
   if (!node->ett.lval)
      error(@$, string("++ operator expects lval argument, recieved rval of type " + node->ett.print()));
   if (!node->ett.array_sz.empty())
      error(@$, string("++ operator requires modifiable lval argument, recived type ") + node->ett.print());
   if (node->ett.strcnt == 0 && node->ett.basetype != "int" && node->ett.basetype!="float")
      error(@$, string("++ operator cannot work on type ") + node->ett.print());
   node->ett.lval = false;
   $$ = node;
}

primary_expression:
IDENTIFIER
{
   identifier_astnode* node = new identifier_astnode($1);
   if (curst->Entries.find($1) == curst->Entries.end())
      error(@$, string("identifier of name ") + $1 + string(" not defiend"));
   typeTree tt = curst->Entries[$1].type;
   node->ett.strcnt = tt.strcnt;
   node->ett.array_sz = tt.arrayspec;
   node->ett.lval = true;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = tt.basetype;
   $$ = node;
}
| INT_CONSTANT
{
   intconst_astnode* node = new intconst_astnode(std::stoi($1));
   node->ett.strcnt = 0;
   node->ett.lval = false;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = "int";
   if ($1 == "0")
      node->ett.menullptr = true;
   $$ = node;
}
| FLOAT_CONSTANT
{
   floatconst_astnode* node = new floatconst_astnode(std::stof($1));
   node->ett.strcnt = 0;
   node->ett.lval = false;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = "float";
   $$ = node;
}
| STRING_LITERAL
{
   string_astnode* node = new string_astnode($1);
   node->ett.strcnt = 1;
   node->ett.lval = false;
   node->ett.sec_strcnt = 0;
   node->ett.basetype = "char";
   $$ = node;
}
| '(' expression ')'
{
   $$ = $2;
}

expression_list:
expression
{
   std::vector<exp_astnode*> args;
   args.push_back(nullptr);
   args.push_back($1);
   funcall_astnode* node = new funcall_astnode(args);
   $$ = node;
}
| expression_list ',' expression
{
   std::vector<exp_astnode*> args = ($1)->args;
   args.insert(args.begin(), nullptr);
   // free($1); do this if you want science
   args.push_back($3);
   funcall_astnode* node = new funcall_astnode(args);
   // printf("%d\n", $1->args.size());
   $$ = node;
}

unary_operator:
'-'
{
   $$ = typeExp::UMINUS;
}
| '!'
{
   $$ = typeExp::NOT;
}
| '&'
{
   $$ = typeExp::ADDRESS;  
}
| '*'
{
   $$ = typeExp::DEREF;
}

selection_statement:
IF '(' expression ')' statement ELSE statement
{
   if_astnode* node = new if_astnode($3,$5,$7);
   if ($3->ett.sec_strcnt == 0 && $3->ett.strcnt == 0 && $3->ett.array_sz.empty() && $3->ett.basetype.substr(0,6)==string("struct"))
      error(@$, string("cannot use type ") + $3->ett.print() + string(" as condition, need scalar"));
   $$ = node;
}

iteration_statement:
WHILE '(' expression ')' statement
{
   while_astnode* node = new while_astnode($3,$5);
   if ($3->ett.sec_strcnt == 0 && $3->ett.strcnt == 0 && $3->ett.array_sz.empty() && $3->ett.basetype.substr(0,6)==string("struct"))
      error(@$, string("cannot use type ") + $3->ett.print() + string(" as condition, need scalar"));
   $$ = node;
}
| FOR '(' assignment_expression ';' expression ';' assignment_expression ')' statement
{
   for_astnode* node = new for_astnode($3,$5,$7,$9);
   if ($5->ett.sec_strcnt == 0 && $5->ett.strcnt == 0 && $5->ett.array_sz.empty() && $5->ett.basetype.substr(0,6)==string("struct"))
      error(@$, string("cannot use type ") + $5->ett.print() + string(" as condition, need scalar"));
   $$ = node;
}

declaration_list:
declaration
{
   
}
| declaration_list declaration
{
   
}

declaration:
type_specifier declarator_list ';'
{
   
}

declarator_list:
declarator
{
   strcnt = 0;
}
| declarator_list ',' declarator
{
   strcnt = 0;
}

%%
void IPL::Parser::error( const location_type &l, const std::string &err_message )
{
   std::cout << "Error at line " << l.begin.line << ": " << err_message << "\n";
   exit(1);
}


