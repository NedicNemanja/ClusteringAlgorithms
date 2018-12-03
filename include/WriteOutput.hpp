#ifndef WRITEOUTPUT_HPP
#define WRITEOUTPUT_HPP

#include <fstream>
#include <list>

#include "myvector.hpp"
#include "ClusterSpace.hpp"

void WriteResult(std::ofstream&, int code, ClusterSpace&, double time);

#endif
