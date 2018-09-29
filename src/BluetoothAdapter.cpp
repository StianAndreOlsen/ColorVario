#include "BluetoothAdapter.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "TizenError.h"
#include <bluetooth.h>

Kystsoft::BluetoothAdapter::BluetoothAdapter()
{
	initialize();
	setName(appName());
}

Kystsoft::BluetoothAdapter::~BluetoothAdapter() noexcept
{
	try { deinitialize(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

std::string Kystsoft::BluetoothAdapter::name() const
{
	char* localName = nullptr;
	int error = bt_adapter_get_name(&localName);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_get_name", error);
	std::string name(localName);
	free(localName);
	return name;
}

void Kystsoft::BluetoothAdapter::setName(const std::string& name)
{
	int error = bt_adapter_set_name(name.c_str());
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_set_name", error);
}

void Kystsoft::BluetoothAdapter::initialize()
{
	int error = bt_initialize();
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_initialize", error);
}

void Kystsoft::BluetoothAdapter::deinitialize()
{
	int error = bt_deinitialize();
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_deinitialize", error);
}
