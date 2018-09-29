#include "BluetoothGattCharacteristic.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::BluetoothGattCharacteristic::BluetoothGattCharacteristic(const std::string& uuid,
	int permissions /*= BT_GATT_PERMISSION_READ*/,
	int properties /*= BT_GATT_PROPERTY_READ | BT_GATT_PROPERTY_NOTIFY*/)
{
	create(uuid, permissions, properties);
}

Kystsoft::BluetoothGattCharacteristic::~BluetoothGattCharacteristic() noexcept
{
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::BluetoothGattCharacteristic::create(const std::string& uuid, int permissions, int properties)
{
	int error = bt_gatt_characteristic_create(uuid.c_str(), permissions, properties, "", 0, &handle);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_characteristic_create", error);
}

void Kystsoft::BluetoothGattCharacteristic::destroy()
{
	int error = bt_gatt_characteristic_destroy(handle);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_characteristic_destroy", error);
	handle = nullptr;
}
