
#pragma once

#include "hei_user_interface.hpp"

namespace HEI
{

class Isolator
{
  public:

    Isolator();

    ~Isolator();

    void initialize();

    void isolate();

  private:

}; // end class Isolator

void HEI_TRAC(const char *msg) {
    msg = msg;
}

} // end namespace HEI
