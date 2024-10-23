#include <omp.h>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;

/*
Compile as: g++ -g -o2 -fopenmp -mavx demo_omp.cpp -o a.exe
Output of the program for matrix multiplication of N*N where N = 1024:
Number of threads:8 //on my system, and includes hyperthreading
Total time taken: 4376ms 
Total time taken by openmp 2 threads: 2195ms
Total time taken by openmp 4 threads: 1450ms
Total time taken by openmp 6 threads: 1286ms
Total time taken by openmp 8 threads: 1258ms
Total time taken by openmp SIMD: 1204ms
With transpose---------------//transpose of B matrix to make use of cache hits and burst modes
Total time taken: 2575ms
Total time taken by openmp 2 threads: 1349ms
Total time taken by openmp 4 threads: 872ms
Total time taken by openmp 6 threads: 761ms
Total time taken by openmp 8 threads: 695ms
Total time taken by openmp SIMD: 672ms
*/
void printmatrix(const float a[], const float b[], const float c[], const int n)
{
    cout<<"a:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
             cout << a[i * n + j] << " ";
        }
        cout<<"\n";
    }

    cout<<"b:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout<<b[i * n + j]<<" ";
        }
        cout<<"\n";
    }
    
    cout<<"c:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout<<c[i * n + j]<<" ";
        }
        cout<<"\n";
    }

}

// inner dot product uses registers, still goes in
// non-sequential direction in memory
void multiplymatrix(const float a[], const float b[], float c[], const int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float dot = 0.0;
            for (int k = 0; k < n; k++) {
                dot += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = dot;
        }
    }
    //printmatrix(a, b, c, n);
}

// Function to multiply matrices a and bT, storing the result in c
void matrixMultiplyT(const float* a, const float* bT, float* c, const int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < n; ++k) {
                sum += a[i * n + k] * bT[j * n + k];  // Using transposed bT
            }
            c[i * n + j] = sum;
        }
    }
    //printmatrix(a, bT, c, n);
}

// multithreading with OpenMP
void multiplymatrix_openmp(const float a[], const float b[], float c[], const int n) {

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float dot = 0.0;
            for (int k = 0; k < n; k++) {
                dot += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = dot;
        }
    }
    //printmatrix(a, b, c, n);
}

//transposed matrix multiplication with openmp
void matrixMultiplyT_openmp(const float* a, const float* bT, float* c, const int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < n; ++k) {
                sum += a[i * n + k] * bT[j * n + k];  // Using transposed bT
            }
            c[i * n + j] = sum;
        }
    }
    //printmatrix(a, bT, c, n);
}

// multithreading with OpenMP
void multiplymatrix_openmp_simd(const float a[], const float b[], float c[], const int n) {

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float dot = 0.0;
            #pragma omp simd
            for (int k = 0; k < n; k++) {
                dot += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = dot;
        }
    }

    //printmatrix(a, b, c, n);
}

//transposed matrix multiplication with openmp and simd avx
void matrixMultiplyT_openmp_simd(const float* a, const float* bT, float* c, const int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            #pragma omp simd
            for (int k = 0; k < n; ++k) {
                sum += a[i * n + k] * bT[j * n + k];  // Using transposed bT
            }
            c[i * n + j] = sum;
        }
    }
    //printmatrix(a, bT, c, n);
}

// Function to transpose matrix b into bT
void transpose(const float *b, float *bT, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            bT[j * n + i] = b[i * n + j];  // Transpose element
        }
    }
}


int main() {
    int num_threads = omp_get_num_procs();
    cout<<"Number of threads:"<<num_threads<<endl;
    omp_set_num_threads(num_threads);

    int n = 1024;//build a matrix n*n dimensions
    float* a = new float[n*n];
    float* b = new float[n*n];
    float* c = new float[n*n];
    float *copy_c = new float[n*n];
    
    //set the matrices to random values
    for (int i = 0; i < n*n; i++) {
        a[i] = 1;
        b[i] = 1;
        c[i] = 0;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    multiplymatrix(a, b, c, n);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "Total time taken: " << duration_ms.count() << "ms" << endl;
    copy_c = c;

    for(int i=2; i <= num_threads; i+=2)
    {
        omp_set_num_threads(i);
        start_time = std::chrono::high_resolution_clock::now();
        multiplymatrix_openmp(a, b, c, n);
        end_time = std::chrono::high_resolution_clock::now();
        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        cout << "Total time taken by openmp "<< i <<" threads: " << duration_ms.count() << "ms" << endl;
        if(c != copy_c)
            cout<<"OpenMp has different C matrix! \n";
    }

    start_time = std::chrono::high_resolution_clock::now();
    multiplymatrix_openmp_simd(a, b, c, n);
    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "Total time taken by openmp SIMD: " << duration_ms.count() << "ms" << endl;
    if(c != copy_c)
            cout<<"OpenMp SIMD has different C matrix! \n";

    cout<<"With transpose--------------\n";
    float *bT = b;
    transpose(b, bT, n);
    start_time = std::chrono::high_resolution_clock::now();
    matrixMultiplyT(a, bT, c, n);
    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "Total time taken: " << duration_ms.count() << "ms" << endl;
    if(c != copy_c)
        cout<<"Transpose has different C matrix! \n";

    for(int i=2; i <= num_threads; i+=2)
    {
        omp_set_num_threads(i);
        start_time = std::chrono::high_resolution_clock::now();
        matrixMultiplyT_openmp(a, bT, c, n);
        end_time = std::chrono::high_resolution_clock::now();
        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        cout << "Total time taken by openmp "<< i <<" threads: " << duration_ms.count() << "ms" << endl;
        if(c != copy_c)
            cout<<"Transpose OpenMP has different C matrix! \n";
    }

    start_time = std::chrono::high_resolution_clock::now();
    matrixMultiplyT_openmp_simd(a, bT, c, n);
    end_time = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "Total time taken by openmp SIMD: " << duration_ms.count() << "ms" << endl;
    if(c != copy_c)
        cout<<"Transpose OpenMP SIMD has different C matrix! \n";

    delete[] a;
    delete[] b;
    delete[] c;
    return 0;
}