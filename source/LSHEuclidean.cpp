#include "LSHEuclidean.hpp"
#include "ReadInput.hpp"
#include "utility.hpp"

#include <random>
#include <utility>  //move
#include <limits>
#include <cstdint>
#include <numeric>  //inner_product

using namespace std;

int LSH::Euclidean::w=300;
long int LSH::Euclidean::M = UINT32_MAX-4;   //4294967291 is prime
std::vector<int> LSH::Euclidean::r;

LSH::Euclidean::Euclidean(int dim, int tsize)
:Metric("euclidean"), dimension(dim), tablesize(tsize){
  this->SetRandVectors();
  if(r.empty())
    this->SetRandR();
  this->SetRandT();
}

LSH::Euclidean::~Euclidean(){}

extern int CmdArgs::K;

/*Create k vectors whoose coords are independently distributed
normal distribution with mean=0 and standard deviation=1*/
void LSH::Euclidean::SetRandVectors(){
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

void LSH::Euclidean::SetRandR(){
  random_device generator;
  uniform_int_distribution<int> distribution(INT32_MIN,INT32_MAX);

  for(int i=0; i<CmdArgs::K; i++){
    r.push_back(distribution(generator));
  }
}

void LSH::Euclidean::SetRandT(){
  random_device generator;
  uniform_int_distribution<int> distribution(0,w);
  int n;
  for(int i=0; i<CmdArgs::K; i++){
    while((n=distribution(generator)) == w);  //set [0,w) does not include w
    t.push_back(n);
  }
}

unsigned int LSH::Euclidean::Hash(myvector& p){
  long int f=0;
  for(int i=0; i<CmdArgs::K; i++){
    long int ip = this->get_h(i,p);
    f += MOD((r[i]*ip),M); //handle overflow with modM
    //cout << "here " << r[i] << "*" << ip << " % " << M << " = " << (r[i]*ip)%M << endl;
  }
  //cout << f << " % " << tablesize << " = "<< MOD(f,tablesize) << endl;
  //cout << "----------" << endl;
  return MOD(f,tablesize);
}

long int LSH::Euclidean::get_h(int i, myvector& p){
  double pv_inner = inner_product(p.begin(), p.end(), vectors[i].begin(), 0);
/*  cout << "inner_product of ";
  p.print();
  cout << endl << " and " << endl;
  printVector(vectors[i]);
  cout << "=" << pv_inner << endl;
  cout << "(" << pv_inner << "+" << t[i] << ")/" << w << "=" << floor( (pv_inner +t[i]) / w) << endl;
*/  return floor( (pv_inner +t[i]) / w);
}

std::vector<long int> LSH::Euclidean::get_g(myvector &p){
  vector<long int> g(CmdArgs::K);
  for(int i=0; i<CmdArgs::K; i++){
    g[i] = this->get_h(i,p);
  }
  return g;
}

int LSH::Euclidean::dim(){
  return dimension;
}

double LSH::Euclidean::vectorDistance(std::vector<coord>::iterator first,
                                std::vector<coord>::iterator last,
                                std::vector<coord>::iterator first2)
{
  return EuclideanVectorDistance(first,last,first2);
}
