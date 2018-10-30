#include "stdafx.h"
#include "console.h"



console::console()
{
	console::printAscii();
	//console::printHelp();
	console::poll();
}

void console::poll()
{
	std::vector<std::string> tokens;
	std::string token;
	std::string inputString;
	bool isTokenized = false;
	while (true) {
		//poll
		printf("~~> "); // such a nice prompt
		std::getline(std::cin, inputString);
		tokens = console::delimitString(inputString);
		if (tokens.size() == 0) { continue; }
		parseInput(tokens);
		tokens.clear(); // empty input tokens
	}
	
}

void console::parseInput(std::vector<std::string> tokens) {
	transformLower(tokens.at(0)); // convert cmd name to lowercase
	//wipe empty variables
	size_t tmpSize = tokens.size();

	size_t tokensSize = tokens.size();
	if (tokens.at(0).compare("help") == 0) {
		if (tokensSize > 1) {
			transformLower(tokens.at(1));
			if (tokens.at(1).compare("add") == 0) {
				//print help for add
				if (tokensSize == 2) {
					printf("[*] Use add to create variables, types, structures or functions. Use \"help add\" + item name to get more info about adding specific items \n");
					printf("[*] Available types:\nvar\ntype\nfunc\nstruct\n");
				}
				else {
					transformLower(tokens.at(2));
					if (tokens.at(2).compare("var") == 0) {
						printf("[*] Use following commad to create variable: add var VARIABLETYPE VARIABLENAME [VALUE]\n");
					}
					else if (tokens.at(2).compare("type") == 0) {
						printf("[*] Use following commad to create type: add type TYPENAME TYPESIZE [OUTPUT_FORMAT]\n");
					}
					else if (tokens.at(2).compare("func") == 0) {
						printf("[*] Use following commad to resolve function: add func LIBRARY FUNCTION_NAME ARGC [RETURN_TYPE\"NULL\"]\n");
					}
					else if (tokens.at(2).compare("struct") == 0) {
						printf("[*] Use following commad to create type: add struct STRUCT_NAME NUMBER_OF_FIELDS\n");
					}


				}
			}
			else if (tokens.at(1).compare("load") == 0) {
				printf("[*] Use load + library name (absolute path also acceptable) to load library for function resolving\n");
			}
			else if (tokens.at(1).compare("print") == 0) {
				printf("[*] Print types,vars or their values\n");
			}
			else if (tokens.at(1).compare("shell") == 0) {
				printf("[*] It's just dropping you to shell\n");
			}
			else if (tokens.at(1).compare("call") == 0) {
				printf("[*] To call a resolved function use call + functionName\n");
			}
			else if (tokens.at(1).compare("quickcall") == 0) {

			}
		}
		else {
			console::printHelp();
		}
	}
	else if (tokens.at(0).compare("exit") == 0) {
		console::exit();
	}
	else if (tokens.at(0).compare("add") == 0) {
		if (tokensSize < 3) {
			printf("[-] Too few arguments\n");
			return;
		}
		transformLower(tokens.at(1));
		if (tokens.at(1).compare("var") == 0) {
			// call add variable
			int format = FORMAT_HEX;
			std::pair<int, uintptr_t> result;
			if (tokensSize > 4) {
				this->curSession.createVariable(tokens.at(2), tokens.at(3), (uintptr_t)tokens.at(4).c_str()); // placeholder
			}
			else if(tokensSize == 4){
				this->curSession.createVariable(tokens.at(2), tokens.at(3), 0);
			}
			else {
				printf("[-] Too few arguments\n");
			}
		}
		else if (tokens.at(1).compare("type") == 0) {
			// call add type
			if (tokensSize >= 5) {
				if (tokens.at(4).compare("hex") == 0) {
					this->curSession.addType(tokens.at(2), atoi(tokens.at(3).c_str()), FORMAT_HEX);
				}
				else if(tokens.at(4).compare("str") == 0){
					this->curSession.addType(tokens.at(2), atoi(tokens.at(3).c_str()), FORMAT_STRING);
				}
				else if (tokens.at(4).compare("int") == 0) {
					this->curSession.addType(tokens.at(2), atoi(tokens.at(3).c_str()), FORMAT_INT);
				}
				else {
					printf("[-] Unknown type\n");
				}

			}
			else if(tokensSize == 4){
				this->curSession.addType(tokens.at(2), atoi(tokens.at(3).c_str()), FORMAT_HEX); // default
			}
			else
			{
				printf("[-] Too few arguments\n");
			}
			
		}
		else if (tokens.at(1).compare("func") == 0) {
			// call add function
			if (tokensSize < 6) {
				printf("[-] Too few arguments\n");
				return;

			}
			transformLower(tokens.at(2)); //lowercase module name
			int res = this->curSession.addFunc(tokens.at(2), tokens.at(3), atoi(tokens.at(4).c_str()), tokens.at(5));
			if (res == FUNCTION_NOT_FOUND) {
				printf("[-] Function with that name wasn't resolved\n");
				return;
			}
			else if (res == LIBRARY_NOT_LOADED) {
				printf("[-] Library with that name wasn't resolved\n");
				return;
			}
			printf("[+] Function resolved\n");
		}
		else if (tokens.at(1).compare("struct") == 0) {
			// call add structure
			if (tokensSize >= 4) {
				this->curSession.defineStruct(tokens.at(2), atoi(tokens.at(3).c_str()));
			}
			else {
				printf("[-] Please speicify number of fields in structure\n");
			}
		}
		else {
			printf("[-] Unknown type for add command\n");
			return;
		}
	}
	else if (tokens.at(0).compare("load") == 0) {
		if (tokensSize < 2) {
			printf("[-] Please provide library name\n");
			return;
		}
		
		//concat strings that have whitespace in path
		std::string libName;
		for (int i = 1; i < tokensSize; i++)
		{
			libName += tokens.at(i) + ' ';
		}
		//clear last whitespace
		libName = libName.substr(0, libName.size() - 1);

		transformLower(libName); // to lowercase dll name
		// call dll loader
		int res = this->curSession.loadLibrary(libName);
		if (res == LIBRARY_NOT_FOUND) {
			printf("[-] Library wasn't found\n");
		}
		else {
			printf("[+] Library was loaded successfully\n");
		}
	}
	else if (tokens.at(0).compare("shell") == 0) {
		console::drop2Shell();
	}
	else if (tokens.at(0).compare("call") == 0) {
		if (tokensSize > 2) {
			printf("[-] Please provide function name to call\n");
			return;
		}
		else {
			// call with args
			this->curSession.callWrapper(tokens.at(1));
		}
	}
	else if (tokens.at(0).compare("print") == 0) {
		if (tokensSize < 2) {
			printf("[-] Too few arguments\n");
		}
		else
		{
			bool unknown = false;
			transformLower(tokens.at(1));
			if (tokensSize > 2) {
				if (tokens.at(1).compare("var") == 0) {
					this->curSession.printVariableValue(tokens.at(2));
				}
				else if (tokens.at(1).compare("func") == 0) {
					this->curSession.printFuncData(tokens.at(2));
				}
				else if (tokens.at(1).compare("type") == 0) {
					this->curSession.printTypeData(tokens.at(2));
				}
				else {
					unknown = true;
				}
			}
			else {
				if (tokens.at(1).compare("vars") == 0) {
					this->curSession.printVariables();
				}
				else if (tokens.at(1).compare("types") == 0) {
					this->curSession.printTypes();
				}
				else if (tokens.at(1).compare("libs") == 0) {
					this->curSession.printLoadedLibs();
				}
				else if (tokens.at(1).compare("funcs") == 0) {
					this->curSession.printFuctions();
				}
				else {
					unknown = true;
				}
			}
			if (unknown) {
				printf("[-] Unknown element\n");
			}
			
		}

	}
	else if (tokens.at(0).compare("quickcall") == 0) {
		// to do, implement quickcall
	}
	else if (tokens.at(0).compare("del") == 0) {
		if (tokensSize < 2) {
			printf("[-] Please provide variable name\n");
			return;
		}
		this->curSession.deleteVariable(tokens.at(1));
	}
	else if (tokens.at(0).compare("raw") == 0) {
		if (tokensSize < 2) {
			printf("[-] Please specify operation (read\write)\n");
		}
		else {
			transformLower(tokens.at(1));
			if (tokens.at(1).compare("read") == 0) {
				printf("[?] addr, size: ");
				std::string tmpInput;
				std::vector<std::string> tmpTokens;
				std::getline(std::cin, tmpInput);
				tmpTokens = delimitString(tmpInput);
				size_t tmpSize = tmpTokens.size();
				if (tmpSize != 2) {
					printf("[-] Wrong amount of arguments\n");
					return;
				}
				else {
					
					uintptr_t *dstPointer = (uintptr_t*) malloc(sizeof(uintptr_t));
					if (dstPointer == nullptr) {
						printf("[-] Allocation error\n");
						return;
					}
					this->curSession.processData(tmpTokens.at(0), atoi(tmpTokens.at(1).c_str()), dstPointer);
					printf("[+] Data: ");
					this->curSession.printWithFormat(atoi(tmpTokens.at(1).c_str()), (uintptr_t*)*dstPointer, FORMAT_HEX);
					free(dstPointer);
				}
				
			}
			else if (tokens.at(1).compare("write") == 0) {
				printf("[?] addr, size, value: ");
				std::string tmpInput;
				std::vector<std::string> tmpTokens;
				std::getline(std::cin, tmpInput);
				tmpTokens = delimitString(tmpInput);
				size_t tmpSize = tmpTokens.size();
				if (tmpSize != 3) {
					printf("[-] Wrong amount of arguments\n");
					return;
				}
				uintptr_t *dstPointer = (uintptr_t*) malloc(sizeof(uintptr_t));
				if (dstPointer == nullptr) {
					printf("[-] Allocation error\n");
					return;
				}
				this->curSession.processData(tmpTokens.at(0), atoi(tmpTokens.at(1).c_str()), dstPointer);
				this->curSession.processData(tmpTokens.at(2), atoi(tmpTokens.at(1).c_str()), (uintptr_t*)*dstPointer);
				free(dstPointer);

			}
			else {
				printf("[-] Unknown operation\n");
			}
		}
	}
	else if (tokens.at(0).compare("edit") == 0) {
		if (tokensSize < 5) {
			printf("[-] Too few arguments\n");
		}
		else {
			transformLower(tokens.at(1));
			int field_type = 0;
			if (tokens.at(1).compare("var")==0) {
				transformLower(tokens.at(3));
				if (tokens.at(3).compare("name") == 0) {
					field_type = TYPE_FIELD_NAME;
				}
				else if (tokens.at(3).compare("val") == 0) {
					field_type = TYPE_FIELD_VAL;
				}
				else if (tokens.at(3).compare("addr") == 0) {
					field_type = TYPE_FIELD_ADDR;
				}
				else if (tokens.at(3).compare("type") == 0) {
					field_type = TYPE_FIELD_TYPE;
				}
				if (field_type != 0) {
					this->curSession.editVar(field_type, tokens.at(4), tokens.at(2));
				}
				else {
					printf("[-] Unknown field\n");
				}
			}
			else if (tokens.at(1).compare("type") == 0) {
				if (tokens.at(3).compare("name") == 0) {
					field_type = TYPE_FIELD_NAME;
				}
				else if (tokens.at(3).compare("size") == 0) {
					field_type = TYPE_FIELD_SIZE;
				}
				if (field_type != 0) {
					this->curSession.editType(field_type, tokens.at(4), tokens.at(2));
				}
				else {
					printf("[-] Unknown field\n");
				}
			}
			else if (tokens.at(1).compare("func") == 0) {
				if (tokens.at(3).compare("return") == 0) {
					field_type = TYPE_FIELD_RET;
				}
				else if (tokens.at(3).compare("argc") == 0) {
					field_type = TYPE_FIELD_ARGC;
				}
				if (field_type != 0) {
					this->curSession.editFunc(field_type, tokens.at(4), tokens.at(2));
				}
				else {
					printf("[-] Unknown field\n");
				}
			}
			else {
				printf("[-] Unknown element\n");
			}

		}
	}
	else if (tokens.at(0).compare("shellcode") == 0) {
		if (tokensSize < 3) {
			printf("[-] Provide shellcode address and size\n");
			return;
		}
		uintptr_t shellcodeAddr = (uintptr_t)malloc(sizeof(uintptr_t));
		this->curSession.processData(tokens.at(1), sizeof(uintptr_t), (void *)shellcodeAddr);
		if (tokensSize == 4) {
			transformLower(tokens.at(3));
			if (tokens.at(3).compare("noexec") == 0) {
				this->curSession.execShellcode((void*)*(uintptr_t*)shellcodeAddr, atoi(tokens.at(2).c_str()),true);
				return;
			}
		}
		this->curSession.execShellcode((void*)*(uintptr_t*)shellcodeAddr, atoi(tokens.at(2).c_str()),false);


	}
	else {
		printf("[-] Unknown command, use \"help\" for reference\n");
		return;
	}

}

std::vector<std::string> console::delimitString(std::string stringToDelimit)
{
	if (stringToDelimit.empty()) {
		return std::vector<std::string>();
	}
	std::string tmpStr;
	std::vector<std::string> tokens;
	bool isEnquotedStr = false;
	bool enquotedCorrectly = true;
	bool skipNextChar = false;
	char curChar;
	char quote;
	for (int i = 0; i < stringToDelimit.size(); i++) {
		curChar = stringToDelimit.at(i);
		if (isEnquotedStr) {
			if (enquotedCorrectly) { break; }
			if (skipNextChar) { skipNextChar = false; continue; }
			switch (curChar) {
			case '\\':
				if ((i + 1) < stringToDelimit.size()) {
					switch (stringToDelimit.at(i+1))
					{
					case 'n':
						tmpStr.push_back('\n');
						break;
					case 't':
						tmpStr.push_back('\t');
						break;
					case '\'':
						tmpStr.push_back('\'');
						break;
					case '"':
						tmpStr.push_back('"');
						break;
					case '\\':
						tmpStr.push_back('\\');
						break;
					default:
						tmpStr.push_back('\\');
						break;
					}
					skipNextChar = true;

				}
				break;
			case '\'':
			case '"':
				tmpStr.push_back(curChar);
				if(curChar == quote)
				{
					enquotedCorrectly = true;
				}
				break;
			default:

				tmpStr.push_back(curChar);
				break;
			}
			
			
		}
		else {
			switch (curChar)
			{
			case '\n':
			case '\0':
				tokens.push_back(tmpStr);
				break;
			case ' ':
			case '\t':
				if (!tmpStr.empty()) {
					tokens.push_back(tmpStr);
					tmpStr.clear();
				}
				break;
			case '"':
			case '\'':
				tmpStr.push_back(curChar);
				quote = curChar;
				isEnquotedStr = true;
				enquotedCorrectly = false;
				break;
			default:
				tmpStr.push_back(curChar);
				break;
			}
		}
	}
	tokens.push_back(tmpStr);
	if (!enquotedCorrectly) {
		printf("[-] String is not enquoted properly\n");
		return std::vector<std::string>();
	}
	return tokens;
	
}



void console::printHelp() 
{
	printf("Use help + commandname to get more info\n");
	printf("Available commands:\n");
	printf("[*] help\n");
	printf("[*] add\n");
	printf("[*] shell\n");
	printf("[*] print\n");
	printf("[*] load\n");
	printf("[*] call\n");
	printf("[*] quickcall\n");
	printf("[*] exit\n");
	

}

void console::exit()
{
	//close the program
	_exit(0);
}


void console::drop2Shell()
{
	
	printf("[+] Executing shell now\n\n");
	//powershell or command prompt?
	system("cmd.exe");
	printf("[+] Shell closed\n");
}

void console::printAscii()
{
	printf("\n");
	printf("  ___ ___ ___ _    ___ _   _ _  _\n");
	printf(" / __/ _ \\_ _| |  / __| | | | \\| |\n");
	printf("| (_| (_) | || |_| (_ | |_| | .` |\n");
	printf(" \\___\\___/___|____\\___|\\___/|_|\\_|\n");
	printf("\n");
}

void console::transformLower(std::string &toTransform) {
	std::transform(toTransform.begin(), toTransform.end(), toTransform.begin(), ::tolower);
}

console::~console()
{
	
}
