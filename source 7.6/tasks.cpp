//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////
#include "otpch.h"

#include "tasks.h"
#include "outputmessage.h"
#include "game.h"

extern Game g_game;

#if defined __EXCEPTION_TRACER__
#include "exception.h"
#endif

Dispatcher::DispatcherState Dispatcher::m_threadState = Dispatcher::STATE_TERMINATED;

Dispatcher::Dispatcher()
{
	m_taskList.clear();
	Dispatcher::m_threadState = Dispatcher::STATE_RUNNING;
	boost::thread(boost::bind(&Dispatcher::dispatcherThread, (void*)NULL));
}

void Dispatcher::dispatcherThread(void* p)
{
	#if defined __EXCEPTION_TRACER__
	ExceptionHandler dispatcherExceptionHandler;
	dispatcherExceptionHandler.InstallHandler();
	#endif
	srand((unsigned int)OTSYS_TIME());
	#ifdef __DEBUG_SCHEDULER__
	std::cout << "Starting Dispatcher" << std::endl;
	#endif

	OutputMessagePool* outputPool;

	// NOTE: second argument defer_lock is to prevent from immediate locking
	boost::unique_lock<boost::mutex> taskLockUnique(getDispatcher().m_taskLock, boost::defer_lock);

	while(Dispatcher::m_threadState != Dispatcher::STATE_TERMINATED){
		Task* task = NULL;

		// check if there are tasks waiting
		taskLockUnique.lock();//getDispatcher().m_taskLock.lock();

		if(getDispatcher().m_taskList.empty()){
			//if the list is empty wait for signal
			#ifdef __DEBUG_SCHEDULER__
			std::cout << "Dispatcher: Waiting for task" << std::endl;
			#endif
			getDispatcher().m_taskSignal.wait(taskLockUnique);
		}

		#ifdef __DEBUG_SCHEDULER__
		std::cout << "Dispatcher: Signalled" << std::endl;
		#endif

		if(!getDispatcher().m_taskList.empty() && (Dispatcher::m_threadState != Dispatcher::STATE_TERMINATED)){
			// take the first task
			task = getDispatcher().m_taskList.front();
			getDispatcher().m_taskList.pop_front();
		}

		taskLockUnique.unlock();

		// finally execute the task...
		if(task){
			OutputMessagePool::getInstance()->startExecutionFrame();
			(*task)();
			delete task;
			
			outputPool = OutputMessagePool::getInstance();
			if(outputPool){
				outputPool->sendAll();
			}

			g_game.clearSpectatorCache();
			#ifdef __DEBUG_SCHEDULER__
			std::cout << "Dispatcher: Executing task" << std::endl;
			#endif
		}
	}
#if defined __EXCEPTION_TRACER__
	dispatcherExceptionHandler.RemoveHandler();
#endif
}

void Dispatcher::addTask(Task* task, bool push_front /*= false*/)
{
	bool do_signal = false;
	m_taskLock.lock();
	if(Dispatcher::m_threadState == Dispatcher::STATE_RUNNING){
		do_signal = m_taskList.empty();
		if(push_front){
			m_taskList.push_front(task);
		}
		else {
			m_taskList.push_back(task);
		}

		#ifdef __DEBUG_SCHEDULER__
		std::cout << "Dispatcher: Added task" << std::endl;
		#endif
	}
#ifdef __DEBUG_SCHEDULER__
	else{
		std::cout << "Error: [Dispatcher::addTask] Dispatcher thread is terminated." << std::endl;
	}
#endif

	m_taskLock.unlock();

	// send a signal if the list was empty
	if(do_signal){
		m_taskSignal.notify_one();
	}
}

void Dispatcher::flush()
{
	Task* task = NULL;
	while(!m_taskList.empty()){
		task = getDispatcher().m_taskList.front();
		m_taskList.pop_front();
		(*task)();
		delete task;
		OutputMessagePool::getInstance()->sendAll();
		g_game.clearSpectatorCache();
	}
	#ifdef __DEBUG_SCHEDULER__
	std::cout << "Flushing Dispatcher" << std::endl;
	#endif
}

void Dispatcher::stop()
{
	m_taskLock.lock();
	m_threadState = Dispatcher::STATE_CLOSING;
	m_taskLock.unlock();
	#ifdef __DEBUG_SCHEDULER__
	std::cout << "Stopping Dispatcher" << std::endl;
	#endif
}

void Dispatcher::shutdown()
{
	m_taskLock.lock();
	m_threadState = Dispatcher::STATE_TERMINATED;
	flush();
	m_taskLock.unlock();
	#ifdef __DEBUG_SCHEDULER__
	std::cout << "Shutdown Dispatcher" << std::endl;
	#endif
}
