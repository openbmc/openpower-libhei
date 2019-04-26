
#include "../hei_isolator.hpp"

#if HEI_SIM_ENABLE

namespace HEI_SIM
{

int main()
{
    HEI::Isolator iso;

    iso.initialize();

    iso.isolate();

    return 0;
}

} // end namespace HEI_SIM

#endif // HEI_SIM_ENABLE
