#include "Cluster.hpp"

using namespace std;

Cluster::Cluster(myvector &c)
:center(c){
}

Cluster::~Cluster(){}

myvector Cluster::getCenter(){
  return center;
}

std::vector<vector_index> Cluster::getMembers(){
  return members;
}

void Cluster::setCenter(myvector &c){
  center = c;
}


void Cluster::Print(){
  cout << "\t" << "center: ";
  center.print(cout);
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
