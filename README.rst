Introduction
------------

**nc-argparse** is a very compact command line argument parser written in C++.

With **very few** lines of code, you can create an executable which supports UNIX style arguments,
something like **Git** .
And the code will be beautiful and **maintainable**.

For example, you can detect a optional flag:

.. code-block:: cpp

   if (parser.hasArg("h", "help"))

Or to get an argument with a default value:

.. code-block:: cpp
   
   parser.setDefault("mode", "fast");
   const char* mode = parser.getArg("mode");

Demo Program
------------

The project contains a demo program for you to start with.
It has merely 140 lines of code to do all of the things below and more.

It supports "-h"::

   $ ./nc-argparse -h
   An example program to demonstrate how to use argparse.

   Syntax:

       argparse SUBCMD <OPTIONS>
       argparse -h/--help
       argparse SUBCMD -h/--help

   Subcommands:

       compile     Compile a file into another file
       test        Run Google Test

It supports subcommands::

   $ ./nc-argparse compile -h
   Compile a source file into a target file.

   Syntax:

       argparse compile SRC DEST <OPTIONS>

       SRC                 Source File
       DEST                Target File
       --mode MODE         "fast" or "slow"
       -i --interactive    Interactive mode

   $ ./nc-argparse test
   [==========] Running 3 tests from 1 test case.
   [----------] Global test environment set-up.
   [----------] 3 tests from ArgParser
   [ RUN      ] ArgParser.simpleOneArgument
   [       OK ] ArgParser.simpleOneArgument (0 ms)
   [ RUN      ] ArgParser.basic
   [       OK ] ArgParser.basic (0 ms)
   [ RUN      ] ArgParser.subcommand
   [       OK ] ArgParser.subcommand (1 ms)
   [----------] 3 tests from ArgParser (1 ms total)

   [----------] Global test environment tear-down
   [==========] 3 tests from 1 test case ran. (1 ms total)
   [  PASSED  ] 3 tests.

It supports positional arguments and optional arguments::

   $ ./nc-argparse compile a.dat b.dat --mode fast -i
   Compiling a.dat into b.dat in 'fast' mode ... Done


It supports detection of unknown arguments::

   $ ./nc-argparse compile a.dat b.dat --mode fast -L
   error: Unknown argument: L
