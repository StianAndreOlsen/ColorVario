#ifndef KYSTSOFT_BLUETOOTHIADAPTER_H
#define KYSTSOFT_BLUETOOTHIADAPTER_H

#include <string>

namespace Kystsoft {

class BluetoothAdapter
{
public:
	BluetoothAdapter();
	~BluetoothAdapter() noexcept;
	static bool isEnabled();
	static std::string address();
	static std::string name();
	static void setName(const std::string& name);
private:
	static void initialize();
	static void deinitialize();
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHIADAPTER_H
