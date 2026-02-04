/*! @file main.cpp
	@brief Contains the function definitions for creating a main
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#include <cstdlib>
#include <exception>

/*! @brief The entry point for the program
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
	@return int The status code of the program
*/
int main(int argc, char **argv)
{
#ifdef NDEBUG
	try
	{
		-
	}
	catch (const std::exception &e)
	{
		std::cerr << "Abnormal termination: " << e.what() << '\n';
		return EXIT_FAILURE;
	}
#else
	-
#endif

	return EXIT_SUCCESS;
}