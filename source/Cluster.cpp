#include "Cluster.hpp"
#include "utility.hpp"

using namespace std;

Cluster::Cluster(myvector &c)
:centroid(c),medoid(-1){
}

//in case center is part of the dataset, save its index
Cluster::Cluster(vector_index c_index)
:medoid(c_index){
}

Cluster::~Cluster(){}

myvector Cluster::getCenter(){
  if(medoid == -1)
    return centroid;
  return AllVectors[medoid];
}

vector_index Cluster::getMedoid(){
  return medoid;
}

void Cluster::setMedoid(vector_index vindex){
  medoid = vindex;
}

void Cluster::setCentroid(myvector c){
  medoid = -1;
  centroid = c;
}

std::vector<vector_index> Cluster::getMembers(){
  return members;
}



void Cluster::Print(){
  cout << "\t" << "center: ";
  getCenter().print(cout);
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

/*Calc dissimilarity of v from cluster*/
double Cluster::ClusterDissimilarity(myvector &v){
  double sum = 0;
  for(auto it=members.begin(); it!=members.end(); it++){
    sum += EuclideanVectorDistance(v.begin(),v.end(),AllVectors[*it].begin());
  }
  if(medoid != -1)
   sum += EuclideanVectorDistance(v.begin(),v.end(),AllVectors[medoid].begin());
  return sum;
}
