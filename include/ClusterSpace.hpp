#ifndef CLUSTERSPACE_HPP
#define CLUSTERSPACE_HPP

#include "Cluster.hpp"
#include "myvector.hpp"
#include "HashTable.hpp"
#include <vector>
#include <map>

class ClusterSpace{
    std::vector<Cluster> Clusters;
    std::string init_algorithm;
    std::string assign_algorithm;
    std::string update_algorithm;
    //bitmap that mirrors MyVectorContainer indexing (true if vector's assigned)
    std::vector<bool> AssignedVectorBitMap;
    int num_assigned_vectors=0; //num of values set to true in BitMap
  public:
    ClusterSpace();
    ClusterSpace(MyVectorContainer&, std::string, std::string, std::string);
    ~ClusterSpace();
    std::vector<myvector> getCenters();
    void Print();
    bool isCenter(const myvector &p);
    Cluster getCluster(int i);
    double MinDistanceToCenterSquared(myvector &v);
    double MinDistanceBetweenCenters();
    int NearestCenter(myvector &v);
    int NearestCenter(myvector &v,const std::vector<Cluster*>&,double*);
    Cluster SecondNearestCluster(myvector&, int assigned);
    /*Create a CenterMap for every HashTable*/
    void SetCenterMaps(std::vector<std::multimap<int,Cluster*>>&,std::vector<HashTable*>);
    //multiple centers can map to one bucket. multimap:(bucket_hash->centers)
    std::multimap<int,Cluster*> MapCentersToBuckets(HashTable&);
    //Overloaded so that a center within hamming_dist is mapped to a bucket
    std::multimap<int,Cluster*> MapCentersToBuckets(HashTable&,int hamm_dist);
    //unassign all members from clusters and reset AssignedVectorBitMap
    void UnassignAll();
    /*Run the clustering algorithms pointed by init_algorithm,assign...etc*/
    void RunClusteringAlgorithms(MyVectorContainer&,std::vector<HashTable*>,HashTable*);
    /*Assign vectors to their nearest center*/
    void LloydsAssign(MyVectorContainer &vectors);
    void LloydsAssignWrapper(MyVectorContainer&,std::vector<HashTable*>&);
    /*Assign vectors that are unassigned to their nearest center*/
    void LloydsAssign(MyVectorContainer &vectors,const std::string);
    /*Assign vectors by radius, doubling it until stop criteria is met.*/
    void RangeSearchLSHAssign(MyVectorContainer&,std::vector<HashTable*>&);
    void RangeSearchHypercubeAssign(MyVectorContainer&,HashTable&);
    void RangeSearchHypercubeAssignWrapper(MyVectorContainer&,std::vector<HashTable*>&);
    /*For every vector in bucket,assign to nearest Cluster center within radius*/
    void NearestCenterRangeAssign(Bucket,double,const std::vector<Cluster*>&,MyVectorContainer&);
    /*Calculate mean vector of the  every cluster and set it as the new center*/
    void K_means(MyVectorContainer &vectors);
    /*PAM imporoved like Lloyd's*/
    void PAM(MyVectorContainer &vectors);
    /*In conf you can define a parameter that set the precision of convergence*/
    bool ObjectiveFunctionCoverges(ClusterSpace&);
    //for every cluster calculate average of s(p), where p all clusters points
    std::vector<double> Silhouette(MyVectorContainer &vectors);
};


/*Return all the CLusters that have centers in this bucket*/
std::vector<Cluster*> GetBucketClusters(int, std::multimap<int,Cluster*> &CMap);
/*return hashes of every center in clusters vector*/
std::vector<int> CenterHashes(std::vector<Cluster*> &clusters, HashTable*);

#endif
