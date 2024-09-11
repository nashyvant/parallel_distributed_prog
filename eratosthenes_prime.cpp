#include <iostream>
#include <cmath>
#include <cstdint>
#include <thread>
#include <chrono>
#include <cstring>

using namespace std;

/*
 * No need to beyond sqrt(n) because the other factor would have already marked it as non-prime
 * init evens as 0, odd as 1
 * only do upto sqrt(n) first
 * then split rest of it
 */
// Count primes without multithreading
uint64_t countPrimes(uint8_t *b_primes, uint64_t n) 
{
    uint64_t count = 0;
    for (uint64_t i = 2; i <= n; ++i) {
        if (b_primes[i]) {
            ++count;
            for (uint64_t j = i * i; j <= n; j += i) {
                b_primes[j] = 0;
            }
        }
    }
    return count;
}

void countInMultithread(uint8_t *b_primes, uint64_t start, uint64_t end, uint64_t *count)
{
    uint64_t lcount = 0;
    
    for (uint64_t i = start; i <= end; ++i) {
        if(b_primes[i])
            ++lcount;
    }

    *count = lcount;
}

// Count primes for a range using multithreading
void countPrimesMultithread(uint8_t *b_primes, uint64_t start, uint64_t end, uint64_t *count)
{
    uint64_t lcount = 0;
    uint64_t limit = sqrt(end);
    for (uint64_t i = start; i <= limit; ++i) {
        if (b_primes[i]) {
            ++lcount;
            for (uint64_t j = i*i ; j <= end; j += i) { // j+=2*i
                b_primes[j] = 0;
            }
        }
    }

   /* for (uint64_t i = limit+1; i <= end; ++i) {
        if(b_primes[i])
            ++lcount;
    }*/

    *count = lcount;
}

int main(int argc, char* argv[])
{
    uint64_t n = atol(argv[1]);
    uint8_t *b_primes = new uint8_t[n + 1];
    
    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t count = 0;
    uint64_t limit = sqrt(n);
    memset(b_primes, 1, sizeof(uint8_t)*(n+1));
    b_primes[0] = 0, b_primes[1] = 0;
    count = countPrimes(b_primes, n);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "Single thread; Number of primes between 2 and " << n << " is: " << count << endl;
    cout << "Total time taken by 1 thread: " << duration_ms.count() << "ms" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    uint64_t count1 = 0, count2 = 0;
    count = 0;
    memset(b_primes, 1, sizeof(uint8_t)*(n+1));
    b_primes[0] = 0, b_primes[1] = 0;
    
    uint64_t start_range = limit + 1;
    uint64_t mid_range = start_range + (n - limit) / 2;

    countPrimesMultithread(b_primes, 2, n, &count);
    thread t1(countInMultithread, b_primes, start_range, mid_range, &count1);
    thread t2(countInMultithread, b_primes, mid_range + 1, n, &count2);

    t1.join();
    t2.join();
    
    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "2 threads; Number of primes between 2 and " << n << " is: " << count+count1+count2 << endl;
    cout << "Total time taken by 2 threads: " << duration_ms.count() << "ms" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    memset(b_primes, 1, sizeof(uint8_t)*(n+1));
    b_primes[0] = 0, b_primes[1] = 0;
    uint64_t count3 = 0, count4 = 0;
    count1 = 0, count2 = 0, count = 0;
   
    start_range = limit + 1;
    uint64_t mid_range1 = start_range + (n - limit) / 4   ;
    uint64_t mid_range2 = mid_range1 + (n-limit)/4;
    uint64_t mid_range3 = mid_range2 + (n-limit)/4;

    countPrimesMultithread(b_primes, 2, n, &count);
    thread t3(countInMultithread, b_primes, start_range, mid_range1, &count1);
    thread t4(countInMultithread, b_primes, mid_range1 + 1, mid_range2, &count2);
    thread t5(countInMultithread, b_primes, mid_range2 + 1, mid_range3, &count3);
    thread t6(countInMultithread, b_primes, mid_range3 + 1, n, &count4);

    t3.join();
    t4.join();
    t5.join();
    t6.join();

    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "4 threads; Number of primes between 2 and " << n << " is: " << count+count1+count2+count3+count4 << endl;
    cout << "Total time taken by 4 threads: " << duration_ms.count() << "ms" << endl;


    delete[] b_primes;
    return 0;
}
