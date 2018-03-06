
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

boost::mutex mtx;
bool done = false;
boost::condition_variable cond;


void worker()
{
    std::cout << "Waiting a little" << std::endl;
    boost::this_thread::sleep(boost::posix_time::seconds(1));

    {
        boost::mutex::scoped_lock lock(mtx);
        done = true;
    }

    std::cout << "Notifying condition" << std::endl;
    cond.notify_one();

    std::cout << "Waiting a little more" << std::endl;
    boost::this_thread::sleep(boost::posix_time::seconds(1));
}


int main(int argc,char *argv[])
{
    boost::mutex::scoped_lock lock(mtx);
    boost::thread workThread(&worker);

    while(!done) {
	std::cout << ".\n";
	cond.wait(lock);
    }
    std::cout << "Condition notified" << std::endl;

    workThread.join();
    std::cout << "Thread finished" << std::endl;
    return 0;
}
