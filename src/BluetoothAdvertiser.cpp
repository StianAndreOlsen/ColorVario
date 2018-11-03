#include "BluetoothAdvertiser.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::BluetoothAdvertiser::BluetoothAdvertiser()
{
	create();
	try
	{
		// TODO: Consider moving these calls to the user code
		addManufacturer(117); // Samsung
//		addServiceUuid("FFE0"); // TODO: Service uuid is shown by XC Tracer but probably not important
		setAppearance(192); // generic watch
//		setAppearance(193); // sports watch
		setConnectable(true);
		// TODO: Figure out the best mode
//		setMode(BT_ADAPTER_LE_ADVERTISING_MODE_BALANCED);
//		setMode(BT_ADAPTER_LE_ADVERTISING_MODE_LOW_LATENCY);
		setMode(BT_ADAPTER_LE_ADVERTISING_MODE_LOW_ENERGY);
		setShowDeviceName(true);
//		setShowPowerLevel(true); // TODO: Power level is shown by XC Tracer but probably not important
	}
	catch (...)
	{
		destroy();
		throw;
	}
}

Kystsoft::BluetoothAdvertiser::~BluetoothAdvertiser() noexcept
{
	try { stop(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
void Kystsoft::BluetoothAdvertiser::addManufacturer(int manufacturerId, const std::string& manufacturerData /*= std::string()*/, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_add_advertising_manufacturer_data(advertiser, packetType, manufacturerId, manufacturerData.c_str(), manufacturerData.length());
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_add_advertising_manufacturer_data", error);
}

// https://www.bluetooth.com/specifications/gatt/services
void Kystsoft::BluetoothAdvertiser::addServiceUuid(const std::string& uuid, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_add_advertising_service_uuid(advertiser, packetType, uuid.c_str());
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_add_advertising_service_uuid", error);
}

// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml
void Kystsoft::BluetoothAdvertiser::setAppearance(int appearance, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_set_advertising_appearance(advertiser, packetType, appearance);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_appearance", error);
}

void Kystsoft::BluetoothAdvertiser::setConnectable(bool connectable)
{
	int error = bt_adapter_le_set_advertising_connectable(advertiser, connectable);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_connectable", error);
}

void Kystsoft::BluetoothAdvertiser::setMode(bt_adapter_le_advertising_mode_e mode)
{
	int error = bt_adapter_le_set_advertising_mode(advertiser, mode);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_mode", error);
}

void Kystsoft::BluetoothAdvertiser::setShowDeviceName(bool show, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_set_advertising_device_name(advertiser, packetType, show);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_device_name", error);
}

void Kystsoft::BluetoothAdvertiser::setShowPowerLevel(bool show, bt_adapter_le_packet_type_e packetType /*= BT_ADAPTER_LE_PACKET_ADVERTISING*/)
{
	int error = bt_adapter_le_set_advertising_tx_power_level(advertiser, packetType, show);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_set_advertising_tx_power_level", error);
}

void Kystsoft::BluetoothAdvertiser::setStarted(bool started)
{
	if (started)
		start();
	else
		stop();
}

void Kystsoft::BluetoothAdvertiser::start()
{
	if (started)
		return;
	int error = bt_adapter_le_start_advertising_new(advertiser, stateChangedCallback, this);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_start_advertising_new", error);
	started = true;
}

void Kystsoft::BluetoothAdvertiser::stop()
{
	if (!started)
		return;
	int error = bt_adapter_le_stop_advertising(advertiser);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_stop_advertising", error);
	started = false;
}

void Kystsoft::BluetoothAdvertiser::toggleStartStop()
{
	if (started)
		stop();
	else
		start();
}

void Kystsoft::BluetoothAdvertiser::create()
{
	int error = bt_adapter_le_create_advertiser(&advertiser);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_create_advertiser", error);
}

void Kystsoft::BluetoothAdvertiser::destroy()
{
	int error = bt_adapter_le_destroy_advertiser(advertiser);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_destroy_advertiser", error);
	advertiser = nullptr;
}

void Kystsoft::BluetoothAdvertiser::stateChangedCallback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e state, void* user_data)
{
	BluetoothAdvertiser* bleAdvertiser = static_cast<BluetoothAdvertiser*>(user_data);
	if (bleAdvertiser)
		bleAdvertiser->onStateChanged(result, advertiser, state);
}

void Kystsoft::BluetoothAdvertiser::onStateChanged(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e state)
{
	if (result != BT_ERROR_NONE)
		throw TizenError("bt_adapter_le_advertising_state_changed_cb", result);
	// TODO: Remove logging when finished debugging
	dlog(DLOG_DEBUG) << "Advertiser::onStateChanged: Result = " << result << " and State = " << state;
	// TODO: Enable when things are working
	started = (state == BT_ADAPTER_LE_ADVERTISING_STARTED);
}
