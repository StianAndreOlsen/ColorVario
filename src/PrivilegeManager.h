#ifndef KYSTSOFT_PRIVILEGEMANAGER_H
#define KYSTSOFT_PRIVILEGEMANAGER_H

#include "Signal.h"
#include <privacy_privilege_manager.h>
#include <string>

namespace Kystsoft {

class PrivilegeManager
{
public:
	PrivilegeManager() {}
	static bool hasPermission(const std::string& privilege);
	bool getPermission(const std::string& privilege);
	const auto& permissionGrantedSignal() const { return permissionGrantedSignl; }
	const auto& permissionDeniedSignal() const { return permissionDeniedSignl; }
private:
	static ppm_check_result_e checkPermission(const std::string& privilege);
	void requestPermission(const std::string& privilege);
	static void requestResponseCallback(
		ppm_call_cause_e cause,
		ppm_request_result_e result,
		const char* privilege,
		void* user_data);
	void onRequestResponse(ppm_request_result_e result, const std::string& privilege);
	Signal<std::string> permissionGrantedSignl;
	Signal<std::string> permissionDeniedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_PRIVILEGEMANAGER_H
