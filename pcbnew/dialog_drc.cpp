/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_drc.cpp
// Purpose:
// Author:      jean-pierre Charras
// Modified by:
// Created:     27/02/2006 20:42:00
// RCS-ID:
// Copyright:   License GNU
// Licence:
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 27/02/2006 20:42:00

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dialog_drc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include <wx/htmllbox.h>
#include <vector>

////@begin includes
////@end includes

#include "wxstruct.h"
#include "dialog_drc.h"

////@begin XPM images
////@end XPM images


/**
 * Class DRC_LIST_MARKERS
 * is an implementation of the interface named DRC_ITEM_LIST which uses
 * a BOARD instance to fulfill the interface.  No ownership is taken of the
 * BOARD.
 */
class DRC_LIST_MARKERS : public DRC_ITEM_LIST
{
    BOARD*          m_board;

public:

    DRC_LIST_MARKERS( BOARD* aBoard ) :
        m_board(aBoard)
    {
    }

    /* no destructor since we do not own anything to delete, not even the BOARD.
    ~DRC_LIST_MARKERS() {}
    */


    //-----<Interface DRC_ITEM_LIST>---------------------------------------

    void            DeleteAllItems()
    {
        m_board->DeleteMARKERs();
    }


    const DRC_ITEM* GetItem( int aIndex )
    {
        const MARKER* marker = m_board->GetMARKER( aIndex );
        if( marker )
            return &marker->GetReporter();
        return NULL;
    }

    void DeleteItem( int aIndex )
    {
        m_board->DeleteMARKER( aIndex );
    }


    /**
     * Function GetCount
     * returns the number of items in the list.
     */
    int  GetCount()
    {
        return m_board->GetMARKERCount();
    }

    //-----</Interface DRC_ITEM_LIST>--------------------------------------

};


/**
 * Class DRC_LIST_UNCONNECTED
 * is an implementation of the interface named DRC_ITEM_LIST which uses
 * a vector of pointers to DRC_ITEMs to fulfill the interface.  No ownership is taken of the
 * vector, which will reside in class DRC
 */
class DRC_LIST_UNCONNECTED : public DRC_ITEM_LIST
{
    DRC_LIST*         m_vector;

public:

    DRC_LIST_UNCONNECTED( DRC_LIST* aList ) :
        m_vector(aList)
    {
    }

    /* no destructor since we do not own anything to delete, not even the BOARD.
    ~DRC_LIST_UNCONNECTED() {}
    */


    //-----<Interface DRC_ITEM_LIST>---------------------------------------

    void            DeleteAllItems()
    {
        if( m_vector )
        {
            for( unsigned i=0; i<m_vector->size();  ++i )
                delete (*m_vector)[i];

            m_vector->clear();
        }
    }


    const DRC_ITEM* GetItem( int aIndex )
    {
        if( m_vector &&  (unsigned)aIndex < m_vector->size() )
        {
            const DRC_ITEM* item = (*m_vector)[aIndex];
            return item;
        }
        return NULL;
    }

    void DeleteItem( int aIndex )
    {
        if( m_vector &&  (unsigned)aIndex < m_vector->size() )
        {
            delete (*m_vector)[aIndex];
            m_vector->erase( m_vector->begin()+aIndex );
        }
    }


    /**
     * Function GetCount
     * returns the number of items in the list.
     */
    int  GetCount()
    {
        if( m_vector )
        {
            return m_vector->size();
        }
        return 0;
    }

    //-----</Interface DRC_ITEM_LIST>--------------------------------------

};



/**
 * Class DRCLISTBOX
 * is used to display a DRC_ITEM_LIST.
 */
class DRCLISTBOX : public wxHtmlListBox
{
private:
    DRC_ITEM_LIST* m_list;     ///< wxHtmlListBox does not own the list, I do

public:
    DRCLISTBOX( wxWindow* parent, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = wxVListBoxNameStr)
        : wxHtmlListBox( parent, id, pos, size, style, name )
    {
        m_list = 0;
    }


    ~DRCLISTBOX()
    {
        delete m_list;  // I own it, I destroy it.
    }


    /**
     * Function SetList
     * sets the DRC_ITEM_LIST for this listbox.  Ownership of the DRC_ITEM_LIST is
     * transfered to this DRCLISTBOX.
     * @param aList The DRC_ITEM_LIST* containing the DRC_ITEMs which will be
     *  displayed in the wxHtmlListBox
     */
    void SetList( DRC_ITEM_LIST* aList )
    {
        delete m_list;

        m_list = aList;
        SetItemCount( aList->GetCount() );
        Refresh();
    }


    /**
     * Function GetItem
     * returns a requested DRC_ITEM* or NULL.
     */
    const DRC_ITEM* GetItem( int aIndex )
    {
        if( m_list )
        {
            return m_list->GetItem( aIndex );
        }
        return NULL;
    }


    /**
     * Function OnGetItem
     * returns the html text associated with the DRC_ITEM given by index 'n'.
     * @param n An index into the list.
     * @return wxString - the simple html text to show in the listbox.
     */
    wxString OnGetItem( size_t n ) const
    {
        if( m_list )
        {
            const DRC_ITEM*   item = m_list->GetItem( (int) n );
            if( item )
                return item->ShowHtml();
        }
        return wxString();
    }


    /**
     * Function OnGetItem
     * returns the html text associated with the given index 'n'.
     * @param n An index into the list.
     * @return wxString - the simple html text to show in the listbox.
     */
    wxString OnGetItemMarkup( size_t n ) const
    {
        return OnGetItem( n );
    }


    /**
     * Function DeleteElement
     * will delete one of the items in the list.
     * @param aIndex The index into the list to delete.
     */
    void DeleteItem( int aIndex )
    {
        if( m_list )
        {
            int selection = GetSelection();

            m_list->DeleteItem( aIndex );
            int count = m_list->GetCount();
            SetItemCount( count );

            // if old selection >= new count
            if( selection >= count )
                SetSelection( count-1 );    // -1 is "no selection"
            Refresh();
        }
    }


    /**
     * Function DeleteAllItems
     * deletes all items in the list.
     */
    void DeleteAllItems()
    {
        if( m_list )
        {
            m_list->DeleteAllItems();
            SetItemCount(0);
            SetSelection( -1 );    // -1 is no selection
            Refresh();
        }
    }
};



/*!
 * DrcDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DrcDialog, wxDialog )

/*!
 * DrcDialog event table definition
 */

BEGIN_EVENT_TABLE( DrcDialog, wxDialog )

////@begin DrcDialog event table entries
    EVT_INIT_DIALOG( DrcDialog::OnInitDialog )

    EVT_CHECKBOX( ID_CHECKBOX, DrcDialog::OnReportCheckBoxClicked )

    EVT_BUTTON( ID_BUTTON_BROWSE_RPT_FILE, DrcDialog::OnButtonBrowseRptFileClick )

    EVT_BUTTON( ID_STARTDRC, DrcDialog::OnStartdrcClick )

    EVT_BUTTON( ID_LIST_UNCONNECTED, DrcDialog::OnListUnconnectedClick )

    EVT_BUTTON( ID_DELETE_ALL, DrcDialog::OnDeleteAllClick )

    EVT_BUTTON( ID_DELETE_ONE, DrcDialog::OnDeleteOneClick )

    EVT_BUTTON( wxID_CANCEL, DrcDialog::OnCancelClick )

    EVT_BUTTON( wxID_OK, DrcDialog::OnOkClick )

////@end DrcDialog event table entries


    // outside bracket: DialogBlocks does not know about the listbox events on a custom list box.
    EVT_LISTBOX( ID_CLEARANCE_LIST, DrcDialog::OnMarkerSelectionEvent)
    EVT_LISTBOX( ID_UNCONNECTED_LIST, DrcDialog::OnUnconnectedSelectionEvent)

    EVT_MENU( ID_POPUP_UNCONNECTED_A, DrcDialog::OnPopupMenu )
    EVT_MENU( ID_POPUP_UNCONNECTED_B, DrcDialog::OnPopupMenu )
    EVT_MENU( ID_POPUP_MARKERS_A, DrcDialog::OnPopupMenu )
    EVT_MENU( ID_POPUP_MARKERS_B, DrcDialog::OnPopupMenu )

END_EVENT_TABLE()

/*!
 * DrcDialog constructors
 */

DrcDialog::DrcDialog( )
{
}

DrcDialog::DrcDialog( DRC* aTester, WinEDA_PcbFrame* parent,
                                  wxWindowID id,
                                  const wxString& caption,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style )
{
    m_tester = aTester;

    m_Parent = parent;

    Create(parent, id, caption, pos, size, style);
}

/*!
 * WinEDA_DrcFrame creator
 */

bool DrcDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DrcDialog member initialisation
    m_MainSizer = NULL;
    m_CommandSizer = NULL;
    m_ClearenceTitle = NULL;
    m_SetClearance = NULL;
    m_CreateRptCtrl = NULL;
    m_RptFilenameCtrl = NULL;
    m_BrowseButton = NULL;
    m_Pad2PadTestCtrl = NULL;
    m_ZonesTestCtrl = NULL;
    m_UnconnectedTestCtrl = NULL;
    m_DeleteAllButton = NULL;
    m_DeleteCurrentMarkerButton = NULL;
    m_Notebook = NULL;
    m_ClearanceListBox = NULL;
    m_UnconnectedListBox = NULL;
    StdDialogButtonSizer = NULL;
////@end DrcDialog member initialisation


////@begin DrcDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DrcDialog creation

    return true;
}

/*!
 * Control creation for WinEDA_DrcFrame
 */

void DrcDialog::CreateControls()
{
    SetFont( *g_DialogFont );

////@begin DrcDialog content construction
    // Generated by DialogBlocks, Tue 04 Dec 2007 13:38:44 CST (unregistered)

    DrcDialog* itemDialog1 = this;

    m_MainSizer = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_MainSizer);

    m_CommandSizer = new wxBoxSizer(wxHORIZONTAL);
    m_MainSizer->Add(m_CommandSizer, 0, wxGROW|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxHORIZONTAL);
    m_CommandSizer->Add(itemStaticBoxSizer4, 3, wxGROW|wxTOP|wxBOTTOM, 8);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer4->Add(itemBoxSizer5, 2, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer6, 0, wxALIGN_LEFT|wxALL, 5);

    m_ClearenceTitle = new wxStaticText( itemDialog1, wxID_STATIC, _("Clearance"), wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE );
    itemBoxSizer6->Add(m_ClearenceTitle, 0, wxALIGN_TOP|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_SetClearance = new wxTextCtrl( itemDialog1, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxSize(144, -1), 0 );
    if (DrcDialog::ShowToolTips())
        m_SetClearance->SetToolTip(_("In the clearance units, enter the clearance distance"));
    itemBoxSizer6->Add(m_SetClearance, 1, wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    wxStaticBox* itemStaticBoxSizer9Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Create Report File"));
    wxStaticBoxSizer* itemStaticBoxSizer9 = new wxStaticBoxSizer(itemStaticBoxSizer9Static, wxHORIZONTAL);
    itemBoxSizer5->Add(itemStaticBoxSizer9, 1, wxGROW|wxALL, 5);

    m_CreateRptCtrl = new wxCheckBox( itemDialog1, ID_CHECKBOX, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_CreateRptCtrl->SetValue(false);
    if (DrcDialog::ShowToolTips())
        m_CreateRptCtrl->SetToolTip(_("Enable writing report to this file"));
    itemStaticBoxSizer9->Add(m_CreateRptCtrl, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_RptFilenameCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL3, _T(""), wxDefaultPosition, wxSize(250, -1), 0 );
    if (DrcDialog::ShowToolTips())
        m_RptFilenameCtrl->SetToolTip(_("Enter the report filename"));
    itemStaticBoxSizer9->Add(m_RptFilenameCtrl, 2, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_BrowseButton = new wxButton( itemDialog1, ID_BUTTON_BROWSE_RPT_FILE, _("..."), wxDefaultPosition, wxSize(35, -1), 0 );
    if (DrcDialog::ShowToolTips())
        m_BrowseButton->SetToolTip(_("Pick a filename interactively"));
    itemStaticBoxSizer9->Add(m_BrowseButton, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM|wxADJUST_MINSIZE, 5);

    wxStaticBox* itemStaticBoxSizer13Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Include Tests For:"));
    wxStaticBoxSizer* itemStaticBoxSizer13 = new wxStaticBoxSizer(itemStaticBoxSizer13Static, wxVERTICAL);
    itemStaticBoxSizer4->Add(itemStaticBoxSizer13, 0, wxGROW|wxALL, 5);

    m_Pad2PadTestCtrl = new wxCheckBox( itemDialog1, ID_CHECKBOX2, _("Pad to pad"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Pad2PadTestCtrl->SetValue(false);
    if (DrcDialog::ShowToolTips())
        m_Pad2PadTestCtrl->SetToolTip(_("Include tests for clearances between pad to pads"));
    itemStaticBoxSizer13->Add(m_Pad2PadTestCtrl, 0, wxGROW|wxALL, 5);

    m_ZonesTestCtrl = new wxCheckBox( itemDialog1, ID_CHECKBOX7, _("Zones"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ZonesTestCtrl->SetValue(false);
    if (DrcDialog::ShowToolTips())
        m_ZonesTestCtrl->SetToolTip(_("Include zones in clearance or unconnected tests"));
    itemStaticBoxSizer13->Add(m_ZonesTestCtrl, 0, wxGROW|wxALL, 5);

    m_UnconnectedTestCtrl = new wxCheckBox( itemDialog1, ID_CHECKBOX3, _("Unconnected pads"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UnconnectedTestCtrl->SetValue(false);
    if (DrcDialog::ShowToolTips())
        m_UnconnectedTestCtrl->SetToolTip(_("Find unconnected pads"));
    itemStaticBoxSizer13->Add(m_UnconnectedTestCtrl, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    m_CommandSizer->Add(itemBoxSizer17, 0, wxALIGN_TOP|wxALL, 5);

    wxButton* itemButton18 = new wxButton( itemDialog1, ID_STARTDRC, _("Start DRC"), wxDefaultPosition, wxDefaultSize, 0 );
    if (DrcDialog::ShowToolTips())
        itemButton18->SetToolTip(_("Start the Design Rule Checker"));
    itemButton18->SetForegroundColour(wxColour(202, 0, 0));
    itemBoxSizer17->Add(itemButton18, 0, wxGROW|wxALL, 5);

    wxButton* itemButton19 = new wxButton( itemDialog1, ID_LIST_UNCONNECTED, _("List Unconnected"), wxDefaultPosition, wxDefaultSize, 0 );
    if (DrcDialog::ShowToolTips())
        itemButton19->SetToolTip(_("List unconnected pads or tracks"));
    itemButton19->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer17->Add(itemButton19, 0, wxGROW|wxALL, 5);

    m_DeleteAllButton = new wxButton( itemDialog1, ID_DELETE_ALL, _("Delete All Markers"), wxDefaultPosition, wxDefaultSize, 0 );
    if (DrcDialog::ShowToolTips())
        m_DeleteAllButton->SetToolTip(_("Delete every marker"));
    m_DeleteAllButton->SetForegroundColour(wxColour(0, 128, 0));
    itemBoxSizer17->Add(m_DeleteAllButton, 0, wxGROW|wxALL, 5);

    m_DeleteCurrentMarkerButton = new wxButton( itemDialog1, ID_DELETE_ONE, _("Delete Current Marker"), wxDefaultPosition, wxDefaultSize, 0 );
    if (DrcDialog::ShowToolTips())
        m_DeleteCurrentMarkerButton->SetToolTip(_("Delete the marker selected in the listBox below"));
    m_DeleteCurrentMarkerButton->Enable(false);
    itemBoxSizer17->Add(m_DeleteCurrentMarkerButton, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( itemDialog1, wxID_STATIC, _("Error Messages:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MainSizer->Add(itemStaticText22, 0, wxGROW|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT|wxRAISED_BORDER );
#if !wxCHECK_VERSION(2,5,2)
    wxNotebookSizer* m_NotebookSizer = new wxNotebookSizer(m_Notebook);
#endif

    m_ClearanceListBox = new DRCLISTBOX( m_Notebook, ID_CLEARANCE_LIST, wxDefaultPosition, wxSize(100, 300), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    if (DrcDialog::ShowToolTips())
        m_ClearanceListBox->SetToolTip(_("MARKERs, double click any to go there in PCB, right click for popup menu"));

    m_Notebook->AddPage(m_ClearanceListBox, _("Distance Problem Markers"));

    m_UnconnectedListBox = new DRCLISTBOX( m_Notebook, ID_UNCONNECTED_LIST, wxDefaultPosition, wxSize(100, 100), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    if (DrcDialog::ShowToolTips())
        m_UnconnectedListBox->SetToolTip(_("A list of unconnected pads, right click for popup menu"));

    m_Notebook->AddPage(m_UnconnectedListBox, _("Unconnected"));

#if !wxCHECK_VERSION(2,5,2)
    m_MainSizer->Add(m_NotebookSizer, 5, wxGROW|wxALL, 5);
#else
    m_MainSizer->Add(m_Notebook, 5, wxGROW|wxALL, 5);
#endif

    StdDialogButtonSizer = new wxStdDialogButtonSizer;

    m_MainSizer->Add(StdDialogButtonSizer, 0, wxGROW|wxALL, 10);
    wxButton* itemButton27 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton27->SetForegroundColour(wxColour(0, 0, 255));
    StdDialogButtonSizer->AddButton(itemButton27);

    wxButton* itemButton28 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton28->SetDefault();
    StdDialogButtonSizer->AddButton(itemButton28);

    StdDialogButtonSizer->Realize();

    // Connect events and objects
    m_ClearanceListBox->Connect(ID_CLEARANCE_LIST, wxEVT_LEFT_DCLICK, wxMouseEventHandler(DrcDialog::OnLeftDClickClearance), NULL, this);
    m_ClearanceListBox->Connect(ID_CLEARANCE_LIST, wxEVT_RIGHT_UP, wxMouseEventHandler(DrcDialog::OnRightUpClearance), NULL, this);
    m_UnconnectedListBox->Connect(ID_UNCONNECTED_LIST, wxEVT_LEFT_DCLICK, wxMouseEventHandler(DrcDialog::OnLeftDClickUnconnected), NULL, this);
    m_UnconnectedListBox->Connect(ID_UNCONNECTED_LIST, wxEVT_RIGHT_UP, wxMouseEventHandler(DrcDialog::OnRightUpUnconnected), NULL, this);
////@end DrcDialog content construction

    AddUnitSymbol(*m_ClearenceTitle);

    Layout();      // adding the units above expanded Clearance text, now resize.
}

/*!
 * Should we show tooltips?
 */

bool DrcDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap DrcDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DrcDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DrcDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon DrcDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DrcDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DrcDialog icon retrieval
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DRC_RUN
 */

void DrcDialog::OnStartdrcClick( wxCommandEvent& event )
{
    wxString reportName;

    if( m_CreateRptCtrl->IsChecked() )      // Create a file rpt
    {
        reportName = m_RptFilenameCtrl->GetValue();

        if( reportName.IsEmpty() )
        {
            wxCommandEvent junk;
            OnButtonBrowseRptFileClick( junk );
        }

        reportName = m_RptFilenameCtrl->GetValue();
    }

    g_DesignSettings.m_TrackClearence =
        ReturnValueFromTextCtrl( *m_SetClearance, m_Parent->m_InternalUnits );

    m_tester->SetSettings( m_Pad2PadTestCtrl->IsChecked(),
                        m_UnconnectedTestCtrl->IsChecked(),
                        m_ZonesTestCtrl->IsChecked(),
                        reportName, m_CreateRptCtrl->IsChecked() );

    DelDRCMarkers();

    wxBeginBusyCursor();

    // running the module editor and selecting "Update module in current board"
    // causes the list to become obsolete because of the new pads from the
    // revised module.
    m_Parent->build_liste_pads();

    // run all the tests, with no UI at this time.
    m_tester->RunTests();

#if wxCHECK_VERSION( 2, 8, 0 )
    m_Notebook->ChangeSelection(0);     // display the 1at tab "...Markers ..."
#else
    m_Notebook->SetSelection(0);        // display the 1at tab "... Markers..."
#endif


    // Generate the report
    if( !reportName.IsEmpty() )
    {
        FILE* fp = wxFopen( reportName, wxT( "w" ) );

        writeReport( fp );

        fclose(fp);

        wxString msg;

        msg.Printf( _( "Report file \"%s\" created" ), reportName.GetData() );

        wxString caption( _("Disk File Report Completed") );

        wxMessageDialog popupWindow( this, msg, caption );

        popupWindow.ShowModal();
    }

    wxEndBusyCursor();

    RedrawDrawPanel();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ERASE_DRC_MARKERS
 */

void DrcDialog::OnDeleteAllClick( wxCommandEvent& event )
{
    DelDRCMarkers();
    RedrawDrawPanel();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LIST_UNCONNECTED_PADS
 */

void DrcDialog::OnListUnconnectedClick( wxCommandEvent& event )
{
    wxString reportName;

    if( m_CreateRptCtrl->IsChecked() )      // Create a file rpt
    {
        reportName = m_RptFilenameCtrl->GetValue();

        if( reportName.IsEmpty() )
        {
            wxCommandEvent junk;
            OnButtonBrowseRptFileClick( junk );
        }

        reportName = m_RptFilenameCtrl->GetValue();
    }

    g_DesignSettings.m_TrackClearence =
        ReturnValueFromTextCtrl( *m_SetClearance, m_Parent->m_InternalUnits );

    m_tester->SetSettings( m_Pad2PadTestCtrl->IsChecked(),
                        m_UnconnectedTestCtrl->IsChecked(),
                        m_ZonesTestCtrl->IsChecked(),
                        reportName, m_CreateRptCtrl->IsChecked() );

    DelDRCMarkers();

    wxBeginBusyCursor();

    m_tester->ListUnconnectedPads();

#if wxCHECK_VERSION( 2, 8, 0 )
    m_Notebook->ChangeSelection(1);     // display the 2nd tab "Unconnected..."
#else
    m_Notebook->SetSelection(1);        // display the 2nd tab "Unconnected..."
#endif

    // Generate the report
    if( !reportName.IsEmpty() )
    {
        FILE* fp = wxFopen( reportName, wxT( "w" ) );

        writeReport( fp );

        fclose(fp);

        wxString msg;

        msg.Printf( _( "Report file \"%s\" created" ), reportName.GetData() );

        wxString caption( _("Disk File Report Completed") );

        wxMessageDialog popupWindow( this, msg, caption );

        popupWindow.ShowModal();
    }

    wxEndBusyCursor();

    /* there is currently nothing visible on the DrawPanel for unconnected pads
    RedrawDrawPanel();
    */
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSE_RPT_FILE
 */

void DrcDialog::OnButtonBrowseRptFileClick( wxCommandEvent& event )
{
    wxString    FileName;
    wxString    Mask(wxT("*"));
    wxString    Ext(wxT(".rpt"));

    FileName = m_Parent->m_CurrentScreen->m_FileName;
    ChangeFileNameExt(FileName, wxT("-drc") + Ext);
    Mask += Ext;

    FileName = EDA_FileSelector( _("DRC Report file"),
                                 wxEmptyString,     /* Chemin par defaut */
                                 FileName,          /* nom fichier par defaut */
                                 Ext,               /* extension par defaut */
                                 Mask,              /* Masque d'affichage */
                                 this,
                                 wxFD_SAVE,
                                 TRUE
                                 );
    if( FileName.IsEmpty() )
        return;

    m_RptFilenameCtrl->SetValue(FileName);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void DrcDialog::OnOkClick( wxCommandEvent& event )
{
#if defined(DEBUG)
    printf("OK Button handler\n");
#endif

    SetReturnCode( wxID_OK );
    m_tester->DestroyDialog( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void DrcDialog::OnCancelClick( wxCommandEvent& event )
{
#if defined(DEBUG)
    printf("Cancel Button handler\n");
#endif

    SetReturnCode( wxID_CANCEL );
    m_tester->DestroyDialog( wxID_CANCEL );
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX1
 */

void DrcDialog::OnReportCheckBoxClicked( wxCommandEvent& event )
{
    if( m_CreateRptCtrl->IsChecked() )
    {
        m_RptFilenameCtrl->Enable(true);
        m_BrowseButton->Enable(true);
    }
    else
    {
        m_RptFilenameCtrl->Enable(false);
        m_BrowseButton->Enable(false);
    }
//    event.Skip();
}


/*!
 * wxEVT_INIT_DIALOG event handler for ID_DIALOG
 */

void DrcDialog::OnInitDialog( wxInitDialogEvent& event )
{
    wxCommandEvent junk;

    // Set the initial "enabled" status of the browse button and the text
    // field for report name
    OnReportCheckBoxClicked( junk );

    m_SetClearance->SetFocus();

    // deselect the existing text, seems SetFocus() wants to emulate
    // Microsoft and select all text, which is not desireable here.
    m_SetClearance->SetSelection(0,0);

    event.Skip();
}


/*!
 * wxEVT_LEFT_DCLICK event handler for ID_CLEARANCE_LIST
 */

void DrcDialog::OnLeftDClickClearance( wxMouseEvent& event )
{
    event.Skip();

    // I am assuming that the double click actually changed the selected item.
    // please verify this.
    int selection = m_ClearanceListBox->GetSelection();

    if( selection != wxNOT_FOUND )
    {
        // Find the selected MARKER in the PCB, position cursor there.
        // Then close the dialog.
        const DRC_ITEM* item = m_ClearanceListBox->GetItem( selection );
        if( item )
        {
            /*
            // after the goto, process a button OK command later.
            wxCommandEvent  cmd( wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK );
            ::wxPostEvent( GetEventHandler(), cmd );
            */

            m_Parent->CursorGoto( item->GetPosition() );

            // turn control over to m_Parent, hide this DrcDialog window,
            // no destruction so we can preserve listbox cursor
            Hide();

            event.StopPropagation();    // still get the popup window.
        }
    }
}


void DrcDialog::OnPopupMenu( wxCommandEvent& event )
{
    int source = event.GetId();

    const DRC_ITEM* item = 0;
    wxPoint         pos;

    int selection;

    switch( source )
    {
    case ID_POPUP_UNCONNECTED_A:
        selection = m_UnconnectedListBox->GetSelection();
        item = m_UnconnectedListBox->GetItem( selection );
        pos  = item->GetPointA();
        break;
    case ID_POPUP_UNCONNECTED_B:
        selection = m_UnconnectedListBox->GetSelection();
        item = m_UnconnectedListBox->GetItem( selection );
        pos  = item->GetPointB();
        break;
    case ID_POPUP_MARKERS_A:
        selection = m_ClearanceListBox->GetSelection();
        item = m_ClearanceListBox->GetItem( selection );
        pos  = item->GetPointA();
        break;
    case ID_POPUP_MARKERS_B:
        selection = m_ClearanceListBox->GetSelection();
        item = m_ClearanceListBox->GetItem( selection );
        pos  = item->GetPointB();
        break;
    }

    if( item )
    {
        m_Parent->CursorGoto( pos );
        Hide();
    }
}



/*!
 * wxEVT_RIGHT_UP event handler for ID_CLEARANCE_LIST
 */

void DrcDialog::OnRightUpUnconnected( wxMouseEvent& event )
{
    event.Skip();

    // popup menu to go to either of the items listed in the DRC_ITEM.

    int selection = m_UnconnectedListBox->GetSelection();

    if( selection != wxNOT_FOUND )
    {
        wxMenu          menu;
        wxMenuItem*     mItem;
        const DRC_ITEM* dItem = m_UnconnectedListBox->GetItem( selection );

        mItem = new wxMenuItem( &menu, ID_POPUP_UNCONNECTED_A,  dItem->GetTextA() );
        menu.Append( mItem );

        if( dItem->HasSecondItem() )
        {
            mItem = new wxMenuItem( &menu, ID_POPUP_UNCONNECTED_B,  dItem->GetTextB() );
            menu.Append( mItem );
        }

        PopupMenu( &menu );
    }
}


/*!
 * wxEVT_RIGHT_UP event handler for ID_CLEARANCE_LIST
 */

void DrcDialog::OnRightUpClearance( wxMouseEvent& event )
{
    event.Skip();

    // popup menu to go to either of the items listed in the DRC_ITEM.

    int selection = m_ClearanceListBox->GetSelection();

    if( selection != wxNOT_FOUND )
    {
        wxMenu          menu;
        wxMenuItem*     mItem;
        const DRC_ITEM* dItem = m_ClearanceListBox->GetItem( selection );

        mItem = new wxMenuItem( &menu, ID_POPUP_MARKERS_A,  dItem->GetTextA() );
        menu.Append( mItem );

        if( dItem->HasSecondItem() )
        {
            mItem = new wxMenuItem( &menu, ID_POPUP_MARKERS_B,  dItem->GetTextB() );
            menu.Append( mItem );
        }

        PopupMenu( &menu );
    }
}


/*!
 * wxEVT_LEFT_DCLICK event handler for ID_UNCONNECTED_LIST
 */

void DrcDialog::OnLeftDClickUnconnected( wxMouseEvent& event )
{
    event.Skip();

    // I am assuming that the double click actually changed the selected item.
    // please verify this.
    int selection = m_UnconnectedListBox->GetSelection();

    if( selection != wxNOT_FOUND )
    {
        // Find the selected DRC_ITEM in the listbox, position cursor there,
        // at the first of the two pads.
        // Then hide the dialog.
        const DRC_ITEM* item = m_UnconnectedListBox->GetItem( selection );
        if( item )
        {
            m_Parent->CursorGoto( item->GetPosition() );

            Hide();

            // intermittently, still get the popup window, even with this.
            event.StopPropagation();
        }
    }
}


void DrcDialog::OnMarkerSelectionEvent( wxCommandEvent& event )
{
    int selection = event.GetSelection();

    if( selection != wxNOT_FOUND )
    {
        // until a MARKER is selected, this button is not enabled.
        m_DeleteCurrentMarkerButton->Enable(true);
    }

    event.Skip();
}

void DrcDialog::OnUnconnectedSelectionEvent( wxCommandEvent& event )
{
    int selection = event.GetSelection();

    if( selection != wxNOT_FOUND )
    {
        // until a MARKER is selected, this button is not enabled.
        m_DeleteCurrentMarkerButton->Enable(true);
    }

    event.Skip();
}


void DrcDialog::RedrawDrawPanel()
{
    m_Parent->DrawPanel->Refresh();
}


/*********************************************************/
void DrcDialog::DelDRCMarkers()
/*********************************************************/
{
    m_ClearanceListBox->DeleteAllItems();
    m_UnconnectedListBox->DeleteAllItems();
}


void DrcDialog::writeReport( FILE* fp )
{
    int count;

    fprintf( fp, "** Drc report for %s **\n",
            CONV_TO_UTF8( m_Parent->GetScreen()->m_FileName ) );

    wxDateTime now = wxDateTime::Now();

    fprintf( fp, "** Created on %s **\n", CONV_TO_UTF8(now.Format( wxT("%F %T"))) );

    count = m_ClearanceListBox->GetItemCount();

    fprintf( fp, "\n** Found %d DRC errors **\n", count );

    for( int i=0;  i<count;  ++i )
        fprintf( fp, m_ClearanceListBox->GetItem(i)->ShowReport().mb_str() );

    count = m_UnconnectedListBox->GetItemCount();

    fprintf( fp, "\n** Found %d unconnected pads **\n", count );

    for( int i=0;  i<count;  ++i )
        fprintf( fp, m_UnconnectedListBox->GetItem(i)->ShowReport().mb_str() );

    fprintf( fp, "\n** End of Report **\n" );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DELETE_ONE
 */

void DrcDialog::OnDeleteOneClick( wxCommandEvent& event )
{
    int selectedIndex;
    int curTab =  m_Notebook->GetSelection();

    if( curTab == 0 )
    {
        selectedIndex = m_ClearanceListBox->GetSelection();
        if( selectedIndex != wxNOT_FOUND )
        {
            m_ClearanceListBox->DeleteItem( selectedIndex );

            // redraw the pcb
            RedrawDrawPanel();
        }
    }

    else if( curTab == 1 )
    {
        selectedIndex = m_UnconnectedListBox->GetSelection();
        if( selectedIndex != wxNOT_FOUND )
        {
            m_UnconnectedListBox->DeleteItem( selectedIndex );

            /* these unconnected DRC_ITEMs are not currently visible on the pcb
            RedrawDrawPanel();
            */
        }
    }

//    event.Skip();
}

