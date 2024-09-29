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

using namespace std;

namespace fs = std::filesystem;
mutex mtx;

void threadReadsBook(int thread_num, unordered_map<string, uint64_t>& word_count, stack<std::string>& fileNames)
{
    while(true)
    {
        std::vector<std::string> fileContents;
        string local_file = "";
        mtx.lock();
        if (!fileNames.empty())
        {
            local_file = fileNames.top();
            fileNames.pop();
        }
        else
        {
            mtx.unlock();
            return; //no more files to process
        }
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

int main() {
    std::string folderPath = "C:\\Users\\Anusha\\repos\\parallel_distributed_prog\\data\\books";
    std::stack<std::string> fileNames;
    
    // Read all file names from the folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            fileNames.push(entry.path().string());
        }
    }
    
    unordered_map<string, uint64_t> word_count1;
    unordered_map<string, uint64_t> word_count2;
    unordered_map<string, uint64_t> word_count3;
    unordered_map<string, uint64_t> word_count4;

    thread t1(threadReadsBook, 1, std::ref(word_count1), std::ref(fileNames));
    thread t2(threadReadsBook, 2, std::ref(word_count2), std::ref(fileNames));
    thread t3(threadReadsBook, 3, std::ref(word_count3), std::ref(fileNames));
    thread t4(threadReadsBook, 4, std::ref(word_count4), std::ref(fileNames));
    
    t1.join(); 
    t2.join(); 
    t3.join(); 
    t4.join();

    uint64_t total_num_words = 0;
    for(auto it = word_count1.begin(); it != word_count1.end(); ++it)
        total_num_words += it->second;
    for(auto it = word_count2.begin(); it != word_count2.end(); ++it)
        total_num_words += it->second;
    for(auto it = word_count3.begin(); it != word_count3.end(); ++it)
        total_num_words += it->second;
    for(auto it = word_count4.begin(); it != word_count4.end(); ++it)
        total_num_words += it->second;

    cout<<"Total words:"<<total_num_words<<endl;

    return 0;
}
