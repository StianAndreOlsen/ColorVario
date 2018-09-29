#ifndef KYSTSOFT_BLUETOOTHGATTOBJECT_H
#define KYSTSOFT_BLUETOOTHGATTOBJECT_H

#include <bluetooth.h>
#include <string>

namespace Kystsoft {

class BluetoothGattObject
{
public:
	operator bt_gatt_h() const { return handle; }
	std::string value() const;
	void setValue(const std::string& value);
protected:
	bt_gatt_h handle = nullptr;
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHGATTOBJECT_H
