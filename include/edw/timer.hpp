// last modified <2004/01/22 17:08:26 2004 JST>
#ifndef EDW_TIMER_HPP
#define EDW_TIMER_HPP

namespace edw {

  /**
   * タイマー whileのまわる時間を一定以上早くならないようにする
   *
   * ポリシークラスをテンプレート引数にとり、
   * 時間取得と停止の方法を決める。
   *
   * ポリシークラスに必要なもの
   * - T::time_t 時間の型 ミリ秒
   * - T::time_t GetTicks() 時間取得
   * - T::time_t Delay(T::time_t) 指定時間だけ待つ
   *
   * Wait() の返値は fps 。一秒に一回だけ値を返す。
   */
  template <class T> class Timer
  {
  public:
    void SetFPS( unsigned int in )
    {
      interval = 1000 / in;
    }
    void SetInterval( double in )
    {
      interval = (unsigned long)( in * 1000 );
    }
    bool Start()
    {
      stime = impl.GetTicks();
      count++;
      return true;
    }
    double Wait()
    {
      typename T::time_t ntime = impl.GetTicks();
      if ( interval > ntime - stime )
	impl.Delay( interval - ( ntime - stime ) );
      double fps = 0;
      if ( ntime - fpstime > 1000 ) {
	fps = count * 1000.0 / ( ntime - fpstime );
	count = 0;
	fpstime = ntime;
      }
      return fps;
    }
  private:
    typename T::time_t interval;
    typename T::time_t stime;
    typename T::time_t fpstime;
    unsigned int count;
    T impl;
  public:
    Timer() {}
    ~Timer() {}
  };

}

#endif // EDW_TIMER_HPP
