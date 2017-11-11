#ifndef KYSTSOFT_VARIOMETER_H
#define KYSTSOFT_VARIOMETER_H

#include <sensor.h>

namespace Dali { namespace Toolkit { class TextLabel; } }

namespace Kystsoft {

class Variometer
{
public:
	Variometer() {}
	~Variometer() { Stop(); }
	int Start();
	void Stop();
	bool IsStarted() const { return pressureListener != NULL; }
	static void SetClimbLabel(Dali::Toolkit::TextLabel* label);
	static void SetAltitudeLabel(Dali::Toolkit::TextLabel* label);
private:
	sensor_h pressureSensor = NULL;
	sensor_listener_h pressureListener = NULL;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOMETER_H
