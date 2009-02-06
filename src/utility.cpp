// last modified <2004/08/19 00:56:27 2004>
#include <SDL.h>
#include <SDL_net.h>
#include "utility.hpp"
#include "comtypes.hpp"

bool
zu3ga::PushEvent( int code, void* data1, void* data2 )
{
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.code = code;
  event.user.data1 = data1;
  event.user.data2 = data2;
  if ( SDL_PushEvent( &event ) == 0 )
    return true;
  else
    return false;
}

bool
zu3ga::IsValidHostName( const std::string& host )
{
  IPaddress ip;
  if ( SDLNet_ResolveHost(&ip, host.c_str(), 9999) != 0 ) {
    return false;
  } else {
    return true;
  }
}

int
zu3ga::StrToInt( const std::string& str )
{
  int temp;
  int ret = sscanf(str.c_str(), "%d", &temp);
  if ( ret != 1 )
    throw Zu3gaError("文字列から数値の読み取り失敗。");
  return temp;
}

std::string
zu3ga::IntToStr( int i )
{
  char buf[60];
  int ret = sprintf(buf,"%d",i);
  return std::string(buf,ret);
}
