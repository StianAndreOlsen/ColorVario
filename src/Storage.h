#ifndef KYSTSOFT_STORAGE_H
#define KYSTSOFT_STORAGE_H

#include <storage.h>
#include <string>

namespace Kystsoft {

class Storage
{
public:
	Storage(storage_type_e type = STORAGE_TYPE_INTERNAL) { findDevice(type); }
	bool isValid() const { return id >= 0; }
	int identifier() const { return id; }
	storage_type_e type() const;
	storage_state_e state() const;
	uint64_t availableSpace() const;
	uint64_t totalSpace() const;
	std::string root() const;
	std::string directory(storage_directory_e type) const;
private:
	void findDevice(storage_type_e type);
	static bool device(int storage_id, storage_type_e type, storage_state_e state, const char* path, void* user_data);
	bool deviceAccepted(int storage_id, storage_type_e type, storage_state_e state);
	storage_type_e wantedType = STORAGE_TYPE_INTERNAL;
	int id = -1;
};

// convenience functions
std::string internalStorageRoot();
std::string internalStorageDirectory(storage_directory_e type);

} // namespace Kystsoft

#endif // KYSTSOFT_STORAGE_H
