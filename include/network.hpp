/** @file
 * ネットワーク接続
 */
// last modified <2004/03/02 21:58:41 2004 JST>
#ifndef ZUGAGAGA_NETWORK_1_HPP
#define ZUGAGAGA_NETWORK_1_HPP

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <SDL_net.h>
#include "comtypes.hpp"

namespace zu3ga {
  namespace network {
    class Error : public Zu3gaError
    {
    public:
      Error(const std::string& ja)
	: Zu3gaError(ja, SDLNet_GetError()) {}
    };

    class Connection
    {
    public:
      Connection(const std::string& host, int port)
	: connected_(false), host_(host), port_(port) {}

      explicit Connection(TCPsocket sock)
	: connected_(true), port_(0), sock_(sock) {}

      void Open();
      bool IsOpened() { return connected_; }
      void Close();

      std::string Read();
      bool Write(const std::string&);
      void PutBack(const std::string&);

      int GetDescriptor();

      ~Connection();
    private:

      bool connected_;
      std::string host_;
      int port_;
      TCPsocket sock_;
      std::string cache_;
    };
    typedef boost::shared_ptr<Connection> sp_Connection;

    class INetworkData
    {
    public:
      INetworkData(const std::string& str)
	: data_(str) {}

      INetworkData& operator >> ( unsigned char& );
      INetworkData& operator >> ( unsigned short& );
      INetworkData& operator >> ( unsigned int& );
      INetworkData& operator >> ( unsigned long& );
      INetworkData& operator >> ( std::string& );
      template <class T> INetworkData& operator >> ( std::vector<T>& arg )
      {
	unsigned long size;
	*this >> size;
	arg.clear();
	arg.reserve(size);
	for ( unsigned int i = 0; i < size; i++ ) {
	  T temp;
	  *this >> temp;
	  arg.push_back(temp);
	}
	return *this;
      }
    private:
      std::string data_;
    };

    inline INetworkData& operator >> ( INetworkData& ind, SendPos& pos )
    {
      unsigned char sign;
      unsigned short x, y;
      ind >> sign >> x >> y;
      if (sign&1)
	pos.x = -x;
      else
	pos.x = x;
      if (sign&(1<<1))
	pos.y = -y;
      else
	pos.y = y;
      return ind;
    }

    inline INetworkData& operator >> ( INetworkData& ind, SendObject& so )
    {
      return ind >> so.pos >> so.type >> so.state;
    }

    inline INetworkData& operator >> ( INetworkData& ind, NonTypeSendObject& ntso )
    {
      return ind >> ntso.pos >> ntso.state;
    }

    inline INetworkData& operator >> ( INetworkData& ind, SendParam& sp )
    {
      return ind >> sp.view_pos >> sp.HP >> sp.dash
		 >> sp.dash_gauge_pos >> sp.site >> sp.arrows;
    }

    class ONetworkData
    {
    public:
      ONetworkData& operator << ( unsigned char );
      ONetworkData& operator << ( unsigned short );
      ONetworkData& operator << ( unsigned int );
      ONetworkData& operator << ( unsigned long );
      ONetworkData& operator << ( const std::string& );
      template <class T> ONetworkData& operator << ( const std::vector<T>& arg )
      {
	*this << static_cast<unsigned long>(arg.size());
	for ( unsigned int i = 0; i < arg.size(); i++ ) {
	  *this << arg[i];
	}
	return *this;
      }
      std::string Get() { return data_; }
    private:
      std::string data_;
    };

    inline ONetworkData& operator << ( ONetworkData& ond, const SendPos& sp )
    {
      unsigned char sign = 0;
      unsigned short x, y;
      if ( sp.x < 0 ) {
	x = -sp.x;
	sign = 1;
      } else {
	x = sp.x;
      }
      if ( sp.y < 0 ) {
	y = -sp.y;
	sign |= (1<<1);
      } else {
	y = sp.y;
      }
      return ond << sign << x << y;
    }

    inline ONetworkData& operator << ( ONetworkData& ond, const SendObject& so )
    {
      return ond << so.pos << so.type << so.state;
    }

    inline ONetworkData& operator << ( ONetworkData& ond, const NonTypeSendObject& ntso )
    {
      return ond << ntso.pos << ntso.state;
    }

    inline ONetworkData& operator << ( ONetworkData& ond, const SendParam& sp )
    {
      return ond << sp.view_pos << sp.HP << sp.dash
		 << sp.dash_gauge_pos << sp.site << sp.arrows;
    }

  }
}

#endif // ZUGAGAGA_NETWORK_1_HPP
