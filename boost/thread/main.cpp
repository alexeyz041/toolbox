//
// based on examples from Boost book https://theboostcpplibraries.com/boost.thread-management
//

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>

boost::mutex mutex;
int count = 0;

void wait(int seconds)
{
    boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}



class Obj {
    boost::thread *t;
    int delta;

public:
    Obj(int delta) : delta(delta)
    {
	t = new boost::thread(&Obj::run,this);
    }

    ~Obj()
    {
	if(t) {
	    t->interrupt();
	    t->join();
	    delete t;
	    t = nullptr;
	}
    }


    void run()
    {
	try {
	    for (int i = 0; i < 10; ++i) {
		wait(1);
		std::cout << i << '\n';
		mutex.lock();
		count += delta;
		mutex.unlock();
	    }
	} catch (boost::thread_interrupted&) {
	}
	std::cout << "exiting...\n";
    }

};




int main(int argc,char **argv)
{
    Obj o1(1);
    Obj o2(-1);
    wait(5);
    std::cout << "count = " << count << "\n";
}


