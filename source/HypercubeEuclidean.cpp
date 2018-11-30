#include "HypercubeEuclidean.hpp"
#include "ReadInput.hpp"
#include "utility.hpp"

#include <random>
#include <utility>  //move
#include <limits>
#include <cstdint>
#include <numeric>  //inner_product

using namespace std;

double Hypercube::IP_COEFFICIENT=1000;
int Hypercube::Euclidean::w=4;

Hypercube::Euclidean::Euclidean(int dim, int tsize)
:Metric("euclidean"), dimension(dim), hmap(){
  this->SetRandVectors();
  this->SetRandT();
}

Hypercube::Euclidean::~Euclidean(){}

extern int CmdArgs::K;

/*Create k vectors whoose coords are independently distributed
normal distribution with mean=0 and standard deviation=1*/
void Hypercube::Euclidean::SetRandVectors(){
  random_device generator;
  normal_distribution<float> distribution(0,1);

  for(int i=0; i<CmdArgs::K; i++){  //k vectors
    vector<float> v;
    for(int j=0; j<dimension; j++){
      v.push_back(distribution(generator));
    }
    vectors.push_back(move(v));
  }
}

void Hypercube::Euclidean::SetRandT(){
  random_device generator;
  uniform_int_distribution<int> distribution(0,w);
  int n;
  for(int i=0; i<CmdArgs::K; i++){
    while((n=distribution(generator)) == w);  //set [0,w) does not include w
    t.push_back(n);
  }
}

unsigned int Hypercube::Euclidean::Hash(const myvector &p){
  random_device generator;
  uniform_int_distribution<int> distribution(0,1);
  unsigned int result=0;
  for(int i=0; i<CmdArgs::K; i++){  //overflow danger in case 2^k>uint size
    int hi = get_h(i,p);  //**watch out for overflow here***
    if( hmap.find(hi) == hmap.end()){ //hi not in hmap, then add it
      int a = distribution(generator);
      hmap[hi] = a; //set a random bit for this hi in hmap
      //cout << "in map " << hi << "->" << hmap[hi] << endl;
    }
    result <<= 1;         //shift to make room for new rand lsb
    result += hmap[hi]; //add 1 or 0
  }
  return result;
}

int Hypercube::Euclidean::get_h(int i,const myvector& p){
  double pv_inner = inner_product(p.begin(),p.end(),vectors[i].begin(),(coord)0)
                    *IP_COEFFICIENT;
/*  cout << "inner_product of ";
  p.print();
  cout << endl << " and " << endl;
  printVector(vectors[i]);
  cout << "=" << pv_inner << endl;
  cout << "(" << pv_inner << "+" << t[i] << ")/" << w << "=" << floor( (pv_inner +t[i]) / w) << endl;
*/  return floor( (pv_inner +t[i]) / w);
}

std::vector<long int> Hypercube::Euclidean::get_g(myvector &p){
  vector<long int> g(CmdArgs::K);
  for(int i=0; i<CmdArgs::K; i++){
    g[i] = this->get_h(i,p);
  }
  return g;
}

int Hypercube::Euclidean::dim(){
  return dimension;
}

double Hypercube::Euclidean::vectorDistance(std::vector<coord>::const_iterator first,
                                        std::vector<coord>::const_iterator last,
                                        std::vector<coord>::const_iterator first2)
{
  return EuclideanVectorDistance(first,last,first2);
}
