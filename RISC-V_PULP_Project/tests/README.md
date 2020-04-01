# Tests

Tests and measurements of FreeRTOS primitives.


## Queue
### Description
Creates two tasks that communicate over a single queue. One task acts as a
producer, the other a consumer.

The producer loops for three iteration, posting an incrementing number onto
the queue each cycle. It then delays for a fixed period before doing exactly
the same again.

The consumer loops emptying the queue. Each item removed from the queue is
checked to ensure it contains the expected value. When the queue is empty it
blocks for a fixed period, then does the same again.

All queue access is performed without blocking. The consumer completely
empties the queue each time it runs so the producer should never find the
queue full.

An error is flagged if the consumer obtains an unexpected value or the
producer find the queue is full. The error is propagated via newlib's `exit`.

### Measurements
TODO: Describe in detail


## Semaphores
