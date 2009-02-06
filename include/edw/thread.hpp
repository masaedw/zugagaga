/** \file
 * スレッド関係のテンプレート
 */
#ifndef EDW_THREAD_HPP
#define EDW_THREAD_HPP

namespace edw {
  namespace thread {
    class Mutex
    {
    public:
      virtual ~Mutex() {}
      virtual void Lock() = 0;
      virtual void Unlock() = 0;
    };

    class PT_Mutex : public Mutex
    {
    private:
      void* data;
    public:
      PT_Mutex();
      ~PT_Mutex();
      void Lock();
      void Unlock();
    };

    template <class mut> class TUnlocker;
    template <class mut> class TLocker
    {

      friend class TUnlocker<mut>;
    private:
      mut* a;

    public:
      explicit TLocker( mut* l )
	: a( l ) { if ( a ) a->Lock(); }
      explicit TLocker( mut& l )
	: a( &l ) { if ( a ) a->Lock(); }
      ~TLocker() { if ( a ) a->Unlock(); }
    };

    template <class mut> class TUnlocker
    {
    private:
      mut* a;
    public:
      TUnlocker( TLocker<mut>& l )
	: a( l.a ) { if ( a ) a->Unlock(); }
      ~TUnlocker() { if ( a ) a->Lock(); }
    };

    typedef TLocker<Mutex> Locker;
    typedef TUnlocker<Mutex> Unlocker;

    class Thread : public virtual PT_Mutex
    {
    private:
      bool run;
      bool det;
      bool stop;
      void* data;
      static void* ThreadHandler( void* );

    public:
      Thread();
      virtual ~Thread();
      virtual void Start();
      virtual void Join();
      virtual void Cancel();
      virtual bool IsRunning();
      virtual void Run() = 0;
      virtual void StopRequest()
      {
	Locker l( this );
	stop = true;
      }
      virtual bool StopRequested()
      {
	Locker l( this );
	return stop;
      }
    };
  }
}

#endif // EDW_THREAD_HPP
