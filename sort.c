#include "sort.h"

void swap(float *data, int num1, int num2)
{
  int temp = data[num1];
  data[num1] = data[num2];
  data[num2] = temp;
}

int partition(float *data, int left, int right, int pivot)
{
  int leftPointer = left - 1;
  int rightPointer = right;

  while (1)
  {
    while (data[++leftPointer] < pivot)
    {
    }

    while (rightPointer > 0 && data[--rightPointer] > pivot)
    {
    }

    if (leftPointer >= rightPointer)
    {
      break;
    }
    else
    {
      swap(data, leftPointer, rightPointer);
    }
  }
  swap(data, leftPointer, right);
  return leftPointer;
}

void quicksort(float *data, int left, int right)
{
  if (right - left <= 0)
  {
    return;
  }
  else
  {
    int pivot = data[right];
    int partitionPoint = partition(data, left, right, pivot);
    quicksort(data, left, partitionPoint - 1);
    quicksort(data, partitionPoint + 1, right);
  }
}

int bisectLeft(float *data, int size, float target)
{
  int i = 0;
  int j = size;
  while (i < j)
  {
    int h = (int)(((unsigned int)(i + j)) >> 1);
    if (data[h] < target)
    {
      i = h + 1;
    }
    else
    {
      j = h;
    }
  }
  return i;
}

int bisectRight(float *data, int size, float target)
{
  int i = 0;
  int j = size;
  while (i < j)
  {
    int h = (int)(((unsigned int)(i + j)) >> 1);
    if (data[h] <= target)
    {
      i = h + 1;
    }
    else
    {
      j = h;
    }
  }
  return i;
}
