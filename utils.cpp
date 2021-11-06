#include "utils.h"

//Bin Search
template<typename T>
int smallestGreater(T *arr, int n, T k){
  if(arr==nullptr || n==0) return 0;
  int l=0, r=n-1, found=n, mid;

  while(l<=r){
    mid=(l+r)/2;
    if(arr[mid]>=k){
      found=mid;
      r=mid-1;
    }
    else l=mid+1;
  }

  return found;
}