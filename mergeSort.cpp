#include <bits/stdc++.h>
#define N 1000001
using namespace std;

 
void merge(int leftArr[], int rightArr[], int arr[], int n) {
   int leftSize = n / 2;
   int rightSize = n - leftSize;
   int i = 0, l = 0, r = 0;
   while(l < leftSize && r < rightSize) {
      if (leftArr[l] < rightArr[r]) {
         arr[i] = leftArr[l];
         i++, l++;
      }
      else {
         arr[i] = rightArr[r];
         i++, r++;
      }
   }
   while (l < leftSize) {
      arr[i] = leftArr[l];
      i++, l++;
   }
   while (r < rightSize) {
      arr[i] = rightArr[r];
      i++, r++;
   }
}
 
void mergeSort(int arr[], int n) {
   if (n <= 1) return;
   int mid = n/2;
   int* leftArr = new int[mid];
   int* rightArr = new int[n - mid];
   for (int i = 0; i < mid; i++) leftArr[i] = arr[i];
   for (int i = mid; i < n; i++) rightArr[i - mid] = arr[i];
 
   mergeSort(leftArr, mid);
   mergeSort(rightArr, n - mid);
   merge(leftArr, rightArr, arr, n);
   delete[] leftArr;
   delete[] rightArr;
}

int main() {
   ios::sync_with_stdio(false); cin.tie(); cout.tie();
   int arr[N], n;
   cin >> n;
   for (int i = 0; i < n; i++) cin >> arr[i];
   mergeSort(arr, n);
   for (int i = 0; i < n; i++) cout << arr[i] << " ";
   cout << "\n";
   return 0;
}
