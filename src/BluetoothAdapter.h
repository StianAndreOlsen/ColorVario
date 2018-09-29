#ifndef KYSTSOFT_BLUETOOTHIADAPTER_H
#define KYSTSOFT_BLUETOOTHIADAPTER_H

#include <string>

namespace Kystsoft {

class BluetoothAdapter
{
public:
	BluetoothAdapter();
	~BluetoothAdapter() noexcept;
	std::string name() const;
	void setName(const std::string& name);
private:
	void initialize();
	void deinitialize();
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHIADAPTER_H
