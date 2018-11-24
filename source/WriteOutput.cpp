#include "WriteOutput.hpp"

#include <iostream>

using namespace std;

void WriteResult( std::ofstream &outfile, list<myvector> &rNN,
                  myvector &q, myvector &nn,
                  double distanceLHS, double distanceTrue,
                  double timeLHS, double timeTrue)
{
  outfile << "Query: " << q.get_id() << endl;
  outfile << "R-near neighbors:" << endl;
  for(list<myvector>::iterator it=rNN.begin(); it != rNN.end(); it++){
    outfile << (*it).get_id() << endl;
  }
  outfile << "Nearest neighbor: " << nn.get_id() << endl;
  outfile << "distanceLHS: " << distanceLHS << endl;
  outfile << "distanceTrue: " << distanceTrue << endl;
  outfile << "tLHS: " << timeLHS << endl;
  outfile << "tTrue: " << timeTrue << endl << endl;
}
