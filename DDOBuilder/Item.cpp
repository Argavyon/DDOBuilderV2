// Item.cpp
//
#include "StdAfx.h"
#include "Item.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include "LogPane.h"
#include "Buff.h"
#include "SetBonus.h"
#include "DDOBuilder.h"
#include "Build.h"
#include <algorithm>

#define DL_ELEMENT Item

namespace
{
    const wchar_t f_saxElementName[] = L"Item";
    DL_DEFINE_NAMES(Item_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Item::Item() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_Slots(L"EquipmentSlot"),
    m_RestrictedSlots(L"RestrictedSlots"),
    m_iconIndex(0)          // defaults to no image
{
    DL_INIT(Item_PROPERTIES)
}

Item::Item(const XmlLib::SaxString & objectName) :
    XmlLib::SaxContentElement(objectName, f_verCurrent),
    m_Slots(L"EquipmentSlot"),
    m_RestrictedSlots(L"RestrictedSlots"),
    m_iconIndex(0)          // defaults to no image
{
    DL_INIT(Item_PROPERTIES)
}

DL_DEFINE_ACCESS(Item_PROPERTIES)

XmlLib::SaxContentElementInterface * Item::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Item_PROPERTIES)

    return subHandler;
}

void Item::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Item_PROPERTIES)
}

void Item::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Item_PROPERTIES)
    writer->EndElement();
}

bool Item::CanEquipToSlot(InventorySlotType slot) const
{
    bool canEquipToSlot = false;
    switch (slot)
    {
    case Inventory_Arrows:
        canEquipToSlot = m_Slots.HasArrow();
        break;
    case Inventory_CosmeticArmor:
        canEquipToSlot = m_Slots.HasCosmeticArmor();
        break;
    case Inventory_Armor:
        canEquipToSlot = m_Slots.HasArmor();
        break;
    case Inventory_ArmorCloth:
        canEquipToSlot = m_Slots.HasArmor() && Armor() == Armor_Cloth;
        break;
    case Inventory_ArmorLight:
        canEquipToSlot = m_Slots.HasArmor() && Armor() == Armor_Light;
        break;
    case Inventory_ArmorMedium:
        canEquipToSlot = m_Slots.HasArmor() && Armor() == Armor_Medium;
        break;
    case Inventory_ArmorHeavy:
        canEquipToSlot = m_Slots.HasArmor() && Armor() == Armor_Heavy;
        break;
    case Inventory_ArmorDocent:
        canEquipToSlot = m_Slots.HasArmor() && Armor() == Armor_Docent;
        break;
    case Inventory_Belt:
        canEquipToSlot = m_Slots.HasBelt();
        break;
    case Inventory_Boots:
        canEquipToSlot = m_Slots.HasBoots();
        break;
    case Inventory_Bracers:
        canEquipToSlot = m_Slots.HasBracers();
        break;
    case Inventory_CosmeticCloak:
        canEquipToSlot = m_Slots.HasCosmeticCloak();
        break;
    case Inventory_Cloak:
        canEquipToSlot = m_Slots.HasCloak();
        break;
    case Inventory_Gloves:
        canEquipToSlot = m_Slots.HasGloves();
        break;
    case Inventory_Goggles:
        canEquipToSlot = m_Slots.HasGoggles();
        break;
    case Inventory_CosmeticHelm:
        canEquipToSlot = m_Slots.HasCosmeticHelm();
        break;
    case Inventory_Helmet:
        canEquipToSlot = m_Slots.HasHelmet();
        break;
    case Inventory_Necklace:
        canEquipToSlot = m_Slots.HasNecklace();
        break;
    case Inventory_Quiver:
        canEquipToSlot = m_Slots.HasQuiver();
        break;
    case Inventory_Ring1:
    case Inventory_Ring2:
        canEquipToSlot = m_Slots.HasRing();
        break;
    case Inventory_Trinket:
        canEquipToSlot = m_Slots.HasTrinket();
        break;
    case Inventory_Weapon1:
        canEquipToSlot = m_Slots.HasWeapon1();
        break;
    case Inventory_Weapon2:
        canEquipToSlot = m_Slots.HasWeapon2();
        break;
    case Inventory_CosmeticWeapon1:
    case Inventory_CosmeticWeapon2:
        //canEquipToSlot = m_Slots.HasCosmeticWeapon();
        break;
    }
    return canEquipToSlot;
}

void Item::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    if (HasIcon())
    {
        CDDOBuilderApp* pApp = dynamic_cast<CDDOBuilderApp*>(AfxGetApp());
        if (pApp->m_imagesMap.find(Icon()) == pApp->m_imagesMap.end())
        {
            ss << "Item is missing image file \"" << Icon() << "\"\n";
            ok = false;
        }
    }
    else
    {
        ss << "Item is missing Icon field\n";
        ok = false;
    }
    if (m_hasDamageDice)
    {
        //ok &= m_DamageDice.VerifyObject(&ss);
    }
    // check the item effects also
    std::vector<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(&ss);
        ++it;
    }
    // verify its DC objects
    //std::list<DC>::const_iterator edcit = m_EffectDC.begin();
    //while (edcit != m_EffectDC.end())
    //{
    //    ok &= (*edcit).VerifyObject(&ss);
    //    ++edcit;
    //}
    // verify any augments
    std::vector<ItemAugment>::const_iterator iacit = m_Augments.begin();
    while (iacit != m_Augments.end())
    {
        ok &= (*iacit).VerifyObject(&ss);
        ++iacit;
    }
    // check any set bonuses exist
    const std::list<::SetBonus> & loadedSets = SetBonuses();
    std::list<std::string>::const_iterator sbit = m_SetBonus.begin();
    while (sbit != m_SetBonus.end())
    {
        bool bFound = false;
        std::list<::SetBonus>::const_iterator sit = loadedSets.begin();
        while (!bFound && sit != loadedSets.end())
        {
            bFound = ((*sit).Type() == (*sbit));
            ++sit;
        }
        if (!bFound)
        {
            ok = false;
            ss << "Has unknown set bonus \"" << (*sbit) << "\"\n";
        }
        ++sbit;
    }

    if (!ok)
    {
        GetLog().AddLogEntry(ss.str().c_str());
    }
}

void Item::CopyUserSetValues(const Item & original)
{
    // when updating an item to the latest version there are certain user values
    // which we need to copy
    // augments/upgrade slots
    m_Augments = original.m_Augments;
    m_SlotUpgrades = original.m_SlotUpgrades;
}

bool Item::ContainsSearchText(const std::string& searchText) const
{
    bool bHasSearchText = true;
    // break up the search text into individual fields and search for each
    std::string parsedItem;
    std::stringstream ss(searchText);
    while (bHasSearchText && std::getline(ss, parsedItem, ' '))
    {
        bHasSearchText = false; // must have all terms
        // we have to search all of the following text fields of this item:
        // Name
        // Description
        // Drop Location
        // EffectDescription(s)
        // Set Bonuses
        bHasSearchText |= SearchForText(Name(), parsedItem);
        bHasSearchText |= SearchForText(ItemType(), parsedItem);
        bHasSearchText |= SearchForText(Description(), parsedItem);
        if (HasMinorArtifact())
        {
            bHasSearchText |= SearchForText("Minor Artifact", parsedItem);
        }
        if (m_hasDropLocation)
        {
            bHasSearchText |= SearchForText(DropLocation(), parsedItem);
        }
        //std::list<std::string>::const_iterator it = m_EffectDescription.begin();
        //while (!bHasSearchText && it != m_EffectDescription.end())
        //{
        //    bHasSearchText |= SearchForText((*it), parsedItem);
        //    ++it;
        //}
        //it = m_SetBonus.begin();
        //while (!bHasSearchText && it != m_SetBonus.end())
        //{
        //    const ::SetBonus& set = FindSetBonus((*it));
        //    bHasSearchText |= SearchForText(set.Description(), parsedItem);
        //    ++it;
        //}
    }
    return bHasSearchText;
}

bool Item::SearchForText(std::string source, const std::string& find) const
{
    // the search is done in all lower case
    // note that std::tolower is wrapped in a lamda function to avoid a compile warning
    std::transform(source.begin(), source.end(), source.begin(), 
        [](char c) {return static_cast<char>(std::tolower(c)); });
    bool bTextPresent = (source.find(find.c_str()) != std::string::npos);
    return bTextPresent;
}

std::string Item::ItemType() const
{
    std::stringstream ss;
    bool bFirst = true;
    for (size_t i = Inventory_Unknown; i < Inventory_Count; ++i)
    {
        if (Slots().HasSlot(static_cast<InventorySlotType>(i)))
        {
            if (!bFirst)
            {
                ss << ", ";
            }
            if (i == Inventory_Armor && HasArmor())
            {
                ss << EnumEntryText(Armor(), armorTypeMap);
                ss << " ";
            }
            if ((i == Inventory_Weapon1 || i == Inventory_Weapon2)
                    && HasWeapon())
            {
                ss << EnumEntryText(Weapon(), weaponTypeMap);
                ss << " ";
            }
            ss << EnumEntryText(static_cast<InventorySlotType>(i), InventorySlotTypeMap);
            bFirst = false;
        }
    }

    return ss.str();
}

void Item::SetIconIndex(size_t index)
{
    m_iconIndex = index;
}

size_t Item::IconIndex() const
{
    return m_iconIndex;
}

void Item::SetFilename(const std::string& filename)
{
    m_filename = filename;
}

const std::string& Item::Filename() const
{
    return m_filename;
}

std::vector<CString> Item::BuffDescriptions(const Build* pBuild) const
{
    std::vector<CString> buffs;
    for (auto&& it : m_Buffs)
    {
        // we can use front here as there is always only 1 entry
        // for an item entry. EffectInfo's can have multiple types
        // when in the Buff list only
        Buff buff = FindBuff(it.Type());
        if (it.HasBonusType()) buff.Set_BonusType(it.BonusType());
        if (it.HasIgnore()) buff.Set_Ignore(it.Ignore());
        if (it.HasValue1()) buff.Set_Value1(it.Value1());
        if (it.HasValue2()) buff.Set_Value2(it.Value2());
        if (it.HasItem()) buff.Set_Item(it.Item());
        if (it.HasDescription1()) buff.Set_Description1(it.Description1());
        buffs.push_back(buff.MakeDescription());
        if (it.HasRequirementsToUse())
        {
            std::vector<bool> metIgnored;
            it.RequirementsToUse().CreateRequirementStrings(*pBuild, pBuild->Level(), true, &buffs, &metIgnored);
        }
    }
    return buffs;
}

void Item::AddSetBonus(const std::string& setName)
{
    m_SetBonus.push_back(setName);
}

void Item::AddItemAugment(const ItemAugment& ia)
{
    m_Augments.push_back(ia);
}

void Item::AddRaceRequirement(const std::string& race)
{
    Requirement raceRequirement(Requirement_Race, race, 1);
    m_RequirementsToUse.AddRequirement(raceRequirement);
    m_hasRequirementsToUse = true;
}

void Item::AddClassRequirement(const std::string& c)
{
    Requirement classRequirement(Requirement_BaseClass, c);
    m_RequirementsToUse.AddRequirement(classRequirement);
    m_hasRequirementsToUse = true;
}

void Item::AddFeatRequirement(const std::string& feat)
{
    Requirement featRequirement(Requirement_Feat, feat);
    m_RequirementsToUse.AddRequirement(featRequirement);
    m_hasRequirementsToUse = true;
}

void Item::AddFeatAnySourceRequirement(const std::string& feat)
{
    Requirement featRequirement(Requirement_FeatAnySource, feat, 1);
    m_RequirementsToUse.AddRequirement(featRequirement);
    m_hasRequirementsToUse = true;
}

void Item::AddBuff(const Buff& b)
{
    m_Buffs.push_back(b);
}

void Item::SetAugments(const std::vector<ItemAugment>& augments)
{
    Set_Augments(augments);
}
