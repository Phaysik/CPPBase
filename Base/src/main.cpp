/*! @file main.cpp
	@brief Contains the function definitions for creating a main
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#include <iostream>

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
	catch (...)
	{
		std::cerr << "Abnormal termination" << '\n';
	}
#else
	-
#endif

	return 0;
}