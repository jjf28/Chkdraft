#include "UnitChange.h"
#include "../../../Windows/MainWindows/GuiMap.h"

UnitChange::~UnitChange()
{

}

std::shared_ptr<UnitChange> UnitChange::Make(u16 unitIndex, Chk::Unit::Field field, u32 data)
{
    return std::shared_ptr<UnitChange>(new UnitChange(unitIndex, field, data));
}

void UnitChange::Reverse(void *guiMap)
{
    u32 replacedData = ((GuiMap*)guiMap)->GetUnitFieldData(unitIndex, field);
    ((GuiMap*)guiMap)->SetUnitFieldData(unitIndex, field, data);
    data = replacedData;
}

int32_t UnitChange::GetType()
{
    return (int32_t)UndoTypes::UnitChange;
}

UnitChange::UnitChange(u16 unitIndex, Chk::Unit::Field field, u32 data)
    : unitIndex(unitIndex), field(field), data(data)
{

}
