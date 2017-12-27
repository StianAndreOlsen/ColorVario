#include "dlog.h"
#include "VarioController.h"
#include <dali/dali.h>
#include <exception>

int main(int argc, char *argv[])
{
	try
	{
		Kystsoft::dlog(DLOG_DEBUG) << "Hello!";
		Dali::Application application = Dali::Application::New(&argc, &argv);
		Kystsoft::VarioController controller(application);
		application.MainLoop();
		Kystsoft::dlog(DLOG_DEBUG) << "Goodbye!";
	}
	catch (std::exception& e)
	{
		Kystsoft::dlog(DLOG_FATAL) << e.what();
		return 1;
	}
	catch (...)
	{
		Kystsoft::dlog(DLOG_FATAL) << "Unknown error";
		return 2;
	}
	return 0;
}
