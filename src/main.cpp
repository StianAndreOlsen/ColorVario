#include "VarioController.h"

int main(int argc, char *argv[])
{
	Dali::Application application = Dali::Application::New(&argc, &argv);
	Kystsoft::VarioController controller(application);
	application.MainLoop();
	return 0;
}
