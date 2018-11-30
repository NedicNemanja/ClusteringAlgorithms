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
    ClusterSpace(MyVectorContainer&, std::string, std::string, std::string);
    ~ClusterSpace();
    std::vector<myvector> getCenters();
    void Print();
    bool isCenter(myvector &p);
    double MinDistanceToCenterSquared(myvector &v);
    double MinDistanceBetweenCenters();
    int NearestCenter(myvector &v);
    int NearestCenter(myvector &v,std::vector<Cluster*>&,double*);
    //multiple centers can map to one bucket. multimap:(bucket_hash->centers)
    void MapCentersToBuckets(std::multimap<int,Cluster*> &map,HashTable*);

    void RunClusteringAlgorithms(MyVectorContainer&,std::vector<HashTable*>);
    /*Assign vectors to their nearest center*/
    void LloydsAssignment(MyVectorContainer &vectors);
    /*Assign vectors that are unassigned to their nearest center*/
    void LloydsAssignment(MyVectorContainer &vectors,const std::string);
    void RangeSearchLSHAssignment(MyVectorContainer&,std::vector<HashTable*>);
    /*For every vector in bucket,assign to nearest Cluster center within radius*/
    void NearestCenterAssign(Bucket,double,std::vector<Cluster*>&,MyVectorContainer&);
};


/*Return all the CLusters that have centers in this bucket*/
std::vector<Cluster*> GetBucketClusters(int, std::multimap<int,Cluster*> &CMap);
/*return hashes of every center in clusters vector*/
std::vector<int> CenterHashes(std::vector<Cluster*> &clusters, HashTable*);
#endif
