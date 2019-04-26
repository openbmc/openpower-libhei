
#include "hei_isolator.hpp"

namespace HEI
{

Isolator::Isolator()
{
    HEI_TRAC( "Isolator::Isolator()" );
}

Isolator::~Isolator()
{
    HEI_TRAC( "Isolator::~Isolator()" );
}

void Isolator::initialize()
{
    HEI_TRAC( "Isolator::initialize()" );
}

void Isolator::isolate()
{
    HEI_TRAC( "Isolator::isolate()" );
}

} // end namespace HEI

#if HEI_SIM_ENABLE
#else
int main()
{
    return 0;
}
#endif // HEI_SIM_ENABLE
