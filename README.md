# BlockingQueue
A simple BlockingQueue implemented in c++17
## Benchmark
  Magnitude     | Enqueue     | Dequeue     | Enqueue & Dequeue|
  :-----------  | :-----------| :-----------| :-----------------
  10K           | 4.9ms       | 4.2ms       | 4.5ms
  100K          | 74.4ms      | 26.7ms      | 108.4ms
  1000K         | 290.1ms     | 250.7ms     | 503.2ms

The above data was tested on my i5-7500 cpu with gcc -O3.\
You can also compare the tested data with [LockFreeQueue](https://github.com/bhhbazinga/LockFreeQueue)

The data of first and second column was obtained by starting 4 threads to enqueue concurrently and dequeue concurrently, the data of third column was obtained by starting 2 threads to enqueue and 2 threads to dequeue concurrently, each looped 10 times to calculate the average time consumption.
## Build
```
make && ./test
```
## API
```
void Enqueue(const T& data);
void Enqueue(T&& data);
T Dequeue();
bool TryDequeue(T& value);
bool Dequeue(T& data);
bool empty() const;
size_t size() const;
```
