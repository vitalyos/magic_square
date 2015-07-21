#include "WorkerThread.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <list>
#include <algorithm>

constexpr int dim = 4;

#ifdef MULTI
constexpr int threadCount = 8;
std::list<int> bounds = {2 ,4, 6, 8, 10, 12, 14, 16};
//constexpr int threadCount = 6;
//std::list<int> bounds = {3 ,6, 9, 11, 13, 16};
//constexpr int threadCount = 4;
//std::list<int> bounds = {4, 8, 12, 16};
#else
constexpr int threadCount = 1;
std::list<int> bounds = {16};
#endif // MULTI

constexpr int experiment_count = 100;

int main(void)
{
    int nr_solutions = 0;
    double total_duration = 0.0;
    double min_duration = 0.0;
    double max_duration = 0.0;

    for (int i = 0; i < experiment_count; ++i)
    {
        std::vector<WorkerThread<dim>> workers;
        int prev = 0;
        for (const auto & bound : bounds)
        {
            workers.push_back(WorkerThread<dim>(prev, bound));
            prev = bound;
        }

        auto start = std::chrono::steady_clock::now();

        std::vector<std::thread> threads;
        for (auto & worker : workers)
        {
            threads.push_back(WorkerThread<dim>::work(&worker));
        }

        for (auto & thread : threads)
        {
            thread.join ();
        }

        auto end = std::chrono::steady_clock::now();

        if (i == 0)
        {
            for (const auto & worker : workers)
            {
                nr_solutions += worker.numberOfSolutions ();
            }
        }

        auto duration = std::chrono::duration<double, std::milli> (end - start).count ();
        min_duration = i == 0 ? duration : std::min(min_duration, duration);
        max_duration = i == 0 ? duration : std::max(max_duration, duration);
        total_duration += duration;
    }
    std::cout << "min duration: " << min_duration << std::endl;
    std::cout << "max duration: " << max_duration << std::endl;
    std::cout << "average duration: " << total_duration / experiment_count << " ms" << std::endl;
    std::cout << "solutions: " << nr_solutions << std::endl;
    return 0;
}
