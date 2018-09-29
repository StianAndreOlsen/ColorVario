#include "BluetoothLowEnergyAdvertiser.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::BluetoothLowEnergyAdvertiser::BluetoothLowEnergyAdvertiser()
{
	create();
	addManufacturer(117); // Samsung
	setAppearance(192); // generic watch
//	setAppearance(193); // sports watch
	setConnectable(true);
	setMode(BT_ADAPTER_LE_ADVERTISING_MODE_BALANCED); // TODO: Figure out the best mode
	setShowDeviceName(true);
//	setShowPowerLevel(true); // TODO: Probably not important. Consider removing.
}

Kystsoft::BluetoothLowEnergyAdvertiser::~BluetoothLowEnergyAdvertiser() noexcept
{
	try { stop(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::BluetoothLowEnergyAdvertiser::setStarted(bool started)
{
	if (started)
		start();
	else
		stop();
}

void Kystsoft::BluetoothLowEnergyAdvertiser::start()
{
	if (started)
		return;
	int error = bt_adapter_le_start_advertising_new(advertiser, stateChangedCallback, this);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_start_advertising_new", error);
	started = true;
}

void Kystsoft::BluetoothLowEnergyAdvertiser::stop()
{
	if (!started)
		return;
	int error = bt_adapter_le_stop_advertising(advertiser);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_stop_advertising", error);
	started = false;
}

void Kystsoft::BluetoothLowEnergyAdvertiser::toggleStartStop()
{
	if (started)
		stop();
	else
		start();
}

// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
void Kystsoft::BluetoothLowEnergyAdvertiser::addManufacturer(int manufacturerId, const std::string& manufacturerData /*= std::string()*/, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_add_advertising_manufacturer_data(advertiser, packetType, manufacturerId, manufacturerData.c_str(), manufacturerData.length());
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_add_advertising_manufacturer_data", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::addServiceUuid(const std::string& uuid, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_add_advertising_service_uuid(advertiser, packetType, uuid.c_str());
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_add_advertising_service_uuid", error);
}

// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml
void Kystsoft::BluetoothLowEnergyAdvertiser::setAppearance(int appearance, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_set_advertising_appearance(advertiser, packetType, appearance);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_appearance", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::setConnectable(bool connectable)
{
	int error = bt_adapter_le_set_advertising_connectable(advertiser, connectable);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_connectable", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::setMode(bt_adapter_le_advertising_mode_e mode)
{
	int error = bt_adapter_le_set_advertising_mode(advertiser, mode);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_mode", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::setShowDeviceName(bool show, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_set_advertising_device_name(advertiser, packetType, show);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_device_name", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::setShowPowerLevel(bool show, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_set_advertising_tx_power_level(advertiser, packetType, show);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_tx_power_level", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::create()
{
	int error = bt_adapter_le_create_advertiser(&advertiser);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_create_advertiser", error);
}

void Kystsoft::BluetoothLowEnergyAdvertiser::destroy()
{
	int error = bt_adapter_le_destroy_advertiser(advertiser);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_destroy_advertiser", error);
	advertiser = nullptr;
}

void Kystsoft::BluetoothLowEnergyAdvertiser::stateChangedCallback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e state, void* user_data)
{
	BluetoothLowEnergyAdvertiser* bleAdvertiser = static_cast<BluetoothLowEnergyAdvertiser*>(user_data);
	if (bleAdvertiser)
		bleAdvertiser->onStateChanged(result, advertiser, state);
}
