// last modified <2004/08/19 02:55:24 2004 JST>
#include <iostream>
#include "network.hpp"
#include "verbose.hpp"

using namespace std;
using namespace zu3ga;
using namespace network;

void
Connection::Open()
{
  if ( !connected_ ) {
    IPaddress addr;
    if ( SDLNet_ResolveHost(&addr,host_.c_str(),port_) != 0 ) throw Error("名前引き失敗");
    if ((sock_ = SDLNet_TCP_Open(&addr)) == 0 ) throw Error("接続に失敗");
    connected_ = true;
  }
}

void
Connection::Close()
{
  if ( connected_ ) {
    connected_ = false;
    SDLNet_TCP_Close(sock_);
    verbose::puts(2,"close");
  }
}

Connection::~Connection()
{
  Close();
}

namespace {
  /**
     data に受信したデータを継ぎ足す。
     @return 正常に受信できたかどうか。
   */
  enum RecvStatus {
    RS_TRUE,
    RS_FALSE,
    RS_EOF,
  };

  inline RecvStatus SockRead( TCPsocket sock, string& data )
  {
    RecvStatus ret = RS_TRUE;

    SDLNet_SocketSet ss = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(ss,sock);

    const Uint32 bufsize = 200;
    char buf[bufsize];

    while ( SDLNet_CheckSockets(ss,1) == 1 ) {
      int readsize = SDLNet_TCP_Recv(sock,buf,bufsize);
      data.append(buf,readsize);
      if ( readsize == 0 ) {
        ret = RS_EOF;
      } else if ( readsize < 0 ) {
        ret = RS_FALSE;
      }
      if ( (Uint32)readsize < bufsize ) {
        break;
      }
    }

    SDLNet_FreeSocketSet(ss);
    return ret;
  }

  inline bool SockWrite( TCPsocket sock, const string& data )
  {
    return SDLNet_TCP_Send(sock,const_cast<char*>(data.data()),data.size()) == (Sint32)data.size();
  }
}

string
Connection::Read()
{
  if ( connected_ == false ) {
    if ( cache_.empty() ) {
      throw Error( "Read: 接続が切れています" );
    } else {
      goto endfunc;
    }
  }

  switch ( SockRead(sock_,cache_) ) {
  case RS_TRUE:
    break;
  case RS_FALSE:
    break;
  case RS_EOF:
    Close();
    break;
  }

 endfunc:
  string temp;
  temp.swap(cache_);
  return temp;
}

bool
Connection::Write( const string& msg )
{
  if ( connected_ == false )
    throw Error("Write: 接続が切れています");
  return SockWrite(sock_,msg);
}

void
Connection::PutBack( const string& msg )
{
  if ( connected_ == false )
    throw Error( "PutBack: 接続が切れています" );

  cache_ = msg + cache_;
}

INetworkData&
INetworkData::operator >> ( unsigned char& arg )
{
  arg = data_[0];
  data_.erase(0,1);
  return *this;
}

INetworkData&
INetworkData::operator >> ( unsigned short& arg )
{
  arg = SDLNet_Read16((void*)data_.data());
  data_.erase(0,2);
  return *this;
}

INetworkData&
INetworkData::operator >> ( unsigned int& arg )
{
  arg = SDLNet_Read32((void*)data_.data());
  data_.erase(0,4);
  return *this;
}

INetworkData&
INetworkData::operator >> ( unsigned long& arg )
{
  arg = SDLNet_Read32((void*)data_.data());
  data_.erase(0,4);
  return *this;
}

INetworkData&
INetworkData::operator >> ( std::string& arg )
{
  unsigned long size = SDLNet_Read32((void*)data_.data());
  arg = data_.substr(4,size);
  data_.erase(0,4+size);
  return *this;
}

ONetworkData&
ONetworkData::operator << ( unsigned char arg )
{
  data_ += arg;
  return *this;
}

ONetworkData&
ONetworkData::operator << ( unsigned short arg )
{
  char buf[2];
  SDLNet_Write16(arg,buf);
  data_.append(buf,2);
  return *this;
}

ONetworkData&
ONetworkData::operator << ( unsigned int arg )
{
  char buf[4];
  SDLNet_Write32(arg,buf);
  data_.append(buf,4);
  return *this;
}

ONetworkData&
ONetworkData::operator << ( unsigned long arg )
{
  char buf[4];
  SDLNet_Write32(arg,buf);
  data_.append(buf,4);
  return *this;
}

ONetworkData&
ONetworkData::operator << ( const std::string& arg )
{
  char buf[4];
  SDLNet_Write32(arg.size(),buf);
  data_.append(buf,4);
  data_.append(arg);
  return *this;
}
