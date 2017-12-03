#include "VarioController.h"
#include <exception>
#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		Dali::Application application = Dali::Application::New(&argc, &argv);
		Kystsoft::VarioController controller(application);
		application.MainLoop();
	}
	// TODO: Check where std::cerr is routed and consider other options
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		return 2;
	}
	return 0;
}
