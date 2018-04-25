#include <wx/wx.h>
#include <stdarg.h>
#include "sqlite3.h"
#include "functions.h"
#include "CheckoutThread.h"
#include "ReaperDialog.h"

ReaperDialog::ReaperDialog( wxWindow* parent )
:   ReaperDialogBase( parent ),
    m_pDBHandle(0L)
{
    wxString sCwd= ::wxGetCwd();

    wxLogDebug("ReaperDialog::ReaperDialog()");

    wxString sTitle = wxString::Format("reaper-gui v%d.%d",REAPER_VERSION_MAJOR,REAPER_VERSION_MINOR);
    SetTitle( sTitle );

    this->Connect(CHECKOUT_EVENT, wxCommandEventHandler(ReaperDialog::OnCheckoutEvent));
    this->Connect(TEXT_EVENT, wxCommandEventHandler(ReaperDialog::OnTextEvent));

    /* initialize output mechanism with the c interface. */
    set_output_char_func(_outputChar);
    set_printf_func(_printf);
    set_user_data((void*)this);

    m_CheckoutDirPicker->SetPath( sCwd );

    if (get_repodb_path(m_sRepoDbPath) == true) {
        wxLogDebug("Repodatabase path = %s", m_sRepoDbPath);

        if (open_database(m_sRepoDbPath, &m_pDBHandle)) {
            if (get_branch_vector(m_pDBHandle, m_branchVec)) {
                wxLogDebug("-- found %zu branches...", m_branchVec.size());
                populate_choice_control();
            } else {
                wxLogDebug("ERROR: Unable to get branch vector!");
            }
        } else {
            wxLogDebug("ERROR: Unable to open database!");
        }
    } else {
        wxLogDebug("ERROR: Unable to find repo database!");
    }


    InsertLogo();

    if (m_pDBHandle == NULL) {
        AddTextToOutput("Unable to open repository database!\n");
    } else {
        std::string sVersion, sDate;

        if (get_database_version(m_pDBHandle, sVersion, sDate)) {
            AddTextToOutput("Database version %s [%s]\n", sVersion.c_str(), sDate.c_str());
        }
    }
}

void ReaperDialog::OnClose(wxCloseEvent& event) {
    wxLogDebug("ReaperDialog::OnClose()");
    Destroy();
}

void ReaperDialog::OnExit(wxCommandEvent& event) {
    wxLogDebug("ReaperDialog::OnExit()");
    Close();
}

//void ReaperDialog::AddTextToOutput(wxString sMessage) {
//    m_reaperText->AppendText(sMessage + "\n");
//}

void ReaperDialog::AddTextToOutput(wxString sFormat, ...) {
    va_list     list;
    wxString    sText;

    va_start(list, sFormat);

    sText = wxString::FormatV(sFormat, list);

    m_reaperText->AppendText( sText );
    m_reaperText->Refresh();
    m_reaperText->Update();
    va_end(list);
}

void ReaperDialog::InsertLogo() {
    unsigned char _logo_txt[] = {
      0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x0a, 0x7c, 0x20, 0x5f, 0x20, 0x5c, 0x5f, 0x5f,
      0x5f, 0x20, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x5f, 0x20,
      0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x0a, 0x7c, 0x20,
      0x20, 0x20, 0x2f, 0x20, 0x2d, 0x5f, 0x29, 0x20, 0x5f, 0x60, 0x20, 0x7c,
      0x20, 0x27, 0x5f, 0x20, 0x5c, 0x2f, 0x20, 0x2d, 0x5f, 0x29, 0x20, 0x27,
      0x5f, 0x7c, 0x0a, 0x7c, 0x5f, 0x7c, 0x5f, 0x5c, 0x5f, 0x5f, 0x5f, 0x5c,
      0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x20, 0x2e, 0x5f, 0x5f, 0x2f, 0x5c, 0x5f,
      0x5f, 0x5f, 0x7c, 0x5f, 0x7c, 0x20, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f, 0x7c,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x00,
    };

    AddTextToOutput("%s V%d.%d\n", _logo_txt,
                    REAPER_VERSION_MAJOR, REAPER_VERSION_MINOR);

    return;
}

void ReaperDialog::OnProjectChoice( wxCommandEvent& event ) {
    wxLogDebug("ReaperDialog::OnProjectChoice()");
}

void ReaperDialog::OnCheckout( wxCommandEvent& event ) {
    CHECKOUT_PARMS      parms;
//    int                 err = ERROR_NO_ERROR;
    wxString            sRepository;
    wxString            sDirectory;
    wxString            sSubDir;
    size_t              offset;

    wxLogDebug("ReaperDialog::OnCheckout()");

    sRepository = m_projectChoice->GetString(m_projectChoice->GetSelection());

    if ((offset = sRepository.find_last_of('/')) != (size_t)wxNOT_FOUND) {
        sSubDir = sRepository.substr(offset+1);
    } else {
        sSubDir = sRepository;
    }

    sDirectory  = m_CheckoutDirPicker->GetPath() + "/" + sSubDir;

    wxLogDebug("checking out %s", sRepository);
    wxLogDebug("subdir = %s", sSubDir);
    wxLogDebug("checkout path = %s", sDirectory);

    parms.m_entryVector         = &m_branchVec;
    parms.m_bDoCheckout         = true;
    parms.m_sBranchName         = sRepository.c_str();
    parms.m_sCheckoutDir        = sDirectory.c_str();
    parms.m_sWorkingBranchName  = "working_branch";
    parms.m_sCheckoutDate.clear();

    m_pThread = new CheckoutThread(this, parms);

    wxLogDebug("-- spawning checkout thread!");
    if (m_pThread != 0) {
        m_pThread->Run();
    }
}

void ReaperDialog::OnDirChanged( wxFileDirPickerEvent& event ) {
    wxLogDebug("ReaperDialog::OnDirChanged()");
}

void ReaperDialog::OnUpdateUI( wxUpdateUIEvent& event ) {
    //wxLogDebug("ReaperDialog::OnUpdateUI()");

    switch (event.GetId()) {
    case wxID_EXIT:
    case wxID_DIRECTORY:
    case wxID_CHECKOUT:
    case wxID_PROJECT_CHOICE:
        event.Enable( m_pThread == NULL );
        break;
    case wxID_CANCEL_CHECKOUT:
        event.Enable( m_pThread != NULL );
        break;
    default:
        break;
    }
}

void ReaperDialog::populate_choice_control() {
    REAPERENTRY_VECTOR_ITER rIter;
    wxLogDebug("ReaperDialog::populate_choice_control()");

    m_projectChoice->Clear();

    for (rIter = m_branchVec.begin() ; rIter != m_branchVec.end() ; rIter++) {
        m_projectChoice->Append( (*rIter).m_sBranchSpec.c_str() );
    }

    m_projectChoice->SetSelection( 0 );
}

void ReaperDialog::_outputChar(char ch, void* pUserData) {
    ReaperDialog* pThis = (ReaperDialog*)pUserData;

    pThis->outputChar(ch);
}

/**
 *
 */

void ReaperDialog::outputChar(char ch) {

    m_sOutputLine << ch;

    if ((ch == '\n') || (ch == 0x08)) {
//        AddTextToOutput("%s", (const char*)m_sOutputLine.c_str());
        wxCommandEvent      event(TEXT_EVENT);

        event.SetString( m_sOutputLine );
        wxPostEvent(this, event);

        m_sOutputLine.Clear();
    }
}

/**
 *
 */

void ReaperDialog::_printf(void* pUserData, const char* sFormat, ...) {
    va_list varargs;

    va_start(varargs, sFormat);

    ReaperDialog* pThis = (ReaperDialog*)pUserData;
    assert(pThis != 0L);

    pThis->vprintf(sFormat, varargs);

    va_end(varargs);

    return;
}

/**
 *
 */

void ReaperDialog::vprintf(const char* sFormat, va_list varargs) {
    wxString sText;

    sText = wxString::FormatV(sFormat, varargs);

//    AddTextToOutput("%s", (const char*)sText.c_str());
    wxCommandEvent      event(TEXT_EVENT);
    event.SetString( sText );
    wxPostEvent(this, event);

    return;
}

/**
 *
 */

void ReaperDialog::SaveSettings() {
    wxLogDebug("ReaperDialog::SaveSettings()");
}

/**
 *
 */

void ReaperDialog::RestoreSettings() {
    wxLogDebug("ReaperDialog::RestoreSettings()");
}

/**
 *
 */

void ReaperDialog::OnCheckoutEvent( wxCommandEvent& event ) {
    int res = event.GetInt();

    wxLogDebug("ReaperDialog::OnCheckoutEvent() res=%d", res);

//  delete m_pThread;
    m_pThread = 0L;

    if (res == ERROR_NO_ERROR) {
        AddTextToOutput("%s\n", "Repository checked out...");
    } else if (res == ERROR_OPERATION_CANCELED) {
        AddTextToOutput("%s\n", "Check out cancelled by user...");
    }

    return;
}

/**
 *
 */

void ReaperDialog::OnTextEvent( wxCommandEvent& event ) {
    wxString sText = event.GetString();
    AddTextToOutput(sText);
    return;
}

void ReaperDialog::OnCancelCheckout( wxCommandEvent& event ) {
    wxLogDebug("ReaperDialog::OnCancelCheckout()");

    m_pThread->Delete();

    return;
}
