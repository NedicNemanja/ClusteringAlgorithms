#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "myvector.hpp"
#include "Metric.hpp"
#include <list>


class Cluster{
    Metric* metric;
    myvector center;
    std::vector<myvector> vectors;  //points that belong to the cluster, excluding center
  public:
    Cluster(myvector &center);
    ~Cluster();
    myvector getCenter();
    void Print();
};

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
