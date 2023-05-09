#include "scanner.hh"
#include <fstream>
#include <map>
using namespace std;

SymbTab gst, gstfun, gststruct; 
string filename;
std::vector<LC> lcvec;
extern std::map<string,abstract_astnode*> ast;
// std::map<std::string, datatype> predefined {
//             {"printf", createtype(VOID_TYPE)},
//             {"scanf", createtype(VOID_TYPE)},
//             {"mod", createtype(INT_TYPE)}
//         };
extern string dis_func;
int main(int argc, char **argv)
{
	using namespace std;
	fstream in_file, out_file;
	

	in_file.open(argv[1], ios::in);

	IPL::Scanner scanner(in_file);

	IPL::Parser parser(scanner);

#ifdef YYDEBUG
	parser.set_debug_level(1);
#endif
parser.parse();
// create gstfun with function entries only

for (const auto &entry : gst.Entries)
{
	if (entry.second.flag == "fun")
	gstfun.Entries.insert({entry.first, entry.second});
}
// create gststruct with struct entries only

for (const auto &entry : gst.Entries)
{
	if (entry.second.flag == "struct")
	gststruct.Entries.insert({entry.first, entry.second});
}
/* BELOW WAS ASS 2
// start the JSON printing

cout << "{\"globalST\": " << endl;
gst.printgst();
cout << "," << endl;

cout << "  \"structs\": [" << endl;
for (auto it = gststruct.Entries.begin(); it != gststruct.Entries.end(); ++it)

{   cout << "{" << endl;
	cout << "\"name\": " << "\"" << it->first << "\"," << endl;
	cout << "\"localST\": " << endl;
	it->second.symbtab->print();
	cout << "}" << endl;
	if (next(it,1) != gststruct.Entries.end()) 
	cout << "," << endl;
}
cout << "]," << endl;
cout << "  \"functions\": [" << endl;

for (auto it = gstfun.Entries.begin(); it != gstfun.Entries.end(); ++it)

{
	cout << "{" << endl;
	cout << "\"name\": " << "\"" << it->first << "\"," << endl;
	cout << "\"localST\": " << endl;
	it->second.symbtab->print();
	cout << "," << endl;
	cout << "\"ast\": " << endl;
	ast[it->first]->print(0);
	cout << "}" << endl;
	if (next(it,1) != gstfun.Entries.end()) cout << "," << endl;
	
}
	cout << "]" << endl;
	cout << "}" << endl;

	fclose(stdout); */
	string fname, fname2 = argv[1];
	int off = fname2.size();
	while (off >= 0 && fname2[off]!='/')
		--off;
	fname = fname2.substr(off+1);
	cout << "\t.file\t\"" << fname << "\"" << endl;
	cout << "\t.text" << endl;
	cout << "\t.section\t.rodata" << endl;
	for (int i=0; i<lcvec.size(); ++i)
	{
		cout << ".LC" << i << ":" << endl;
		cout << "\t.string\t" << lcvec[i].da_str << endl;
	}

	for (auto it = gstfun.Entries.begin(); it != gstfun.Entries.end(); ++it)
	{
		if (it->first=="main")
			continue;
		cout << "\t.text" << endl;
		cout << "\t.globl\t" << it->first << endl;
		cout << "\t.type\t" << it->first << ", @function" << endl;
		cout << it->first << ":" << endl;
		cout << "\tpushl\t\%ebp" << endl;
		cout << "\tmovl\t\%esp, \%ebp" << endl;
		cout << "\tsubl\t$" << it->second.size << ", \%esp" << endl;
		dis_func = it->first;
		ast[it->first]->jaincode();
		cout << "\tleave" << endl;
		cout << "\tret" << endl;
		cout << "\t.size\t" << it->first << ", .-" << it->first << endl;
	}
	cout << "\t.text" << endl;
	cout << "\t.globl\t" << "main" << endl;
	cout << "\t.type\t" << "main" << ", @function" << endl;
	cout << "main" << ":" << endl;
	cout << "\tpushl\t\%ebp" << endl;
	cout << "\tmovl\t\%esp, \%ebp" << endl;
	cout << "\tsubl\t$" << gstfun.Entries["main"].size << ", \%esp" << endl;
	dis_func = "main";
	ast["main"]->jaincode();
	cout << "\tmovl\t$0, \%eax" << endl;
	cout << "\tleave" << endl;
	cout << "\tret" << endl;
	cout << "\t.size\t" << "main" << ", .-" << "main" << endl;
	cout << "\t.ident\t\"GCC: (Ubuntu 8.1.0-9ubuntu1~16.04.york1) 8.1.0\"" << endl;
	cout << "\t.section\t.note.GNU-stack,\"\",@progbits" << endl;
}

