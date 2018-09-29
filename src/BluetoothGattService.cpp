#include "BluetoothGattService.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::BluetoothGattService::BluetoothGattService(const std::string& uuid, bt_gatt_service_type_e type /*= BT_GATT_SERVICE_TYPE_PRIMARY*/)
{
	create(uuid, type);
}

Kystsoft::BluetoothGattService::~BluetoothGattService() noexcept
{
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::BluetoothGattService::addCharacteristic(bt_gatt_h characteristic)
{
	int error = bt_gatt_service_add_characteristic(handle, characteristic);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_service_add_characteristic", error);
}

void Kystsoft::BluetoothGattService::create(const std::string& uuid, bt_gatt_service_type_e type)
{
	int error = bt_gatt_service_create(uuid.c_str(), type, &handle);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_service_create", error);
}

void Kystsoft::BluetoothGattService::destroy()
{
	int error = bt_gatt_service_destroy(handle);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_service_destroy", error);
	handle = nullptr;
}
