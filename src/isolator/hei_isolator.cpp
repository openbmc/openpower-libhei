
#include <isolator/hei_isolation_node.hpp>
#include <isolator/hei_isolator.hpp>
#include <register/hei_operator_register.hpp>
#include <register/hei_scom_register.hpp>
#include <util/hei_flyweight.hpp>

namespace libhei
{

void Isolator::uninitialize()
{
    // Clear all of the IsolationChip objects.
    iv_isoChips.clear();

    // Remove all of the IsolationNode objects stored in the flyweights. This
    // must be done before removing the HardwareRegister objects
    Flyweight<IsolationNode>::getSingleton().clear();

    // Must flush the hardware register cache before deleting any
    // HardwareRegister objects.
    HardwareRegister::flushAll();

    // Remove all of the HardwareRegister objects stored in the flyweights.
    Flyweight<ScomRegister>::getSingleton().clear();
    Flyweight<IdScomRegister>::getSingleton().clear();
}

void Isolator::isolate(const std::vector<Chip>& i_chipList,
                       IsolationData& o_isoData) const
{
    // Flush the isolation data to ensure a clean slate.
    o_isoData.flush();

    // Flush the hardware register cache to avoid using stale data.
    HardwareRegister::flushAll();

    // Analyze active error on each chip.
    for (const auto& chip : i_chipList)
    {
        // Isolation objects for this chip's type must exist.
        const auto& itr = iv_isoChips.find(chip.getType());
        HEI_ASSERT(iv_isoChips.end() != itr);

        // Analyze this chip.
        itr->second->analyze(chip, o_isoData);
    }
}

} // end namespace libhei
