// last modified <2004/01/22 17:08:26 2004 JST>
#ifndef EDW_TIMER_HPP
#define EDW_TIMER_HPP

namespace edw {

  /**
   * �����ޡ� while�Τޤ����֤����ʾ��᤯�ʤ�ʤ��褦�ˤ���
   *
   * �ݥꥷ�����饹��ƥ�ץ졼�Ȱ����ˤȤꡢ
   * ���ּ�������ߤ���ˡ����롣
   *
   * �ݥꥷ�����饹��ɬ�פʤ��
   * - T::time_t ���֤η� �ߥ���
   * - T::time_t GetTicks() ���ּ���
   * - T::time_t Delay(T::time_t) ������֤����Ԥ�
   *
   * Wait() �����ͤ� fps �����ä˰������ͤ��֤���
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
