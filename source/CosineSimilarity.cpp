#include "CosineSimilarity.hpp"
#include "ReadInput.hpp"

#include <numeric>  //inner_product
#include <random>

using namespace std;
using CmdArgs::K;

CosineSimilarity::CosineSimilarity(int dim)
:Metric("cosine"),dimension(dim),r_vectors(CmdArgs::K){
  cout << "Creating Cosine" << endl;
  SetRandRvectors(dimension);
}

CosineSimilarity::~CosineSimilarity(){}

void CosineSimilarity::SetRandRvectors(int d){
  random_device generator;
  normal_distribution<float> distribution(0,1);
  double n;
  for(int i=0; i<CmdArgs::K; i++){  //k random vectors
    for(int j=0; j<d; j++){       //of d dimension
      do{
        n=distribution(generator);
      }while(n==0 || n==1); //open set (0,1)
      r_vectors[i].push_back(n);
      //cout << n << " ";
    }
    //cout << endl;
  }
}

unsigned int CosineSimilarity::Hash(myvector& p){
  unsigned int result=0;
  for(int i=0; i<CmdArgs::K; i++){  //overflow danger in case 2^k>uint size
    result <<= 1;         ///shift to make room for new lsb
    result += get_h(i,p); //add 1 or 0
  }
  return result;
}

int CosineSimilarity::get_h(int i,myvector &p){
  double prod = inner_product(p.begin(), p.end(),r_vectors[i].begin(),(coord)0);
  return prod >= 0 ? 1 : 0;
}

std::vector<long int> CosineSimilarity::get_g(myvector &p){
  std::vector<long int> g(CmdArgs::K);
  for(int i=0; i<CmdArgs::K; i++){  //overflow danger in case 2^k>uint size
    g[i] = get_h(i,p); //add 1 or 0
  }
  return g;
}

int CosineSimilarity::dim(){ return dimension; }


double CosineSimilarity::vectorDistance(std::vector<coord>::iterator first,
                                        std::vector<coord>::iterator last,
                                        std::vector<coord>::iterator first2)
{
  return CosineVectorDistance(first,last,first2);
}
