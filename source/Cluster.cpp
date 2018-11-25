#include "Cluster.hpp"
#include "ReadInput.hpp"

#include <cstdlib>
#include <ctime>
#include <stdexcept>  //out_of_range

using namespace std;

ClusterSpace::ClusterSpace(list<myvector> &vectors, const std::string algo_name)
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
    uniform_int_distribution<int> distribution(0,vectors.size());
    Cluster c(vectors[distribution(generator)]);
    Clusters.push_back(c);
    //distances (vector to nearest center) that get updated every loop
    std::vector<double> distances(vectors.size(),0);
    std::vector<double> probabilities(vectors.size());
    for(int t=1; t<=CmdArgs::number_of_clusters; t++){
      /*For every non-centroid,compute D(x), the distance between x and the
      nearest center that has already been chosen.*/
      double sum_of_distances=0;
      for(int v=0; v<vectors.size(); v++){
        if(isCenter(vectors[v])) continue;
        //update distance to nearest center
        sum_of_distances -= distances[v];
        //distances[v] = MinDistanceToCenter(vectors[v],getCenters()); //D(x)
        sum_of_distances += distances[v];
      }
      /*Choose one new data point at random as a new center,
      using a weighted probability distribution where a point x is chosen
      with probability proportional to D(x)2.*/
      double max_prob;
      int max_v;
      for(int v=0; v<vectors.size(); v++){
        if(isCenter(vectors[v])) continue;
        probabilities[v] = ((double)rand()/(RAND_MAX))
                            *pow(distances[v],2)/sum_of_distances;
        if(probabilities[v] > max_prob){
          max_prob = probabilities[v];
          max_v = v;
        }
      }
      //create new center
      Cluster new_cluster(vectors[max_v]);
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







/***************Cluster*******************************************/
Cluster::Cluster(myvector &c)
:center(c){
}

Cluster::~Cluster(){}

myvector Cluster::getCenter(){
  return center
}
