#include <stdio.h>
#include <stdlib.h>

int* func(int n) {
  int* arr = (int*)malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    arr[i] = i * i;
  }
  return arr;
}

int main() {
  for (int j = 1; j < 19; j++) {
    int* mainArr = func(j);
    for (int i = 0; i < j; i++) {
      printf("%d ", mainArr[i]);
    }
  }

  return 0;
}