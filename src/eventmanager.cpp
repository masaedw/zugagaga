// last modified <2004/02/07 22:51:57 2004 JST>
#include <memory>
#include <vector>
#include <algorithm>
#include <cctype>
#include "event.hpp"
#include "verbose.hpp"

using namespace std;
using namespace zu3ga;
using namespace boost;

namespace {
  EventManager* em = 0;
}

void
EventManager::Init()
{
  if (!em)
    em = new EventManager;
}

void
EventManager::Quit()
{
  if (em) {
    delete em;
    em = 0;
  }
}

EventManager*
EventManager::GetInstance()
{
  return em;
}

EventManager::EventManager()
  : padstick_(2)
  , padbutton_(3)
  , mode_(Key)
  , stick_(0)
{
  int num = SDL_NumJoysticks();
  verbose::puts(3, num, " joysticks found.");
  if (num != 0) {
    SDL_JoystickEventState(SDL_ENABLE);
    stick_ = SDL_JoystickOpen(0);
  }
  GameMode();
  padstick_[Vertical] = 1;
  padstick_[Horizontal] = 0;
  padbutton_[Button1] = 7;
  padbutton_[Button2] = 0;
  padbutton_[Button3] = 1;
//   quitkey_.push_back(SDLK_ESCAPE);
//   quitkey_.push_back(SDLK_q);
//   upkey_.push_back(SDLK_UP);
//   downkey_.push_back(SDLK_DOWN);
//   leftkey_.push_back(SDLK_LEFT);
//   rightkey_.push_back(SDLK_RIGHT);
//   b1key_.push_back(SDLK_LSHIFT);
//   b2key_.push_back(SDLK_z);
//   b3key_.push_back(SDLK_x);
  key_[SDLK_ESCAPE] = ClientEvent::Quit;
  key_[SDLK_q] = ClientEvent::Quit;
  key_[SDLK_UP] = ClientEvent::Up;
  key_[SDLK_DOWN] = ClientEvent::Down;
  key_[SDLK_LEFT] = ClientEvent::Left;
  key_[SDLK_RIGHT] = ClientEvent::Right;
  key_[SDLK_LSHIFT] = ClientEvent::Button1;
  key_[SDLK_z] = ClientEvent::Button2;
  key_[SDLK_x] = ClientEvent::Button3;
}

EventManager::~EventManager()
{
  if (stick_ && SDL_JoystickOpened(0))
    SDL_JoystickClose(stick_);
}

// void
// EventManager::SetQuitKey(SDLKey)
// {

// }

// void
// EventManager::SetUpKey(SDLKey)
// {

// }

// void
// EventManager::SetDownKey(SDLKey)
// {

// }

// void
// EventManager::SetLeftKey(SDLKey)
// {

// }

// void
// EventManager::SetRightKey(SDLKey)
// {

// }

// void
// EventManager::SetButton1Key(SDLKey)
// {

// }

// void
// EventManager::SetButton2Key(SDLKey)
// {

// }

// void
// EventManager::SetButton3Key(SDLKey)
// {

// }

void
EventManager::SetVerticalAxis(Uint8)
{

}

void
EventManager::SetHorizontalAxis(Uint8)
{

}

void
EventManager::SetButton1Button(Uint8)
{

}

void
EventManager::SetButton2Button(Uint8)
{

}

void
EventManager::SetButton3Button(Uint8)
{

}

void
EventManager::DumpConfigFile()
{

}

bool
EventManager::GetEvent( Uint8& a_eve )
{
  GameMode();
  SDL_Event eve;
  a_eve = ClientEvent::None;
  while (a_eve == ClientEvent::None && SDL_PollEvent(&eve)) {
    Uint8 press = 0;
    switch (eve.type) {
    case SDL_QUIT:
      a_eve = ClientEvent::Quit;
      break;

    case SDL_KEYDOWN:
      press = ClientEvent::Press;
    case SDL_KEYUP:
      {
	Keymap::const_iterator i = key_.find(eve.key.keysym.sym);
	if ( i != key_.end() ) {
	  a_eve = i->second;
	  if ( a_eve != ClientEvent::Quit ) {
	    a_eve |= press;
	  }
	}
      }
      break;

    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
      {
	JoyData jd = GetJoyEvent(eve.jbutton);
	a_eve = jd.client;
      }
      break;

    case SDL_JOYAXISMOTION:
      {
	JoyData jd = GetJoyEvent(eve.jaxis);
	a_eve = jd.client;
      }
      break;

    default:
      break;
    } // switch
  } // while
  return a_eve != ClientEvent::None;
}

EventManager::JoyData
EventManager::GetJoyEvent( const SDL_JoyAxisEvent& jaxis )
{
  JoyData jd;
  if ((jaxis.value > -3200) && (jaxis.value < 3200)) {
    if ( jaxis.axis == padstick_[Vertical] ) {
      if ( stickvalue_.IsUp() ) {
	jd.client = ClientEvent::Up;
	stickvalue_.Down();
      } else if ( stickvalue_.IsDown() ) {
	jd.client = ClientEvent::Down;
	stickvalue_.Up();
      }
    } else if ( jaxis.axis == padstick_[Horizontal] ) {
      if ( stickvalue_.IsLeft() ) {
	jd.client = ClientEvent::Left;
	stickvalue_.Right();
      } else if ( stickvalue_.IsRight() ) {
	jd.client = ClientEvent::Right;
	stickvalue_.Left();
      }
    }
  } else {
    if ( jaxis.axis == padstick_[Vertical] ) {
      if (jaxis.value < 0) {
	stickvalue_.Up();
	jd.client = ClientEvent::Press | ClientEvent::Up;
	jd.lobby = LobbyEvent::Up;
      } else {
	stickvalue_.Down();
	jd.client = ClientEvent::Press | ClientEvent::Down;
	jd.lobby = LobbyEvent::Down;
      }
    } else if ( jaxis.axis == padstick_[Horizontal] ) {
      if (jaxis.value < 0) {
	stickvalue_.Left();
	jd.client = ClientEvent::Press | ClientEvent::Left;
	jd.lobby = LobbyEvent::Left;
      } else {
	stickvalue_.Right();
	jd.client = ClientEvent::Press | ClientEvent::Right;
	jd.lobby = LobbyEvent::Right;
      }
    }
  } // if
  return jd;
}

EventManager::JoyData
EventManager::GetJoyEvent( const SDL_JoyButtonEvent& jbutton )
{
  JoyData jd;
  Uint8 press = (jbutton.state == SDL_PRESSED) ? ClientEvent::Press : 0;
  if ( jbutton.button == padbutton_[Button1] ) {
    jd.client = ClientEvent::Button1 | press;
    if ( SDL_GetModState() & KMOD_SHIFT )
      jd.lobby = LobbyEvent::BackChange;
    else
      jd.lobby = LobbyEvent::Change;
  } else if ( jbutton.button == padbutton_[Button2] ) {
    jd.client = ClientEvent::Button2 | press;
    jd.lobby = LobbyEvent::OK;
  } else if ( jbutton.button == padbutton_[Button3] ) {
    jd.client = ClientEvent::Button3 | press;
  }
  return jd;
}

EventManager::KeyData
EventManager::GetLobbyKeyEvent( const SDL_KeyboardEvent& key )
{
  Uint16 uni = key.keysym.unicode;
  KeyData kd;
  if ( key.keysym.mod & KMOD_CTRL ) {
    switch (key.keysym.sym) {
    case SDLK_p:
      kd.event = LobbyEvent::Up;
      break;
    case SDLK_n:
      kd.event = LobbyEvent::Down;
      break;
    case SDLK_b:
      kd.event = LobbyEvent::Left;
      break;
    case SDLK_f:
      kd.event = LobbyEvent::Right;
      break;
    default:
      break;
    }
  } else if ( isprint(uni) ) {
    kd.event = LobbyEvent::Key;
    kd.unicode = uni;
  } else {
    switch (key.keysym.sym) {
    case SDLK_ESCAPE:
      kd.event = LobbyEvent::Quit;
      break;
    case SDLK_BACKSPACE:
      kd.event = LobbyEvent::Key;
      kd.unicode = '\b';
      break;
    case SDLK_UP:
      kd.event = LobbyEvent::Up;
      break;
    case SDLK_DOWN:
      kd.event = LobbyEvent::Down;
      break;
    case SDLK_LEFT:
      kd.event = LobbyEvent::Left;
      break;
    case SDLK_RIGHT:
      kd.event = LobbyEvent::Right;
      break;
    case SDLK_TAB:
      if ( key.keysym.mod & KMOD_SHIFT )
	kd.event = LobbyEvent::BackChange;
      else
	kd.event = LobbyEvent::Change;
      break;
    case SDLK_RETURN:
      kd.event = LobbyEvent::OK;
    default:
      break;
    }
  }
  return kd;
}

bool
EventManager::GetEvent( LobbyEvent::t& a_eve, Uint16& a_uni )
{
  KeyMode();
  SDL_Event eve;
  a_eve = LobbyEvent::None;
  while (a_eve == LobbyEvent::None && SDL_PollEvent(&eve)) {
    switch (eve.type) {
    case SDL_QUIT:
      a_eve = LobbyEvent::Quit;
      break;

    case SDL_KEYDOWN:
      {
	KeyData kd = GetLobbyKeyEvent(eve.key);
	if ( (a_eve = kd.event) == LobbyEvent::Key ) {
	  a_uni = kd.unicode;
	}
      }
      break;

    case SDL_JOYAXISMOTION:
      {
	JoyData jd = GetJoyEvent(eve.jaxis);
	a_eve = jd.lobby;
      }
      break;

    case SDL_JOYBUTTONDOWN:
      {
	JoyData jd = GetJoyEvent(eve.jbutton);
	a_eve = jd.lobby;
      }
      break;

    default:
      break;
    } // switch
  } // while
  return a_eve != LobbyEvent::None;
}

void
EventManager::GameMode()
{
  if ( mode_ == Game )
    return;

  SDL_EnableUNICODE(0);
  SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
  mode_ = Game;
}

void
EventManager::KeyMode()
{
  if ( mode_ == Key )
    return;

  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  mode_ = Key;
}
