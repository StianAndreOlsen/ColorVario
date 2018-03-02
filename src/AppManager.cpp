#include "AppManager.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::AppManager::AppManager(const std::string& id /*= std::string()*/)
{
	getContext(id.empty() ? appId() : id);
}

Kystsoft::AppManager::~AppManager() noexcept
{
	try { destroyContext(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::AppManager::resume()
{
	int error = app_manager_resume_app(context);
	if (error != APP_MANAGER_ERROR_NONE)
		throw TizenError("app_manager_resume_app", error);
}

void Kystsoft::AppManager::getContext(const std::string& id)
{
	int error = app_manager_get_app_context(id.c_str(), &context);
	if (error != APP_MANAGER_ERROR_NONE)
		throw TizenError("app_manager_get_app_context", error);
}

void Kystsoft::AppManager::destroyContext()
{
	int error = app_context_destroy(context);
	if (error != APP_MANAGER_ERROR_NONE)
		throw TizenError("app_context_destroy", error);
}
