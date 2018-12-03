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

int Cluster::size(){
  int cluster_size = members.size();
  if(medoid != -1)
    cluster_size++;
  return cluster_size;
}

void Cluster::Print(std::ostream &out){
  for(auto it=members.begin(); it!=members.end(); it++){
    out << AllVectors[*it].get_id();
    if(it+1 != members.end())
       out << ",";
  }
}

void Cluster::AddVector(vector_index vindex){
  members.push_back(vindex);
}

/*Calc dissimilarity of v from cluster (sum of distances)*/
double Cluster::ClusterDissimilarity(myvector &v){
  double sum = 0;
  for(auto it=members.begin(); it!=members.end(); it++){
    sum += EuclideanVectorDistance(v.begin(),v.end(),AllVectors[*it].begin());
  }
  if(medoid != -1)
   sum += EuclideanVectorDistance(v.begin(),v.end(),AllVectors[medoid].begin());
  return sum;
}

void Cluster::UnassignMembers(){
  members.clear();
}
