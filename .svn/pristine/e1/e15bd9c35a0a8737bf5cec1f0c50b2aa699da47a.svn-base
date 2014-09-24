/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : 绾跨▼绠＄悊瀵硅薄锛岄儴鍒嗗弬鑰僡pache绾跨▼姹犲疄鐜帮紝鍏朵腑锛宎pache瀵圭嚎绋嬪璞＄鐞嗕娇鐢╞oost涓櫤鑳芥寚閽堬紝
 *         杩欓噷浣跨敤鑷繁鐨勫紩鐢ㄨ鏁板璞℃潵杩涜寮曠敤绠＄悊锛屽浜庣嚎绋嬪璞′娇鐢ㄩ渶瑕佺户鎵�Runnable鎺ュ彛瀹炵幇run鎺ュ彛锛� *         鍏朵腑锛宲aram鏄嚎绋嬩釜鎬у寲鍙傛暟锛屼富瑕侀拡瀵瑰悓涓�釜瀵硅薄浣跨敤Runnable鏉ュ疄鐜板缁勭嚎绋嬫睜瀹屾垚涓嶅悓鍔熻兘鎯呭喌灏卞彲浠ラ�杩囦釜鎬у寲鍙傛暟鏉ュ尯鍒� */

#ifndef __TRIONES_THREAD_H__
#define __TRIONES_THREAD_H__

#include <list>
#include <pthread.h>
#include "ref.h"

namespace triones
{

// 绾跨▼杩愯瀵硅薄鎺ュ彛
class Runnable
{
public:
	virtual ~Runnable()
	{
	}

	virtual void run(void *param) = 0;
};


//对linux线程的简单封装，用在非线程池的情况下
class TBThread {

public:
    /**
     * 构造函数
     */
	TBThread() {
        tid = 0;
        pid = 0;
    }

    /**
     * 起一个线程，开始运行
     */
    bool start(Runnable *r, void *a) {
        runnable = r;
        args = a;
        return 0 == pthread_create(&tid, NULL, CThread::hook, this);
    }

    /**
     * 等待线程退出
     */
    void join() {
        if (tid) {
            pthread_join(tid, NULL);
            tid = 0;
            pid = 0;
        }
    }

    /**
     * 得到Runnable对象
     *
     * @return Runnable
     */
    Runnable *getRunnable() {
        return runnable;
    }

    /**
     * 得到回调参数
     *
     * @return args
     */
    void *getArgs() {
        return args;
    }

    /***
     * 得到线程的进程ID
     */
    int getpid() {
        return pid;
    }

    /**
     * 线程的回调函数
     *
     */

    static void *hook(void *arg) {
        CThread *thread = (CThread*) arg;
        thread->pid = gettid();

        if (thread->getRunnable()) {
            thread->getRunnable()->run(thread, thread->getArgs());
        }

        return (void*) NULL;
    }

private:
    /**
     * 得到tid号
     */
    #ifdef _syscall0
    static _syscall0(pid_t,gettid)
    #else
    static pid_t gettid() { return static_cast<pid_t>(syscall(__NR_gettid));}
    #endif

private:
    pthread_t tid;      // pthread_self() id
    int pid;            // 线程的进程ID
    Runnable *runnable;
    void *args;
};

// 绾跨▼澶勭悊瀵硅薄
class Thread : public Ref
{

	// POSIX Thread scheduler policies
	enum POLICY
	{
		OTHER,
		FIFO,
		ROUND_ROBIN
	};

	// POSIX Thread scheduler relative priorities,
	//
	// Absolute priority is determined by scheduler policy and OS. This
	// enumeration specifies relative priorities such that one can specify a
	// priority withing a giving scheduler policy without knowing the absolute
	// value of the priority.
	enum PRIORITY
	{
		LOWEST = 0,
		LOWER = 1,
		LOW = 2,
		NORMAL = 3,
		HIGH = 4,
		HIGHER = 5,
		HIGHEST = 6,
		INCREMENT = 7,
		DECREMENT = 8
	};

	enum STATE
	{
		uninitialized,
		starting,
		started,
		stopping,
		stopped
	};

	static const int MB = 1024 * 1024;

public:
	// 绾跨▼鏋勯�瀵硅薄
	Thread( Runnable *runner , void *param = NULL ,  int policy = FIFO , int priority = NORMAL , int stack_size = 8 , bool detached = false ) ;

	virtual ~Thread() ;

	/**
	 * Starts the thread. Does platform specific thread creation and
	 * configuration then invokes the run method of the Runnable object bound
	 * to this thread.
	 */
	virtual void start(void);

	/**
	 * Join this thread. Current thread blocks until this target thread
	 * completes.
	 */
	virtual void join();

	/**
	 * Gets the thread's platform-specific ID
	 */
	virtual pthread_t id()
	{
		return _pthread;
	}

public:
	/**
	 * 鎵ц绾跨▼鐨勪富鍑芥暟
	 */
	static void * ThreadMain(void *param);

	/**
	 * 鍙栧緱杩愯瀵硅薄
	 */
	Runnable * runable(void)
	{
		return _runner;
	}

private:
	/**
	 *  绾跨▼杩愯瀵硅薄
	 */
	Runnable *_runner;

	/**
	 *  绾跨▼ID
	 */
	pthread_t _pthread;

	/**
	 *  甯﹁繃鐨勫弬鏁�	 */
	void * _param;

	/**
	 * 绾跨▼鐘舵�
	 */
	STATE _state;

	/**
	 * POSIX Thread scheduler policies
	 */
	int _policy;

	/**
	 *  绾跨▼浼樺厛绾�	 */
	int _priority;

	/**
	 * 鏍堢┖闂村ぇ灏�	 */
	int _stackSize;

	/**
	 * 绫诲瀷
	 */
	bool _detached;

	/**
	 * 绾跨▼瀵硅薄
	 */
	Thread *_selfRef;
};

/**
 * 绾跨▼绠＄悊瀵硅薄
 */
class ThreadManager
{
public:
	ThreadManager() :
			_thread_state(false)
	{
	}

	virtual ~ThreadManager();
	/**
	 *  鍒濆鍖栫嚎绋嬪璞�	 */
	virtual bool init(unsigned int nthread, void *param, Runnable *runner);

	/**
	 *  寮�杩愯绾跨▼
	 */
	virtual void start(void);

	/**
	 *  鍋滄绾跨▼
	 */
	virtual void stop(void);

private:
	/**
	 * 绾跨▼瀵硅薄鍒楄〃
	 */
	typedef std::list<Thread*> ThreadList;

	/**
	 * 绾跨▼瀛樻斁瀵硅薄
	 */
	ThreadList _thread_lst;

	/**
	 * 绾跨▼姹犵殑鐘舵�
	 */
	bool _thread_state;
};

} // namespace triones

#endif // #ifndef __TRIONES_THREAD_H__
