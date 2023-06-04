// BuildsPane.cpp : implementation file
//

#include "stdafx.h"
#include "BuildsPane.h"
#include "GlobalSupportFunctions.h"
#include "DDOBuilderDoc.h"
#include "LogPane.h"

// CBuildsPane
namespace
{
    DWORD MakeItemData(TreeEntryItem type, size_t lifeIndex, size_t buildIndex)
    {
        return MAKELONG(type, MAKEWORD(lifeIndex, buildIndex));
    }
    TreeEntryItem ExtractType(DWORD itemData)
    {
        TreeEntryItem type = static_cast<TreeEntryItem>(LOWORD(itemData));
        return type;
    }
    size_t ExtractLifeIndex(DWORD itemData)
    {
        size_t lifeIndex = LOBYTE(HIWORD(itemData));
        return lifeIndex;
    }
    size_t ExtractBuildIndex(DWORD itemData)
    {
        size_t buildIndex = HIBYTE(HIWORD(itemData));
        return buildIndex;
    }
}

IMPLEMENT_DYNCREATE(CBuildsPane, CFormView)
BEGIN_MESSAGE_MAP(CBuildsPane, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_BN_CLICKED(IDC_BUTTON_NEW_LIFE, OnButtonNewLife)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_LIFE, OnButtonDeleteLife)
    ON_BN_CLICKED(IDC_BUTTON_ADD_BUILD, OnButtonAddBuild)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_BUILD, OnButtonDeleteBuild)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BUILDS, &CBuildsPane::OnSelchangedTreeBuilds)
    ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_BUILDS, &CBuildsPane::OnBeginlabeleditTreeBuilds)
    ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_BUILDS, &CBuildsPane::OnEndlabeleditTreeBuilds)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_BUILDS, &CBuildsPane::OnDblclkTreeBuilds)
    ON_UPDATE_COMMAND_UI_RANGE(ID_LEVELSELECT_1, ID_LEVELSELECT_40, &CBuildsPane::OnUpdateBuildLevel)
END_MESSAGE_MAP()

CBuildsPane::CBuildsPane() :
    CFormView(CBuildsPane::IDD),
    m_pDoc(NULL),
    m_pCharacter(NULL),
    m_hPopupMenuItem(NULL)
{
}

CBuildsPane::~CBuildsPane()
{
}

LRESULT CBuildsPane::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }

    // wParam is the document pointer
    CDDOBuilderDoc * pDoc = (CDDOBuilderDoc*)(wParam);
    m_pDoc = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
        // only add life starts enabled, overridden if we get a
        // selected item
        m_buttonDeleteLife.EnableWindow(false);
        m_buttonAddBuild.EnableWindow(false);
        m_buttonDeleteBuild.EnableWindow(false);
        PopulateBuildsList();
    }
    return 0L;
}

void CBuildsPane::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_NEW_LIFE, m_buttonNewLife);
    DDX_Control(pDX, IDC_BUTTON_DELETE_LIFE, m_buttonDeleteLife);
    DDX_Control(pDX, IDC_BUTTON_ADD_BUILD, m_buttonAddBuild);
    DDX_Control(pDX, IDC_BUTTON_DELETE_BUILD, m_buttonDeleteBuild);
    DDX_Control(pDX, IDC_TREE_BUILDS, m_treeBuilds);
}

void CBuildsPane::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);
    if (IsWindow(m_treeBuilds.GetSafeHwnd()))
    {
        // [Add][Del][Add][Del]
        // +------------------+
        // |Tree              |
        // |                  |
        // |                  |
        // |                  |
        // +------------------+
        CRect rctAddLife;
        CRect rctDelLife;
        CRect rctAddBuild;
        CRect rctDelBuild;
        CRect rctTree;
        m_buttonNewLife.GetWindowRect(rctAddLife);
        m_buttonDeleteLife.GetWindowRect(rctDelLife);
        m_buttonAddBuild.GetWindowRect(rctAddBuild);
        m_buttonDeleteBuild.GetWindowRect(rctDelBuild);
        rctAddLife -= rctAddLife.TopLeft();
        rctAddLife += CPoint(c_controlSpacing, c_controlSpacing);
        // note that following button bump down a line if not enough room
        // to be viewed fully
        rctDelLife -= rctDelLife.TopLeft();
        rctDelLife += CPoint(c_controlSpacing + rctAddLife.right, rctAddLife.top);
        if (rctDelLife.right > cx)
        {
            rctDelLife -= rctDelLife.TopLeft();
            rctDelLife += CPoint(c_controlSpacing, c_controlSpacing + rctAddLife.bottom);
        }

        rctAddBuild -= rctAddBuild.TopLeft();
        rctAddBuild += CPoint(c_controlSpacing + rctDelLife.right, rctDelLife.top);
        if (rctAddBuild.right > cx)
        {
            rctAddBuild -= rctAddBuild.TopLeft();
            rctAddBuild += CPoint(c_controlSpacing, c_controlSpacing + rctDelLife.bottom);
        }

        rctDelBuild -= rctDelBuild.TopLeft();
        rctDelBuild += CPoint(c_controlSpacing + rctAddBuild.right, rctAddBuild.top);
        if (rctDelBuild.right > cx)
        {
            rctDelBuild -= rctDelBuild.TopLeft();
            rctDelBuild += CPoint(c_controlSpacing, c_controlSpacing + rctAddBuild.bottom);
        }

        rctTree = CRect(
                c_controlSpacing,
                rctDelBuild.bottom + c_controlSpacing,
                cx - c_controlSpacing,
                cy - c_controlSpacing);
        // now we know where they need to be moved to, move them
        m_buttonNewLife.MoveWindow(rctAddLife, FALSE);
        m_buttonDeleteLife.MoveWindow(rctDelLife, FALSE);
        m_buttonAddBuild.MoveWindow(rctAddBuild, FALSE);
        m_buttonDeleteBuild.MoveWindow(rctDelBuild, FALSE);
        m_treeBuilds.MoveWindow(rctTree, FALSE);
    }
    SetScaleToFitSize(CSize(cx, cy));
}

BOOL CBuildsPane::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_BUTTON_NEW_LIFE,
        IDC_BUTTON_DELETE_LIFE,
        IDC_BUTTON_ADD_BUILD,
        IDC_BUTTON_DELETE_BUILD,
        IDC_TREE_BUILDS,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CBuildsPane::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    PopulateBuildsList();
}

void CBuildsPane::UpdateNumBuildsChanged(Character *)
{
    PopulateBuildsList();
}

void CBuildsPane::PopulateBuildsList()
{
    if (m_pCharacter != NULL)
    {
        // keep the same item selected if there was one
        HTREEITEM hSelItem = m_treeBuilds.GetSelectedItem();
        DWORD selItemData = MakeItemData(TEI_Build, 0, 0);  // cause life(0, 0) to start selected if no selection
        if (hSelItem != NULL)
        {
            selItemData = m_treeBuilds.GetItemData(hSelItem);
        }
        // the item data for each element in the tree is the life:build index
        m_treeBuilds.LockWindowUpdate();
        m_treeBuilds.DeleteAllItems();
        // add each life as a root element in the tree
        size_t lifeIndex = 0;
        const std::list<Life> & lives = m_pCharacter->Lives();
        std::list<Life>::const_iterator lit = lives.begin();
        while (lit != lives.end())
        {
            CString name = (*lit).UIDescription(lifeIndex);
            HTREEITEM hItem = m_treeBuilds.InsertItem(
                    name,
                    -1,
                    -1,
                    TVI_ROOT,
                    TVI_LAST);
            // the life is the first item in the build list
            DWORD itemId = MakeItemData(TEI_Life, lifeIndex, 0);
            m_treeBuilds.SetItemData(hItem, itemId);
            // now add each of the builds under this Life
            size_t buildIndex = 0;
            const std::list<Build> & builds = (*lit).Builds();
            std::list<Build>::const_iterator bit = builds.begin();
            while (bit != builds.end())
            {
                // each build in a life has a maximum level
                name = (*bit).UIDescription(buildIndex);
                HTREEITEM hSubItem = m_treeBuilds.InsertItem(
                        name,
                        -1,
                        -1,
                        hItem,
                        TVI_LAST);
                itemId = MakeItemData(TEI_Build, lifeIndex, buildIndex);
                m_treeBuilds.SetItemData(hSubItem, itemId);
                ++bit;
                ++buildIndex;
            }
            if (!(*lit).HasTreeCollapsed())
            {
                m_treeBuilds.Expand(hItem, TVE_EXPAND);
            }
            ++lit;
            ++lifeIndex;
        }
        // restore the selection if there was one
        SelectTreeItem(selItemData);
        m_treeBuilds.UnlockWindowUpdate();
    }
}

void CBuildsPane::OnButtonNewLife()
{
    size_t lifeIndex = m_pCharacter->AddLife();
    PopulateBuildsList();
    // new life, 1st build starts selected
    SelectTreeItem(TEI_Build, lifeIndex, 0);
    GetLog().AddLogEntry("New life added");
}

void CBuildsPane::OnButtonDeleteLife()
{
    HTREEITEM hItem = m_treeBuilds.GetSelectedItem();
    DWORD itemData = m_treeBuilds.GetItemData(hItem);
    size_t lifeIndex = ExtractLifeIndex(itemData);
    m_pCharacter->DeleteLife(lifeIndex);
    PopulateBuildsList();
    if (m_pCharacter->Lives().size() > 0)
    {
        if (m_pCharacter->Lives().size() > lifeIndex)
        {
            // just select the one that was next in the list
            SelectTreeItem(TEI_Life, lifeIndex, 0);
        }
        else
        {
            if (lifeIndex > 0)
            {
                // go to the previous life in the list
                SelectTreeItem(TEI_Life, lifeIndex-1, 0);
            }
        }
    }
    else
    {
        // no lives in character, no selection
        // ensure buttons are correct for no selection
        LRESULT result;
        OnSelchangedTreeBuilds(NULL, &result);
    }
    GetLog().AddLogEntry("Life deleted");
}

void CBuildsPane::OnButtonAddBuild()
{
    HTREEITEM hItem = m_treeBuilds.GetSelectedItem();
    DWORD itemData = m_treeBuilds.GetItemData(hItem);
    size_t lifeIndex = ExtractLifeIndex(itemData);
    size_t buildIndex = m_pCharacter->AddBuild(lifeIndex);
    PopulateBuildsList();
    // new build starts selected
    SelectTreeItem(TEI_Build, lifeIndex, buildIndex);
    GetLog().AddLogEntry("Build added to life");
}

void CBuildsPane::OnButtonDeleteBuild()
{
    HTREEITEM hItem = m_treeBuilds.GetSelectedItem();
    DWORD itemData = m_treeBuilds.GetItemData(hItem);
    size_t lifeIndex = ExtractLifeIndex(itemData);
    size_t buildIndex = ExtractBuildIndex(itemData);
    m_pCharacter->DeleteBuild(lifeIndex, buildIndex);
    PopulateBuildsList();
    const Life & life = m_pCharacter->GetLife(lifeIndex);
    if (life.Builds().size() > 0)
    {
        if (life.Builds().size() > buildIndex)
        {
            // just select the one that was next in the list
            SelectTreeItem(TEI_Build, lifeIndex, buildIndex);
        }
        else
        {
            if (buildIndex > 0)
            {
                // go to the previous Build in the list
                SelectTreeItem(TEI_Build, lifeIndex, buildIndex-1);
            }
        }
    }
    else
    {
        // no builds left in this life, drop back to the Life element being selected
        SelectTreeItem(TEI_Life, lifeIndex, 0);
    }
    GetLog().AddLogEntry("Build deleted from life");
}

void CBuildsPane::OnSelchangedTreeBuilds(NMHDR *, LRESULT *pResult)
{
    // new life is always available
    m_buttonNewLife.EnableWindow(true);
    // enable the button states based on what item is selected in the tree
    HTREEITEM hSelItem = m_treeBuilds.GetSelectedItem();
    if (hSelItem != NULL)
    {
        // we have a valid selection
        DWORD itemData = m_treeBuilds.GetItemData(hSelItem);
        TreeEntryItem type = ExtractType(itemData);
        size_t lifeIndex = ExtractLifeIndex(itemData);
        size_t buildIndex = ExtractBuildIndex(itemData);
        switch (type)
        {
        case TEI_Life:
            m_buttonDeleteLife.EnableWindow(true);
            m_buttonAddBuild.EnableWindow(true);
            m_buttonDeleteBuild.EnableWindow(false);
            break;
        case TEI_Build:
            m_buttonDeleteLife.EnableWindow(false);
            m_buttonAddBuild.EnableWindow(true);
            m_buttonDeleteBuild.EnableWindow(true);
            break;
        default:
            // fail!
            m_buttonDeleteLife.EnableWindow(false);
            m_buttonAddBuild.EnableWindow(false);
            m_buttonDeleteBuild.EnableWindow(false);
            break;
        }
        m_pCharacter->SetActiveBuild(lifeIndex, buildIndex);
    }
    else
    {
        // only New life available
        m_buttonDeleteLife.EnableWindow(false);
        m_buttonAddBuild.EnableWindow(false);
        m_buttonDeleteBuild.EnableWindow(false);
        m_pCharacter->SetActiveBuild(0, 0);
    }
    *pResult = 0;
}

void CBuildsPane::OnBeginlabeleditTreeBuilds(NMHDR *pNMHDR, LRESULT *pResult)
{
    // the user can only edit life names
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    DWORD itemData = m_treeBuilds.GetItemData(pTVDispInfo->item.hItem);
    TreeEntryItem type = ExtractType(itemData);
    *pResult = (type == TEI_Life)
            ? FALSE     // allow edit of life tree elements
            : TRUE;     // cancel edit of non-life tree elements
}

void CBuildsPane::OnEndlabeleditTreeBuilds(NMHDR *pNMHDR, LRESULT *pResult)
{
    // the user has renamed a life or build
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    DWORD itemData = m_treeBuilds.GetItemData(pTVDispInfo->item.hItem);
    CString strText = pTVDispInfo->item.pszText;
    CString strOldName = m_treeBuilds.GetItemText(pTVDispInfo->item.hItem);
    m_treeBuilds.SetItemText(pTVDispInfo->item.hItem, strText);
    // update the item
    TreeEntryItem type = ExtractType(itemData);
    size_t lifeIndex = ExtractLifeIndex(itemData);
    switch (type)
    {
        case TEI_Life:
            m_pCharacter->SetLifeName(lifeIndex, strText);
            break;
        default:
            // fail!
            break;
    }
    *pResult = 0;
    CString strLogText;
    strLogText.Format("Life renamed from \"%s\" to \"%s\"", (LPCSTR)strOldName, (LPCSTR)strText);
    GetLog().AddLogEntry(strLogText);
}

void CBuildsPane::SelectTreeItem(
        TreeEntryItem type,
        size_t lifeIndex,
        size_t buildIndex)
{
    SelectTreeItem(MakeItemData(type, lifeIndex, buildIndex));
}

void CBuildsPane::SelectTreeItem(DWORD itemData)
{
    // iterate the tree and find the item with this item data
    // if not found, select the default item
    bool found = false;
    HTREEITEM hItem = m_treeBuilds.GetChildItem(TVI_ROOT);
    while (!found && hItem != NULL)
    {
        DWORD dwItem = m_treeBuilds.GetItemData(hItem);
        CString strText = m_treeBuilds.GetItemText(hItem);
        if (dwItem == itemData)
        {
            // this is the one we want
            found = true;
        }
        else
        {
            hItem = GetNextTreeItem(m_treeBuilds, hItem);
        }
    }
    if (hItem != NULL)
    {
        m_treeBuilds.Select(hItem, TVGN_CARET);
        m_treeBuilds.EnsureVisible(hItem);
    }
}

void CBuildsPane::OnDblclkTreeBuilds(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pNMHDR);
    // identify the item that was double clicked, and if its a build item
    // allow them to set the level via a drop menu
    CPoint mouse;
    GetCursorPos(&mouse);
    m_treeBuilds.ScreenToClient(&mouse);
    UINT uFlags = 0;
    HTREEITEM hItem = m_treeBuilds.HitTest(mouse, &uFlags);
    if (hItem != NULL)
    {
        DWORD itemData = m_treeBuilds.GetItemData(hItem);
        size_t lifeIndex = ExtractLifeIndex(itemData);
        size_t buildIndex = ExtractBuildIndex(itemData);
        // display a popup menu to allow the user to select the level they want
        // this build to be at
        CMenu menuLevelSelect;
        menuLevelSelect.LoadMenu(IDR_LEVEL_SELECT_MENU);
        CMenu * pMenu = menuLevelSelect.GetSubMenu(0);
        // find where to display it
        CRect rectItem;
        m_treeBuilds.GetItemRect(hItem, &rectItem, FALSE);
        m_treeBuilds.ClientToScreen(&rectItem);
        int x = rectItem.left;
        int y = rectItem.bottom + 1;
        CWinAppEx * pApp = dynamic_cast<CWinAppEx*>(AfxGetApp());
        m_hPopupMenuItem = hItem;
        UINT sel = pApp->GetContextMenuManager()->TrackPopupMenu(
                pMenu->GetSafeHmenu(),
                x,
                y,
                this);
        if (sel != 0)
        {
            // only update if the level selected is different
            int selectedLevel = (sel - ID_LEVELSELECT_1) + 1; // 1 based
            int buildLevel = m_pCharacter->GetBuildLevel(lifeIndex, buildIndex);
            if (selectedLevel != buildLevel)
            {
                CString logEntry;
                logEntry.Format("Build changed from level %d to level %d", buildLevel, selectedLevel);
                GetLog().AddLogEntry(logEntry);
                // update the build
                m_pCharacter->SetBuildLevel(lifeIndex, buildIndex, selectedLevel);
                // update our UI element
                CString name = m_pCharacter->GetLife(lifeIndex).GetBuild(buildIndex).UIDescription(buildIndex);
                m_treeBuilds.SetItemText(
                        m_hPopupMenuItem,
                        name);
            }
        }
        m_hPopupMenuItem = NULL;
    }
    *pResult = 0;
}

void CBuildsPane::OnUpdateBuildLevel(CCmdUI* pCmdUI)
{
    // show a check mark against the builds current level selection
    DWORD itemData = m_treeBuilds.GetItemData(m_hPopupMenuItem);
    size_t lifeIndex = ExtractLifeIndex(itemData);
    size_t buildIndex = ExtractBuildIndex(itemData);
    int buildLevel = m_pCharacter->GetBuildLevel(lifeIndex, buildIndex);
    int menuItemLevel = (pCmdUI->m_nID - ID_LEVELSELECT_1) + 1;
    pCmdUI->SetCheck(menuItemLevel == buildLevel);
    // disable item for levels not yet supported in game
    pCmdUI->Enable(menuItemLevel <= MAX_GAME_LEVEL);
}
