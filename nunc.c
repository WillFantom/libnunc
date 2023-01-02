#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nunc.h"

float cdfCost(float value, int size)
{
  if (value <= 0 || value >= 1)
  {
    return 0;
  }
  float conj = 1 - value;
  return size * (value * log(value) - conj * log(conj));
}

float empDist(float datapoint, float q)
{
  if (datapoint < q)
  {
    return 1;
  }
  else if (datapoint > q)
  {
    return 0;
  }
  return 0.5;
}

float ecdf(float *data, int size, float q)
{
  int left = bisectLeft(data, size, q);
  int right = bisectRight(data, size, q);
  return (left + 0.5 * (right - left)) / size;
}

float quantile(float *sortedWindow, int size, float pct)
{
  float index = (size - 1) * pct;
  float lower = sortedWindow[(int)floor(index)];
  float upper = sortedWindow[(int)ceil(index)];
  if (lower == upper)
  {
    return lower;
  }
  return lower + ((index - floor(index)) * (upper - lower));
}

void quantiles(float *quantileArray, float *sortedWindow, int size, int quantileCount)
{
  float c = log((2 * size) - 1);
  for (int i = 0; i < quantileCount; i++)
  {
    float pct = 1 / (1 + (2 * (size - 1)) * exp(((c * -1) / quantileCount) * ((2 * i) - 1)));
    quantileArray[i] = quantile(sortedWindow, size, pct);
  }
}

void windowUpdate(float datapoint, int size, int quantileCount, float *qs, float *cdf)
{
  for (int i = 0; i < quantileCount; i++)
  {
    cdf[i] *= size;
    cdf[i] -= empDist(datapoint, qs[i]);
    cdf[i] /= (size - 1);
  }
}

int nuncInit(nunc *nuncProcessor, int capacity, int quantileCount)
{
  if (capacity <= 0 || quantiles <= 0)
  {
    return 1;
  }
  nuncProcessor->cap = capacity;
  nuncProcessor->count = 0;
  nuncProcessor->quantileCount = quantileCount;
  nuncProcessor->window = (float *)malloc(sizeof(float) * capacity);

  nuncProcessor->windowSorted = (float *)malloc(sizeof(float) * capacity);
  nuncProcessor->quantileArray = (float *)malloc(sizeof(float) * quantileCount);
  nuncProcessor->fullCdf = (float *)malloc(sizeof(float) * quantileCount);
  nuncProcessor->rightCdf = (float *)malloc(sizeof(float) * quantileCount);
  nuncProcessor->leftCdf = (float *)malloc(sizeof(float) * quantileCount);

  return 0;
}

void *nuncNew(int capacity, int quantileCount)
{
  nunc *nuncProcessor = malloc(sizeof(nunc));
  if (nuncInit(nuncProcessor, capacity, quantileCount))
  {
    printf("nunc could not be initialized with the given values\n");
    return NULL;
  }
  return nuncProcessor;
}

void nuncPush(nunc *nuncProcessor, float value, cost *maxCost)
{
  // add datapoint to window
  nuncProcessor->window[nuncProcessor->count % nuncProcessor->cap] = value;
  nuncProcessor->count++;

  // return if window has not yet reached capacity
  if (nuncProcessor->count < nuncProcessor->cap)
  {
    return;
  }

  // get sorted copy of window data
  memcpy(nuncProcessor->windowSorted, nuncProcessor->window, sizeof(float) * nuncProcessor->cap);
  quicksort(nuncProcessor->windowSorted, 0, (nuncProcessor->cap - 1));

  // get quantile values
  quantiles(nuncProcessor->quantileArray, nuncProcessor->windowSorted, nuncProcessor->cap, nuncProcessor->quantileCount);

  // get cdf values
  float fullCost = 0;
  for (int i = 0; i < nuncProcessor->quantileCount; i++)
  {
    nuncProcessor->fullCdf[i] = ecdf(nuncProcessor->windowSorted, nuncProcessor->cap, nuncProcessor->quantileArray[i]);
    fullCost += cdfCost(nuncProcessor->fullCdf[i], nuncProcessor->cap);
  }

  // segment cost loop
  memcpy(nuncProcessor->rightCdf, nuncProcessor->fullCdf, sizeof(float) * nuncProcessor->quantileCount);

  float maxCostValue = -1;
  float maxCostIndex = -1;
  for (int i = 0; i < nuncProcessor->cap - 1; i++)
  {
    int length = (nuncProcessor->cap - i);
    float datapoint = nuncProcessor->window[(nuncProcessor->count + i) % nuncProcessor->cap];
    windowUpdate(datapoint, length, nuncProcessor->quantileCount, nuncProcessor->quantileArray, nuncProcessor->rightCdf);
    length -= 1;
    for (int j = 0; j < nuncProcessor->quantileCount; j++)
    {
      nuncProcessor->leftCdf[j] = ((nuncProcessor->fullCdf[j] * nuncProcessor->cap) - (nuncProcessor->rightCdf[j] * length)) / (nuncProcessor->cap - length);
    }
    float leftCost = 0;
    for (int j = 0; j < nuncProcessor->quantileCount; j++)
    {
      leftCost += cdfCost(nuncProcessor->leftCdf[j], nuncProcessor->cap - length);
    }
    float rightCost = 0;
    for (int j = 0; j < nuncProcessor->quantileCount; j++)
    {
      rightCost += cdfCost(nuncProcessor->rightCdf[j], length);
    }
    float cost = 2 * ((leftCost + rightCost) - fullCost);
    if (cost > maxCostValue)
    {
      maxCostValue = cost;
      maxCostIndex = nuncProcessor->count - nuncProcessor->cap + i;
    }
  }
  maxCost->value = maxCostValue;
  maxCost->index = maxCostIndex;
}

void nuncFree(nunc *nuncProcessor)
{
  free(nuncProcessor->window);
  free(nuncProcessor->quantileArray);
  free(nuncProcessor->fullCdf);
  free(nuncProcessor->rightCdf);
  free(nuncProcessor->leftCdf);
}

float nuncThreshold(nunc *nuncProcessor, float probability, int datapoints)
{
  if (probability <= 0 || probability > 1)
  {
    return -1;
  }
  else if (datapoints == 0)
  {
    return -1;
  }
  int ws = nuncProcessor->cap;
  int qc = nuncProcessor->quantileCount;
  float ea = 1 - (8 * (1 / qc) * logf(probability / (ws * (datapoints - ws + 1))));
  float eb = 1 + (2 * (sqrtf(2 * (logf(ws * (datapoints - ws + 1)) / probability))));
  return fmaxf(ea, eb);
}
