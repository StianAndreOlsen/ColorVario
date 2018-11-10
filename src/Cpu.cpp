#include "Cpu.h"
#include "dlog.h"
#include "TizenError.h"
#include <device/power.h>

Kystsoft::Cpu::~Cpu() noexcept
{
	try { unlock(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::Cpu::setLocked(bool locked)
{
	if (locked)
		lock();
	else
		unlock();
}

void Kystsoft::Cpu::lock()
{
	if (locked)
		return;
	int error = device_power_request_lock(POWER_LOCK_CPU, 0);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_power_request_lock", error);
	locked = true;
}

void Kystsoft::Cpu::unlock()
{
	if (!locked)
		return;
	int error = device_power_release_lock(POWER_LOCK_CPU);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_power_release_lock", error);
	locked = false;
}

void Kystsoft::Cpu::toggleLockUnlock()
{
	if (locked)
		unlock();
	else
		lock();
}
