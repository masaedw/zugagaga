#ifndef EDW_ALGORITHM_HPP
#define EDW_ALGORITHM_HPP

#include <algorithm>

namespace edw {

  template <template <class S> class C, class T>
  C<T>& for_each( C<T>& l, void (T::* func)() )
  {
    for ( typename C<T>::iterator i = l.begin();
	  i != l.end(); ++i ) {
      ((*i).*func)();
    }
    return l;
  }

  template <template <class S> class C, class T>
  C<T>& for_each( C<T>& l, void (T::* func)() const )
  {
    for ( typename C<T>::const_iterator i = l.begin();
	  i != l.end(); ++i ) {
      ((*i).*func)();
    }
    return l;
  }

  template <template <class S> class C, class T>
  C<T*>& for_each( C<T*>& l, void (T::* func)() )
  {
    for ( typename C<T*>::iterator i = l.begin();
	  i != l.end(); ++i ) {
      ((**i).*func)();
    }
    return l;
  }

  template <template <class S> class C, class T>
  C<T*>& for_each( C<T*>& l, void (T::* func)() const )
  {
    for ( typename C<T*>::const_iterator i = l.begin();
	  i != l.end(); ++i ) {
      ((**i).*func)();
    }
    return l;
  }

  template <template <class S> class C, class T>
  C<const T*>& for_each( C<const T*>& l, void (T::* func)() const )
  {
    for ( typename C<const T*>::const_iterator i = l.begin();
	  i != l.end(); ++i ) {
      ((**i).*func)();
    }
    return l;
  }

  template <class T, class F> T& for_each( T& t, const F& f )
  {
    std::for_each( t.begin(), t.end(), f );
    return t;
  }
}

#endif // EDW_ALGORITHM_HPP
