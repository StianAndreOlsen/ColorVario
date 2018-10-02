#ifndef KYSTSOFT_BLUETOOTHGATTSERVICE_H
#define KYSTSOFT_BLUETOOTHGATTSERVICE_H

#include "BluetoothGattObject.h"
#include <string>

namespace Kystsoft {

// https://www.bluetooth.com/specifications/gatt/services
class BluetoothGattService : public BluetoothGattObject
{
public:
	BluetoothGattService(const std::string& uuid, bt_gatt_service_type_e type = BT_GATT_SERVICE_TYPE_PRIMARY);
	~BluetoothGattService() noexcept;
	void addCharacteristic(bt_gatt_h characteristic);
private:
	void create(const std::string& uuid, bt_gatt_service_type_e type);
	void destroy();
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHGATTSERVICE_H
