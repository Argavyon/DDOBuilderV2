// AboutDlg.cpp
//
#include "stdafx.h"
#include "AboutDlg.h"
#include "BuildInfo.h"
#include "GlobalSupportFunctions.h"

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_VERSION, m_staticVersion);
    DDX_Control(pDX, IDC_BUG_REPORTERS, m_bugReporters);

    CString text;
    text.Format("DDOBuilder, Version %s", BUILDINFO_VERSION);
    m_staticVersion.SetWindowText(text);

    int count = StandardFeats().size()
                + HeroicPastLifeFeats().size()
                + RacialPastLifeFeats().size()
                + IconicPastLifeFeats().size()
                + EpicPastLifeFeats().size()
                + SpecialFeats().size()
                + UniversalTreeFeats().size()
                + DestinyTreeFeats().size()
                + FavorFeats().size();
    text.Format("Feat Count: %d", count);
    SetDlgItemText(IDC_STATIC_FEATCOUNT, text);

    count = Augments().size();
    text.Format("Augment Count: %d", count);
    SetDlgItemText(IDC_STATIC_AUGMENTCOUNT, text);

    count = Items().size();
    text.Format("Item Count: %d", count);
    SetDlgItemText(IDC_STATIC_ITEMCOUNT, text);

    count = Spells().size();
    text.Format("Spell Count: %d", count);
    SetDlgItemText(IDC_STATIC_SPELLCOUNT, text);

    count = EnhancementTrees().size();
    text.Format("Enhancement Tree Count: %d", count);
    SetDlgItemText(IDC_STATIC_TREECOUNT, text);

    count = -6;//OptionalBuffs().size();
    text.Format("Self and Party Buff Count: %d", count);
    SetDlgItemText(IDC_STATIC_PARTYBUFF_COUNT, text);

    m_bugReporters.SetWindowText(
            "In alphabetical order:\r\n"
            "\r\n"
            "Aaumini, "
            "d3x-dt3, "
            "Daawsomeone, "
            "DODOCung, "
            "DougSK, "
            "JelloDDO, "
            "keluynn, "
            "Melkizadek1, "
            "MuazAlhaidar, "
            "Mummykiller, "
            "nb756, "
            "Ntoukis, "
            "Orbalus, "
            "rosstracy, "
            "SpartanKiller13, "
            "thomas-langworthy, "
            "zilgabex");
    m_bugReporters.SetSel((DWORD)-1);    // nothing starts selected
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    GetDlgItem(IDOK)->SetFocus();

    return FALSE;
}
