
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>

uint32_t cnt = 0;

class Arg {
public:
	Arg(std::string type,std::string name,bool isConst) : name(name), type(type), isConst(isConst) {}
	std::string name;
	std::string type;
	bool isConst;
};

class FunctionDef {
public:
	FunctionDef(std::string type,std::string name,bool isConst, uint32_t id)
		: name(name), type(type), isConst(isConst), id(id)
	{
	}

	std::string name;
	std::string type;
	bool isConst;
	uint32_t id;
	std::vector<Arg> args;
};


std::string trim(std::string str)
{
	std::string temp = std::regex_replace(str, std::regex("^ +"), "");
	return std::regex_replace(temp, std::regex(" +$"), "");
}


bool parseline(std::string line,std::vector<FunctionDef> &defs)
{
	int pos = line.find("(");
	if(pos == std::string::npos) {
		std::cout << "no arguments found\n" << line << "\n";
		return false;
	}

	std::string fn = line.substr(0,pos-1);
	std::string args = line.substr(pos+1);

	std::vector<std::string> results;
	bool isConst = false;
	boost::split(results, fn, [](char c) { return c == ' '; });
	if(results[0] == "const") {
		results.erase(results.begin());
		isConst = true;
	}
	if(results.size() != 2) {
		std::cout << "can\'t parse function name/type\n" << line << "\n";
		return false;
	}

	FunctionDef def(trim(results[0]),trim(results[1]),isConst,cnt++);

	results.clear();
	args = std::regex_replace(args, std::regex("\\);+$"), "");
	boost::split(results, args, [](char c) { return c == ','; });
	for(int i=0; i < results.size(); i++) {
		std::vector<std::string> a;
		results[i] = trim(results[i]);
		boost::split(a, results[i], [](char c) { return c == ' '; });

		isConst = false;
		if(a[0] == "const") {
			a.erase(a.begin());
			isConst = true;
		} else if(a[0] == "void" && a.size() == 1) {	//no arguments
			break;
		}
		if(a.size() != 2) {
			std::cout << "can\'t parse argument " << i << "\n" << line << "\n";
			return false;
		}
		def.args.push_back(Arg( trim(a[0]), trim(a[1]), isConst ));
	}

	defs.push_back(def);
	return true;
}


bool parse(const char *file,std::vector<FunctionDef> &defs)
{
bool result = true;
	std::ifstream infile(file,std::ifstream::in);
	if(!infile.is_open()) {
		return false;
	}
	std::string line;
	while (std::getline(infile, line)) {
		if(line.length() == 0) continue;
	    if(!parseline(line,defs)) {
	    	result = false;
	    	break;
	    }
	}
	infile.close();
	return result;
}

//=============================================================================

void gen_client_stub(FunctionDef &fn,std::ofstream &of)
{
	if(fn.isConst) {
		of << "const ";
	}
	of << fn.type << " " << fn.name << "(";

	if(fn.args.size() == 0) {
		of << "void";
	} else {
		bool bFirst = true;
		for(Arg &a : fn.args) {
			if(!bFirst) of << ",";
			if(a.isConst) {
				of << "const ";
			}
			of << a.type << " " << a.name;
			bFirst = false;
		}
	}
	of << ")\n{\n";

	of << "Buffer buf(" << fn.id << ");\n";
	for(Arg &a : fn.args) {
		of << "\tbuf.add(" << a.name << ");\n";
	}
	of << "\tinvoke(buf);\n";

	if(fn.type != "void") {
		of << "\t" << fn.type << " result;\n";
		of << "\tbuf.get(result);\n";
		of << "\treturn result;\n";
	}

	of << "}\n\n\n";
}


void gen_client_stubs(std::vector<FunctionDef> &defs,const char *file)
{
	std::ofstream outfile(file,std::ifstream::out);
	if(!outfile.is_open()) {
		std::cout << "can\'t open " << file;
		return;
	}

	outfile << "\n#include \"../buffer.h\"\n\n\n";

	for(int i=0; i < defs.size(); i++) {
		gen_client_stub(defs[i],outfile);
	}
	outfile.close();
}

//=============================================================================

void gen_server_stub(FunctionDef &fn,std::ofstream &of)
{
	of << "void " << fn.name << "_stub(Buffer &args, Buffer &res)\n{\n";

	for(Arg &a : fn.args) {
		of << a.type << " " << a.name << ";\n";
	}
	for(Arg &a : fn.args) {
		of << "\tEXT(args," << a.name << ");\n";
	}

	if(fn.type == "void") {
		of << "\t";
	} else {
		of << fn.type << " result = ";
	}
	of << fn.name << "(";
	bool bFirst = true;
	for(Arg &a : fn.args) {
		if(!bFirst) of << ",";
		of << a.name;
		bFirst = false;
	}
	of << ");\n";

	if(fn.type != "void") {
		of << "\tADD(res,result);\n";
	}

	of << "}\n\n\n";
}


void gen_server_stubs(std::vector<FunctionDef> &defs,const char *file)
{
	std::ofstream of(file,std::ifstream::out);
	if(!of.is_open()) {
		std::cout << "can\'t open " << file;
		return;
	}

	of << "\n#include \"../buffer.h\"\n\n\n";

	for(int i=0; i < defs.size(); i++) {
		gen_server_stub(defs[i],of);
	}

	of << "void invoke(Buffer &args,Buffer &res)\n{\n";

	of << "uint32_t id = 0;\n";
	of << "\tEXT(args,id)\n";
	of << "\tswitch(id) {\n";
	for(FunctionDef &fn : defs) {
		of << "\t\tcase " << fn.id << ": " << fn.name << "_stub(args,res);\tbreak;\n";
	}
	of << "\t\tdefault:;\n";
	of << "\t}\n";

	of << "}\n\n";
	of.close();
}

//=============================================================================

int main(int argc,char *argv[])
{
std::vector<FunctionDef> defs;

	if(argc < 2) {
		std::cout << "Usage: \ntool <header file>\n";
		return 1;
	}

	if(!parse(argv[1],defs)) {
		std::cout << "can\'t parse " << argv[1] << "\n";
		return 1;
	}

	for(int i=0; i < defs.size(); i++) {
		std::cout << defs[i].type << " " << defs[i].name << "\n";
		for(int j=0; j < defs[i].args.size(); j++) {
			std::cout << "  " << defs[i].args[j].type << " " << defs[i].args[j].name << "\n";
		}
		std::cout << "\n";
	}

	gen_client_stubs(defs,"clent.cpp");
	gen_server_stubs(defs,"server.cpp");

	return 0;
}
