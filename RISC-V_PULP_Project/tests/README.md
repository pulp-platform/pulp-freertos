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
### Description
Creates two sets of two tasks. The tasks within a set share a variable, access
to which is guarded by a semaphore.

Each task starts by attempting to obtain the semaphore. On obtaining a semaphore
a task checks to ensure that the guarded variable has an expected value. It then
clears the variable to zero before counting it back up to the expected value in
increments of 1. After each increment the variable is checked to ensure it
contains the value to which it was just set. When the starting value is again
reached the task releases the semaphore giving the other task in the set a
chance to do exactly the same thing. The starting value is high enough to ensure
that a tick is likely to occur during the incrementing loop.

An error is flagged if at any time during the process a shared variable is found
to have a value other than that expected. Such an occurrence would suggest an
error in the mutual exclusion mechanism by which access to the variable is
restricted. The error is propagated via newlib's `exit`.

The first set of two tasks poll their semaphore. The second set use blocking
calls.

### Measurements
TODO: describe in detail

## Stream Buffers
### Description

A simple example that shows a stream buffer being used to pass data from an
interrupt to a task.

There are two strings, pcStringToSend and pcStringToReceive, where
pcStringToReceive is a substring of pcStringToSend. The interrupt sends a few
bytes of pcStringToSend to a stream buffer ever few times that it executes. A
task reads the bytes from the stream buffer, looking for the substring, and
flagging an error if the received data is invalid. The error is propagated via
newlib's `exit`.

### Measurements
TODO: describe in detail
