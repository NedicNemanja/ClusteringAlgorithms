#ifndef CLUSTERSPACE_HPP
#define CLUSTERSPACE_HPP

#include "Cluster.hpp"
#include "myvector.hpp"
#include "HashTable.hpp"
#include <vector>

class ClusterSpace{
    std::vector<Cluster> Clusters;
  public:
    ClusterSpace(std::vector<myvector> &vectors, const std::string algo_name);
    ~ClusterSpace();
    std::vector<myvector> getCenters();
    void Print();
    bool isCenter(myvector &p);
    double MinDistanceToCenterSquared(myvector &v);
    double MinDistanceBetweenCenters();
    int NearestCenter(myvector &v);
    void LloydsAssignment(std::vector<myvector> &vectors);
    void RangeSearchLSHAssignment(vector<myvector>&,vector<HashTable*>&);
};

#endif
