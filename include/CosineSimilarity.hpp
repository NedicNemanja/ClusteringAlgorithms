#ifndef COSINESIMILARITY_HPP
#define COSINESIMILARITY_HPP

#include <vector>
#include <cmath>

#include "myvector.hpp"
#include "Metric.hpp"
#include "utility.hpp"

class CosineSimilarity: public Metric{
    int dimension;
    std::vector<std::vector<double>> r_vectors;
  public:
    CosineSimilarity(int dim);
    ~CosineSimilarity();
    void SetRandRvectors(int d);
    unsigned int Hash(const myvector &p);
    int get_h(int i,const myvector &p);
    int dim();
    std::vector<long int> get_g(myvector &p);
    //cosine vector distance
    double vectorDistance(std::vector<coord>::const_iterator first,
                          std::vector<coord>::const_iterator last,
                          std::vector<coord>::const_iterator first2);
};

#endif
