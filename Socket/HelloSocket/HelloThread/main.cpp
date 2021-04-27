#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#define N 3
using namespace std;

mutex m;
atomic_int sum = 0;
void DealFun(int index)
{
    for (int i = 0; i < 100000; ++i)
    {
        // lock_guard<mutex> lg(m);
        ++sum;
    }
}

int main()
{
    thread t[N];

    for (int i = 0; i < N; ++i)
    {
        t[i] = thread(DealFun, i);
    }
    
    for (int i = 0; i < N; ++i)
    {
        //t[i].detach();
        t[i].join();
    }

    cout << sum << endl;
    return 0;
}