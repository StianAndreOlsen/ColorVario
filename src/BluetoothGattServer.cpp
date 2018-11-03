#include "BluetoothGattServer.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::BluetoothGattServer::BluetoothGattServer()
{
	// TODO: Figure out if we need to call start and/or initialize
	initialize();
	try
	{
		create();
	}
	catch (...)
	{
		deinitialize();
		throw;
	}
}

Kystsoft::BluetoothGattServer::~BluetoothGattServer() noexcept
{
	try { unregisterAllServices(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { deinitialize(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::BluetoothGattServer::registerService(bt_gatt_h service)
{
	int error = bt_gatt_server_register_service(server, service);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_register_service", error);
}

void Kystsoft::BluetoothGattServer::unregisterService(bt_gatt_h service)
{
	int error = bt_gatt_server_unregister_service(server, service);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_unregister_service", error);
}

void Kystsoft::BluetoothGattServer::unregisterAllServices()
{
	int error = bt_gatt_server_unregister_all_services(server);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_unregister_all_services", error);
}

void Kystsoft::BluetoothGattServer::start()
{
	int error = bt_gatt_server_start();
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_start", error);
}

void Kystsoft::BluetoothGattServer::initialize()
{
	int error = bt_gatt_server_initialize();
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_initialize", error);
}

void Kystsoft::BluetoothGattServer::deinitialize()
{
	int error = bt_gatt_server_deinitialize();
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_deinitialize", error);
}

void Kystsoft::BluetoothGattServer::create()
{
	int error = bt_gatt_server_create(&server);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_create", error);
}

void Kystsoft::BluetoothGattServer::destroy()
{
	int error = bt_gatt_server_destroy(server);
	if (error != BT_ERROR_NONE)
		throw TizenError("bt_gatt_server_destroy", error);
	server = nullptr;
}
