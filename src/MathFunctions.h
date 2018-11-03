#ifndef KYSTSOFT_MATHFUNCTIONS_H
#define KYSTSOFT_MATHFUNCTIONS_H

#include <cmath>

namespace Kystsoft {

inline double mround(double x, double m) { return m == 0 ? x : std::round(x / m) * m; }

} // namespace Kystsoft

#endif // KYSTSOFT_MATHFUNCTIONS_H
