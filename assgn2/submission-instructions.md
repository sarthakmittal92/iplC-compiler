# Submission Instructions for Assignment 2

1. You need to submit a tarred gzipped version of your directory (file name must be `group<id>.tar.gz`).

2. On untarring, it must produce the directory `group<id>` (e.g. `group1`, `group19`).

3. The directory must contain a `Makefile` that produces an executable called `iplC`. The target of the Makefile that creates the executable `iplC` should be a .PHONY called all. Similarly the target that creates the relocatable files `parser.o` and `scanner.o` should be phonies parser and lexer.

4. The Makefile must also have a target `clean` such that the command `$ make clean` deletes all generated files in the same directory.

5. The directory should have a `scanner.l` (lex script), `parser.yy` (yacc script), `driver.cpp` (driver program) and other helper files as required.

6. The directory must not contain the binary `iplC` (or any other binary file) when untarred. It must be generated only when make all is run.

7. You must use `g++-8` in Makefile for compiling.

Any deviation from this will incur a penalty.

# Evaluation of Assignment 2

Following factors will be considered for evaluation :

1.  A set of positive and negative test cases.

    The executable `iplC` should take the test program as commandline argument `$ ./iplC input.c > output.json 2> output.debug` and do the following:

    - Positive Cases:
      It should print a json object on stdout and exit with status 0 . For evaluations, Outputs will be matched using the following command
      `$ diff <(jq --sort-keys . ref.json) <(jq --sort-keys . output.json)`

    - Negative Cases:
      It should give a error message of the format `Error at line <lineno>: <Reason>` and exit with status 1 . For evaluations, line number producing error will be matched.

2.  Absence of shift-reduce or reduce-reduce conflicts in the parser.

    Use the command `$ bison -dv` in your Makefile to produce the `parser.tab.cc`, `parser.tab.hh` and `parser.output` files (do not rename them). `parser.output` contains the debug info and shows shift-reduce, reduce-reduce conflicts if any. The command `$ make all` should not report any shift-reduce or reduce-reduce conflicts. Similarly no conflicts should appear in the `parser.output` file.

# Assignment 2 Grading Policy

- Total Marks: 35 marks
- Deadline: 23:59, March 26
- No late submissions for Assignment 2 are allowed.

