
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

void cpu_bound_task(int id, int num_iterations) 
{
    std::vector<int> data(1000000);
    for (int i = 0; i < num_iterations; ++i) 
    {
        std::transform(data.begin(), data.end(), data.begin(), [](int x) { return x * 2; });
    }
    std::cout << "Task " << id << " completed on core " << sched_getcpu() << std::endl;
}

int main() 
{
    std::vector<std::thread> threads;
    unsigned int num_cores = std::thread::hardware_concurrency();

    for (unsigned int i = 0; i < num_cores; ++i) 
    {
        threads.emplace_back(std::thread([i]() 
        {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i, &cpuset);
            int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            if (rc != 0) 
            {
                std::cerr << "Error calling pthread_setaffinity_np: " << rc << std::endl;
            }
            cpu_bound_task(i, 10000);
        }));
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
