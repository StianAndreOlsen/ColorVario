#include "BluetoothGattObject.h"
#include "TizenError.h"

std::string Kystsoft::BluetoothGattObject::value() const
{
	char* value = nullptr;
	int length = 0;
	int error = bt_gatt_get_value(handle, &value, &length);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_get_value", error);
	std::string valueStr(value, length);
	free(value);
	return valueStr;
}

void Kystsoft::BluetoothGattObject::setValue(const std::string& value)
{
	int error = bt_gatt_set_value(handle, value.c_str(), value.length());
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_set_value", error);
}
