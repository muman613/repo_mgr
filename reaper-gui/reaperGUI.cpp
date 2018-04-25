///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "reaperGUI.h"

///////////////////////////////////////////////////////////////////////////

ReaperDialogBase::ReaperDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Project Info") ), wxVERTICAL );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Project :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString m_projectChoiceChoices;
	m_projectChoice = new wxChoice( this, wxID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, m_projectChoiceChoices, 0 );
	m_projectChoice->SetSelection( 0 );
	bSizer1->Add( m_projectChoice, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_btnCheckOut = new wxButton( this, wxID_CHECKOUT, wxT("&Check Out"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_btnCheckOut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_btnCancel = new wxButton( this, wxID_CANCEL_CHECKOUT, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_btnCancel, 0, wxALL, 5 );
	
	
	sbSizer1->Add( bSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Checkout Directory :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_CheckoutDirPicker = new wxDirPickerCtrl( this, wxID_DIRECTORY, wxT("/home/muman2"), wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE );
	bSizer2->Add( m_CheckoutDirPicker, 2, wxALL, 5 );
	
	
	sbSizer1->Add( bSizer2, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer4->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Output") ), wxVERTICAL );
	
	m_reaperText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	m_reaperText->SetFont( wxFont( 10, 76, 90, 90, false, wxT("Courier 10 Pitch") ) );
	
	sbSizer2->Add( m_reaperText, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_btnExit = new wxButton( this, wxID_EXIT, wxT("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_btnExit, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	sbSizer2->Add( bSizer5, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer4->Add( sbSizer2, 1, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ReaperDialogBase::OnClose ) );
	this->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ReaperDialogBase::OnUpdateUI ) );
	m_projectChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ReaperDialogBase::OnProjectChoice ), NULL, this );
	m_btnCheckOut->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReaperDialogBase::OnCheckout ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReaperDialogBase::OnCancelCheckout ), NULL, this );
	m_CheckoutDirPicker->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( ReaperDialogBase::OnDirChanged ), NULL, this );
	m_btnExit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReaperDialogBase::OnExit ), NULL, this );
}

ReaperDialogBase::~ReaperDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ReaperDialogBase::OnClose ) );
	this->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ReaperDialogBase::OnUpdateUI ) );
	m_projectChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ReaperDialogBase::OnProjectChoice ), NULL, this );
	m_btnCheckOut->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReaperDialogBase::OnCheckout ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReaperDialogBase::OnCancelCheckout ), NULL, this );
	m_CheckoutDirPicker->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( ReaperDialogBase::OnDirChanged ), NULL, this );
	m_btnExit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReaperDialogBase::OnExit ), NULL, this );
	
}
