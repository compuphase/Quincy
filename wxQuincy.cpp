/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright ITB CompuPhase, 2009-2016
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *  use this file except in compliance with the License. You may obtain a copy
 *  of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 *
 *  Version: $Id: wxQuincy.cpp 5504 2016-05-15 13:42:30Z  $
 */
#include "wxQuincy.h"
#include "QuincyFrame.h"
#include "minIni.h"
#include <wx/dir.h>
#include <wx/file.h>

#if !defined wxDIR_NO_FOLLOW
	#define wxDIR_NO_FOLLOW	0	/* only defined for wxWidgets 2.9 and up */
#endif

IMPLEMENT_APP(QuincyApp)

QuincyApp* theApp;

bool QuincyApp::OnInit()
{
	theApp = this;

	/* get the root path */
	MainPath = wxStandardPaths::Get().GetExecutablePath();
	MainPath = wxPathOnly(MainPath);

	/* set other system directories */
	DocPath = MainPath.BeforeLast(DIRSEP_CHAR) + wxT(DIRSEP_STR) + wxT("doc");

	/* create INI file path, then get settings */
	UserDataPath = wxStandardPaths::Get().GetUserDataDir();
	if (!wxDirExists(UserDataPath)) {
		#if defined _MSC_VER && wxMAJOR_VERSION < 3
			wxMkDir(UserDataPath);
		#else
			wxMkDir(UserDataPath.utf8_str(), 0777);
		#endif
	}

	/* create the list of menu shortcuts (which the menu needs); at the same
	   time, set the defaults */
	Shortcuts.Add(wxT("New"), wxT("&New"), wxT("Ctrl+N"), wxT("File"));
	Shortcuts.Add(wxT("Open"), wxT("&Open..."), wxT("Ctrl+O"), wxT("File"));
	Shortcuts.Add(wxT("Save"), wxT("&Save"), wxT("Ctrl+S"), wxT("File"));
	Shortcuts.Add(wxT("SaveAs"), wxT("Save &as..."), wxT("F12"), wxT("File"));
	Shortcuts.Add(wxT("SaveAll"), wxT("Save a&ll"), wxT("Ctrl+Shift+S"), wxT("File"));
	Shortcuts.Add(wxT("Close"), wxT("&Close"), wxT("Ctrl+F4"), wxT("File"));
	Shortcuts.Add(wxT("OpenWorkspace"), wxT("Open &workspace..."), wxEmptyString, wxT("File"));
	Shortcuts.Add(wxT("SaveWorkspace"), wxT("Save w&orkspace..."), wxEmptyString, wxT("File"));
	Shortcuts.Add(wxT("CloseWorkspace"), wxT("Close workspace..."), wxEmptyString, wxT("File"));
	Shortcuts.Add(wxT("Print"), wxT("&Print..."), wxT("Ctrl+P"), wxT("File"));
	Shortcuts.Add(wxT("Quit"), wxT("&Quit"), wxT("Alt+F4"), wxT("File"));
	Shortcuts.Add(wxT("Undo"), wxT("&Undo"), wxT("Ctrl+Z"), wxT("Edit"));
	Shortcuts.Add(wxT("Redo"), wxT("Re&do"), wxEmptyString, wxT("Edit"));
	Shortcuts.Add(wxT("Cut"), wxT("Cu&t"), wxT("Ctrl+X"), wxT("Edit"));
	Shortcuts.Add(wxT("Copy"), wxT("&Copy"), wxT("Ctrl+C"), wxT("Edit"));
	Shortcuts.Add(wxT("Paste"), wxT("&Paste"), wxT("Ctrl+V"), wxT("Edit"));
	Shortcuts.Add(wxT("Find"), wxT("&Find..."), wxT("Ctrl+F"), wxT("Edit"));
	Shortcuts.Add(wxT("FindNext"), wxT("Find &next"), wxT("F3"), wxT("Edit"));
	Shortcuts.Add(wxT("Replace"), wxT("&Replace"), wxT("Ctrl+H"), wxT("Edit"));
	Shortcuts.Add(wxT("GotoLine"), wxT("&Go to line..."), wxT("Ctrl+G"), wxT("Edit"));
	Shortcuts.Add(wxT("GotoSymbol"), wxT("Go to &symbol definition"), wxT("Ctrl+F6"), wxT("Edit"));
	Shortcuts.Add(wxT("MatchBrace"), wxT("&Match brace"), wxT("Ctrl+]"), wxT("Edit"));
	Shortcuts.Add(wxT("FillColumn"), wxT("Fill/insert columns"), wxEmptyString, wxT("Edit"));
	Shortcuts.Add(wxT("Autocomplete"), wxT("&Autocomplete"), wxT("Ctrl+Space"), wxT("Edit"));
	Shortcuts.Add(wxT("ToggleBookmark"), wxT("Toggle &Bookmark"), wxT("Ctrl+F2"), wxT("Bookmarks"));
	Shortcuts.Add(wxT("NextBookmark"), wxT("&Next bookmark"), wxT("F2"), wxT("Bookmarks"));
	Shortcuts.Add(wxT("PrevBookmark"), wxT("P&revious bookmark"), wxT("Shift+F2"), wxT("Bookmarks"));
	Shortcuts.Add(wxT("ViewWhitespace"), wxT("&TABs and spaces"), wxEmptyString, wxT("View"));
	Shortcuts.Add(wxT("ViewIndentGuides"), wxT("Indentation &Guides"), wxEmptyString, wxT("View"));
	Shortcuts.Add(wxT("Compile"), wxT("&Compile"), wxT("F7"), wxT("Build / Run"));
	Shortcuts.Add(wxT("Transfer"), wxT("&Transfer"), wxT("Ctrl+F7"), wxT("Build / Run"));
	Shortcuts.Add(wxT("Debug"), wxT("Start &Debugging"), wxT("F5"), wxT("Build / Run"));
	Shortcuts.Add(wxT("Run"), wxT("&Run without debugging"), wxT("Ctrl+F5"), wxT("Build / Run"));
	Shortcuts.Add(wxT("Stop"), wxT("&Stop"), wxT("Shift-F5"), wxT("Build / Run"));
	Shortcuts.Add(wxT("StepInto"), wxT("Step &Into"), wxT("F11"), wxT("Build / Run"));
	Shortcuts.Add(wxT("StepOver"), wxT("Step &Over"), wxT("F10"), wxT("Build / Run"));
	Shortcuts.Add(wxT("StepOut"), wxT("Step Out"), wxT("Ctrl+F11"), wxT("Build / Run"));
	Shortcuts.Add(wxT("RunToCursor"), wxT("Run to &Cursor"), wxT("Ctrl+F10"), wxT("Build / Run"));
	Shortcuts.Add(wxT("ToggleBreakpoint"), wxT("Toggle &Breakpoint"), wxT("F9"), wxT("Breakpoints"));
	Shortcuts.Add(wxT("ClearBreakpoints"), wxT("Clear all breakpoints"), wxEmptyString, wxT("Breakpoints"));
	Shortcuts.Add(wxT("Options"), wxT("&Options..."), wxT("Alt+F7"), wxT("Tools"));
	Shortcuts.Add(wxT("TabToSpace"), wxT("Tabs to Spaces"), wxEmptyString, wxT("Whitespace"));
	Shortcuts.Add(wxT("IndentToTab"), wxT("Spaces to Tabs (indent only)"), wxEmptyString, wxT("Whitespace"));
	Shortcuts.Add(wxT("SpaceToTab"), wxT("Spaces to Tabs (all)"), wxEmptyString, wxT("Whitespace"));
	Shortcuts.Add(wxT("TrimTrailing"), wxT("Trim trailing whitespace"), wxEmptyString, wxT("Whitespace"));
	Shortcuts.Add(wxT("GeneralHelp"), wxT("&IDE User Guide"), wxT("Shift+F1"), wxT("Help"));
	Shortcuts.Add(wxT("ContextHelp"), wxT("Context help"), wxT("F1"), wxT("Help"));

	/* first check whether an INI file is available in the main path (and
	   verify that it is writable). Otherwise go to the "application data"
	   directory */
	LocalIniFile = true;
	wxString strIniName = MainPath + wxT(DIRSEP_STR) + wxT("quincy.ini");
	if (!wxFileExists(strIniName) || !wxFile::Access(strIniName, wxFile::write)) {
		wxString strIniName = UserDataPath + wxT(DIRSEP_STR) + wxT("quincy.ini");
		LocalIniFile = false;
	}
	ini = new minIni(strIniName);
	wxSize size;
	LoadSettings(&size);

	QuincyFrame *frame = new QuincyFrame(wxT("Quincy for Pawn"), size);
	frame->Show(true);

	/* check whether this is the first run and the installation directory is
	   read-only; if so, copy the examples to a user-directory and set this
	   as the default directory */
	wxString strExamples = UserDataPath + wxT(DIRSEP_STR) + wxT("examples");
	if (!LocalIniFile && !wxDirExists(strExamples) ) {
		#if defined _MSC_VER && wxMAJOR_VERSION < 3
			wxMkDir(strExamples);
		#else
			wxMkDir(strExamples.utf8_str(), 0777);
		#endif
		ini->put(wxT("Session"), wxT("Directory"), strExamples);
		wxString strSource = MainPath.BeforeLast(DIRSEP_CHAR) + wxT(DIRSEP_STR) + wxT("examples");
		wxDir dir(strSource);
		if (dir.IsOpened()) {
			wxString filename;
			bool result = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_NO_FOLLOW);
			while (result) {
				wxString source = strSource + wxT(DIRSEP_STR) + filename;
				wxString target = strExamples + wxT(DIRSEP_STR) + filename;
				wxCopyFile(source, target, false);
				result = dir.GetNext(&filename);
			}
		}
	}

	return true;
}

void QuincyApp::PushRecentFile(const wxString& path)
{
    RemoveRecentFile(path);
    while (RecentFiles.Count() >= MAX_RECENTFILES)
        RecentFiles.RemoveAt(MAX_RECENTFILES - 1);
    RecentFiles.Insert(path, 0);
}

void QuincyApp::PushRecentWorkspace(const wxString& path)
{
    RemoveRecentWorkspace(path);
    while (RecentWorkspaces.Count() >= MAX_RECENTWORKSPACES)
        RecentWorkspaces.RemoveAt(MAX_RECENTWORKSPACES - 1);
    RecentWorkspaces.Insert(path, 0);
}

void QuincyApp::RemoveRecentFile(const wxString& path)
{
    int idx;
    while ((idx = RecentFiles.Index(path)) != wxNOT_FOUND)
        RecentFiles.RemoveAt(idx);
}

void QuincyApp::RemoveRecentWorkspace(const wxString& path)
{
    int idx;
    while ((idx = RecentWorkspaces.Index(path)) != wxNOT_FOUND)
        RecentWorkspaces.RemoveAt(idx);
}

void QuincyApp::SetLocalIniFile(bool enable, const wxString& path)
{
	LocalIniFile = enable;
	if (ini)
		delete ini;
	ini = new minIni(path);
}

void QuincyApp::LoadSettings(wxSize *size)
{
	static const wxChar* colournames[] = { wxT("Background"), wxT("Text"),
		wxT("Keywords"), wxT("Strings"), wxT("Active"), wxT("Comments"),
		wxT("Operators"), wxT("Numbers"), wxT("Preprocessor"), wxT("StringEOL"),
		wxT("Highlight") };
	static const wxChar* colourdefs[] = { wxT("FFFFFF"), wxT("000000"),
		wxT("0000C0"), wxT("800000"), wxT("FAFAC0"), wxT("808000"),
		wxT("008000"), wxT("900060"), wxT("0000c0"), wxT("FF0000"),
		wxT("000060") };

	EditTabWidth = (int)ini->getl(wxT("Editor"), wxT("TabWidth"), 4);
	EditUseTabs = ini->getbool(wxT("Editor"), wxT("HardTabs"), false);
	EditAutoIndent = ini->getbool(wxT("Editor"), wxT("AutoIndent"), true);
	EditIndentPaste = ini->getbool(wxT("Editor"), wxT("IndentClip"), false);
	EditTrailingSpaces = ini->getbool(wxT("Editor"), wxT("TrailingSpaces"));

	CodeFont.Create(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier"));
	wxString descr = ini->gets(wxT("Editor"), wxT("Font"), wxT("Courier"));
	CodeFont.SetNativeFontInfo(descr);

	for (int idx = 0; idx < CLR_COUNT; idx++) {
		wxString str = ini->gets(wxT("Colours"), colournames[idx], colourdefs[idx]);
		long clr;
		str.ToLong(&clr, 16);
		EditColours[idx] = wxColour((clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff);
	}

	SearchAdvanced = ini->getbool(wxT("Search"), wxT("Advanced"));
	SearchFlags = ini->getl(wxT("Search"), wxT("Flags"), wxFR_MATCHCASE);
	SearchScope = (int)ini->getl(wxT("Search"), wxT("Scope"), 0);
	for (int idx = 0; idx < MAX_SEARCHES; idx++) {
		wxString item = wxString::Format(wxT("Recent%d"), idx + 1);
		wxString text = ini->gets(wxT("Search"), item);
		if (text.Length() > 0)
			SearchRecent.Add(text);
	}
	for (int idx = 0; idx < MAX_SEARCHES; idx++) {
		wxString item = wxString::Format(wxT("Replace%d"), idx + 1);
		wxString text = ini->gets(wxT("Search"), item);
		if (text.Length() > 0)
			ReplaceRecent.Add(text);
	}

	UserPDFReaderPath = ini->gets(wxT("Options"), wxT("PDFReader"));
	UserPDFReaderActive = ini->getbool(wxT("Options"), wxT("PDFReaderActive"));

	for (int idx = 0;; idx++) {
		wxString key = ini->getkey(wxT("Snippets"), idx);
		if (key.Length() == 0)
			break;
		wxString expansion = ini->gets(wxT("Snippets"), key);
		if (expansion.Length() == 0)
			break;
		expansion.Replace(wxT("\\n"), wxT("\n"));
		expansion.Replace(wxT("\\t"), wxT("\t"));
		expansion.Replace(wxT("\\s"), wxT("\v"));
		expansion.Replace(wxT("\\\\"), wxT("\\"));
		SnippetList.insert(std::make_pair(key, expansion));
	}

	for (int idx = 0; idx < MAX_RECENTFILES; idx++) {
		wxString item = wxString::Format(wxT("File%d"), idx + 1);
		wxString text = ini->gets(wxT("Recent Files"), item);
		if (text.Length() == 0)
            break;
		RecentFiles.Add(text);
	}
	for (int idx = 0; idx < MAX_RECENTWORKSPACES; idx++) {
		wxString item = wxString::Format(wxT("Workspace%d"), idx + 1);
		wxString text = ini->gets(wxT("Recent Workspaces"), item);
		if (text.Length() == 0)
            break;
		RecentWorkspaces.Add(text);
	}

    for (int index = 0; index < Shortcuts.Count(); index++) {
		KbdShortcut* key = Shortcuts.GetItem(index);
		wxASSERT(key);
		wxString shortcut = ini->gets(wxT("Keyboard"), key->GetLabel(), key->GetShortcut());
		key->SetShortcut(shortcut);
	}

	long width = 600, height = 440;
	wxString buffer = ini->gets(wxT("Position"), wxT("Frame"));
	if (buffer.Len() > 0) {
		wxString strPos = buffer.BeforeFirst(' ');
		strPos.ToLong(&width);
		strPos = buffer.AfterFirst(' ');
		strPos.ToLong(&height);
	}
	size->Set(width, height);
}

void QuincyApp::SaveSettings(const wxSize& size, long splitterpos)
{
	static const wxChar* colournames[] = { wxT("Background"), wxT("Text"),
		wxT("Keywords"), wxT("Strings"), wxT("Active"), wxT("Comments"),
		wxT("Operators"), wxT("Numbers"), wxT("Preprocessor"), wxT("StringEOL"),
		wxT("Highlight") };

	ini->put(wxT("Editor"), wxT("TabWidth"), EditTabWidth);
	ini->put(wxT("Editor"), wxT("HardTabs"), EditUseTabs);
	ini->put(wxT("Editor"), wxT("AutoIndent"), EditAutoIndent);
	ini->put(wxT("Editor"), wxT("IndentClip"), EditIndentPaste);
	ini->put(wxT("Editor"), wxT("TrailingSpaces"), EditTrailingSpaces);
	ini->put(wxT("Editor"), wxT("Font"), CodeFont.GetNativeFontInfoDesc());

	for (int idx = 0; idx < CLR_COUNT; idx++) {
		long clr = (EditColours[idx].Red() << 16) | (EditColours[idx].Green() << 8) | EditColours[idx].Blue();
		wxString str = wxString::Format(wxT("%06X"), clr);
		ini->put(wxT("Colours"), colournames[idx], str);
	}

	ini->put(wxT("Search"), wxT("Advanced"), SearchAdvanced);
	ini->put(wxT("Search"), wxT("Flags"), SearchFlags);
	ini->put(wxT("Search"), wxT("Scope"), SearchScope);
	for (unsigned idx = 0; idx < MAX_SEARCHES && idx < SearchRecent.Count(); idx++) {
		wxString item = wxString::Format(wxT("Recent%d"), idx + 1);
		ini->put(wxT("Search"), item, SearchRecent[idx]);
	}
	for (unsigned idx = 0; idx < MAX_SEARCHES && idx < ReplaceRecent.Count(); idx++) {
		wxString item = wxString::Format(wxT("Replace%d"), idx + 1);
		ini->put(wxT("Search"), item, ReplaceRecent[idx]);
	}

	ini->put(wxT("Options"), wxT("PDFReader"), UserPDFReaderPath);
	ini->put(wxT("Options"), wxT("PDFReaderActive"), UserPDFReaderActive);

	std::map<wxString,wxString>::iterator iter = SnippetList.begin();
	while (iter != SnippetList.end()) {
		wxString key = iter->first;
		wxString expansion = iter->second;
		if (expansion.Length() > 0) {
			expansion.Replace(wxT("\\"), wxT("\\\\"));
			expansion.Replace(wxT("\n"), wxT("\\n"));
			expansion.Replace(wxT("\t"), wxT("\\t"));
			expansion.Replace(wxT("\v"), wxT("\\s"));
			ini->put(wxT("Snippets"), key, expansion);
		} else {
			ini->del(wxT("Snippets"), key);
		}
		iter++;
	}

	for (unsigned idx = 0; idx < RecentFiles.Count(); idx++) {
		wxString item = wxString::Format(wxT("File%d"), idx + 1);
		ini->put(wxT("Recent Files"), item, RecentFiles[idx]);
	}
	for (unsigned idx = 0; idx < RecentWorkspaces.Count(); idx++) {
		wxString item = wxString::Format(wxT("Workspace%d"), idx + 1);
		ini->put(wxT("Recent Workspaces"), item, RecentWorkspaces[idx]);
	}

	for (int index = 0; index < Shortcuts.Count(); index++) {
		KbdShortcut* key = Shortcuts.GetItem(index);
		wxASSERT(key);
		ini->put(wxT("Keyboard"), key->GetLabel(), key->GetShortcut());
	}

	wxString strPos;
	strPos.Printf(wxT("%d %d"), size.GetWidth(), size.GetHeight());
	ini->put(wxT("Position"), wxT("Frame"), strPos);
	ini->put(wxT("Position"), wxT("Splitter"), splitterpos);
}

