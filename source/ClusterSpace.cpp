#include "ClusterSpace.hpp"
#include "ReadInput.hpp"

#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm> //uper_bound
#include <utility.hpp>
#include <map> //multimap

using namespace std;

ClusterSpace::ClusterSpace(){}

ClusterSpace::ClusterSpace(MyVectorContainer &vectors,string init,
string assign, string update)
:init_algorithm(init),assign_algorithm(assign),update_algorithm(update),
AssignedVectorBitMap(vectors.size(),false)
{
  cout<<"Starting "<<init<<" | "<<assign<<" | "<<update<<endl;
  if(init_algorithm == "Random Init"){
    srand(time(NULL));
    for(int i=0; i<CmdArgs::number_of_clusters; i++){
      //init cluster with random center
      Clusters.push_back(Cluster(rand()%vectors.size()));
    }
  }
  if(init_algorithm == "K-means++"){
    //choose a centroid uniformly
    random_device generator;
    uniform_int_distribution<int> distribution(0,vectors.size()-1);
    Clusters.push_back(Cluster(distribution(generator)));
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

bool ClusterSpace::isCenter(const myvector &p){
  for(auto it=Clusters.begin(); it!=Clusters.end(); it++){
    if(p.get_id() == it->getCenter().get_id()){ //if same id as center
      return true;
    }
  }
  return false;
}

Cluster ClusterSpace::getCluster(int i){
  return Clusters[i];
}

void ClusterSpace::Print(){
  for(auto it=Clusters.begin(); it!=Clusters.end(); it++){
    it->Print();
  }
}

void ClusterSpace::RunClusteringAlgorithms(MyVectorContainer &vectors,
vector<HashTable*> HTables, HashTable* HypercubeTable){
  void (ClusterSpace::*assign_func)(MyVectorContainer&,std::vector<HashTable*>&);
  void (ClusterSpace::*update_func)(MyVectorContainer&);
  vector<HashTable*> hashtables = HTables;
  if(assign_algorithm == "Lloyd's"){
    assign_func = &ClusterSpace::LloydsAssignWrapper;
  }
  if(assign_algorithm == "RangeSearchLSH"){
    hashtables = HTables;
    assign_func = &ClusterSpace::RangeSearchLSHAssign;
  }
  if(assign_algorithm == "RangeSearchHypercube"){
    hashtables.push_back(HypercubeTable);
    assign_func = &ClusterSpace::RangeSearchHypercubeAssignWrapper;
  }
  if(update_algorithm == "K-means")
    update_func = &ClusterSpace::K_means;
  if(update_algorithm == "PAM")
    update_func = &ClusterSpace::PAM;

  int iteration = 0;
  ClusterSpace prevClusterSpaceImage;
  (this->*assign_func)(vectors,hashtables);
  (this->*update_func)(vectors);
  do{
    iteration++;
    prevClusterSpaceImage = *this;
    (this->*assign_func)(vectors,hashtables);
    (this->*update_func)(vectors);
  }while(!ObjectiveFunctionCoverges(prevClusterSpaceImage) &&
          iteration<CmdArgs::max_iterations);
  cout << "\t" << "iterations:" << iteration+1 << endl << endl;
}

/*Calculate by how much the centers have shifted since last iteration*/
bool ClusterSpace::ObjectiveFunctionCoverges(ClusterSpace &prevCS){
  double avg_center_shift=0;
  for(int i=0; i<Clusters.size(); i++){
    myvector curr_center = Clusters[i].getCenter();
    avg_center_shift += EuclideanVectorDistance(curr_center.begin(),
                                                curr_center.end(),
                                      prevCS.getCluster(i).getCenter().begin());
  }
  avg_center_shift /= Clusters.size();
  if(avg_center_shift <= CmdArgs::center_convergence_tolerance){
    cout << "\t" << "avg convergence:" << avg_center_shift << endl;
    return true;  //convergence tolerance met, Clustering can stop
  }
  else
    return false;
}

/*Assign vectors to their nearest center*/
void ClusterSpace::LloydsAssign(MyVectorContainer &vectors){
  for(int index=0; index<vectors.size(); index++){
    if(isCenter(vectors[index])) continue;
    Clusters[NearestCenter(vectors[index])].AddVector(index);
  }
}

/*Assign vectors that are unassigned to their nearest center*/
void ClusterSpace::LloydsAssign(MyVectorContainer &vectors, const string s){
  for(int index=0,size=vectors.size(); index<size; index++){
    if(AssignedVectorBitMap[index]) continue; //already assigned
    if(isCenter(vectors[index])) continue;  //dont assigng a center
    Clusters[NearestCenter(vectors[index])].AddVector(index);
    AssignedVectorBitMap[index] = true;
    num_assigned_vectors++;
  }
}

void ClusterSpace::RangeSearchLSHAssign(MyVectorContainer &vectors,
  vector<HashTable*> &HTables){
  double radius = MinDistanceBetweenCenters()/2;
  int iteration=0;
  /*For every hashtable create a multimap with
  bucket_pos as key to all the clusters centers in that bucket*/
  vector<multimap<int,Cluster*>> CenterMaps(HTables.size());
  SetCenterMaps(CenterMaps,HTables);

  while(num_assigned_vectors<vectors.size() &&
  iteration<CmdArgs::RANGESEARCH_ITERATIONS){
    //range search for every hashtable
    for(int i=0; i<HTables.size(); i++){
      //range search for every bucket
      for(int b_hash=0; b_hash<HTables[i]->num_buckets(); b_hash++){
        int count=CenterMaps[i].count(b_hash);
        if(count == 1){ //one center, do normal range search inside bucket
          //get cluster that is centered in this bucket
          pair<multimap<int,Cluster*>::iterator,
          multimap<int,Cluster*>::iterator> ret=CenterMaps[i].equal_range(b_hash);
          Cluster* cluster = (ret.first)->second;
          //range search in bucket
          vector<vector_index> results=HTables[i]->RangeSearch(b_hash,
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
          NearestCenterRangeAssign(HTables[i]->get_bucket_at(b_hash),radius,
                              GetBucketClusters(b_hash,CenterMaps[i]),vectors);
        }
      }
    }
    radius*=2;
    iteration++;
  }
  //everything that left unassigned assign with Llyod's
  LloydsAssign(vectors,"assign leftovers");
}

void ClusterSpace::RangeSearchHypercubeAssign(MyVectorContainer &vectors,
  HashTable &htable){
  double radius = MinDistanceBetweenCenters()/2;
  int iteration=0, max_probes = pow(2,CmdArgs::K);
  /*multiple centers can map to one bucket. multimap:(bucket_hash->cluster)
  The bucket hash is used as a key, it maps to all Clusters whose centers are
  within Hamming Distance (specified in HYPERCUBE_PROBES) of the bucket*/
  multimap<int,Cluster*> CenterMap=MapCentersToBuckets(htable,CmdArgs::HYPERCUBE_PROBES);

  /****************check the buckets centers are in themselves**********/
  while(num_assigned_vectors<vectors.size() &&
  iteration<CmdArgs::RANGESEARCH_ITERATIONS){
    //range search for every bucket
    for(int b_hash=0; b_hash<htable.num_buckets(); b_hash++){
      int count=CenterMap.count(b_hash);
      if(count == 1){ //one center, do normal range search inside bucket
        //get cluster that is centered in this bucket
        pair<multimap<int,Cluster*>::iterator,
        multimap<int,Cluster*>::iterator> ret=CenterMap.equal_range(b_hash);
        Cluster* cluster = (ret.first)->second;
        //range search in bucket
        vector<vector_index> results=htable.RangeSearch(b_hash,
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
        NearestCenterRangeAssign(htable.get_bucket_at(b_hash),radius,
        GetBucketClusters(b_hash,CenterMap),vectors);
      }
    }
    radius*=2;
    iteration++;
  }
  //everything that left unassigned assign with Llyod's
  LloydsAssign(vectors,"assign leftovers");
}

void ClusterSpace::NearestCenterRangeAssign(Bucket bucket,double radius,
  const vector<Cluster*> &clusters,MyVectorContainer &vectors){
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

void ClusterSpace::K_means(MyVectorContainer &vectors){
  //for every cluster
  for(auto cluster=Clusters.begin(); cluster!=Clusters.end(); cluster++){
    vector<coord> mean(CmdArgs::dimension,0);
    vector<vector_index> members = cluster->getMembers();
    //add all members to mean
    for(auto member=members.begin(); member!=members.end(); member++){
      AddVector(mean,vectors[*member].getCoords());
    }
    //if center has id, then its part of the dataset and a member of the cluster
    myvector center = cluster->getCenter();
    if(!center.get_id().empty()){
      AddVector(mean,center.getCoords()); //add center to mean
      DivVector(mean,members.size()+1); //divide mean
    }
    else
      DivVector(mean,members.size()); //divide means
    //set mean as new center
    cluster->setCentroid(myvector(mean));
  }
}

/*PAM imporoved like Lloyd's*/
void ClusterSpace::PAM(MyVectorContainer &vectors){
  //for every cluster compute its medoid
  for(auto cluster=Clusters.begin(); cluster!=Clusters.end(); cluster++){
    vector_index medoid = cluster->getMedoid();
    double min_dis = cluster->ClusterDissimilarity(vectors[medoid]);
    vector<vector_index> members = cluster->getMembers();
    //for every member calc sum of distances assuming its the medoid
    for(auto member=members.begin(); member!=members.end(); member++){
      double dissimilarity = cluster->ClusterDissimilarity(vectors[*member]);
      if(dissimilarity < min_dis){
        min_dis = dissimilarity;
        medoid = *member;
      }
    }
    //set medoid as new center
    cluster->setMedoid(medoid);
  }
}
/***********************Utility**********************************************/

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
int ClusterSpace::NearestCenter(myvector &v,const vector<Cluster*> &clusters,
  double* min_dist){
  //find distance to first center and set it as min
  int nearest_center_pos=0;
  *min_dist = EuclideanVectorDistance(v.begin(),v.end(),
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

void ClusterSpace::LloydsAssignWrapper(MyVectorContainer &vectors,
  std::vector<HashTable*> &mocktable){
  LloydsAssign(vectors);
}

void ClusterSpace::RangeSearchHypercubeAssignWrapper(MyVectorContainer
  &vectors,std::vector<HashTable*> &HTable){
    RangeSearchHypercubeAssign(vectors,*(HTable[0]));
}

/*return hashes to HashTable of every center in Clusters*/
vector<int> CenterHashes(vector<Cluster*> &clusters, HashTable* HTable){
  vector<int> result(clusters.size());
  for(int i=0; i<clusters.size(); i++){
    result[i] = HTable->get_hash(clusters[i]->getCenter());
  }
  return result;
}


/**************Relevant to Multimap*****************************************/
/*Create a CenterMap for every HashTable*/
void ClusterSpace::SetCenterMaps(vector<multimap<int,Cluster*>> &CenterMaps,
  vector<HashTable*> HTables){
  for(int i=0,size=HTables.size(); i<size; i++){
    //multiple centers can map to one bucket. multimap:(bucket_hash->cluster)
    CenterMaps[i] = MapCentersToBuckets(*(HTables[i]));
  }
}

//multiple centers can map to one bucket. multimap:(bucket_hash->centers)
multimap<int,Cluster*> ClusterSpace::MapCentersToBuckets(HashTable &HTable){
  multimap<int,Cluster*> hashmap;
  for(auto c=Clusters.begin(); c!=Clusters.end(); c++){
    int hash = HTable.get_hash(c->getCenter());
    hashmap.insert(std::pair<int,Cluster*>(hash,&(*c)));
  }
  return hashmap;
}

//Overloaded so that a center within hamming_dist is mapped to a bucket
multimap<int,Cluster*> ClusterSpace::MapCentersToBuckets(HashTable &HTable,
  int hamming_dist){
  //original map, with all centers to corresponding bucket
  multimap<int,Cluster*> center_map;
  for(auto c=Clusters.begin(); c!=Clusters.end(); c++){
    //map itself
    int key = HTable.get_hash(c->getCenter());
    center_map.insert(std::pair<int,Cluster*>(key,&(*c)));
  }
  //map neighbors
  //merged map, with buckets mapping to all centers within hamming_dist
  multimap<int,Cluster*> merged_map = center_map;
  for(auto c=Clusters.begin(); c!=Clusters.end(); c++){
    //get the bucket (map key)
    int key = HTable.get_hash(c->getCenter());
    //get relevant map-keys (buckets)
    vector<int> neighbors = HammingNeighbors(key,CmdArgs::K,hamming_dist);
    //merge every map-key with its relevant ones
    for(auto n_key=neighbors.begin(); n_key!=neighbors.end(); n_key++){
      auto range = center_map.equal_range(*n_key);
      merged_map.insert(range.first,range.second);
      /*for(auto it=range.first; it!=range.second; it++){
        merged_map.insert(std::pair<int,Cluster*>(it->first,it->second));
      }*/
    }
  }
  return merged_map;
}

/*Return all the Clusters that have centers in this bucket*/
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
