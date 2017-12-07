#ifndef KYSTSOFT_VARIOCOLOR_H
#define KYSTSOFT_VARIOCOLOR_H

#include "VarioColorPoint.h"
#include <vector>

namespace Kystsoft {

class VarioColor
{
public:
	VarioColor();
	void addColorPoint(const VarioColorPoint& point) { colorPoints.push_back(point); }
	void addColorPoint(float climb, const Color& color)
		{ addColorPoint(VarioColorPoint(climb, color)); }
	void clearColorPoints() { colorPoints.clear(); }
	void sortColorPoints();
	Color color(float climb) const;
	Color operator()(float climb) const { return color(climb); }
	static Color defaultColor(float climb);
private:
	size_t colorPointInterval(float climb) const;
	std::vector<VarioColorPoint> colorPoints;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCOLOR_H