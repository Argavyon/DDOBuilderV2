// BreakdownItemSpellPoints.h
//
#pragma once
#include "BreakdownItem.h"

class CBreakdownsPane;

class BreakdownItemSpellPoints :
        public BreakdownItem
{
    public:
        BreakdownItemSpellPoints(
                CBreakdownsPane* pPane,
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemSpellPoints();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
        virtual double Multiplier() const override;

        virtual void ClassChanged(Build* , const std::string& classFrom, const std::string& classTo, size_t level) override;
        // BreakdownObserver overrides (may be specialised in inheriting classes)
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        CString m_title;
        EffectType m_effect;
};
