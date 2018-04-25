///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __REAPERGUI_H__
#define __REAPERGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_PROJECT_CHOICE 1000
#define wxID_CHECKOUT 1001
#define wxID_CANCEL_CHECKOUT 1002
#define wxID_DIRECTORY 1003

///////////////////////////////////////////////////////////////////////////////
/// Class ReaperDialogBase
///////////////////////////////////////////////////////////////////////////////
class ReaperDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxChoice* m_projectChoice;
		wxButton* m_btnCheckOut;
		wxButton* m_btnCancel;
		wxStaticText* m_staticText2;
		wxDirPickerCtrl* m_CheckoutDirPicker;
		wxTextCtrl* m_reaperText;
		wxButton* m_btnExit;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnProjectChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelCheckout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ReaperDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Reaper V1.0"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 525,542 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~ReaperDialogBase();
	
};

#endif //__REAPERGUI_H__
