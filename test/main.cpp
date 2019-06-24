#include "gtest/gtest.h"
#include "../src/nc_argparse.h"

#if defined(MAPBAR_WIN32)
#	define chdir _chdir
#endif

#define APP_NAME  "argparse"

int printHelp()
{
	printf(R"(An example program to demonstrate how to use )" APP_NAME R"(.

Syntax:

    )" APP_NAME R"( SUBCMD <OPTIONS>
    )" APP_NAME R"( -h/--help
    )" APP_NAME R"( SUBCMD -h/--help

Subcommands:
	
    compile     Compile a file into another file
    test        Run Google Test
)");

	return 0;
}

class TestSubcommand : public Subcommand
{
public:
	virtual void printHelp() override
	{
		printf("Run Google test framework.");
	}

	virtual bool parseArguments(ArgParser& parser) override
	{
		int argc = parser.argc();
		::testing::InitGoogleTest(&argc, parser.argv());

		return true;
	}

	virtual int run() override
	{
		return RUN_ALL_TESTS();
	}
};

class CompileSubcommand : public Subcommand
{
public:
	void printHelp() override
	{
		printf(R"(Compile a source file into a target file.

Syntax:

    argparse compile SRC DEST <OPTIONS>

    SRC                 Source File
    DEST                Target File
    --mode MODE         "fast" or "slow"
    -i --interactive    Interactive mode

)");
	}

	bool parseArguments(ArgParser& parser) override
	{
		if (parser.getPositionalArgNumber() != 2)
		{
			printf("error: needs two arguments\n");
			return false;
		}
		m_srcFile = parser.getPositionalArgByIndex(0);
		m_destFile = parser.getPositionalArgByIndex(1);

		m_interactive = parser.hasArg("i", "interactive");
		m_mode = parser.getArg("mode");
		return true;
	}

	int run() override
	{
		printf("Compiling %s into %s ... Done\n", m_srcFile, m_destFile);
		return 0;
	}

private:
	const char* m_srcFile;
	const char* m_destFile;
	bool m_interactive;
	const char* m_mode;
};

int main(int argc, char** argv)
{
	int result = 0;

	ArgParser parser;
	parser.parse(argc, argv);

	bool hasHelp = parser.hasArg("h", "help");

	const char* commandName = parser.getSubcommand("compile,test");
	if (commandName == NULL)
	{
		if (hasHelp)
			return printHelp();
	}

	Subcommand* cmd = NULL;
	if (strcmp(commandName, "test") == 0)
	{
		cmd = new TestSubcommand;
	}
	else if (strcmp(commandName, "compile") == 0)
	{
		cmd = new CompileSubcommand;
	}

	if (cmd != NULL)
	{
		if (hasHelp)
		{
			cmd->printHelp();
			return 0;
		}

		if (cmd->parseArguments(parser))
		{
			if (!parser.hasUnknownArgs())
			{
				result = cmd->run();
			}
			else
			{
				parser.printUnknownArgs();
			}
		}
	}

	delete cmd;

	return result;
}
