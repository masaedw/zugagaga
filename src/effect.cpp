// last modified <2007/01/27 14:20:11 2007>
#include "effect.hpp"

using namespace zu3ga;

const double zu3ga::DotEffect::default_ver_pos;

// 譎る俣繧呈峩譁ｰ縺吶ｋ
void ExplosionA::Update()
{
  ++frame;
  if (frame >= 12) {
    erase = true;
  }
}
SendObject ExplosionA::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));
  if (frame <= 2) {
    so.state = 0;
  } else if (frame <= 4) {
    so.state = 1;
  } else if (frame <= 6) {
    so.state = 2;
  } else if (frame <= 8) {
    so.state = 3;
  } else if (frame <= 10) {
    so.state = 4;
  } else {
    so.state = 5;
  }
  return so;
}

// 譎る俣繧呈峩譁ｰ縺吶ｋ
void ExplosionAx2::Update()
{
  ++frame;
  if (frame >= 12) {
    erase = true;
  }
}
SendObject ExplosionAx2::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));
  if (frame <= 2) {
    so.state = 0;
  } else if (frame <= 4) {
    so.state = 1;
  } else if (frame <= 6) {
    so.state = 2;
  } else if (frame <= 8) {
    so.state = 3;
  } else if (frame <= 10) {
    so.state = 4;
  } else {
    so.state = 5;
  }
  return so;
}

// 譎る俣繧呈峩譁ｰ縺吶ｋ
void ExplosionAx4::Update()
{
  ++frame;
  if (frame >= 24) {
    erase = true;
  }
}
SendObject ExplosionAx4::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));
  if (frame <= 4) {
    so.state = 0;
  } else if (frame <= 8) {
    so.state = 1;
  } else if (frame <= 12) {
    so.state = 2;
  } else if (frame <= 16) {
    so.state = 3;
  } else if (frame <= 20) {
    so.state = 4;
  } else {
    so.state = 5;
  }
  return so;
}

SendObject SmokeA::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));
  so.state = frame;
  if (frame >= 3) {
    erase = true;
  }
  return so;
}

SendObject SmokeB::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));
  if (frame <= 2) {
    so.state = 0;
  } else if (frame <= 4) {
    so.state = 1;
  } else if (frame <= 6) {
    so.state = 2;
  } else if (frame <= 8) {
    so.state = 3;
  } else if (frame >= 9) {
    so.state = 3;
    erase = true;
  }
  return so;
}

SendObject ExplosionB::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));

  if (frame <= 0) {
    so.state = 0;
  }
  else if (frame <= 1) {
    so.state = 1;
  }
  else if (frame <= 2) {
    so.state = 2;
  }
  else if (frame <= 3) {
    so.state = 3;
  }
  else {
    so.state = 4;
    erase = true;
  }
  return so;
}

SendObject ExplosionC::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));

  if (frame <= 0) {
    so.state = 0;
  }
  else if (frame == 1) {
    so.state = 1;
  }
  else if (frame == 2) {
    so.state = 2;
  }
  else if (frame == 3) {
    so.state = 3;
  }
  else {
    so.state = 4;
    erase = true;
    return so;
  }
  return so;
}

SlashA::SlashA(const DPos& pos, Direction::t a_dir)
  : pos(pos)
{
  // 菴薙?蜷代″縺ｫ繧医▲縺ｦ蛻?ｌ縺吶§縺ｮ蜷代″繧呈ｱｺ繧√ｋ
  switch (a_dir) {
  case Direction::Dir1:
    dir = Direction::Dir2;
    break;
  case Direction::Dir2:
    dir = Direction::Dir3;
    break;
  case Direction::Dir3:
    dir = Direction::Dir2;
    break;
  case Direction::Dir4:
    dir = Direction::Dir1;
    break;
  case Direction::Dir6:
    dir = Direction::Dir3;
    break;
  case Direction::Dir7:
    dir = Direction::Dir4;
    break;
  case Direction::Dir8:
    dir = Direction::Dir7;
    break;
  case Direction::Dir9:
    dir = Direction::Dir6;
    break;
  default:
    dir = Direction::Dir1;
    break;
  }
}

SendObject SlashA::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));

  if (frame <= 1) {
    so.state = dir;
  }
  else if (frame <= 3) {
    so.state = dir + 8;		// 8譁ｹ蜷*hoge蛟九?state繧呈戟縺､
    if (frame == 3) {
      erase = true;
    }
  }
  else {

  }
  return so;
}

SlashAA::SlashAA(const DPos& pos, Direction::t a_dir)
  : pos(pos)
{
  dir = a_dir;
}
SendObject SlashAA::GetSendObject()
{
  SendObject so;
  so.type = type;
  so.pos = SendPos(short(pos.x), short(pos.y));

  if (frame <= 1) {
    so.state = dir;
  }
  else if (frame <= 3) {
    so.state = dir + 8;		// 8譁ｹ蜷*hoge蛟九?state繧呈戟縺､
    if (frame == 3) {
      erase = true;
    }
  }
  else {

  }
  return so;
}
