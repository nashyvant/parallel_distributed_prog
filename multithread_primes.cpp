#include <iostream>
#include <cmath>
#include <cstdint>
#include <thread>
#include <chrono>
#include <cstring>

using namespace std;

// Count primes without multithreading
uint64_t countPrimes(uint8_t *b_primes, uint64_t n) 
{
    uint64_t count = 1;
    uint64_t sqrt_n = sqrt(n);
    for (uint64_t i = 3; i <= sqrt_n; i+=2) {
        if (b_primes[i]) {
            ++count;
            for (uint64_t j = i * i; j <= n; j += 2*i) {
                b_primes[j] = 0;
            }
        }
    }

    uint64_t start = sqrt_n+1;
    start |= 1;//make it odd
    for(uint64_t i = start; i<=n; i+=2)
    {
        if(b_primes[i])
            ++count;
    }
    return count;
}

uint64_t initMarkNonPrimes(uint8_t *b_primes, uint64_t n) 
{
    uint64_t count = 1;
    uint64_t sqrt_n = sqrt(n);
    for (uint64_t i = 3; i <= sqrt_n; i+=2) {
        if (b_primes[i]) {
            for (uint64_t j = i * i; j <= n; j += 2*i) {
                b_primes[j] = 0;
            }
        }
    }
    return count;
}

void countInMultithread(uint8_t *b_primes, uint64_t start, uint64_t end, uint64_t *count)
{
    uint64_t lcount = 0;
    if(start == 2 )
        ++lcount;

    start = start | 1;

    for (uint64_t i = start; i <= end; i+=2) {
        if(b_primes[i])
            ++lcount;
    }

    *count = lcount;
}

// Count primes for a range using multithreading
void markNonPrimes(uint8_t *b_primes, uint64_t start, uint64_t end)
{
    if(start %2 == 0)
        start |= 1;
    uint64_t limit = sqrt(end);
    for (uint64_t i = 3; i <= limit; i+=2) { //skip even numbers
        if (b_primes[i]) {
            uint64_t first_val = (start%i == 0)?start:start+i-start%i;//next even multiple of i; if even, add another i
            while(first_val%2 == 0)
                first_val += i;
            for (uint64_t j = first_val ; j <= end; j += 2*i) { // j+=2*i
                b_primes[j] = 0;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    uint64_t n = atol(argv[1]);
    uint8_t *b_primes = new uint8_t[n + 1];
    
    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t count = 0;
    uint64_t limit = sqrt(n);
    memset(b_primes, 1, sizeof(uint8_t)*(n+1));
    count = countPrimes(b_primes, n);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "Single thread; Number of primes between 2 and " << n << " is: " << count << endl;
    cout << "Total time taken by 1 thread: " << duration_ms.count() << "ms" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    uint64_t count1 = 0, count2 = 0;
    memset(b_primes, 1, sizeof(uint8_t)*(n+1));
    uint64_t mid = n / 2;
    initMarkNonPrimes(b_primes, mid);
    uint64_t num_ele = n-sqrt(n/2)+1;
    uint64_t start1 = sqrt(n/2)+1, end1 = sqrt(n/2)+num_ele/2;
    // Launch 2 threads for marking non-primes
    thread t1(markNonPrimes, b_primes, start1, end1);
    thread t2(markNonPrimes, b_primes,end1+1, n);

    t1.join();
    t2.join();
    
    #if 0
    // Launch 2 threads for counting primes
    thread t3(countInMultithread, b_primes, 2, n/2, &count1);
    thread t4(countInMultithread, b_primes, n/2 + 1, n, &count2);

    t3.join();
    t4.join();
    #endif

    count = 0;
    countInMultithread(b_primes, 2, n, &count);
    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "2 threads; Number of primes between 2 and " << n << " is: " << count << endl;
    cout << "Total time taken by 2 threads: " << duration_ms.count() << "ms" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    uint64_t count3 = 0, count4 = 0;
    count1 = 0, count2 = 0;
    memset(b_primes, 1, sizeof(uint8_t)*(n+1));
    b_primes[0] = 0, b_primes[1] = 0;

    uint64_t qtr = n / 4;
    initMarkNonPrimes(b_primes, qtr);
    num_ele = n-sqrt(qtr)+1;
    start1 = sqrt(qtr)+1;
    end1 = sqrt(qtr)+num_ele/4;
    uint64_t end2 = sqrt(qtr)+num_ele/2, end3 = sqrt(qtr)+3*num_ele/4;

    // Launch 4 threads for marking non-primes
    thread t41(markNonPrimes, b_primes, start1, end1);
    thread t42(markNonPrimes, b_primes,end1+1, end2);
    thread t43(markNonPrimes, b_primes, end2+1, end3);
    thread t44(markNonPrimes, b_primes, end3+1, n);

    t41.join(); t42.join(); t43.join(); t44.join();
    
    #if 0
    // Launch 4 threads for counting primes
    thread t5(countInMultithread, b_primes, 2, n/4, &count1);
    thread t6(countInMultithread, b_primes, (n/4) + 1, n/2, &count2);
    thread t7(countInMultithread, b_primes, (n/2) + 1, 3*n/4, &count3);
    thread t8(countInMultithread, b_primes, (3*n/4) + 1, n, &count4);
   
    t5.join(); t6.join(); t7.join(); t8.join();
    #endif

    count = 0;
    countInMultithread(b_primes, 2, n, &count);
    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "4 threads; Number of primes between 2 and " << n << " is: " << count << endl;
    cout << "Total time taken by 4 threads: " << duration_ms.count() << "ms" << endl;
    
    delete[] b_primes;
    return 0;
}
