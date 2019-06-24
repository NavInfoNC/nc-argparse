#include "gtest/gtest.h"
#include "../src/nc_argparse.h"

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
    --mode MODE         "fast" or "slow". "fast" is the default.
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
		parser.setDefault("mode", "fast");
		m_mode = parser.getArg("mode");
		if (strcmp(m_mode, "fast") != 0 && strcmp(m_mode, "slow") != 0)
		{
			printf("error: mode should be 'fast' or 'slow'");
			return false;
		}
		return true;
	}

	int run() override
	{
		printf("Compiling %s into %s in '%s' mode ... Done\n", m_srcFile, m_destFile, m_mode);
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
		return -1;
	}

	Subcommand* cmd = NULL;
	if (strcmp(commandName, "test") == 0)
		cmd = new TestSubcommand;
	else if (strcmp(commandName, "compile") == 0)
		cmd = new CompileSubcommand;

	if (cmd != NULL)
	{
		if (hasHelp)
		{
			cmd->printHelp();
		}
		else if (cmd->parseArguments(parser))
		{
			if (!parser.hasUnknownArgs())
				result = cmd->run();
			else
				parser.printUnknownArgs();
		}
	}

	delete cmd;

	return result;
}
