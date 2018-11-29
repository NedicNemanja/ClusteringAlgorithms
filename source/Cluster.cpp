#include "Cluster.hpp"

using namespace std;

Cluster::Cluster(myvector &c)
:center(c){
}

Cluster::~Cluster(){}

myvector Cluster::getCenter(){
  return center;
}

void Cluster::Print(){
  cout << "\t" << "center: ";
  center.print(cout);
  cout << endl;
  return;
  for(vector<myvector>::iterator it=vectors.begin(); it!=vectors.end(); it++){
    cout << "\t";
    it->print(cout);
    cout << endl;
  }
}

void Cluster::AddVector(myvector &v){
  vectors.push_back(v);
}
