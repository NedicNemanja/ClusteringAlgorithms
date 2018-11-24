#ifndef WRITEOUTPUT_HPP
#define WRITEOUTPUT_HPP

#include <fstream>
#include <list>

#include "myvector.hpp"

void WriteResult( std::ofstream &outfile, std::list<myvector> &rNN,
                  myvector &q, myvector &nn,
                  double distanceLHS, double distanceTrue,
                  double timeLHS, double timeTrue);

#endif
