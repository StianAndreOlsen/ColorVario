#include "Storage.h"
#include "TizenError.h"

storage_type_e Kystsoft::Storage::type() const
{
	storage_type_e type = STORAGE_TYPE_INTERNAL;
	int error = storage_get_type(id, &type);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_get_type", error);
	return type;
}

storage_state_e Kystsoft::Storage::state() const
{
	storage_state_e state = STORAGE_STATE_MOUNTED;
	int error = storage_get_state(id, &state);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_get_state", error);
	return state;
}

uint64_t Kystsoft::Storage::availableSpace() const
{
	uint64_t bytes = 0;
	int error = storage_get_available_space(id, &bytes);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_get_available_space", error);
	return bytes;
}

uint64_t Kystsoft::Storage::totalSpace() const
{
	uint64_t bytes = 0;
	int error = storage_get_total_space(id, &bytes);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_get_total_space", error);
	return bytes;
}

std::string Kystsoft::Storage::root() const
{
	char* path = nullptr;
	int error = storage_get_root_directory(id, &path);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_get_root_directory", error);
	std::string root(path);
	free(path);
	return root;
}

std::string Kystsoft::Storage::directory(storage_directory_e type) const
{
	char* path = nullptr;
	int error = storage_get_directory(id, type, &path);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_get_directory", error);
	std::string dir(path);
	free(path);
	return dir;
}

void Kystsoft::Storage::findDevice(storage_type_e type)
{
	wantedType = type;
	int error = storage_foreach_device_supported(device, this);
	if (error != STORAGE_ERROR_NONE)
		throw TizenError("storage_foreach_device_supported", error);
}

bool Kystsoft::Storage::device(int storage_id, storage_type_e type, storage_state_e state, const char* /*path*/, void* user_data)
{
	Storage* storage = static_cast<Storage*>(user_data);
	if (storage && storage->deviceAccepted(storage_id, type, state))
		return false;
	return true; // next device
}

bool Kystsoft::Storage::deviceAccepted(int storage_id, storage_type_e type, storage_state_e state)
{
	if (type == wantedType && state == STORAGE_STATE_MOUNTED)
	{
		id = storage_id;
		return true;
	}
	return false;
}

std::string Kystsoft::internalStorageRoot()
{
	Storage storage;
	if (storage.isValid())
		return storage.root();
	return std::string();
}

std::string Kystsoft::internalStorageDirectory(storage_directory_e type)
{
	Storage storage;
	if (storage.isValid())
		return storage.directory(type);
	return std::string();
}
