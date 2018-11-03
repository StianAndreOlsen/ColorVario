#ifndef KYSTSOFT_UNITCONVERSIONFUNCTIONS_H
#define KYSTSOFT_UNITCONVERSIONFUNCTIONS_H

namespace Kystsoft {

// length (altitude)
inline void m_to_ft(double m, double* ft) { *ft = m / 0.3048; } // m to ft

// velocity (climb, speed)
inline void mps_to_kmph(double mps, double* kmph) { *kmph = mps * 60 * 60 / 1000;          } // m/s to km/h
inline void mps_to_fps (double mps, double* fps ) { *fps  = mps / 0.3048;                  } // m/s to ft/s
inline void mps_to_fpm (double mps, double* fpm ) { *fpm  = mps * 60 / 0.3048;             } // m/s to ft/min
inline void mps_to_hfpm(double mps, double* hfpm) { *hfpm = mps * 60 / 0.3048 / 100;       } // m/s to 100ft/min
inline void mps_to_kn  (double mps, double* kn  ) { *kn   = mps * 60 * 60 / 1852;          } // m/s to knots
inline void mps_to_mph (double mps, double* mph ) { *mph  = mps * 60 * 60 / 0.3048 / 5280; } // m/s to miles/h

} // namespace Kystsoft

#endif // KYSTSOFT_UNITCONVERSIONFUNCTIONS_H
