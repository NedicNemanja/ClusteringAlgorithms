#include "utility.hpp"
#include "ErrorCodes.hpp"

#include <float.h> //DBL_MAX
#include <time.h>
#include <cmath>
#include <algorithm>  //find

using namespace std;

double ExhaustiveSearch(list<myvector> &vlist, myvector &q, double* distTrue){
  struct timespec start,end;
  clock_gettime(CLOCK_MONOTONIC,&start);

  myvector nn;
  double min_dist = DBL_MAX, timeTrue;
  for(list<myvector>::iterator it=vlist.begin(); it != vlist.end(); it++){
    double distance = EuclideanVectorDistance(q.begin(),q.end(),(*it).begin());
    if(distance < min_dist){
      nn = *it;
      min_dist = distance;
    }
  }

  clock_gettime(CLOCK_MONOTONIC,&end);
  *distTrue = min_dist;
  timeTrue = end.tv_sec-start.tv_sec + (end.tv_nsec-start.tv_nsec)/1000000000.0;
  return timeTrue;
}


//Modulo for integers both positive and negative
unsigned int MOD(long int a , long int b){
  if(b == 0)
    exit(MOD_ZERO_EXCEPTION);
  return (a%b+b)%b;
}

//Modulo for doubles
double MOD(double a, double b){
  if(b == 0.0)
    exit(MOD_ZERO_EXCEPTION);
  return fmod((fmod(a,b)+b),b);
}

/*Get all bit strings that have a binary-hamming-distance=1 from n
(CmdArgs::K is the lenght/number of bits)*/
std::vector<int> HammingNeighbors(int n, int k, int dist){
  int index=0;
  if(dist == 0){
    std::vector<int> neighbors;
    return neighbors;
  }
  if(dist == 1){
    std::vector<int> neighbors(k);
    for(index=0; index<k; index++){
      neighbors[index] = n ^ (1 << index);
    }
    return neighbors;
  }
  else{
    /*Get the neighbors at dist but exclude those at dist-1 and dist-2 because
    we will already have checked those*/
    std::vector<int> neighbors;
    std::vector<int> dist1 = HammingNeighbors(n,k,dist-1);
    std::vector<int> dist2 = HammingNeighbors(n,k,dist-2);
    for(int elem=0; elem<dist1.size(); elem++){
      for(int i=0; i<k; i++){
        int h = dist1[elem] ^ (1 << i);
        //if h not in neighbors and not dist1 add it to neighbors
        if(  std::find(neighbors.begin(), neighbors.end(), h) == neighbors.end()
          && std::find(dist1.begin(), dist1.end(), h) == dist1.end()
          && std::find(dist2.begin(), dist2.end(), h) == dist2.end()
          && h!=n ){
            bool b1 = std::find(neighbors.begin(), neighbors.end(), h) == neighbors.end();
            bool b2 = std::find(dist1.begin(), dist1.end(), h) == dist1.end();
            neighbors.push_back(h);
        }
      }
    }
    return neighbors;
  }
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress (double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}
