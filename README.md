# NUNC

C library for the [NUNC](https://www.sciencedirect.com/science/article/pii/S0167947322001311) change detection algorithm.

## Usage

```c
...
#include <nunc.h>
...


int main()
{
  ...

  nunc nuncProcessor;
  if (nuncInit(&nuncProcessor, 350, 3))
  {
    return 1;
  }
  float threshold = nuncThreshold(&nuncProcessor, 0.3, 200);
  cost *maxCost = (cost *)malloc(sizeof(cost));

  nuncPush(&nuncProcessor, SOME_DATAPOINT, maxCost);

  if (maxCost->value > threshold)
  {
    printf("changepoint at index: %d (cost: %f)\n", maxCost->index, maxCost->value);
  }
  free(maxCost);
  nuncFree(&nuncProcessor);

  ...
}
```

## Configuration

To create a NUNC processor, you must first decide on a **window size** and the number of **quantiles** to use in the cost calculation.

To get a calculated threshold value, you will need to the configuration options of a NUNC processor along with a **probability** and **number of datapoints**. The resulting value provides a suitiable threshold value will aim to produce only false alerts at the given probability per X datapoints.
