#ifndef CFT_H
#define CFT_H

#include <complex>
#include <vector>

using Complex = std::complex<double>;

Complex CFT(const std::vector<double>& x, double omega);

#endif // CFT_H
