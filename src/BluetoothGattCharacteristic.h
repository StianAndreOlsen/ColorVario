#ifndef KYSTSOFT_BLUETOOTHGATTCHARACTERISTIC_H
#define KYSTSOFT_BLUETOOTHGATTCHARACTERISTIC_H

#include "BluetoothGattObject.h"
#include <string>

namespace Kystsoft {

// https://www.bluetooth.com/specifications/gatt/characteristics
class BluetoothGattCharacteristic : public BluetoothGattObject
{
public:
	BluetoothGattCharacteristic(const std::string& uuid,
		int permissions = BT_GATT_PERMISSION_READ,
		int properties = BT_GATT_PROPERTY_READ | BT_GATT_PROPERTY_NOTIFY);
	~BluetoothGattCharacteristic() noexcept;
private:
	void create(const std::string& uuid, int permissions, int properties);
	void destroy();
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHGATTCHARACTERISTIC_H
