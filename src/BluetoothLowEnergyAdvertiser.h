#ifndef KYSTSOFT_BLUETOOTHLOWENERGYADVERTISER_H
#define KYSTSOFT_BLUETOOTHLOWENERGYADVERTISER_H

#include <bluetooth.h>
#include <string>

namespace Kystsoft {

class BluetoothLowEnergyAdvertiser
{
public:
	BluetoothLowEnergyAdvertiser();
	~BluetoothLowEnergyAdvertiser() noexcept;
	operator bt_advertiser_h() const { return advertiser; }
	bool isStarted() const { return started; }
	void setStarted(bool started);
	void start();
	void stop();
	void toggleStartStop();
	void addManufacturer(int manufacturerId, const std::string& manufacturerData = std::string(), bt_adapter_le_packet_type_e packetType = BT_ADAPTER_LE_PACKET_ADVERTISING);
	void addServiceUuid(const std::string& uuid, bt_adapter_le_packet_type_e packetType = BT_ADAPTER_LE_PACKET_ADVERTISING);
	void setAppearance(int appearance, bt_adapter_le_packet_type_e packetType = BT_ADAPTER_LE_PACKET_ADVERTISING);
	void setConnectable(bool connectable);
	void setMode(bt_adapter_le_advertising_mode_e mode);
	void setShowDeviceName(bool show, bt_adapter_le_packet_type_e packetType = BT_ADAPTER_LE_PACKET_ADVERTISING);
	void setShowPowerLevel(bool show, bt_adapter_le_packet_type_e packetType = BT_ADAPTER_LE_PACKET_ADVERTISING);
private:
	void create();
	void destroy();
	static void stateChangedCallback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e state, void* user_data);
	void onStateChanged(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e state) {} // TODO: If required, do something! If not, remove!
	bt_advertiser_h advertiser = nullptr;
	bool started = false;
};

} // namespace Kystsoft

#endif // KYSTSOFT_BLUETOOTHLOWENERGYADVERTISER_H
