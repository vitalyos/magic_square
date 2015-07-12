
#include "WorkerThread.hpp"
#include <iostream>

int main(void)
{
    measureTime(&tStart);
    WorkerThread<4> w(0, 4);
    std::thread t([](WorkerThread<4> thre){
        thre.solve();
    }, w);
    t.join();

    measureTime(&tStop);
    timeDiff = (unsigned long)((tStop - tStart)) / MILI;
    writeNrOfResults();

    std::cout << w.numberOfSolutions() << std::endl;
    return 0;
}
