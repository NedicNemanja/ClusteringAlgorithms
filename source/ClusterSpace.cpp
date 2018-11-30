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
:init_algorithm(init),assign_algorithm(assign),update_algorithm(update),
AssignedVectorBitMap(vectors.size(),false)
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
  double dist;
  for(int i=1; i<centers.size(); i++){
    //for all next distances find the smallest
    dist = EuclideanVectorDistance(v.begin(),v.end(),centers[i].begin());
    if(min_dist> dist){
      min_dist = dist;
      nearest_center_pos = i;
    }
  }
  return nearest_center_pos;
}
//Overloaded
int ClusterSpace::NearestCenter(myvector &v,vector<Cluster*> &clusters,
  double* min_dist){
  //find distance to first center and set it as min
  int nearest_center_pos=0;
  *min_dist=EuclideanVectorDistance(v.begin(),v.end(),
                                    clusters[0]->getCenter().begin());
  double dist;
  for(int i=1; i<clusters.size(); i++){
    //for all next distances find the smallest
    dist = EuclideanVectorDistance( v.begin(),v.end(),
                                    clusters[i]->getCenter().begin());
    if(*min_dist > dist){
      *min_dist = dist;
      nearest_center_pos = i;
    }
  }
  return nearest_center_pos;
}

void ClusterSpace::RunClusteringAlgorithms(MyVectorContainer &vectors,
vector<HashTable*> HTables){
  void (ClusterSpace::*assign_func)(MyVectorContainer&,vector<HashTable*>);
  void (ClusterSpace::*update_func)(MyVectorContainer&,vector<HashTable*>);
  if(assign_algorithm == "Llyod's"){
    //assign_func = &ClusterSpace::LloydsAssignment;
  }
  if(assign_algorithm == "RangeSearchLSH"){
    assign_func = &ClusterSpace::RangeSearchLSHAssignment;
  }
  if(assign_algorithm == "RangeSearchHypercube"){

  }


  bool stop_criteria_met=false;
  while(stop_criteria_met == false){
  //  (this->*assign_func)(vectors,HTables);
    //(this->*update_func)(vectors,HTables);
  }
}

/*Assign vectors to their nearest center*/
void ClusterSpace::LloydsAssignment(MyVectorContainer &vectors){
  for(int index=0,size=vectors.size(); index<size; index++){
    if(isCenter(vectors[index])) continue;
    Clusters[NearestCenter(vectors[index])].AddVector(index);
  }
}

/*Assign vectors that are unassigned to their nearest center*/
void ClusterSpace::LloydsAssignment(MyVectorContainer &vectors, const string s){
  for(int index=0,size=vectors.size(); index<size; index++){
    if(AssignedVectorBitMap[index]) continue; //already assigned
    if(isCenter(vectors[index])) continue;  //dont assigng a center
    Clusters[NearestCenter(vectors[index])].AddVector(index);
    AssignedVectorBitMap[index] = true;
    num_assigned_vectors++;
  }
}

void ClusterSpace::RangeSearchLSHAssignment(MyVectorContainer &vectors,
  vector<HashTable*> HTables){
  double radius = MinDistanceBetweenCenters()/2;

  while(num_assigned_vectors<vectors.size() /*&& colision_num<max_colisions*/
  /*&& no center changed*/ /*&& range>=max_range*/){
    //detect colisions between centers
    //range search for every hashtable
    for(auto htable=HTables.begin(); htable!=HTables.end(); htable++){
      //multiple centers can map to one bucket. multimap:(bucket_hash->cluster)
      multimap<int,Cluster*> CenterMap;
      MapCentersToBuckets(CenterMap,*htable);
      //range search for every bucket
      for(int b_hash; b_hash<(*htable)->num_buckets(); b_hash++){
        int count=CenterMap.count(b_hash);
        if(count == 1){ //one center, do normal range search inside bucket
          //get cluster that is centered in this bucket
          pair<multimap<int,Cluster*>::iterator,
          multimap<int,Cluster*>::iterator> ret=CenterMap.equal_range(b_hash);
          Cluster* cluster = (ret.first)->second;
          //range search in bucket
          vector<vector_index> results=(*htable)->RangeSearch(b_hash,
          cluster->getCenter(),radius,vectors,AssignedVectorBitMap);
          //assign results to cluster
          for(auto res=results.begin(); res!=results.end(); res++){
            if(isCenter(vectors[*res])) continue; //dont assign a center
            cluster->AddVector(*res);
            AssignedVectorBitMap[*res] = true;
            num_assigned_vectors++;
          }
        }
        else if(count > 1){ //more than one centers in bucket
          /*assign to nearest Cluster center within radius*/
          vector<Cluster*> bucketClusters = GetBucketClusters(b_hash,CenterMap);
          NearestCenterAssign((*htable)->get_bucket_at(b_hash),radius,
                              bucketClusters,vectors);
        }
      }
    }
    radius*=2;
  }
  //everything that left unassigned assign with Llyod's
  LloydsAssignment(vectors,"assign leftovers");
}

void ClusterSpace::NearestCenterAssign(Bucket bucket,double radius,
  vector<Cluster*> &clusters,MyVectorContainer &vectors){
  //for every vector of the bucket
  for(auto it=bucket.begin(); it!=bucket.end(); it++){
    if(AssignedVectorBitMap[*it]) continue;
    if(isCenter(vectors[*it])) continue;
    //find nearest center
    double min_dist;
    int pos = NearestCenter(vectors[*it],clusters,&min_dist);
    //if within radius and if they indeed have the same Hash: assign
    if(min_dist<radius){
      clusters[pos]->AddVector(*it);
      AssignedVectorBitMap[*it] = true;
      num_assigned_vectors++;
    }
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

//multiple centers can map to one bucket. multimap:(bucket_hash->centers)
void ClusterSpace::MapCentersToBuckets(multimap<int,Cluster*> &hashmap,
  HashTable* HTable){
  for(auto c=Clusters.begin(); c!=Clusters.end(); c++){
    myvector center = c->getCenter();
    int hash = HTable->get_hash(center);
    hashmap.insert(std::pair<int,Cluster*>(hash,&(*c)));
  }
}

/*Return all the CLusters that have centers in this bucket*/
vector<Cluster*> GetBucketClusters(int b_hash, multimap<int,Cluster*> &CMap){
  vector<Cluster*> clusters;
  //get elements from map with b_hash key
  pair<multimap<int,Cluster*>::iterator,multimap<int,Cluster*>::iterator>
  element_range = CMap.equal_range(b_hash);
  for(auto it=element_range.first; it!=element_range.second; it++){
    clusters.push_back((*it).second);
  }
  return clusters;
}

/*return hashes of every center in clusters vector*/
vector<int> CenterHashes(vector<Cluster*> &clusters, HashTable* HTable){
  vector<int> result(clusters.size());
  for(int i=0; i<clusters.size(); i++){
    result[i] = HTable->get_hash(clusters[i]->getCenter());
  }
  return result;
}
