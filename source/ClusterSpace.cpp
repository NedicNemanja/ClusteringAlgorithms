#include "ClusterSpace.hpp"
#include "ReadInput.hpp"

#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm> //uper_bound
#include <utility.hpp>

using namespace std;

ClusterSpace::ClusterSpace(vector<myvector> &vectors,const string algo_name)
{
  if(algo_name == "Random"){
    srand(time(NULL));
    for(int i=0; i<CmdArgs::number_of_clusters; i++){
      //init cluster with random center
      Cluster c(vectors[rand()%vectors.size()]);
      Clusters.push_back(c);
    }
  }
  if(algo_name == "K-means++"){
    //choose a centroid uniformly
    random_device generator;
    uniform_int_distribution<int> distribution(0,vectors.size()-1);
    Cluster c(vectors[distribution(generator)]);
    Clusters.push_back(c);
    //distances (vector to nearest center) that get updated every loop
    std::vector<double> DistPartialSums(vectors.size(),0);
    double prev_sum=0;
    for(int t=1; t<=CmdArgs::number_of_clusters; t++){
      /*For every non-centroid,compute D(x), the distance between x and the
      nearest center that has already been chosen.*/
      for(int v=0; v<vectors.size(); v++){
        if(isCenter(vectors[v])) continue;
        //update distance to nearest center and keep track in partial sums
        DistPartialSums[v] = prev_sum + pow(MinDistanceToCenter(vectors[v]),2);
        prev_sum = DistPartialSums[v];
      }
      /*Choose one new data point at random as a new center,
      using a probability distribution where a point x is chosen
      with probability proportional to D(x)2.*/
      uniform_real_distribution<double> x(0,DistPartialSums[vectors.size()-1]);
      typename std::vector<double>::iterator new_center =
      upper_bound(DistPartialSums.begin(),DistPartialSums.end(),x(generator));
      //create cluster with new center
      int pos = new_center-DistPartialSums.begin();
      //cout << *new_center << " at " << pos << endl << flush;
      Cluster new_cluster(vectors[pos]);
      Clusters.push_back(new_cluster);
    }
  }
}

ClusterSpace::~ClusterSpace(){}

vector<myvector> ClusterSpace::getCenters(){
  vector<myvector> result;
  for(vector<Cluster>::iterator it=Clusters.begin(); it!=Clusters.end(); it++){
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
double ClusterSpace::MinDistanceToCenter(myvector &v){
  //get all centers
  vector<myvector> centers = getCenters();
  //find distance to first center and set it as min
  double min_dist=EuclideanVectorDistance(v.begin(),v.end(),centers[0].begin());
  for(int i=1; i<centers.size(); i++){
    //for all next distances find the smallest
    double dist = EuclideanVectorDistance(v.begin(),v.end(),centers[i].begin());
    if(min_dist> dist){
      min_dist = dist;
    }
  }
  return min_dist;
}
