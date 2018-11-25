#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "myvector.hpp"
#include <list>


class Cluster{
    myvector center;
    std::vector<myvector> vectors;  //points that belong to the cluster, excluding center
  public:
    Cluster(myvector &center);
    ~Cluster();
    myvector getCenter();
};

class ClusterSpace{
    std::vector<Cluster> Clusters;
  public:
    ClusterSpace(std::list<myvector> &vectors, const std::string algo_name);
    ~ClusterSpace();
    std::vector<myvector> getCenters();
    bool isCenter(myvector &p);
};
#endif
