/*
 * Problem Statement:
 * Write a multithreaded dictionary (2-4 threads) that splits the 110 books in
 * https://github.com/RU-ECE/ECE451-Parallel/tree/main/data/books. Assuming 4 threads,
 * each thread should read in 1/4 of the books and build a dictionary showing the number of times the word appears.
 * At the end, you will have 4 dictionaries in parallel.Merge the 4 dictonaries. Output should be:
 * 1. The number of words (after throwing out words with only 1 occurrence)
 * 2. Output the top k=300 words(sort the words by frequency, and print the top 300 most-frequent words)
 * Output completion timings on Linux:
 * single thread: 3600 ms, 2 threads: 2300 ms, 4 threads: 1700 ms
 * The program was running on VirtualBox with 4 GB base memory and 4 cores(including logical cores) 
 */
#include <iostream>
#include <cmath>
#include <cstdint>
#include <thread>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <atomic>
#include <unordered_map>
#include <sstream>
#include <stack>
#include <mutex>
#include <queue>

using namespace std;

namespace fs = std::filesystem;
atomic<uint32_t> book_num(0); 
mutex mtx;

void threadReadsBook(int thread_num, unordered_map<string, uint64_t>& word_count, vector<std::string> fileNames)
{
    while(true)
    {
        std::vector<std::string> fileContents;
        string local_file = "";
        mtx.lock();
        uint32_t temp = book_num.load();
        if(temp >= fileNames.size())
        {
            mtx.unlock();
            return; //no more files to process
        }
        local_file = fileNames[temp];
        book_num.store(temp+1);//let the thread process next book in the next iteration
        mtx.unlock();
        
        std::ifstream file(local_file); 
        //cout<<"\n\nThread "<<thread_num<<" processing file "<<local_file<<endl;
        if (file.is_open())
        {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            fileContents.push_back(content);
            file.close();

            for(int i=0; i<fileContents.size(); ++i)
            {
                stringstream ss(fileContents[i]);
                string word;
                while(ss >> word)
                    word_count[word]++;
            }
        }
        else 
        {
            std::cerr << "Failed to open file: " << local_file << std::endl;
        }
    }
}

/*
 * merge dictionary 2,3,4 to 1 */
void mergeDicts(unordered_map<string, uint64_t>& word_count1, 
                unordered_map<string, uint64_t>& word_count2, 
                unordered_map<string, uint64_t>& word_count3, 
                unordered_map<string, uint64_t>& word_count4)
{
    
    for(auto it = word_count2.begin(); it != word_count2.end(); ++it)
        word_count1[it->first] += it->second;
    word_count2.clear();

    for(auto it = word_count3.begin(); it != word_count3.end(); ++it)
        word_count1[it->first]+= it->second;
    word_count3.clear();

    for(auto it = word_count4.begin(); it != word_count4.end(); ++it)
        word_count1[it->first]+= it->second;
    word_count4.clear();
}

void printTopKMostUsedWords(unordered_map<string, uint64_t> &word_count, const uint16_t topK)
{
    priority_queue<pair<int, string>> pq;
    for(auto it = word_count.begin(); it != word_count.end(); ++it)
    {
        if(pq.size() < topK)
        {
            pq.push(make_pair(it->second*-1, it->first));//multiply by -1 to make it min heap
        }
        else
        {
            uint32_t curr_least_freq = pq.top().first*-1;//make it positive
            if(it->second > curr_least_freq)
            {
                pq.pop();//remove the least frequently used from the PQ
                pq.push(make_pair(it->second*-1, it->first));
            }
        }
    }
    
    cout<<topK<<" most used words in the text files are: \n";
    uint16_t i = 0;
    uint64_t max = 0;
    while(!pq.empty())
    {
        pair<int, string> top_ele = pq.top();
        cout<<++i<<"] "<<top_ele.second<<" "<<top_ele.first*-1<<"\n";
        max = top_ele.first*-1;
        pq.pop();
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::string folderPath = "books";
    vector<std::string> fileNames;

    // Read all file names from the folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            fileNames.push_back(entry.path().string());
        }
    }

    unordered_map<string, uint64_t> word_count1, word_count2, word_count3, word_count4;

    thread t1(threadReadsBook, 1, std::ref(word_count1), fileNames);
    thread t2(threadReadsBook, 2, std::ref(word_count2), fileNames);
    thread t3(threadReadsBook, 3, std::ref(word_count3), fileNames);
    thread t4(threadReadsBook, 4, std::ref(word_count4), fileNames);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    //threadReadsBook( 1, std::ref(word_count1), fileNames);
    //add all words to 1st dictionary; alternatively, I can find the biggest and add to that instead
    cout<<"Finished parsing words in all files\n";
    mergeDicts(word_count1, word_count2, word_count3, word_count4);
    cout<<"\nMerged all 4 dictionaries into one main dictionary!\n";

    uint64_t total_num_words = 0;
    auto it = word_count1.begin();
    while( it != word_count1.end())
    {
        if(it->second == 1)
            it = word_count1.erase(it);
        else
        {
            total_num_words += it->second;
            ++it;
        }
    }
    cout<<"Erased single entries from the main dictionary and found total words:"<<total_num_words<<"\n";
    /*
     * Finding top most used 300 words in the dictionary
     */
    const uint32_t topK = 300;
    printTopKMostUsedWords(word_count1, topK);
    cout<<"ALL DONE 4 threads!\n";
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    cout<<"Time taken: "<<duration.count()<<" ms"<<endl;
    return 0;
}
