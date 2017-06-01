///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 14 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __QUINCYDIALOGS_H__
#define __QUINCYDIALOGS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/spinctrl.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/radiobut.h>
#include <wx/statbox.h>
#include <wx/grid.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/dialog.h>
#include <wx/combobox.h>
#include <wx/radiobox.h>
#include <wx/filepicker.h>
#include <wx/listbox.h>
#include <wx/html/htmlwin.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_LAST 1000

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlg
///////////////////////////////////////////////////////////////////////////////
class SettingsDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_Notebook;
		wxPanel* m_pBuild;
		wxStaticText* m_lblTargetHost;
		wxChoice* m_TargetHost;
		wxStaticText* m_lblDebugLevel;
		wxChoice* m_DebugLevel;
		wxStaticText* m_lblOptimization;
		wxChoice* m_Optimization;
		wxCheckBox* m_OverlayCode;
		wxCheckBox* m_chkStandardAMXname;
		wxCheckBox* m_chkVerbose;
		wxCheckBox* m_chkCreateReport;
		wxStaticLine* m_staticline2;
		wxStaticText* m_lblDefines;
		wxTextCtrl* m_Defines;
		wxStaticText* m_lblIncludes;
		wxTextCtrl* m_IncludePath;
		wxStaticText* m_lblOutputPath;
		wxTextCtrl* m_OutputPath;
		wxStaticText* m_lblBinPath;
		wxTextCtrl* m_BinPath;
		wxStaticText* m_lblPreBuild;
		wxTextCtrl* m_PreBuild;
		wxStaticText* m_lblMiscCmdLine;
		wxTextCtrl* m_MiscCmdOptions;
		wxPanel* m_pEdit;
		wxStaticText* m_lblTabWidth;
		wxSpinCtrl* m_TabWidth;
		wxCheckBox* m_UseTabs;
		wxStaticText* m_lblAutoIndent;
		wxCheckBox* m_AutoIndent;
		wxCheckBox* m_IndentPastedBlocks;
		wxStaticText* m_lblFont;
		wxFontPickerCtrl* m_EditFont;
		wxStaticLine* m_EditHorLine;
		wxStaticText* m_lblBackground;
		wxColourPickerCtrl* m_clrBackground;
		wxStaticText* m_lblActiveLine;
		wxColourPickerCtrl* m_clrActiveLine;
		wxStaticText* m_lblNormalText;
		wxColourPickerCtrl* m_clrNormalText;
		wxStaticText* m_lblOperators;
		wxColourPickerCtrl* m_clrOperators;
		wxStaticText* m_lblKeywords;
		wxColourPickerCtrl* m_clrKeywords;
		wxStaticText* m_lblNumbers;
		wxColourPickerCtrl* m_clrNumbers;
		wxStaticText* m_lblPreprocessor;
		wxColourPickerCtrl* m_clrPreprocessor;
		wxStaticText* m_lblStrings;
		wxColourPickerCtrl* m_clrStrings;
		wxStaticText* m_lblComments;
		wxColourPickerCtrl* m_clrComments;
		wxStaticText* m_lblStringEOL;
		wxColourPickerCtrl* m_clrStringEOL;
		wxStaticText* m_lblHighlight;
		wxColourPickerCtrl* m_clrHighlight;
		wxPanel* m_pDebug;
		wxRadioButton* m_optDebugLocal;
		wxRadioButton* m_optDebugRS232;
		wxStaticText* m_lblPort;
		wxChoice* m_ctrlPort;
		wxStaticText* m_lblBaudRate;
		wxChoice* m_ctrlBaudRate;
		wxStaticText* m_lblConnInfo;
		wxCheckBox* m_chkEnableLogging;
		wxPanel* m_pSnippets;
		wxGrid* m_gridSnippets;
		wxStaticText* m_lblSnippet;
		wxPanel* m_pKeyboard;
		wxPropertyGrid* m_KbdShortcuts;
		wxPanel* m_pOptions;
		wxCheckBox* m_StripTrailingWhitespace;
		wxCheckBox* m_ModalFind;
		wxCheckBox* m_LocalIniFile;
		wxCheckBox* m_UserPDFReader;
		wxTextCtrl* m_UserReaderPath;
		wxBitmapButton* m_UserReaderBrowse;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTargetHost( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDebuggerLocal( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDebuggerRS232( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSnippetEdit( wxGridEvent& event ) { event.Skip(); }
		virtual void OnKbdChanged( wxPropertyGridEvent& event ) { event.Skip(); }
		virtual void OnUserPDFReader( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUserReaderBrowse( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SettingsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~SettingsDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SearchDlg
///////////////////////////////////////////////////////////////////////////////
class SearchDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* lblSearchText;
		wxComboBox* ctrlSearchText;
		wxRadioBox* optLocation;
		wxCheckBox* chkWholeWord;
		wxCheckBox* chkMatchCase;
		wxCheckBox* chkRegEx;
		wxButton* btnOK;
		wxButton* btnCancel;
	
	public:
		
		SearchDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Find..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~SearchDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ReplaceDlg
///////////////////////////////////////////////////////////////////////////////
class ReplaceDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* lblSearchText;
		wxComboBox* ctrlSearchText;
		wxStaticText* lblReplaceText;
		wxComboBox* ctrlReplaceText;
		wxCheckBox* chkWholeWord;
		wxCheckBox* chkMatchCase;
		wxCheckBox* chkRegEx;
		wxButton* btnOK;
		wxButton* btnCancel;
	
	public:
		
		ReplaceDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Replace..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~ReplaceDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ReplacePrompt
///////////////////////////////////////////////////////////////////////////////
class ReplacePrompt : public wxDialog 
{
	private:
	
	protected:
		wxButton* btnYes;
		wxButton* btnNo;
		wxButton* btnLast;
		wxButton* btnAll;
		wxButton* btnQuit;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnDialogKey( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnYes( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLast( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnYesToAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ReplacePrompt( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Replace this occurrence"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxCLOSE_BOX ); 
		~ReplacePrompt();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CustomDirPicker
///////////////////////////////////////////////////////////////////////////////
class CustomDirPicker : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_lblPicker;
		wxDirPickerCtrl* m_dirPicker;
		wxStdDialogButtonSizer* m_sdbSizerDirSelect;
		wxButton* m_sdbSizerDirSelectOK;
		wxButton* m_sdbSizerDirSelectCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CustomDirPicker( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select folder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER ); 
		~CustomDirPicker();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SampleBrowser
///////////////////////////////////////////////////////////////////////////////
class SampleBrowser : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_listSamples;
		wxHtmlWindow* m_htmlSample;
		wxStdDialogButtonSizer* m_sdbSizerSamples;
		wxButton* m_sdbSizerSamplesOK;
		wxButton* m_sdbSizerSamplesCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnInitDialog( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SampleBrowser( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Samples"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~SampleBrowser();
	
};

#endif //__QUINCYDIALOGS_H__
