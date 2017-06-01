///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 14 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "QuincyDialogs.h"

#include "res/tb_open.xpm"

///////////////////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* dlgSizer;
	dlgSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pBuild = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerBuildPanel;
	bSizerBuildPanel = new wxBoxSizer( wxVERTICAL );
	
	
	bSizerBuildPanel->Add( 0, 12, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerInstructionSet;
	bSizerInstructionSet = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizerInstructionSet;
	fgSizerInstructionSet = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerInstructionSet->SetFlexibleDirection( wxHORIZONTAL );
	fgSizerInstructionSet->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lblTargetHost = new wxStaticText( m_pBuild, wxID_ANY, wxT("Target &host"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTargetHost->Wrap( -1 );
	fgSizerInstructionSet->Add( m_lblTargetHost, 0, wxALL, 5 );
	
	wxString m_TargetHostChoices[] = { wxT("-") };
	int m_TargetHostNChoices = sizeof( m_TargetHostChoices ) / sizeof( wxString );
	m_TargetHost = new wxChoice( m_pBuild, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_TargetHostNChoices, m_TargetHostChoices, 0 );
	m_TargetHost->SetSelection( 0 );
	m_TargetHost->SetToolTip( wxT("A specific device or application may come with predefined settings.\nChoose the appropriate device/application for your scripts.") );
	
	fgSizerInstructionSet->Add( m_TargetHost, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_lblDebugLevel = new wxStaticText( m_pBuild, wxID_ANY, wxT("&Debug info."), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblDebugLevel->Wrap( -1 );
	fgSizerInstructionSet->Add( m_lblDebugLevel, 0, wxALL, 5 );
	
	wxString m_DebugLevelChoices[] = { wxT("None"), wxT("Run-time checks"), wxT("Symbolic debugging") };
	int m_DebugLevelNChoices = sizeof( m_DebugLevelChoices ) / sizeof( wxString );
	m_DebugLevel = new wxChoice( m_pBuild, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DebugLevelNChoices, m_DebugLevelChoices, 0 );
	m_DebugLevel->SetSelection( 0 );
	m_DebugLevel->SetToolTip( wxT("For debugging, full symbolic information is required. Run-time checks enable extra validity checks with low overhead.") );
	
	fgSizerInstructionSet->Add( m_DebugLevel, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_lblOptimization = new wxStaticText( m_pBuild, wxID_ANY, wxT("Instruction &set"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblOptimization->Wrap( -1 );
	fgSizerInstructionSet->Add( m_lblOptimization, 0, wxALL, 5 );
	
	wxString m_OptimizationChoices[] = { wxT("Core (unoptimized)"), wxT("Core (JIT compatible)"), wxT("Supplemental"), wxT("Full") };
	int m_OptimizationNChoices = sizeof( m_OptimizationChoices ) / sizeof( wxString );
	m_Optimization = new wxChoice( m_pBuild, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_OptimizationNChoices, m_OptimizationChoices, 0 );
	m_Optimization->SetSelection( 0 );
	m_Optimization->SetToolTip( wxT("The full instruction set runs faster than the core instruction set (except on a JIT).\nHowever, not all devices (or applications) support the full instruction set.") );
	
	fgSizerInstructionSet->Add( m_Optimization, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	bSizerInstructionSet->Add( fgSizerInstructionSet, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizerBuildOptions;
	sizerBuildOptions = new wxBoxSizer( wxVERTICAL );
	
	m_OverlayCode = new wxCheckBox( m_pBuild, wxID_ANY, wxT("O&verlay code"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OverlayCode->SetToolTip( wxT("Generate overlay code, for arbitrarily large scripts on memory-constrained devices.") );
	
	sizerBuildOptions->Add( m_OverlayCode, 0, wxALL, 5 );
	
	m_chkStandardAMXname = new wxCheckBox( m_pBuild, wxID_ANY, wxT("Standard &executable filename"), wxDefaultPosition, wxDefaultSize, 0 );
	m_chkStandardAMXname->SetToolTip( wxT("Some devices (or applications) require that a compiled script has a fixed name.\nIf so, this option may be set (make sure a Target host is selected).") );
	
	sizerBuildOptions->Add( m_chkStandardAMXname, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_chkVerbose = new wxCheckBox( m_pBuild, wxID_ANY, wxT("Report &memory usage (on compile)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_chkVerbose->SetToolTip( wxT("To display an estimate of the memory used by the compiled script.") );
	
	sizerBuildOptions->Add( m_chkVerbose, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_chkCreateReport = new wxCheckBox( m_pBuild, wxID_ANY, wxT("Create s&ymbol report && documentation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_chkCreateReport->SetToolTip( wxT("To generate a report on each compile.\nA symbol report gives an overview of the functions, including documentation from the comments.") );
	
	sizerBuildOptions->Add( m_chkCreateReport, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	bSizerInstructionSet->Add( sizerBuildOptions, 0, 0, 5 );
	
	
	bSizerBuildPanel->Add( bSizerInstructionSet, 0, 0, 5 );
	
	m_staticline2 = new wxStaticLine( m_pBuild, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerBuildPanel->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* sizerPaths;
	sizerPaths = new wxFlexGridSizer( 0, 2, 0, 0 );
	sizerPaths->AddGrowableCol( 1 );
	sizerPaths->SetFlexibleDirection( wxHORIZONTAL );
	sizerPaths->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lblDefines = new wxStaticText( m_pBuild, wxID_ANY, wxT("De&fines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblDefines->Wrap( -1 );
	sizerPaths->Add( m_lblDefines, 0, wxALL|wxEXPAND, 5 );
	
	m_Defines = new wxTextCtrl( m_pBuild, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_Defines->SetToolTip( wxT("Type in a series of defined keywords (e.g. for conditional compilation).\nThe definitions must be separated with semicolons.") );
	
	sizerPaths->Add( m_Defines, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_lblIncludes = new wxStaticText( m_pBuild, wxID_ANY, wxT("&Include path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblIncludes->Wrap( -1 );
	sizerPaths->Add( m_lblIncludes, 0, wxALL|wxEXPAND, 5 );
	
	m_IncludePath = new wxTextCtrl( m_pBuild, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_IncludePath->SetToolTip( wxT("One or more \"system\" include paths. Separate multiple paths with a semicolon.") );
	
	sizerPaths->Add( m_IncludePath, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_lblOutputPath = new wxStaticText( m_pBuild, wxID_ANY, wxT("&Output path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblOutputPath->Wrap( -1 );
	sizerPaths->Add( m_lblOutputPath, 0, wxALL|wxEXPAND, 5 );
	
	m_OutputPath = new wxTextCtrl( m_pBuild, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_OutputPath->SetToolTip( wxT("The path where the compiled scripts must be stored.\nIf left empty, the output is in the same path as the input scripts.") );
	
	sizerPaths->Add( m_OutputPath, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_lblBinPath = new wxStaticText( m_pBuild, wxID_ANY, wxT("&Compiler path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblBinPath->Wrap( -1 );
	sizerPaths->Add( m_lblBinPath, 0, wxALL|wxEXPAND, 5 );
	
	m_BinPath = new wxTextCtrl( m_pBuild, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_BinPath->SetToolTip( wxT("The path where the Pawn compiler and its tools are installed.") );
	
	sizerPaths->Add( m_BinPath, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_lblPreBuild = new wxStaticText( m_pBuild, wxID_ANY, wxT("&Pre-builld"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPreBuild->Wrap( -1 );
	sizerPaths->Add( m_lblPreBuild, 0, wxALL|wxEXPAND, 5 );
	
	m_PreBuild = new wxTextCtrl( m_pBuild, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_PreBuild->SetToolTip( wxT("An optional command to run before launching the Pawn compiler.") );
	
	sizerPaths->Add( m_PreBuild, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_lblMiscCmdLine = new wxStaticText( m_pBuild, wxID_ANY, wxT("O&ther options"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblMiscCmdLine->Wrap( -1 );
	sizerPaths->Add( m_lblMiscCmdLine, 0, wxALL, 5 );
	
	m_MiscCmdOptions = new wxTextCtrl( m_pBuild, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_MiscCmdOptions->SetToolTip( wxT("Any other options that you wish to pass to the Pawn compiler.") );
	
	sizerPaths->Add( m_MiscCmdOptions, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	bSizerBuildPanel->Add( sizerPaths, 0, wxEXPAND, 5 );
	
	
	m_pBuild->SetSizer( bSizerBuildPanel );
	m_pBuild->Layout();
	bSizerBuildPanel->Fit( m_pBuild );
	m_Notebook->AddPage( m_pBuild, wxT("Build"), true );
	m_pEdit = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* editSizerBlocks;
	editSizerBlocks = new wxBoxSizer( wxVERTICAL );
	
	
	editSizerBlocks->Add( 0, 8, 0, wxTOP, 5 );
	
	wxFlexGridSizer* fgSizerEditOptions;
	fgSizerEditOptions = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizerEditOptions->SetFlexibleDirection( wxBOTH );
	fgSizerEditOptions->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lblTabWidth = new wxStaticText( m_pEdit, wxID_ANY, wxT("TAB stops"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTabWidth->Wrap( -1 );
	fgSizerEditOptions->Add( m_lblTabWidth, 0, wxALL, 5 );
	
	m_TabWidth = new wxSpinCtrl( m_pEdit, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 8, 1 );
	m_TabWidth->SetToolTip( wxT("The number of characters between two TAB positions.") );
	
	fgSizerEditOptions->Add( m_TabWidth, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_UseTabs = new wxCheckBox( m_pEdit, wxID_ANY, wxT("TAB key inserts spaces"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UseTabs->SetToolTip( wxT("If this option has a checkmark, the TAB key will insert space characters up to the next TAB stop.\nIf not checked, the TAB key will insert a TAB character.") );
	
	fgSizerEditOptions->Add( m_UseTabs, 0, wxALL, 5 );
	
	m_lblAutoIndent = new wxStaticText( m_pEdit, wxID_ANY, wxT("Auto-indent"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblAutoIndent->Wrap( -1 );
	fgSizerEditOptions->Add( m_lblAutoIndent, 0, wxALL, 5 );
	
	m_AutoIndent = new wxCheckBox( m_pEdit, wxID_ANY, wxT("Indent lines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AutoIndent->SetToolTip( wxT("To automatically indent inserted lines with the same indentation as the preceding line.") );
	
	fgSizerEditOptions->Add( m_AutoIndent, 0, wxALL, 5 );
	
	m_IndentPastedBlocks = new wxCheckBox( m_pEdit, wxID_ANY, wxT("Indent pasted blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	m_IndentPastedBlocks->SetToolTip( wxT("Indent blocks pasted from the clipboard.") );
	
	fgSizerEditOptions->Add( m_IndentPastedBlocks, 0, wxALL, 5 );
	
	m_lblFont = new wxStaticText( m_pEdit, wxID_ANY, wxT("Font"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblFont->Wrap( -1 );
	fgSizerEditOptions->Add( m_lblFont, 0, wxALL, 5 );
	
	m_EditFont = new wxFontPickerCtrl( m_pEdit, wxID_ANY, wxFont( 10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier") ), wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	m_EditFont->SetMaxPointSize( 100 ); 
	m_EditFont->SetToolTip( wxT("Click this button to select the font for the source code (must be monospaced).") );
	
	fgSizerEditOptions->Add( m_EditFont, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizerEditOptions->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	editSizerBlocks->Add( fgSizerEditOptions, 0, wxEXPAND, 5 );
	
	m_EditHorLine = new wxStaticLine( m_pEdit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	editSizerBlocks->Add( m_EditHorLine, 0, wxEXPAND | wxALL, 5 );
	
	wxGridSizer* editSizerColors;
	editSizerColors = new wxGridSizer( 0, 4, 0, 0 );
	
	m_lblBackground = new wxStaticText( m_pEdit, wxID_ANY, wxT("Background"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblBackground->Wrap( -1 );
	m_lblBackground->SetToolTip( wxT("The background colour of the edit window.") );
	
	editSizerColors->Add( m_lblBackground, 0, wxALL, 5 );
	
	m_clrBackground = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrBackground->SetToolTip( wxT("The background colour of the edit window.") );
	
	editSizerColors->Add( m_clrBackground, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblActiveLine = new wxStaticText( m_pEdit, wxID_ANY, wxT("Active line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblActiveLine->Wrap( -1 );
	m_lblActiveLine->SetToolTip( wxT("The background colour of the line that the text cursor is on.") );
	
	editSizerColors->Add( m_lblActiveLine, 0, wxALL, 5 );
	
	m_clrActiveLine = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrActiveLine->SetToolTip( wxT("The background colour of the line that the text cursor is on.") );
	
	editSizerColors->Add( m_clrActiveLine, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblNormalText = new wxStaticText( m_pEdit, wxID_ANY, wxT("Standard text"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblNormalText->Wrap( -1 );
	m_lblNormalText->SetToolTip( wxT("The foreground colour of normal text.") );
	
	editSizerColors->Add( m_lblNormalText, 0, wxALL, 5 );
	
	m_clrNormalText = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrNormalText->SetToolTip( wxT("The foreground colour of normal text.") );
	
	editSizerColors->Add( m_clrNormalText, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblOperators = new wxStaticText( m_pEdit, wxID_ANY, wxT("Operators"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblOperators->Wrap( -1 );
	m_lblOperators->SetToolTip( wxT("The foreground colour for operators.") );
	
	editSizerColors->Add( m_lblOperators, 0, wxALL, 5 );
	
	m_clrOperators = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrOperators->SetToolTip( wxT("The foreground colour for operators.") );
	
	editSizerColors->Add( m_clrOperators, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblKeywords = new wxStaticText( m_pEdit, wxID_ANY, wxT("Keywords"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblKeywords->Wrap( -1 );
	m_lblKeywords->SetToolTip( wxT("The foreground colour for keywords.") );
	
	editSizerColors->Add( m_lblKeywords, 0, wxALL, 5 );
	
	m_clrKeywords = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrKeywords->SetToolTip( wxT("The foreground colour for keywords.") );
	
	editSizerColors->Add( m_clrKeywords, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblNumbers = new wxStaticText( m_pEdit, wxID_ANY, wxT("Numbers"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblNumbers->Wrap( -1 );
	m_lblNumbers->SetToolTip( wxT("The foreground colour for literal numbers.") );
	
	editSizerColors->Add( m_lblNumbers, 0, wxALL, 5 );
	
	m_clrNumbers = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrNumbers->SetToolTip( wxT("The foreground colour for literal numbers.") );
	
	editSizerColors->Add( m_clrNumbers, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblPreprocessor = new wxStaticText( m_pEdit, wxID_ANY, wxT("Directives"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPreprocessor->Wrap( -1 );
	m_lblPreprocessor->SetToolTip( wxT("The foreground colour of compiler directives (\"preprocessor directives\").") );
	
	editSizerColors->Add( m_lblPreprocessor, 0, wxALL, 5 );
	
	m_clrPreprocessor = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrPreprocessor->SetToolTip( wxT("The foreground colour of compiler directives (\"preprocessor directives\").") );
	
	editSizerColors->Add( m_clrPreprocessor, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblStrings = new wxStaticText( m_pEdit, wxID_ANY, wxT("Strings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblStrings->Wrap( -1 );
	m_lblStrings->SetToolTip( wxT("The foreground colour for strings and literal single characters.") );
	
	editSizerColors->Add( m_lblStrings, 0, wxALL, 5 );
	
	m_clrStrings = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrStrings->SetToolTip( wxT("The foreground colour for strings and literal single characters.") );
	
	editSizerColors->Add( m_clrStrings, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblComments = new wxStaticText( m_pEdit, wxID_ANY, wxT("Comments"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblComments->Wrap( -1 );
	m_lblComments->SetToolTip( wxT("The foreground colour for comments.") );
	
	editSizerColors->Add( m_lblComments, 0, wxALL, 5 );
	
	m_clrComments = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrComments->SetToolTip( wxT("The foreground colour for comments.") );
	
	editSizerColors->Add( m_clrComments, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblStringEOL = new wxStaticText( m_pEdit, wxID_ANY, wxT("Unterminated string"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblStringEOL->Wrap( -1 );
	m_lblStringEOL->SetToolTip( wxT("The foreground colour for strings and characters that lack the trailing quotation mark.") );
	
	editSizerColors->Add( m_lblStringEOL, 0, wxALL, 5 );
	
	m_clrStringEOL = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrStringEOL->SetToolTip( wxT("The foreground colour for strings and characters that lack the trailing quotation mark.") );
	
	editSizerColors->Add( m_clrStringEOL, 0, wxRIGHT|wxLEFT, 5 );
	
	m_lblHighlight = new wxStaticText( m_pEdit, wxID_ANY, wxT("Highlighted text"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblHighlight->Wrap( -1 );
	m_lblHighlight->SetToolTip( wxT("Colour for brace matching and other text highlights.") );
	
	editSizerColors->Add( m_lblHighlight, 0, wxALL, 5 );
	
	m_clrHighlight = new wxColourPickerCtrl( m_pEdit, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_clrHighlight->SetToolTip( wxT("Colour for brace matching and other text highlights.") );
	
	editSizerColors->Add( m_clrHighlight, 0, wxRIGHT|wxLEFT, 5 );
	
	
	editSizerBlocks->Add( editSizerColors, 0, wxEXPAND, 5 );
	
	
	m_pEdit->SetSizer( editSizerBlocks );
	m_pEdit->Layout();
	editSizerBlocks->Fit( m_pEdit );
	m_Notebook->AddPage( m_pEdit, wxT("Editor"), false );
	m_pDebug = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerDebug;
	bSizerDebug = new wxBoxSizer( wxVERTICAL );
	
	
	bSizerDebug->Add( 0, 12, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerConnection;
	sbSizerConnection = new wxStaticBoxSizer( new wxStaticBox( m_pDebug, wxID_ANY, wxT("Debugger && Run-time target") ), wxVERTICAL );
	
	wxBoxSizer* bSizerPort;
	bSizerPort = new wxBoxSizer( wxVERTICAL );
	
	m_optDebugLocal = new wxRadioButton( sbSizerConnection->GetStaticBox(), wxID_ANY, wxT("&Local (script runs on the same computer as the run-time)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_optDebugLocal->SetToolTip( wxT("Use local debugging, run the script in the local host.") );
	
	bSizerPort->Add( m_optDebugLocal, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_optDebugRS232 = new wxRadioButton( sbSizerConnection->GetStaticBox(), wxID_ANY, wxT("&RS232 (remote debugging && logging)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_optDebugRS232->SetToolTip( wxT("Script runs at a remote host.\nRemote debugging via RS232.") );
	
	bSizerPort->Add( m_optDebugRS232, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizerPort;
	fgSizerPort = new wxFlexGridSizer( 1, 5, 0, 0 );
	fgSizerPort->SetFlexibleDirection( wxBOTH );
	fgSizerPort->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizerPort->Add( 20, 0, 1, wxEXPAND, 5 );
	
	m_lblPort = new wxStaticText( sbSizerConnection->GetStaticBox(), wxID_ANY, wxT("&Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPort->Wrap( -1 );
	fgSizerPort->Add( m_lblPort, 0, wxALL, 5 );
	
	wxArrayString m_ctrlPortChoices;
	m_ctrlPort = new wxChoice( sbSizerConnection->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ctrlPortChoices, 0 );
	m_ctrlPort->SetSelection( 0 );
	m_ctrlPort->SetToolTip( wxT("The port to use for communication.") );
	
	fgSizerPort->Add( m_ctrlPort, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_lblBaudRate = new wxStaticText( sbSizerConnection->GetStaticBox(), wxID_ANY, wxT("&Baud rate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblBaudRate->Wrap( -1 );
	fgSizerPort->Add( m_lblBaudRate, 0, wxALL, 5 );
	
	wxString m_ctrlBaudRateChoices[] = { wxT("1200"), wxT("2400"), wxT("4800"), wxT("9600"), wxT("14400"), wxT("19200"), wxT("28800"), wxT("38400"), wxT("57600"), wxT("115200") };
	int m_ctrlBaudRateNChoices = sizeof( m_ctrlBaudRateChoices ) / sizeof( wxString );
	m_ctrlBaudRate = new wxChoice( sbSizerConnection->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ctrlBaudRateNChoices, m_ctrlBaudRateChoices, 0 );
	m_ctrlBaudRate->SetSelection( 0 );
	m_ctrlBaudRate->SetToolTip( wxT("The communication speed.") );
	
	fgSizerPort->Add( m_ctrlBaudRate, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	bSizerPort->Add( fgSizerPort, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizerPortSettings;
	fgSizerPortSettings = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerPortSettings->SetFlexibleDirection( wxBOTH );
	fgSizerPortSettings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizerPortSettings->Add( 20, 0, 1, wxEXPAND, 5 );
	
	m_lblConnInfo = new wxStaticText( sbSizerConnection->GetStaticBox(), wxID_ANY, wxT("(Other settings: 8 data bits, 1 stop bit, no parity, no handshaking)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblConnInfo->Wrap( -1 );
	fgSizerPortSettings->Add( m_lblConnInfo, 0, wxRIGHT|wxLEFT, 5 );
	
	
	bSizerPort->Add( fgSizerPortSettings, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizerPortLogging;
	fgSizerPortLogging = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerPortLogging->SetFlexibleDirection( wxBOTH );
	fgSizerPortLogging->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizerPortLogging->Add( 20, 0, 1, wxEXPAND, 5 );
	
	m_chkEnableLogging = new wxCheckBox( sbSizerConnection->GetStaticBox(), wxID_ANY, wxT("Log incoming data to the output pane"), wxDefaultPosition, wxDefaultSize, 0 );
	m_chkEnableLogging->SetToolTip( wxT("Data received from the RS232 port is logged to the output pane.") );
	
	fgSizerPortLogging->Add( m_chkEnableLogging, 0, wxALL, 5 );
	
	
	bSizerPort->Add( fgSizerPortLogging, 1, wxEXPAND, 5 );
	
	
	sbSizerConnection->Add( bSizerPort, 1, wxEXPAND, 5 );
	
	
	bSizerDebug->Add( sbSizerConnection, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	
	m_pDebug->SetSizer( bSizerDebug );
	m_pDebug->Layout();
	bSizerDebug->Fit( m_pDebug );
	m_Notebook->AddPage( m_pDebug, wxT("Debug/Run"), false );
	m_pSnippets = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerSnippets;
	bSizerSnippets = new wxBoxSizer( wxVERTICAL );
	
	m_gridSnippets = new wxGrid( m_pSnippets, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_gridSnippets->CreateGrid( 0, 2 );
	m_gridSnippets->EnableEditing( true );
	m_gridSnippets->EnableGridLines( true );
	m_gridSnippets->EnableDragGridSize( false );
	m_gridSnippets->SetMargins( 0, 0 );
	
	// Columns
	m_gridSnippets->SetColSize( 0, 100 );
	m_gridSnippets->SetColSize( 1, 300 );
	m_gridSnippets->EnableDragColMove( false );
	m_gridSnippets->EnableDragColSize( true );
	m_gridSnippets->SetColLabelSize( 20 );
	m_gridSnippets->SetColLabelValue( 0, wxT("Shorthand") );
	m_gridSnippets->SetColLabelValue( 1, wxT("Replacement") );
	m_gridSnippets->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_gridSnippets->EnableDragRowSize( false );
	m_gridSnippets->SetRowLabelSize( 0 );
	m_gridSnippets->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_gridSnippets->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizerSnippets->Add( m_gridSnippets, 1, wxALL|wxEXPAND, 5 );
	
	m_lblSnippet = new wxStaticText( m_pSnippets, wxID_ANY, wxT("In the replacement text: \\n = newline, \\t = Tab, \\s = back-Tab, \\\\ = \\ and ^ = caret."), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblSnippet->Wrap( -1 );
	bSizerSnippets->Add( m_lblSnippet, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	m_pSnippets->SetSizer( bSizerSnippets );
	m_pSnippets->Layout();
	bSizerSnippets->Fit( m_pSnippets );
	m_Notebook->AddPage( m_pSnippets, wxT("Snippets"), false );
	m_pKeyboard = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerKbd;
	bSizerKbd = new wxBoxSizer( wxVERTICAL );
	
	m_KbdShortcuts = new wxPropertyGrid(m_pKeyboard, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_HIDE_MARGIN|wxPG_SPLITTER_AUTO_CENTER);
	bSizerKbd->Add( m_KbdShortcuts, 1, wxALL|wxEXPAND, 5 );
	
	
	m_pKeyboard->SetSizer( bSizerKbd );
	m_pKeyboard->Layout();
	bSizerKbd->Fit( m_pKeyboard );
	m_Notebook->AddPage( m_pKeyboard, wxT("Keyboard"), false );
	m_pOptions = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bOptionsSizer;
	bOptionsSizer = new wxBoxSizer( wxVERTICAL );
	
	
	bOptionsSizer->Add( 0, 8, 0, 0, 5 );
	
	m_StripTrailingWhitespace = new wxCheckBox( m_pOptions, wxID_ANY, wxT("Strip trailing whitespace on save"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StripTrailingWhitespace->SetToolTip( wxT("Remove whitespace at the end of the lines when saving a file.") );
	
	bOptionsSizer->Add( m_StripTrailingWhitespace, 0, wxALL, 5 );
	
	m_ModalFind = new wxCheckBox( m_pOptions, wxID_ANY, wxT("Use alternative Find dialog"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ModalFind->SetToolTip( wxT("Use an alternative Find dialog, which allows regular expressions and multi-file searches.") );
	
	bOptionsSizer->Add( m_ModalFind, 0, wxALL, 5 );
	
	m_LocalIniFile = new wxCheckBox( m_pOptions, wxID_ANY, wxT("Use local configuration file"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LocalIniFile->SetToolTip( wxT("Store the configuration in the same directory as the program.\nThis allows for multiple installations of Pawn in separate directories. However, it requires that the user has access rights to the local directory.") );
	
	bOptionsSizer->Add( m_LocalIniFile, 0, wxALL, 5 );
	
	wxBoxSizer* bSizerHelp;
	bSizerHelp = new wxBoxSizer( wxHORIZONTAL );
	
	m_UserPDFReader = new wxCheckBox( m_pOptions, wxID_ANY, wxT("Use specific PDF reader"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerHelp->Add( m_UserPDFReader, 0, wxALL, 5 );
	
	m_UserReaderPath = new wxTextCtrl( m_pOptions, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerHelp->Add( m_UserReaderPath, 1, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_UserReaderBrowse = new wxBitmapButton( m_pOptions, wxID_ANY, wxBitmap( tb_open_xpm ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerHelp->Add( m_UserReaderBrowse, 0, wxTOP|wxRIGHT, 4 );
	
	
	bOptionsSizer->Add( bSizerHelp, 0, wxEXPAND, 5 );
	
	
	m_pOptions->SetSizer( bOptionsSizer );
	m_pOptions->Layout();
	bOptionsSizer->Fit( m_pOptions );
	m_Notebook->AddPage( m_pOptions, wxT("Miscellaneous"), false );
	
	dlgSizer->Add( m_Notebook, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	
	dlgSizer->Add( m_sdbSizer, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	this->SetSizer( dlgSizer );
	this->Layout();
	dlgSizer->Fit( this );
	
	// Connect Events
	m_TargetHost->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SettingsDlg::OnTargetHost ), NULL, this );
	m_optDebugLocal->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SettingsDlg::OnDebuggerLocal ), NULL, this );
	m_optDebugRS232->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SettingsDlg::OnDebuggerRS232 ), NULL, this );
	m_gridSnippets->Connect( wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SettingsDlg::OnSnippetEdit ), NULL, this );
	m_KbdShortcuts->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( SettingsDlg::OnKbdChanged ), NULL, this );
	m_UserPDFReader->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SettingsDlg::OnUserPDFReader ), NULL, this );
	m_UserReaderBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDlg::OnUserReaderBrowse ), NULL, this );
	m_sdbSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDlg::OnCancel ), NULL, this );
	m_sdbSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDlg::OnOK ), NULL, this );
}

SettingsDlg::~SettingsDlg()
{
	// Disconnect Events
	m_TargetHost->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SettingsDlg::OnTargetHost ), NULL, this );
	m_optDebugLocal->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SettingsDlg::OnDebuggerLocal ), NULL, this );
	m_optDebugRS232->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SettingsDlg::OnDebuggerRS232 ), NULL, this );
	m_gridSnippets->Disconnect( wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SettingsDlg::OnSnippetEdit ), NULL, this );
	m_KbdShortcuts->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( SettingsDlg::OnKbdChanged ), NULL, this );
	m_UserPDFReader->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SettingsDlg::OnUserPDFReader ), NULL, this );
	m_UserReaderBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDlg::OnUserReaderBrowse ), NULL, this );
	m_sdbSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDlg::OnCancel ), NULL, this );
	m_sdbSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDlg::OnOK ), NULL, this );
	
}

SearchDlg::SearchDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgsizerMain;
	fgsizerMain = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgsizerMain->SetFlexibleDirection( wxBOTH );
	fgsizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* fbsizerLeft;
	fbsizerLeft = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* fbsizerText;
	fbsizerText = new wxBoxSizer( wxHORIZONTAL );
	
	lblSearchText = new wxStaticText( this, wxID_ANY, wxT("Search for:"), wxDefaultPosition, wxDefaultSize, 0 );
	lblSearchText->Wrap( -1 );
	fbsizerText->Add( lblSearchText, 0, wxTOP|wxBOTTOM|wxLEFT, 8 );
	
	ctrlSearchText = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN ); 
	ctrlSearchText->SetToolTip( wxT("The text to search for.") );
	ctrlSearchText->SetMinSize( wxSize( 200,-1 ) );
	
	fbsizerText->Add( ctrlSearchText, 1, wxALL, 5 );
	
	
	fbsizerLeft->Add( fbsizerText, 0, wxEXPAND, 5 );
	
	wxBoxSizer* fbsizerOptions;
	fbsizerOptions = new wxBoxSizer( wxVERTICAL );
	
	wxString optLocationChoices[] = { wxT("&Active file"), wxT("&Open files"), wxT("Works&pace") };
	int optLocationNChoices = sizeof( optLocationChoices ) / sizeof( wxString );
	optLocation = new wxRadioBox( this, wxID_ANY, wxT("Search in:"), wxDefaultPosition, wxDefaultSize, optLocationNChoices, optLocationChoices, 1, wxRA_SPECIFY_ROWS );
	optLocation->SetSelection( 0 );
	optLocation->SetToolTip( wxT("Where to search:\nthe active file only, all files that are currently in the IDE, or all Pawn files found in the workspace (whether loaded or not loaded)") );
	
	fbsizerOptions->Add( optLocation, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	chkWholeWord = new wxCheckBox( this, wxID_ANY, wxT("Whole &word"), wxDefaultPosition, wxDefaultSize, 0 );
	chkWholeWord->SetToolTip( wxT("Whether to search only for whole words.") );
	
	fbsizerOptions->Add( chkWholeWord, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	chkMatchCase = new wxCheckBox( this, wxID_ANY, wxT("Match &case"), wxDefaultPosition, wxDefaultSize, 0 );
	chkMatchCase->SetToolTip( wxT("Whether to make a distinction between upper case and lower case letters.") );
	
	fbsizerOptions->Add( chkMatchCase, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	chkRegEx = new wxCheckBox( this, wxID_ANY, wxT("&Regular expression"), wxDefaultPosition, wxDefaultSize, 0 );
	chkRegEx->SetToolTip( wxT("Whether the search text uses regular expressions,\nor whether it is the plain literal text to search for.") );
	
	fbsizerOptions->Add( chkRegEx, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	fbsizerLeft->Add( fbsizerOptions, 1, wxEXPAND, 5 );
	
	
	fgsizerMain->Add( fbsizerLeft, 1, wxEXPAND, 5 );
	
	wxBoxSizer* fbSizerButtons;
	fbSizerButtons = new wxBoxSizer( wxVERTICAL );
	
	btnOK = new wxButton( this, wxID_OK, wxT("Find"), wxDefaultPosition, wxDefaultSize, 0 );
	btnOK->SetDefault(); 
	fbSizerButtons->Add( btnOK, 0, wxALL, 5 );
	
	btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fbSizerButtons->Add( btnCancel, 0, wxALL, 5 );
	
	
	fgsizerMain->Add( fbSizerButtons, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgsizerMain );
	this->Layout();
	fgsizerMain->Fit( this );
	
	this->Centre( wxBOTH );
}

SearchDlg::~SearchDlg()
{
}

ReplaceDlg::ReplaceDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgsizerMain;
	fgsizerMain = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgsizerMain->SetFlexibleDirection( wxBOTH );
	fgsizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* fbsizerLeft;
	fbsizerLeft = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerText;
	fgSizerText = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerText->SetFlexibleDirection( wxBOTH );
	fgSizerText->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	lblSearchText = new wxStaticText( this, wxID_ANY, wxT("Search for:"), wxDefaultPosition, wxDefaultSize, 0 );
	lblSearchText->Wrap( -1 );
	fgSizerText->Add( lblSearchText, 0, wxTOP|wxBOTTOM|wxLEFT, 8 );
	
	ctrlSearchText = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN ); 
	ctrlSearchText->SetToolTip( wxT("The text to search for.") );
	ctrlSearchText->SetMinSize( wxSize( 200,-1 ) );
	
	fgSizerText->Add( ctrlSearchText, 0, wxALL|wxEXPAND, 5 );
	
	lblReplaceText = new wxStaticText( this, wxID_ANY, wxT("Replace with:"), wxDefaultPosition, wxDefaultSize, 0 );
	lblReplaceText->Wrap( -1 );
	fgSizerText->Add( lblReplaceText, 0, wxTOP|wxBOTTOM|wxLEFT, 8 );
	
	ctrlReplaceText = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN ); 
	ctrlReplaceText->SetToolTip( wxT("The text to search for.") );
	
	fgSizerText->Add( ctrlReplaceText, 0, wxALL|wxEXPAND, 5 );
	
	
	fbsizerLeft->Add( fgSizerText, 1, wxEXPAND, 5 );
	
	wxBoxSizer* fbsizerOptions;
	fbsizerOptions = new wxBoxSizer( wxVERTICAL );
	
	chkWholeWord = new wxCheckBox( this, wxID_ANY, wxT("Whole &word"), wxDefaultPosition, wxDefaultSize, 0 );
	chkWholeWord->SetToolTip( wxT("Whether to search only for whole words.") );
	
	fbsizerOptions->Add( chkWholeWord, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	chkMatchCase = new wxCheckBox( this, wxID_ANY, wxT("Match &case"), wxDefaultPosition, wxDefaultSize, 0 );
	chkMatchCase->SetToolTip( wxT("Whether to make a distinction between upper case and lower case letters.") );
	
	fbsizerOptions->Add( chkMatchCase, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	chkRegEx = new wxCheckBox( this, wxID_ANY, wxT("&Regular expression"), wxDefaultPosition, wxDefaultSize, 0 );
	chkRegEx->SetToolTip( wxT("Whether the search text uses regular expressions,\nor whether it is the plain literal text to search for.") );
	
	fbsizerOptions->Add( chkRegEx, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	fbsizerLeft->Add( fbsizerOptions, 1, wxEXPAND, 5 );
	
	
	fgsizerMain->Add( fbsizerLeft, 1, wxEXPAND, 5 );
	
	wxBoxSizer* fbSizerButtons;
	fbSizerButtons = new wxBoxSizer( wxVERTICAL );
	
	btnOK = new wxButton( this, wxID_OK, wxT("Replace"), wxDefaultPosition, wxDefaultSize, 0 );
	btnOK->SetDefault(); 
	fbSizerButtons->Add( btnOK, 0, wxALL, 5 );
	
	btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fbSizerButtons->Add( btnCancel, 0, wxALL, 5 );
	
	
	fgsizerMain->Add( fbSizerButtons, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgsizerMain );
	this->Layout();
	fgsizerMain->Fit( this );
	
	this->Centre( wxBOTH );
}

ReplaceDlg::~ReplaceDlg()
{
}

ReplacePrompt::ReplacePrompt( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxHORIZONTAL );
	
	btnYes = new wxButton( this, wxID_YES, wxT("&Yes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( btnYes, 0, wxALL, 5 );
	
	btnNo = new wxButton( this, wxID_NO, wxT("&No"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( btnNo, 0, wxALL, 5 );
	
	btnLast = new wxButton( this, wxID_LAST, wxT("&Last"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( btnLast, 0, wxALL, 5 );
	
	
	bSizerMain->Add( 30, 0, 1, wxEXPAND, 5 );
	
	btnAll = new wxButton( this, wxID_YESTOALL, wxT("Yes to &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( btnAll, 0, wxALL, 5 );
	
	btnQuit = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( btnQuit, 0, wxALL, 5 );
	
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ) );
	btnYes->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnYes ), NULL, this );
	btnYes->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnNo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnNo ), NULL, this );
	btnNo->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnLast->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnLast ), NULL, this );
	btnLast->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnYesToAll ), NULL, this );
	btnAll->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnQuit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnCancel ), NULL, this );
}

ReplacePrompt::~ReplacePrompt()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ) );
	btnYes->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnYes ), NULL, this );
	btnYes->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnNo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnNo ), NULL, this );
	btnNo->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnLast->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnLast ), NULL, this );
	btnLast->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnYesToAll ), NULL, this );
	btnAll->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ReplacePrompt::OnDialogKey ), NULL, this );
	btnQuit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplacePrompt::OnCancel ), NULL, this );
	
}

CustomDirPicker::CustomDirPicker( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerDirSelect;
	bSizerDirSelect = new wxBoxSizer( wxVERTICAL );
	
	m_lblPicker = new wxStaticText( this, wxID_ANY, wxT("Please select the directory."), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPicker->Wrap( -1 );
	bSizerDirSelect->Add( m_lblPicker, 1, wxALL|wxEXPAND, 5 );
	
	m_dirPicker = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_SMALL|wxDIRP_USE_TEXTCTRL );
	m_dirPicker->SetMinSize( wxSize( 300,-1 ) );
	
	bSizerDirSelect->Add( m_dirPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_sdbSizerDirSelect = new wxStdDialogButtonSizer();
	m_sdbSizerDirSelectOK = new wxButton( this, wxID_OK );
	m_sdbSizerDirSelect->AddButton( m_sdbSizerDirSelectOK );
	m_sdbSizerDirSelectCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerDirSelect->AddButton( m_sdbSizerDirSelectCancel );
	m_sdbSizerDirSelect->Realize();
	
	bSizerDirSelect->Add( m_sdbSizerDirSelect, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	
	this->SetSizer( bSizerDirSelect );
	this->Layout();
	bSizerDirSelect->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_sdbSizerDirSelectCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomDirPicker::OnCancel ), NULL, this );
	m_sdbSizerDirSelectOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomDirPicker::OnOK ), NULL, this );
}

CustomDirPicker::~CustomDirPicker()
{
	// Disconnect Events
	m_sdbSizerDirSelectCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomDirPicker::OnCancel ), NULL, this );
	m_sdbSizerDirSelectOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomDirPicker::OnOK ), NULL, this );
	
}

SampleBrowser::SampleBrowser( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerSamplesMain;
	bSizerSamplesMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerSamplesFrame;
	bSizerSamplesFrame = new wxBoxSizer( wxHORIZONTAL );
	
	m_listSamples = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listSamples->SetMinSize( wxSize( 200,-1 ) );
	
	bSizerSamplesFrame->Add( m_listSamples, 0, wxALL|wxEXPAND, 5 );
	
	m_htmlSample = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxDOUBLE_BORDER );
	m_htmlSample->SetMinSize( wxSize( 400,300 ) );
	
	bSizerSamplesFrame->Add( m_htmlSample, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizerSamplesMain->Add( bSizerSamplesFrame, 1, wxEXPAND, 5 );
	
	m_sdbSizerSamples = new wxStdDialogButtonSizer();
	m_sdbSizerSamplesOK = new wxButton( this, wxID_OK );
	m_sdbSizerSamples->AddButton( m_sdbSizerSamplesOK );
	m_sdbSizerSamplesCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerSamples->AddButton( m_sdbSizerSamplesCancel );
	m_sdbSizerSamples->Realize();
	
	bSizerSamplesMain->Add( m_sdbSizerSamples, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	
	this->SetSizer( bSizerSamplesMain );
	this->Layout();
	bSizerSamplesMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( SampleBrowser::OnInitDialog ) );
	m_listSamples->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SampleBrowser::OnSelect ), NULL, this );
	m_sdbSizerSamplesCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SampleBrowser::OnCancel ), NULL, this );
	m_sdbSizerSamplesOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SampleBrowser::OnOK ), NULL, this );
}

SampleBrowser::~SampleBrowser()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( SampleBrowser::OnInitDialog ) );
	m_listSamples->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SampleBrowser::OnSelect ), NULL, this );
	m_sdbSizerSamplesCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SampleBrowser::OnCancel ), NULL, this );
	m_sdbSizerSamplesOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SampleBrowser::OnOK ), NULL, this );
	
}
