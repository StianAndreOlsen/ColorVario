#ifndef KYSTSOFT_BLUETOOTHGATTSERVER_H
#define KYSTSOFT_BLUETOOTHGATTSERVER_H

#include <bluetooth.h>
#include <string>

namespace Kystsoft {

class BluetoothGattServer
{
public:
	BluetoothGattServer();
	~BluetoothGattServer() noexcept;
	operator bt_gatt_server_h() const { return server; }
	void registerService(bt_gatt_h service);
	void unregisterService(bt_gatt_h service);
	void unregisterAllServices();
	static void start();
private:
	static void initialize();
	static void deinitialize();
	void create();
	void destroy();
	bt_gatt_server_h server = nullptr;
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHGATTSERVER_H
