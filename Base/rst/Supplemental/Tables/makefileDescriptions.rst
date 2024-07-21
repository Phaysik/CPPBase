Makefile Descriptions
=====================
Below is a list of all the :file:`Makefile` commands along with a breakdown of what each command does.

.. table:: Makefile Command Descriptions
    :class: [centered_table, evenly_spaced_descriptions]

    +----------------------------+----------------------------------------------------------------------------------------------------+
    | Makefile Command           | Action                                                                                             |
    +============================+====================================================================================================+
    | compile                    | Creates a release executable in the output folder.                                                 |
    |                            |                                                                                                    |
    |                            | Copies resource folder to the output folder.                                                       |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | release                    | Runs the compile command.                                                                          |
    |                            |                                                                                                    |
    |                            | Runs the release executable in the output folder.                                                  |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | debug                      |                                                                    |
    |                            | Creates a dev executable in the output folder with debugging information in the compiled code.     |
    |                            |                                                                                                    |
    |                            | Copies resource folder to the output folder.                                                       |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | dev                        | Runs the debug command.                                                                            |
    |                            |                                                                                                    |
    |                            | Runs the dev executable in the output folder.                                                      |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | val                        | Creates a valgrind executable in the output folder with memory debugging information               |
    |                            | in the compiled code explicitly for valgrind.                                                      |
    |                            |                                                                                                    |
    |                            | Copies resource folder to the output folder.                                                       |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | valgrind                   | Runs the debug command.                                                                            |
    |                            |                                                                                                    |
    |                            | Runs a memory checker on the executable in the output folder to see if there's any memory leaks.   |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | copy_and_run_tests         | Copies the resource folder to the output folder.                                                   |
    |                            |                                                                                                    |
    |                            | Copies and runs the test executable from the test folder to the output folder.                     |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | build_tests                | Compiles a test executable with Google Test flags. Runs the copy_and_run_tests command.            |
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
    | tidy                       | Runs clang tidy on the code base.                                                                  |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | check                      | Runs cppcheck on the code base.                                                                    |
    +----------------------------+---------------------------------+------------------------------------------------------------------+
    | flawfinder                 | Runs flawfinder on the code base.                                                                  |
    +----------------------------+---------------------------------+------------------------------------------------------------------+
    | analysis                   | Runs flawfinder, check, and clang tidy the code base.                                              |
    +----------------------------+---------------------------------+------------------------------------------------------------------+
    | format                     | Runs clang format on the code base.                                                                |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | run_doxygen                | Runs Doxygen on the Doxyfile.                                                                      |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | docs                       | Runs the run_doxygen command.                                                                      |
    |                            |                                                                                                    |
    |                            | Uses sphinx to build the docs from the Doxygen XML output.                                         |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | tracy                      | Creates an executable with the appropriate flags for the Tracy Profile server.                     |
    |                            |                                                                                                    |
    |                            | This is only the client, you need to already be running the Tracy Profiler Server.                 |
    |                            |                                                                                                    |
    |                            | The Server also needs to be currently listening for the incoming client connection.                |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | gprof                      | Runs the dev command.                                                                              |
    |                            |                                                                                                    |
    |                            | Creates a profiling folder that contains the annotations and flat map of gprof.                    |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | profile                    | Runs the gprof command.                                                                            |
    |                            |                                                                                                    |
    |                            | Moves the created annotations files from gprof into the profiling annotations folder.              |
    +----------------------------+----------------------------------------------------------------------------------------------------+
    | initialize_repo            | Clones a base C++ repository structure into the current directory.                                 |
    |                            |                                                                                                    |
    |                            | Does not need to be executed individually.                                                         |
    +----------------------------+----------------------------------------------------------------------------------------------------+
