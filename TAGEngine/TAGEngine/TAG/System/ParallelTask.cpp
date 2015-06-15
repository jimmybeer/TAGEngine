#include "ParallelTask.hpp"

ParallelTask::ParallelTask()
 : mThread(&ParallelTask::runTask, this)
 , mFinished(false)
{}

void ParallelTask::execute()
{
    mFinished = false;
	mElapsedTime.restart();
	mThread.launch();
}

bool ParallelTask::isFinished()
{
    sf::Lock lock(mMutex);
	return mFinished;
}

float ParallelTask::getCompletion()
{
    sf::Lock lock(mMutex);
	
	// 100% at 5 seconds of elapsed time
	return mElapsedTime.getElapsedTime().asSeconds() / 5.f;
}

void ParallelTask::runTask()
{
    // Dummy task - stall 5 seconds.
	bool ended = false;
	while(!ended)
	{
	    sf::Lock lock(mMutex); // protected the clock
		if(mElapsedTime.getElapsedTime().asSeconds() >= 5.f)
		    ended = true;
	}
	
	{ // mFinished may be accessed from multiple threads, protected it
	    sf::Lock lock(mMutex);
		mFinished = true;
	} // These braces ensure the lock goes out of scope as soon as possible and the lock is released.
}