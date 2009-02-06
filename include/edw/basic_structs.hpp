#ifndef EDW_BASIC_STRUCTS_HPP
#define EDW_BASIC_STRUCTS_HPP

#include <stdexcept>
#include <iostream>

namespace edw {
  namespace basic_structs {

    template<class T>
    struct BasicPos
    {
      T x, y;

      BasicPos() : x( 0 ), y( 0 ) {}
      BasicPos( T x, T y ) : x( x ), y( y ) {}
      BasicPos& operator += ( const BasicPos& arg )
      {
	x += arg.x;
	y += arg.y;
	return *this;
      }
      BasicPos& operator -= ( const BasicPos& arg )
      {
	x -= arg.x;
	y -= arg.y;
	return *this;
      }
      template<class S> BasicPos& operator *= ( S arg )
      {
	x *= arg;
	y *= arg;
	return *this;
      }
      template<class S> BasicPos& operator /= ( S arg )
      {
	x /= arg;
	y /= arg;
	return *this;
      }
    };

    template<class Dst, class Src> Dst pos_cast( const Src& src )
    {
      Dst dst;
      dst.x = src.x;
      dst.y = src.y;
      return dst;
    }

    // BasicPos Â­¤·»»
    template<class T>
    BasicPos<T> operator + ( const BasicPos<T>& lhs, const BasicPos<T>& rhs )
    {
      return BasicPos<T>( lhs.x + rhs.x, lhs.y + rhs.y );
    }
    // BasicPos °ú¤­»»
    template<class T>
    BasicPos<T> operator - ( const BasicPos<T>& lhs, const BasicPos<T>& rhs )
    {
      return BasicPos<T>( lhs.x - rhs.x, lhs.y - rhs.y );
    }
    // BasicPos Ã±¹à¥Þ¥¤¥Ê¥¹
    template<class T>
    BasicPos<T> operator - ( const BasicPos<T>& a )
    {
      return BasicPos<T>( -a.x, -a.y );
    }
    // ³Ý¤±»»
    template<class T, class S>
    BasicPos<T> operator * ( const BasicPos<T>& lhs, S rhs )
    {
      return BasicPos<T>( lhs.x * rhs, lhs.y * rhs );
    }
    // ³ä¤ê»»
    template<class T, class S>
    BasicPos<T> operator / ( const BasicPos<T>& lhs, S rhs )
    {
      return BasicPos<T>( lhs.x / rhs, lhs.y / rhs );
    }

    template<class T>
    std::ostream& operator << ( std::ostream& ost, const BasicPos<T>& a )
    {
      return ost << "(" << a.x << ", " << a.y << ")";
    }

    typedef BasicPos<short> Pos;
    typedef BasicPos<long> LPos;
    typedef BasicPos<double> DPos;

    template<class XY, class WH> struct BasicRect
    {
      XY x, y;
      WH w, h;

      BasicRect(){}
      BasicRect( XY x, XY y, WH w, WH h )
	: x( x ), y( y ), w( w ), h( h ) {}

      template<class T> bool Include( const BasicPos<T>& p )
      {
	return x <= p.x && p.x <= x+w && y <= p.y && p.y <= y+h;
      }
    };

    template<class Dst, class Src> Dst rect_cast( const Src& src )
    {
      Dst dst;
      dst.x = src.x;
      dst.y = src.y;
      dst.w = src.w;
      dst.h = src.h;
      return dst;
    }

    typedef BasicRect<signed short, unsigned short> Rect;
    typedef BasicRect<signed long, unsigned long> LRect;
    typedef BasicRect<double, double> DRect;

    template<class XY, class WH>
    std::ostream& operator << ( std::ostream& ost, const BasicRect<XY, WH>& a )
    {
      return ost << "(" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << ")";
    }

    template<class T> bool Overlap( const T& lhs, const T& rhs )
    {
      if ( lhs.x < rhs.x ) {
	int x = lhs.x + lhs.w;
	if ( x <= rhs.x )
	  return false;
      }
      else {
	int x = rhs.x + rhs.w;
	if ( x <= lhs.x )
	  return false;
      }

      if ( lhs.y < rhs.y ) {
	int y = lhs.y + lhs.h;
	if ( y <= rhs.y )
	  return false;
      }
      else {
	int y = rhs.y + rhs.h;
	if ( y <= lhs.y )
	  return false;
      }

      return true;
    }

    template<class T> T operator & ( const T& lhs, const T& rhs )
    {
      T temp;

      if ( lhs.x < rhs.x ) {
	int x = lhs.x + lhs.w;
	if ( x <= rhs.x )
	  throw std::logic_error( "rects do not overlap." );
	else {
	  temp.x = rhs.x;
	  temp.w = ( x - rhs.x > rhs.w ) ? rhs.w : x - rhs.x;
	}
      }
      else {
	int x = rhs.x + rhs.w;
	if ( x <= lhs.x )
	  throw std::logic_error( "rects do not overlap." );
	else {
	  temp.x = lhs.x;
	  temp.w = ( x - lhs.x > lhs.w ) ? lhs.w : x - lhs.x;
	}
      }

      if ( lhs.y < rhs.y ) {
	int y = lhs.y + lhs.h;
	if ( y <= rhs.y )
	  throw std::logic_error( "rects do not overlap." );
	else {
	  temp.y = rhs.y;
	  temp.h = ( y - rhs.y > rhs.h ) ? rhs.h : y - rhs.y;
	}
      }
      else {
	int y = rhs.y + rhs.h;
	if ( y <= lhs.y )
	  throw std::logic_error( "rects do not overlap." );
	else {
	  temp.y = lhs.y;
	  temp.h = ( y - lhs.y > lhs.h ) ? lhs.h : y - lhs.y;
	}
      }

      return temp;
    }
  }
}

#endif // EDW_BASIC_STRUCTS_HPP
