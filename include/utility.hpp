// last modified <2004/02/08 15:33:00 2004 JST>
#ifndef ZUGAGAGA_UTILITY_HPP
#define ZUGAGAGA_UTILITY_HPP

#include <string>
#include <boost/shared_ptr.hpp>

namespace zu3ga {
  class UpdaterBase
  {
  public:
    virtual void Update() = 0;

    virtual ~UpdaterBase() {}
  };
  typedef boost::shared_ptr<UpdaterBase> sp_Updater;

  class NullUpdater : public UpdaterBase
  {
  public:
    void Update() {}
    static sp_Updater Create() { return sp_Updater(new NullUpdater); }
  };

  bool PushEvent(int, void*, void*);
  bool IsValidHostName(const std::string&);

  int StrToInt(const std::string&);
  std::string IntToStr(int);
}

#endif // ZUGAGAGA_UTILITY_HPP
