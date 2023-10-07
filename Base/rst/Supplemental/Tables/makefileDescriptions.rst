Makefile Descriptions
=====================
Below is a list of all the :file:`Makefile` commands along with a breakdown of what each command does.

.. table:: Makefile Command Descriptions
    :class: [centered_table, evenly_spaced_descriptions]

    +----------------------------+----------------------------------------------------------------------------------------------------+
    | Makefile Command           | Action                                                                                             |
    +============================+====================================================================================================+
    | compile                    | Runs the create_folders command.                                                                   |
    |                            |                                                                                                    |
    |                            | Creates an executable in the output folder.                                                        |
    |                            |                                                                                                    |
    |                            | Copies resource folder to the output folder.                                                       |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | run                        | Runs the compile command.                                                                          |
    |                            |                                                                                                    |
    |                            | Runs the executable in the output folder.                                                          |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | debug                      | Creates an executable in the output folder with debugging information in the compiled code.        |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | valgrind                   | Runs the debug command.                                                                            |
    |                            |                                                                                                    |
    |                            | Runs a memory checker on the executable in the output folder to see if there's any memory leaks.   |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | delete_lcov                | Delete any \*.gcno, \*.gcda, and \*.info files associated with the lcov command                    |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | copy_and_run_test          | Copies the resource folder to the output folder.                                                   |
    |                            |                                                                                                    |
    |                            | Copies and runs the test executable from the test folder to the output folder.                     |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | build_test                 | Runs the create_folders and delete_lcov commands.                                                  |
    |                            |                                                                                                    |
    |                            | Compiles a test executable with Google Test flags. Runs the copy_and_test command.                 |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | lcov                       | Runs the build_test command.                                                                       |
    |                            |                                                                                                    |
    |                            | Creates lcov files on the entire codebase.                                                         |
    |                            |                                                                                                    |
    |                            | Then removes the lcov files associated with the lcov folder.                                       |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | genhtml                    | Runs the lcov command.                                                                             |
    |                            |                                                                                                    |
    |                            | Deletes and recreates the genhtml output folder.                                                   |
    |                            |                                                                                                    |
    |                            | Creates a set of web pages to view the code coverage of the codebase in the genhtml output folder. |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | coverage                   | Runs the genhtml command.                                                                          |
    |                            |                                                                                                    |
    |                            | Deletes the test output and lcov output folders.                                                   |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | tidy_setup                 | Creates a clang tidy config file based on allowed and disallowed checks.                           |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | fix_tidy_warnings          | Replace certain values in the clang tidy file that are erroneously created during tidy_setup.      |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | tidy                       | Runs the tidy_setup and fix_tidy_warnings commands.                                                |
    |                            |                                                                                                    |
    |                            | Runs clang tidy on the code base.                                                                  |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | create_doxygen             | Creates a Doxygen config file.                                                                     |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | change_doxygen_options     | Runs the create_doxygen command.                                                                   |
    |                            |                                                                                                    |
    |                            | Changes the default values of the Doxygen config file.                                             |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | create_breathe_file        | Creates the breathe config file for sphinx.                                                        |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | create_index_rst_file      | Creates the index.rst file which displays the sphinx docs.                                         |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | determine_index_rst_exists | Runs the create_breathe_file command.                                                              |
    |                            |                                                                                                    |
    |                            | Checks if index.rst exists and if it doesn't it runs the create_index_rst_file command.            |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | doxygen_complete           | Runs the change_doxygen_options command.                                                           |
    |                            |                                                                                                    |
    |                            | Runs the doxygen config file and then deletes the .bak that is created.                            |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | docs                       | Runs the doxygen_complete and determine_index_rst_exists commands.                                 |
    |                            |                                                                                                    |
    |                            | Uses sphinx to build the docs from the Doxygen XML output.                                         |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | create_folders             | Creates the boilerplate folders for running other commands.                                        |
    |                            |                                                                                                    |
    |                            | Does not need to be executed individually.                                                         |
    +----------------------------+----------------------------------------------------------------------------------------------------+