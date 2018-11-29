#include "Cluster.hpp"

using namespace std;

Cluster::Cluster(vector_index c)
:center(c){
}

Cluster::~Cluster(){}

myvector Cluster::getCenter(){
  return AllVectors[center];
}

void Cluster::Print(){
  cout << "\t" << "center: ";
  AllVectors[center].print(cout);
  cout << endl;
  return;
  for(auto it=members.begin(); it!=members.end(); it++){
    cout << "\t";
    AllVectors[*it].print(cout);
    cout << endl;
  }
}

void Cluster::AddVector(vector_index vindex){
  members.push_back(vindex);
}
