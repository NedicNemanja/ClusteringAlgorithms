#ifndef CLUSTERSPACE_HPP
#define CLUSTERSPACE_HPP

#include "Cluster.hpp"
#include "myvector.hpp"

class ClusterSpace{
    std::vector<Cluster> Clusters;
  public:
    ClusterSpace(std::vector<myvector> &vectors, const std::string algo_name);
    ~ClusterSpace();
    std::vector<myvector> getCenters();
    void Print();
    bool isCenter(myvector &p);
    double MinDistanceToCenter(myvector &v);
};

#endif
