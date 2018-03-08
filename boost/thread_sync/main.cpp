
#include <time.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

boost::mutex mtx;
bool done1 = false;
bool done2 = false;
boost::condition_variable cond;


void worker1()
{
    try {
	int delay = rand() % 10;
	std::cout << "delay1 = " << delay << "\n";
	boost::this_thread::sleep(boost::posix_time::seconds(delay));
    } catch (boost::thread_interrupted&) {
	std::cout << "interrupted 1...\n";
	return;
    }

    {
        boost::mutex::scoped_lock lock(mtx);
        done1 = true;
    }
    cond.notify_one();
    std::cout << "exiting 1...\n";
}


void worker2()
{
    try {
	int delay = rand() % 10;
	std::cout << "delay2 = " << delay << "\n";
	boost::this_thread::sleep(boost::posix_time::seconds(delay));
    } catch (boost::thread_interrupted&) {
	std::cout << "interrupted 2...\n";
	return;
    }

    {
        boost::mutex::scoped_lock lock(mtx);
        done2 = true;
    }
    cond.notify_one();
    std::cout << "exiting 2...\n";
}


int main(int argc,char *argv[])
{
    srand(time(NULL));
    boost::mutex::scoped_lock lock(mtx);
    boost::thread thread1(&worker1);
    boost::thread thread2(&worker2);

    while(!done1 && !done2) {
	cond.wait(lock);
    }
    if(!done1) {
	thread1.interrupt();
    }
    if(!done2) {
	thread2.interrupt();
    }

    thread1.join();
    thread2.join();
    std::cout << "exiting main\n";
    return 0;
}
