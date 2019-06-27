#include "gtest/gtest.h"
#include "../src/nc_argparse.h"

#define element_of(o) (sizeof(o) / sizeof(o[0]))

typedef std::string string_t;

TEST(ArgParser, simpleOneArgument)
{
	char* argv[] = {"cmd.exe", "abc"};

	ArgParser o;
	o.parse(element_of(argv), argv);

	EXPECT_TRUE(o.getPositionalArgNumber() == 1);
	EXPECT_EQ(o.getPositionalArgByIndex(0), string_t("abc"));
}

TEST(ArgParser, basic)
{
	char* argv[] = {"cmd.exe", "--badarg", "--version", "-o", "out.exe", "arg1", "arg2", "--badarg2", "bad", "--thread-num", "4"};

	ArgParser o;
	o.parse(element_of(argv), argv);

	EXPECT_TRUE(o.hasArg("version"));
	EXPECT_TRUE(!o.hasArg("nonExist"));
	EXPECT_TRUE(o.getArg("version") == string_t());

	EXPECT_TRUE(o.hasArg("o", "output"));

	o.bindAliaseName("o", "output");
	EXPECT_TRUE(o.getArg("output") == string_t("out.exe"));
	EXPECT_TRUE(o.getArg("o") == string_t("out.exe"));

	o.setDefault("username", "root");
	EXPECT_TRUE(o.getArg("username") == string_t("root"));

	EXPECT_TRUE(o.getArg("nonExist") == NULL);

	o.bindAliaseName("t", "thread-num");
	o.setDefault("t", "1");
	int threadNum = atoi(o.getArg("t"));
	EXPECT_TRUE(threadNum == 4);

	EXPECT_TRUE(o.hasUnknownArgs());
	const char* unknownArg;
	int c = 0;
	while((unknownArg = o.nextUnknownArg()) != NULL)
	{
		if (c == 0)
		{
			EXPECT_TRUE(string_t(unknownArg) == "badarg");
		}
		else if (c == 1)
		{
			EXPECT_TRUE(string_t(unknownArg) == "badarg2");
		}
		c++;		
	}
	EXPECT_TRUE(c == 2);

	EXPECT_TRUE(o.printUnknownArgs());
}

TEST(ArgParser, subcommand)
{
	{
		char* argv[] = {"cmd.exe"};
		ArgParser o;
		o.parse(element_of(argv), argv);
		EXPECT_TRUE(o.getSubcommand("modify") == NULL);
	}

	{
		char* argv[] = {"cmd.exe", "modify", "arg"};
		ArgParser o;
		o.parse(element_of(argv), argv);
		EXPECT_EQ(o.getSubcommand("modify, add, delete"), string_t("modify"));
		EXPECT_EQ(o.getSubcommand("add, modify, delete"), string_t("modify"));
		EXPECT_EQ(o.getSubcommand("add, delete, modify"), string_t("modify"));
		EXPECT_TRUE(o.getSubcommand("add, delete") == NULL);
		EXPECT_EQ(o.getPositionalArgNumber(), 1);
		EXPECT_EQ(o.getPositionalArgByIndex(0), string_t("arg"));
	}

	{
		char* argv[] = {"cmd.exe", "--help", "help"};
		ArgParser o;
		o.parse(element_of(argv), argv);
		EXPECT_TRUE(o.getSubcommand("help") == NULL);
	}

	{
		char* argv[] = {"cmd.exe", "help", "addition"};
		ArgParser o;
		o.parse(element_of(argv), argv);
		EXPECT_TRUE(o.getSubcommand("help, delete, add") == NULL);
	}
}
