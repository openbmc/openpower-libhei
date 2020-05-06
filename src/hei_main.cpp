#include <hei_main.hpp>
#include <isolator/hei_isolator.hpp>

namespace libhei
{

void initialize(void* i_buffer, size_t i_bufferSize)
{
    Isolator::getSingleton().initialize(i_buffer, i_bufferSize);
}

void uninitialize()
{
    Isolator::getSingleton().uninitialize();
}

void isolate(const std::vector<Chip>& i_chipList, IsolationData& o_isoData)
{
    Isolator::getSingleton().isolate(i_chipList, o_isoData);
}

} // end namespace libhei
