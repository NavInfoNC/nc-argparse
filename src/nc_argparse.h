/*
MIT License

Copyright (c) 2019 NavInfo's NaviCore Department

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include "nc_types.h"

/*
A simple example Code:

struct Options
{
	wstring packetName;
	size_t version;
};

int _tmain(int argc, char* argv[])
{
	Options options;

	ArgParser parser;
	parser.parse(argc, argv);

	if (parser.hasArg(L"version"))
	{
		cout << "2.0" << endl;
		return 0;
	}
	
	if (parser.hasArg(L"h", L"help"))
	{
		return printHelp();
	}

	parser.setDefault(L"outputVersion", L"2");	// default to 2.0
	parser.bindAliaseName(L"outputVersion", L"o");

	if (parser.argEquals(L"o", L"1"))
		options.version = 0x00010000;
	else if (parser.argEquals(L"o", L"2"))
		options.version = 0x00020000;
	else
	{
		cerr << "Unknown version number" << endl;
		return 1;
	}

	if (parser.getPositionalArgNumber() < 1)
	{
		cerr << "Output file name is missing" << endl;
		return 1;
	}
	else if (parser.getFreeOptionNumber() > 1)
	{
		cerr << "Too many output files" << endl;
		return 1;
	}

	options.packetName = parser.getPositionalArgByIndex(0);
}

*/

class ArgParser
{
public:
	ArgParser();

	void parse(int argc, char* argv[]);
	int argc() { return m_argc; }
	char** argv() { return m_argv; }

	const char* getArg(const char* key);
	const char* getArg(const char* key1, const char* key2);
	bool hasArg(const char* key);
	bool hasArg(const char* key1, const char* key2);
	bool argEquals(const char* key, const char* value);

	// alias name
	void bindAliaseName(const char* name1, const char* name2);
	const char* getAliaseName(const char* key);

	// default value
	void setDefault(const char* key, const char* v);
	const char* getDefault(const char* key);
	
	// positional argument
	forceinline size_t getPositionalArgNumber() { return _freeOptionNumber; }
	forceinline const char* getPositionalArgByIndex(size_t i) { return _freeOptions[i]; }

	// unknown arguments
	bool hasUnknownArgs();
	void resetUnknownArgIterator();
	const char* nextUnknownArg();
	bool printUnknownArgs();

	// subcommand
	const char* getSubcommand(const char* commaSplittedCommands);

private:
	int m_argc;
	char** m_argv;

	size_t _keyValueNumber;
	char* _keys[100];
	const char* _values[100];
	bool _keyUsed[100];

	size_t _defaultNumber;
	const char* _defaultKeys[100];
	const char* _defaultValues[100];

	size_t _shortNameNumber;
	const char* _shortNames[100];
	const char* _shortNameValues[100];

	size_t _freeOptionNumber;
	char* _freeOptions[100];

	size_t _unknownArgIter;

	bool _subcommandParsed;
	const char* _subcommand;

	const char* _getArgWithAliase(const char* key, bool useAliase);
};

class Subcommand
{
public:
	Subcommand() {}
	virtual ~Subcommand() {}

public:
	virtual void printHelp() = 0;
	virtual bool parseArguments(ArgParser& parse) = 0;
	virtual int run() = 0;
};
