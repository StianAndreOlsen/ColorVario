#ifndef KYSTSOFT_CPU_H
#define KYSTSOFT_CPU_H

namespace Kystsoft {

class Cpu
{
public:
	Cpu() {}
	Cpu(const Cpu& other) = delete;
	Cpu& operator=(const Cpu& rhs) = delete;
	~Cpu() noexcept;
	bool isLocked() const { return locked; }
	void setLocked(bool locked);
	void lock();
	void unlock();
	void toggleLockUnlock();
private:
	bool locked = false;
};

} // namespace Kystsoft

#endif // KYSTSOFT_CPU_H
