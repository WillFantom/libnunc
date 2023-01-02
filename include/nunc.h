#ifndef NUNC_H
#define NUNC_H

#include "sort.h"

typedef struct nunc
{
  float *window;
  int cap;
  int count;
  int quantileCount;

  float *windowSorted;
  float *quantileArray;
  float *fullCdf, *rightCdf, *leftCdf;
} nunc;

typedef struct cost
{
  float value;
  int index;
} cost;

int nuncInit(nunc *nuncProcessor, int capacity, int quantiles);

void *nuncNew(int capacity, int quantileCount);

/**
 * @brief Calculate an appropriate cost threshold value for the nunc processor based on false alarm probability per X datapoints.
 *
 * @param nuncProcessor
 * @param probability
 * @param datapoints
 * @return float
 */
float nuncThreshold(nunc *nuncProcessor, float probability, int datapoints);

/**
 * @brief Add a new datapoint to a nunc processor's dataset and if possible determine the cost associated with the datapoint
 *
 * @param nuncProcessor the nunc processor to use to perform the cost calculation
 * @param value the float value that will be added to the nunc dataset
 * @return cost associated with the datapoint or NULL if a cost could not be calculated
 */
void nuncPush(nunc *nuncProcessor, float value, cost *maxCost);

/**
 * @brief Free the memory alloction used by the nunc processor. Do not use the processor after using this without using init.
 *
 * @param nuncProcessor
 */
void nuncFree(nunc *nuncProcessor);

#endif /* NUNC_H */
