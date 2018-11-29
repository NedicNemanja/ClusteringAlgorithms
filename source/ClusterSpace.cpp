#include "ClusterSpace.hpp"
#include "ReadInput.hpp"

#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm> //uper_bound
#include <utility.hpp>
#include <map> //multimap

using namespace std;

ClusterSpace::ClusterSpace(MyVectorContainer &vectors,string init,
string assign, string update)
:init_algorithm(init),assign_algorithm(assign),update_algorithm(update)
{
  cout<<"Starting "<<init<<" | "<<assign<<" | "<<update<<endl;
  if(init_algorithm == "Random"){
    srand(time(NULL));
    for(int i=0; i<CmdArgs::number_of_clusters; i++){
      //init cluster with random center
      Cluster c(rand()%vectors.size());
      Clusters.push_back(c);
    }
  }
  if(init_algorithm == "K-means++"){
    //choose a centroid uniformly
    random_device generator;
    uniform_int_distribution<int> distribution(0,vectors.size()-1);
    Cluster c(distribution(generator));
    Clusters.push_back(c);
    //distances (vector to nearest center) that get updated every loop
    std::vector<double> DistPartialSums(vectors.size(),0);
    double prev_sum=0;
    for(int t=1; t<=CmdArgs::number_of_clusters; t++){
      /*For every non-centroid,compute D(x), the distance between x and the
      nearest center that has already been chosen.*/
      for(vector_index v=0; v<vectors.size(); v++){
        if(isCenter(vectors[v])) continue;
        //update distance to nearest center and keep track in partial sums
        DistPartialSums[v] = prev_sum + MinDistanceToCenterSquared(vectors[v]);
        prev_sum = DistPartialSums[v];
      }
      /*Choose one new data point at random as a new center,
      using a probability distribution where a point x is chosen
      with probability proportional to D(x)2.*/
      uniform_real_distribution<double> x(0,DistPartialSums[vectors.size()-1]);
      typename std::vector<double>::iterator new_center =
      upper_bound(DistPartialSums.begin(),DistPartialSums.end(),x(generator));
      //create cluster with new center
      vector_index pos = new_center-DistPartialSums.begin();
      //cout << *new_center << " at " << pos << endl << flush;
      Clusters.push_back(Cluster(pos));
    }
  }
}

ClusterSpace::~ClusterSpace(){}

vector<myvector> ClusterSpace::getCenters(){
  vector<myvector> result;
  for(auto it=Clusters.begin(); it!=Clusters.end(); it++){
    result.push_back(it->getCenter());
  }
  return result;
}

bool ClusterSpace::isCenter(myvector &p){
  for(vector<Cluster>::iterator it=Clusters.begin(); it!=Clusters.end(); it++){
    if(p.get_id() == it->getCenter().get_id()){ //if same id as center
      return true;
    }
  }
  return false;
}

void ClusterSpace::Print(){
  for(vector<Cluster>::iterator it=Clusters.begin(); it!=Clusters.end(); it++){
    it->Print();
  }
}

/*Return the min distance to any center in the Cluster Space*/
double ClusterSpace::MinDistanceToCenterSquared(myvector &v){
  //get all centers
  vector<myvector> centers = getCenters();
  //find distance to first center and set it as min
  double min_dist=
  EuclideanVectorDistanceSquared(v.begin(),v.end(),centers[0].begin()),dist;
  for(int i=1; i<centers.size(); i++){
    //for all next distances find the smallest
    dist = EuclideanVectorDistanceSquared(v.begin(),v.end(),centers[i].begin());
    if(min_dist> dist){
      min_dist = dist;
    }
  }
  return min_dist;
}

/*Return the position of nearest center in the Cluster Space "Clusters" vector*/
int ClusterSpace::NearestCenter(myvector &v){
  //get all centers
  vector<myvector> centers = getCenters();
  //find distance to first center and set it as min
  int nearest_center_pos=0;
  double min_dist=EuclideanVectorDistance(v.begin(),v.end(),centers[0].begin());
  for(int i=1; i<centers.size(); i++){
    //for all next distances find the smallest
    double dist = EuclideanVectorDistance(v.begin(),v.end(),centers[i].begin());
    if(min_dist> dist){
      min_dist = dist;
      nearest_center_pos = i;
    }
  }
  return nearest_center_pos;
}


void RunClusteringAlgorithms(){

  if(assign_algorithm == "Llyod's"){

  }
  if(assign_algorithm == "RangeSearchLSH"){

  }
  if(assign_algorithm == "RangeSearchHypercube"){

  }

}

/*Assign vectors to their nearest center*/
void ClusterSpace::LloydsAssignment(MyVectorContainer &vectors){
  for(vector_index i=0; i<vectors.size(); i++){
    if(isCenter(vectors[i])) continue;
    Clusters[NearestCenter(vectors[i])].AddVector(i);
  }
}

void ClusterSpace::RangeSearchLSHAssignment(MyVectorContainer &vectors,
                                            vector<HashTable*> HTables)
{
  double range = MinDistanceBetweenCenters()/2;
  //bitmap that mirrors MyVectorContainer indexing (true if vector's assigned)
  vector<bool> AssignedVectorBitMap(vectors.size(),false);
  int num_assigned_vectors=0;

  while(num_assigned_vectors<vectors.size() /*&& colision_num<max_colisions*/
      /*&& no center changed*/ /*&& range>=max_range*/){
    //(Re)Assign centers

    //detect colisions between centers
    //range search for every hashtable
    for(auto htable=HTables.begin(); htable!=HTables.end(); htable++){
      //multiple centers can map to one bucket. multimap:(bucket_hash->centers)
      multimap<int,myvector&> CenterMap;
      HashCenters(CenterMap,getCenters(),*htable);
      //range search for every bucket

    }
    range*=2;
  }

}

double ClusterSpace::MinDistanceBetweenCenters(){
  //get all centers
  vector<myvector> centers = getCenters();
  //find distance to first center and set it as min
  double dist, min_dist = EuclideanVectorDistance(centers[0].begin(),
                                                  centers[0].end(),
                                                  centers[1].begin());
  for(int i=0; i<centers.size(); i++){
    for(int j=i+1; j<centers.size(); j++){
      if(i==0 && j==1) continue;
      //for all next distances find the smallest
      dist = EuclideanVectorDistance(centers[i].begin(),
                                     centers[i].end(),
                                     centers[j].begin());
      if(dist < min_dist){
        min_dist = dist;
      }
    }
  }
  return min_dist;
}



/************Non_Members*******************************************************/
void HashCenters(multimap<int,myvector&> &hashmap, vector<myvector> Centers,
HashTable* HTable){
  for(auto it=Centers.begin(); it!=Centers.end(); it++){
    int hash = HTable->get_hash(*it);
    hashmap.insert(std::pair<int,myvector&>(hash,*it));
  }
}
