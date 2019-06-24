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
#include "nc_argparse.h"

ArgParser::ArgParser()
{
	_defaultNumber = 0;
	_shortNameNumber = 0;
	_unknownArgIter = 0;
	_subcommandParsed = false;
	_subcommand = NULL;
}

void ArgParser::bindAliaseName(const char* name1, const char* name2)
{
	_shortNames[_shortNameNumber] = name1;
	_shortNameValues[_shortNameNumber] = name2;
	_shortNameNumber++;
}

const char* ArgParser::getAliaseName(const char* key)
{
	for (size_t i = 0; i < _shortNameNumber; i++)
	{
		if (strcmp(key, _shortNames[i]) == 0)
		{
			return _shortNameValues[i];
		}
		else if (strcmp(key, _shortNameValues[i]) == 0)
		{
			return _shortNames[i];
		}
	}

	return NULL;
}

void ArgParser::setDefault(const char* key, const char* v)
{
	_defaultKeys[_defaultNumber] = key;
	_defaultValues[_defaultNumber] = v;
	_defaultNumber++;
}

const char* ArgParser::getDefault(const char* key)
{
	for (size_t i = 0; i < _defaultNumber; i++)
	{
		if (strcmp(key, _defaultKeys[i]) == 0)
		{
			return _defaultValues[i];
		}
	}

	return NULL;
}

void ArgParser::parse(int argc, char* argv[])
{
	_keyValueNumber = 0;
	_freeOptionNumber = 0;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == '-')
				_keys[_keyValueNumber] = argv[i] + 2; // --version
			else
				_keys[_keyValueNumber] = argv[i] + 1;	// -v

			if (i + 1 < argc && argv[i + 1][0] != '-')
			{
				_values[_keyValueNumber] = argv[i + 1];
				i++;
			}
			else
				_values[_keyValueNumber] = "";

			_keyUsed[_keyValueNumber] = false;

			_keyValueNumber++;
		}
		else
		{
			_freeOptions[_freeOptionNumber++] = argv[i];
		}
	}
}

const char* ArgParser::getArg(const char* key)
{
	return _getArgWithAliase(key, true);
}

const char* ArgParser::_getArgWithAliase(const char* key, bool useAliase)
{
	for (size_t i = 0; i < _keyValueNumber; i++)
	{
		if (strcmp(key, _keys[i]) == 0)
		{
			_keyUsed[i] = true;
			return _values[i];
		}
	}

	if (useAliase)
	{
		const char* aliaseName = getAliaseName(key);
		if (aliaseName != NULL)
		{
			const char* value = _getArgWithAliase(aliaseName, false);
			if (value != NULL)
				return value;
		}
	}

	const char* defaultValue = getDefault(key);
	if (defaultValue)
		return defaultValue;

	return NULL;
}

const char* ArgParser::getArg(const char* key1, const char* key2)
{
	const char* v = getArg(key1);
	if (v == NULL)
		v = getArg(key2);
	return v;
}

bool ArgParser::hasArg(const char* key)
{
	return getArg(key) != NULL;
}

bool ArgParser::hasArg(const char* key1, const char* key2)
{
	return hasArg(key1) || hasArg(key2);
}

bool ArgParser::argEquals(const char* key, const char* value)
{
	const char* v = getArg(key);
	return v != NULL && strcmp(v, value) == 0;
}

bool ArgParser::hasUnknownArgs() 
{
	for (size_t i = 0; i < _keyValueNumber; i++)
	{
		if (!_keyUsed[i])
			return true;
	}
	return false;
}

const char* ArgParser::nextUnknownArg() {
	while (_unknownArgIter != _keyValueNumber && _keyUsed[_unknownArgIter])
		_unknownArgIter++;

	if (_unknownArgIter == _keyValueNumber)
		return NULL;
	else
		return _keys[_unknownArgIter++];
}

void ArgParser::resetUnknownArgIterator() {
	_unknownArgIter = 0;
}

bool ArgParser::printUnknownArgs() {
	const char* unknownArg;
	bool has = false;
	resetUnknownArgIterator();
	while ((unknownArg = nextUnknownArg()) != NULL)
	{
		has = true;
		printf("error: Unknown argument: %s\n", unknownArg);
	}

	return has;
}

static bool _isSubcommand(const char* commaSplittedCommands, const char* subcommand)
{
	const char* p = strstr(commaSplittedCommands, subcommand);
	char nextChar;
	return p != NULL
		&& (p == commaSplittedCommands || *(p - 1) == ',' || *(p - 1) == ' ')
		&& ((nextChar = *(p + strlen(subcommand))) == ',' || nextChar == ' ' || nextChar == '\0');
}

const char* ArgParser::getSubcommand(const char* commaSplittedCommands) 
{
	// parse sub-command
	if (!_subcommandParsed && _freeOptionNumber > 0)
	{
		_subcommand = _freeOptions[0];
		_freeOptionNumber--;
		memmove(_freeOptions, _freeOptions + 1, sizeof(_freeOptions[0]) * _freeOptionNumber);

		_subcommandParsed = true;
	}

	if (_subcommand == NULL)
	{
		printf("error: No subcommand is given. \n");
		return NULL;
	}

	if (_isSubcommand(commaSplittedCommands, _subcommand))
	{
		if (strcmp(_subcommand, "help") == 0)
		{
			if (getPositionalArgNumber() != 1)
			{
				printf("error: Syntax error. Please use \"help SUBCMD\"\n");
			}
			else if (!_isSubcommand(commaSplittedCommands, getPositionalArgByIndex(0)))
			{
				printf("error: Unknown subcommand: %s\n", getPositionalArgByIndex(0));
				return NULL;
			}
		}
		return _subcommand;
	}
	else
	{
		printf("error: Unknown subcommand: %s\n", _subcommand);
		return NULL;
	}
}

