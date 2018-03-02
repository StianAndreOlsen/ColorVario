#ifndef KYSTSOFT_APPMANAGER_H
#define KYSTSOFT_APPMANAGER_H

#include <app_manager.h>
#include <string>

namespace Kystsoft {

class AppManager
{
public:
	AppManager(const std::string& id = std::string());
	~AppManager() noexcept;
	AppManager(const AppManager& other) = delete;
	AppManager& operator=(const AppManager& rhs) = delete;
	void resume();
private:
	void getContext(const std::string& id);
	void destroyContext();
	app_context_h context = nullptr;
};

} // namespace Kystsoft

#endif // KYSTSOFT_APPMANAGER_H
