#include "WorkerThread.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <list>

constexpr int dim = 4;

#ifdef MULTI
constexpr int threadCount = 6;
std::list<int> bounds = {3, 6, 8, 10, 13, 16};
#else
constexpr int threadCount = 1;
std::list<int> bounds = {16};
#endif // MULTI
int main(void)
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
        threads.push_back(WorkerThread<dim>::work(worker));
    }

    for (auto & thread : threads)
    {
        thread.join ();
    }

    auto end = std::chrono::steady_clock::now();

    int nr_solutions = 0;
    for (const auto & worker : workers)
    {
        nr_solutions += worker.numberOfSolutions ();
    }


    std::cout << "duration: " << std::chrono::duration<double, std::milli> (end - start).count () << "ms" << std::endl;
    std::cout << "solutions: " << nr_solutions << std::endl;
    return 0;
}
