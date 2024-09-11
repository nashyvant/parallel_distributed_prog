#include<iostream>
#include<cmath>
#include<stdint.h>
#include<thread>
#include<chrono>

using namespace std;

bool isPrime(uint64_t n)
{
    uint64_t limit = sqrt(n);
    if(n%2 == 0)
        return false;
    for(uint64_t i=3; i<=limit; i+=2)
    {
        if(n%i == 0)
            return false;
    }

    return true;
}

//count primes without multithreading
uint64_t countPrimes(uint64_t n) 
{
    uint64_t count = 1;
    for(uint64_t i=3; i<=n; i+=2)
    {
        if(isPrime(i))
            ++count;
    }

    return count;
}

//never return anything from thread
void countPrimesMultithread( uint64_t start,  uint64_t end, uint64_t *count)
{
    uint64_t local_count = 0; //do not use the pass by ref "count" variable, it takes more time to be able to write to the memory every time
    //makes the delay worse if you have multiple threads and have to write to the same memory via single bus
    local_count = ((start == 2)? 1:0);
    start |= 1; //if odd add 1, else nop
    for(uint64_t i=start; i<=end; i+=2)
    {
        if(isPrime(i))
            ++local_count;
    }

    *count = local_count;
}

int main(int argc, char* argv[])
{
    uint64_t n = atol(argv[1]);
    uint64_t count1 = 0, count2=0;
    /*
     * make sure there are 2 cores for this computationally intensive task; 
     * if there are < 2, then this will just end up doing concurrent processing and may be 
     * slower because of context switching
     */
    auto start = std::chrono::high_resolution_clock::now();
    thread t1(countPrimesMultithread, 2, n/2, &count1);
    thread t2(countPrimesMultithread, n/2+1, n, &count2); 
    t1.join();
    t2.join(); //wait for threads to finish before executing the below statement
    cout<<"Number of primes between 2 and "<<n<<" is :"<<count1+count2;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout<<"\nTotal time taken by 2 threads: "<<duration_ms.count()<<"ms"<<endl;

    start=std::chrono::high_resolution_clock::now();
    cout<<"Number of primes between 2 and "<<n<<" is :"<<countPrimes(n);
    end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout<<"\nTotal time taken by 1 thread: "<<duration_ms.count()<<"ms"<<endl;

}