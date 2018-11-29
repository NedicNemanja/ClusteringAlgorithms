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
  public:
    ClusterSpace(MyVectorContainer&, std::string, std::string, std::string);
    ~ClusterSpace();
    std::vector<myvector> getCenters();
    void Print();
    bool isCenter(myvector &p);
    double MinDistanceToCenterSquared(myvector &v);
    double MinDistanceBetweenCenters();
    int NearestCenter(myvector &v);

    void RunClusteringAlgorithms();
    void LloydsAssignment(MyVectorContainer &vectors);
    void RangeSearchLSHAssignment(MyVectorContainer&,std::vector<HashTable*>);
};

/*Hash centers to buckets and save that information to a multimap*/
void HashCenters(std::multimap<int,myvector&> &hashmap,
                 std::vector<myvector> Centers, HashTable* HTable);

#endif
