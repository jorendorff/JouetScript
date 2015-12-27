default:
	clang++ -std=c++1y -Wall --debug shell.cpp jscript.cpp lexer.cpp jsvalue.cpp -o js
