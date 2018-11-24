#ifndef UTILITY_HPP
#define UTILITY_HPP
#include <list>

#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <numeric> //inner_product

#include "myvector.hpp"
#include "ErrorCodes.hpp"

double ExhaustiveSearch(std::list<myvector> &vlist, myvector &q, double* dist);

unsigned int MOD(long int a , long int b);
double MOD(double a , double b);

template<class T>
bool vectorCompare(std::vector<T> &A, std::vector<T> &B){
  typename std::vector<T>::iterator Aiter = A.begin();
  typename std::vector<T>::iterator Biter = B.begin();
  while(Aiter != A.end() && Biter != B.end()){
    if(*Aiter != *Biter)
      return false;
    Aiter++;
    Biter++;
  }
  if(A.size() != B.size())  //move this up if you don't expect all vectors...
    return false;           //...to be of the same dimension
  return true;
}

/*Get all bit strings that have a binary-hamming-distance=1 from n
(CmdArgs::K is the lenght/number of bits)*/
std::vector<int> HammingNeighbors(int n, int k, int hamming_distance=1);

/************Most of the below were found on the internet #CodeReuse***********/
template<class Iter_T, class Iter2_T>
double EuclideanVectorDistance(Iter_T first, Iter_T last, Iter2_T first2) {
  double ret = 0.0;
  while (first != last) {
    double dist = (*first++) - (*first2++);
    ret += dist * dist;
  }
  return ret > 0.0 ? sqrt(ret) : 0.0;
};

template<class Iter_T, class Iter2_T>
double CosineVectorDistance(Iter_T first, Iter_T last, Iter2_T first2) {
  double ip = inner_product(first, last, first2, 0);
  double norm1 = 0.0, norm2 = 0.0;
  while(first != last){
    norm1 += (*first)*(*first);  //sum of squares
    norm2 += (*first2)*(*first2);
    first++;
    first2++;
  }
  double divisor = sqrt(norm1)*(sqrt(norm2));
  if( divisor == 0)
    exit(UNDEFINED_COSINE_ZERO_VECTOR);
  double cos_similarity = ip/divisor;
  return 1 - cos_similarity;
};

template<class Iter_T>
double Norm(Iter_T first, Iter_T last){
  double norm = 0.0;
  while(first != last){
    //cout << *first << " ";
    norm += (*first)*(*first);  //sum of squares
  }
  return sqrt(norm);
}

template <class T>
void PrintList(std::list<T>& myvectors){
    // allocate an iterator that points to the first
    // element in the list
    typename std::list<T>::iterator iter = myvectors.begin();
    // continue to loop through the list until the
    // iterator hits the end of the list
    while(iter != myvectors.end())
    {
        // retrieve the myvector the iterator points at
        T& s = *iter;
        s.print();
        // now move the iterator over to the next element
        // in the list
        iter++;
    }
}

template<typename T>
void printVector(const T& t) {
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
}

template<typename T>
void printVectorInVector(const T& t) {
    std::for_each(t.cbegin(), t.cend(), printVector<typename T::value_type>);
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage);

#endif
