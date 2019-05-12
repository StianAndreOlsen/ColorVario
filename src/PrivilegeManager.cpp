#include "PrivilegeManager.h"
#include "TizenError.h"

bool Kystsoft::PrivilegeManager::hasPermission(const std::string& privilege)
{
	auto result = checkPermission(privilege);
	return result == PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW;
}

bool Kystsoft::PrivilegeManager::getPermission(const std::string& privilege)
{
	auto result = checkPermission(privilege);
	switch (result)
	{
	case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
		return true;
	case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
		break;
	case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
		requestPermission(privilege);
		break; // no permission yet
	}
	return false;
}

namespace
{
	std::string prefix("http://tizen.org/");
}

ppm_check_result_e Kystsoft::PrivilegeManager::checkPermission(const std::string& privilege)
{
	ppm_check_result_e result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY;
	int error = ppm_check_permission((prefix + privilege).c_str(), &result);
	if (error != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
		throw TizenError("ppm_check_permission", error);
	return result;
}

void Kystsoft::PrivilegeManager::requestPermission(const std::string& privilege)
{
	int error = ppm_request_permission((prefix + privilege).c_str(), requestResponseCallback, this);
	if (error != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
		throw TizenError("ppm_request_permission", error);
}

void Kystsoft::PrivilegeManager::requestResponseCallback(
	ppm_call_cause_e cause,
	ppm_request_result_e result,
	const char* privilege,
	void* user_data)
{
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR)
		throw std::runtime_error("PrivilegeManager::requestResponseCallback was called because of an error");

	// remove prefix from privilege string
	privilege += prefix.size();

	PrivilegeManager* manager = static_cast<PrivilegeManager*>(user_data);
	if (manager)
		manager->onRequestResponse(result, privilege);
}

void Kystsoft::PrivilegeManager::onRequestResponse(ppm_request_result_e result, const std::string& privilege)
{
	if (result == PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER)
		permissionGrantedSignl.emit(privilege);
	else
		permissionDeniedSignl.emit(privilege);
}
