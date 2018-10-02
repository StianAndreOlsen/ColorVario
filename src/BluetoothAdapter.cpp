#include "BluetoothAdapter.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "TizenError.h"
#include <bluetooth.h>

Kystsoft::BluetoothAdapter::BluetoothAdapter()
{
	initialize();
	// TODO: Remove logging when finished debugging
//	if (!isEnabled()) // TODO: Without this test the below name function makes the program crash without printing anything to the log file. Why?
//		return;
//	dlog(DLOG_DEBUG) << "Bluetooth adapter name = " << name();
//	dlog(DLOG_DEBUG) << "Bluetooth adapter address = " << address();
//	dlog(DLOG_DEBUG) << "Existing Bluetooth adapter name = " << name();
//	dlog(DLOG_DEBUG) << "Preferred Bluetooth adapter name = " << appName();
//	setName(appName());
//	setName("Gear S3 (053D)"); // Stian's Gear S3
//	dlog(DLOG_DEBUG) << "New Bluetooth adapter name = " << name();
}

Kystsoft::BluetoothAdapter::~BluetoothAdapter() noexcept
{
	try { deinitialize(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

bool Kystsoft::BluetoothAdapter::isEnabled()
{
	bt_adapter_state_e state = BT_ADAPTER_DISABLED;
	int error = bt_adapter_get_state(&state);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_get_state", error);
	return state == BT_ADAPTER_ENABLED;
}

std::string Kystsoft::BluetoothAdapter::address()
{
	char* localAddress = nullptr;
	int error = bt_adapter_get_address(&localAddress);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_get_address", error);
	std::string address(localAddress);
	free(localAddress);
	return address;
}

std::string Kystsoft::BluetoothAdapter::name()
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
