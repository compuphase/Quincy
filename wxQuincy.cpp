/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright CompuPhase, 2009-2024
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
 *  Version: $Id: wxQuincy.cpp 7151 2024-03-23 16:08:18Z thiadmer $
 */
#include "wxQuincy.h"
#include "QuincyFrame.h"
#include "minIni.h"
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/fs_zip.h>
#include <wx/msw/wrapshl.h>

#if !defined wxDIR_NO_FOLLOW
    #define wxDIR_NO_FOLLOW 0   /* only defined for wxWidgets 2.9 and up */
#endif

static void MergeIni(minIni* dest, minIni* source);

IMPLEMENT_APP(QuincyApp)

QuincyApp* theApp;

bool QuincyApp::OnInit()
{
    theApp = this;

    /* get the path where the binaries are and the root path (which may be the same) */
    BinPath = wxStandardPaths::Get().GetExecutablePath();
    BinPath = wxPathOnly(BinPath);

    if (BinPath.Right(4).CmpNoCase(DIRSEP_STR "bin") == 0)
        RootPath = BinPath.Left(BinPath.Length() - 4); /* strip off "/bin" */
    else
        RootPath = BinPath; /* not installed in ./bin, everything must be below the installation directory */

    /* set other system directories */
    DocPath = RootPath + DIRSEP_STR "doc";
    /* "examples" path is set further down in this routine */

    /* create INI file path, then get settings */
    UserDataPath = wxStandardPaths::Get().GetUserConfigDir();
    #if defined __WXMSW__
        UserDataPath += DIRSEP_STR "pawn";
    #elif defined __WXOSX__
        UserDataPath += DIRSEP_STR "pawn";
    #else
        UserDataPath += DIRSEP_STR ".pawn";
    #endif
    if (!wxDirExists(UserDataPath)) {
        #if defined _MSC_VER && wxMAJOR_VERSION < 3
            wxMkDir(UserDataPath);
        #else
            wxMkDir(UserDataPath.utf8_str(), 0777);
        #endif
    }

    /* configure handlers for specific file formats and protocols */
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem::AddHandler(new wxInternetFSHandler());

    /* create the list of menu shortcuts (which the menu needs); at the same
       time, set the defaults */
    Shortcuts.Add("New", "&New", "Ctrl+N", "File");
    Shortcuts.Add("Open", "&Open...", "Ctrl+O", "File");
    Shortcuts.Add("Save", "&Save", "Ctrl+S", "File");
    Shortcuts.Add("SaveAs", "Save &as...", "F12", "File");
    Shortcuts.Add("SaveAll", "Save a&ll", "Ctrl+Shift+S", "File");
    Shortcuts.Add("Close", "&Close", "Ctrl+F4", "File");
    Shortcuts.Add("OpenWorkspace", "Open &workspace...", wxEmptyString, "File");
    Shortcuts.Add("SaveWorkspace", "Save w&orkspace...", wxEmptyString, "File");
    Shortcuts.Add("CloseWorkspace", "Close workspace...", wxEmptyString, "File");
    Shortcuts.Add("Print", "&Print...", "Ctrl+P", "File");
    Shortcuts.Add("Quit", "&Quit", "Alt+F4", "File");
    Shortcuts.Add("Undo", "&Undo", "Ctrl+Z", "Edit");
    Shortcuts.Add("Redo", "Re&do", wxEmptyString, "Edit");
    Shortcuts.Add("Cut", "Cu&t", "Ctrl+X", "Edit");
    Shortcuts.Add("Copy", "&Copy", "Ctrl+C", "Edit");
    Shortcuts.Add("Paste", "&Paste", "Ctrl+V", "Edit");
    Shortcuts.Add("Find", "&Find...", "Ctrl+F", "Edit");
    Shortcuts.Add("FindNext", "Find &next", "F3", "Edit");
    Shortcuts.Add("Replace", "&Replace", "Ctrl+H", "Edit");
    Shortcuts.Add("GotoLine", "&Go to line...", "Ctrl+G", "Edit");
    Shortcuts.Add("GotoSymbol", "Go to &symbol definition", "Ctrl+F6", "Edit");
    Shortcuts.Add("MatchBrace", "&Match brace", "Ctrl+]", "Edit");
    Shortcuts.Add("FillColumn", "Fill/insert columns", wxEmptyString, "Edit");
    Shortcuts.Add("Autocomplete", "&Autocomplete", "Ctrl+Space", "Edit");
    Shortcuts.Add("ToggleBookmark", "Toggle &Bookmark", "Ctrl+F2", "Bookmarks");
    Shortcuts.Add("NextBookmark", "&Next bookmark", "F2", "Bookmarks");
    Shortcuts.Add("PrevBookmark", "P&revious bookmark", "Shift+F2", "Bookmarks");
    Shortcuts.Add("ViewWhitespace", "&TABs and spaces", wxEmptyString, "View");
    Shortcuts.Add("ViewIndentGuides", "Indentation &Guides", wxEmptyString, "View");
    Shortcuts.Add("Compile", "&Compile", "F7", "Build / Run");
    Shortcuts.Add("Transfer", "&Transfer", "Ctrl+F7", "Build / Run");
    Shortcuts.Add("Debug", "Start &Debugging", "F5", "Build / Run");
    Shortcuts.Add("Run", "&Run without debugging", "Ctrl+F5", "Build / Run");
    Shortcuts.Add("Stop", "&Stop", "Shift-F5", "Build / Run");
    Shortcuts.Add("StepInto", "Step &Into", "F11", "Build / Run");
    Shortcuts.Add("StepOver", "Step &Over", "F10", "Build / Run");
    Shortcuts.Add("StepOut", "Step Out", "Ctrl+F11", "Build / Run");
    Shortcuts.Add("RunToCursor", "Run to &Cursor", "Ctrl+F10", "Build / Run");
    Shortcuts.Add("ToggleBreakpoint", "Toggle &Breakpoint", "F9", "Breakpoints");
    Shortcuts.Add("ClearBreakpoints", "Clear all breakpoints", wxEmptyString, "Breakpoints");
    Shortcuts.Add("Options", "&Options...", "Alt+F7", "Tools");
    Shortcuts.Add("SampleBrowser", "&Sample browser...", "Alt+F1", "Tools");
    Shortcuts.Add("TabToSpace", "Tabs to Spaces", wxEmptyString, "Whitespace");
    Shortcuts.Add("IndentToTab", "Spaces to Tabs (indent only)", wxEmptyString, "Whitespace");
    Shortcuts.Add("SpaceToTab", "Spaces to Tabs (all)", wxEmptyString, "Whitespace");
    Shortcuts.Add("TrimTrailing", "Trim trailing whitespace", wxEmptyString, "Whitespace");
    Shortcuts.Add("DeviceTool", "Configure Device", wxEmptyString, "Tools");
    Shortcuts.Add("GeneralHelp", "&IDE User Guide", "Shift+F1", "Help");
    Shortcuts.Add("ContextHelp", "Context help", "F1", "Help");

    /* first check whether an INI file is available in the main path (and
       verify that it is writable). Otherwise go to the "application data"
       directory */
    LocalIniFile = true;
    wxString strIniName = BinPath + DIRSEP_STR + "quincy.ini";
    if (!wxFileExists(strIniName) || !wxFile::Access(strIniName, wxFile::write))
        LocalIniFile = false;
    /* always consider ProgramFiles and ProgramFiles32 as read-only */
    #if defined _WIN32
        wxString ProgramFiles = wxStandardPaths::MSWGetShellDir(CSIDL_PROGRAM_FILES) + DIRSEP_STR;       /* for either 32-bit or 64-bit programs */
        wxString ProgramFiles64 = wxStandardPaths::MSWGetShellDir(CSIDL_PROGRAM_FILESX86) + DIRSEP_STR;  /* for 32-bit programms in 64-bit Windows */
        if (BinPath.Left(ProgramFiles64.Length()).CmpNoCase(ProgramFiles64) == 0)
            LocalIniFile = false;
        if (BinPath.Left(ProgramFiles.Length()).CmpNoCase(ProgramFiles) == 0)
            LocalIniFile = false;
    #endif
    if (!LocalIniFile)
        strIniName = UserDataPath + DIRSEP_STR + "quincy.ini";
    ini = new minIni(strIniName);
    /* see whether there is a "merge" ini file and whether its timestamp is
       higher than the one stored in the main INI file; if so, merge */
    wxString strMergeName = BinPath + DIRSEP_STR + "quincy_merge.ini";
    if (wxFileExists(strMergeName)) {
        minIni merge(strMergeName);
        wxString mstamp = merge.gets("Merge", "stamp");
        long mdate = 0, mtime = 0;
        mstamp.BeforeFirst('-').ToLong(&mdate);
        mstamp.AfterFirst('-').ToLong(&mtime);
        wxString qstamp = ini->gets("Merge", "stamp");
        long qdate = 0, qtime = 0;
        qstamp.BeforeFirst('-').ToLong(&qdate);
        qstamp.AfterFirst('-').ToLong(&qtime);
        long mbuild = merge.getl("Merge", "build");
        long qbuild = ini->getl("Merge", "build");
        if (mdate > qdate || (mdate == qdate && mtime > qtime) || mbuild > qbuild) {
            /* the merge file is of a later date, we should merge */
            MergeIni(ini, &merge);
        }
    }
    /* load the settings */
    wxSize size;
    LoadSettings(&size);

    QuincyFrame *frame = new QuincyFrame("Quincy for Pawn", size);
    frame->Show(true);

    /* check whether this is the first run and the installation directory is
       read-only; if so, copy the examples to a user-directory and set this
       as the default directory */
    ExamplesPath = UserDataPath + DIRSEP_STR + "examples";
    if (!LocalIniFile && !wxDirExists(ExamplesPath)) {
        #if defined _MSC_VER && wxMAJOR_VERSION < 3
            wxMkDir(ExamplesPath);
        #else
            wxMkDir(ExamplesPath.utf8_str(), 0777);
        #endif
        wxString strSource = RootPath + DIRSEP_STR "examples";
        wxDir dir(strSource);
        if (dir.IsOpened()) {
            wxString filename;
            bool result = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_NO_FOLLOW);
            while (result) {
                wxString source = strSource + DIRSEP_STR + filename;
                wxString target = ExamplesPath + DIRSEP_STR + filename;
                wxCopyFile(source, target, false);
                result = dir.GetNext(&filename);
            }
        }
    } else if (LocalIniFile) {
        wxString path = RootPath + DIRSEP_STR "examples";
        if (!wxDirExists(path)) {
            #if defined _MSC_VER && wxMAJOR_VERSION < 3
                wxMkDir(path);
            #else
                wxMkDir(path.utf8_str(), 0777);
            #endif
        }
        if (wxDirExists(path))
            ExamplesPath = path;
    }
    if (!wxDirExists(ExamplesPath)) {
        /* file copy failed, or file copy skipped (because the home directory is read-write) */
        ExamplesPath = RootPath + DIRSEP_STR "examples";
    }

    /* also make sure to copy the examples of the currently active target host */
    wxString host = frame->GetTargetHost();
    if (!LocalIniFile && host.Length() > 0) {
        wxString strTarget = ExamplesPath + DIRSEP_STR + host;
        if (!wxDirExists(strTarget)) {
            #if defined _MSC_VER && wxMAJOR_VERSION < 3
                wxMkDir(strTarget);
            #else
                wxMkDir(strTarget.utf8_str(), 0777);
            #endif
            wxString strSource = RootPath + DIRSEP_STR "examples" DIRSEP_STR + host;
            wxDir dir(strSource);
            if (dir.IsOpened()) {
                wxString filename;
                bool result = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_NO_FOLLOW);
                while (result) {
                    wxString source = strSource + DIRSEP_STR + filename;
                    wxString target = strTarget + DIRSEP_STR + filename;
                    wxCopyFile(source, target, false);
                    result = dir.GetNext(&filename);
                }
            }
        }
    }

    return true;
}

static void MergeIni(minIni* dest, minIni* source)
{
    int sidx = 0;
    for ( ;;) {
        wxString section = source->getsection(sidx);
        if (section.Length() == 0)
            break;
        int kidx = 0;
        for ( ;;) {
            wxString key = source->getkey(section, kidx);
            if (key.Length() == 0)
                break;
            wxString value = source->gets(section, key);
            if (value.Length() > 0)
                dest->put(section, key, value);
            kidx++;
        }
        sidx++;
    }
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
    static const char* colournames[] = { "Background", "Text",
        "Keywords", "Strings", "Active", "Comments",
        "Operators", "Numbers", "Preprocessor", "StringEOL",
        "Highlight" };
    static const char* colourdefs[] = { "FFFFFF", "000000",
        "0000C0", "800000", "FAFAC0", "808000",
        "008000", "900060", "0000c0", "FF0000",
        "000060" };

    EditTabWidth = (int)ini->getl("Editor", "TabWidth", 4);
    EditUseTabs = ini->getbool("Editor", "HardTabs", false);
    EditAutoIndent = ini->getbool("Editor", "AutoIndent", true);
    EditIndentPaste = ini->getbool("Editor", "IndentClip", false);
    EditTrailingSpaces = ini->getbool("Editor", "TrailingSpaces");

    CodeFont.Create(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier");
    wxString descr = ini->gets("Editor", "Font", "Courier");
    CodeFont.SetNativeFontInfo(descr);

    for (int idx = 0; idx < CLR_COUNT; idx++) {
        wxString str = ini->gets("Colours", colournames[idx], colourdefs[idx]);
        long clr;
        str.ToLong(&clr, 16);
        EditColours[idx] = wxColour((clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff);
    }

    SearchAdvanced = ini->getbool("Search", "Advanced");
    SearchFlags = ini->getl("Search", "Flags", wxFR_MATCHCASE);
    SearchScope = (int)ini->getl("Search", "Scope", 0);
    for (int idx = 0; idx < MAX_SEARCHES; idx++) {
        wxString item = wxString::Format("Recent%d", idx + 1);
        wxString text = ini->gets("Search", item);
        if (text.Length() > 0)
            SearchRecent.Add(text);
    }
    for (int idx = 0; idx < MAX_SEARCHES; idx++) {
        wxString item = wxString::Format("Replace%d", idx + 1);
        wxString text = ini->gets("Search", item);
        if (text.Length() > 0)
            ReplaceRecent.Add(text);
    }

    UserPDFReaderPath = ini->gets("Options", "PDFReader");
    UserPDFReaderActive = ini->getbool("Options", "PDFReaderActive");

    UpdateURL = ini->gets("Config", "UpdateURL");
    if (UpdateURL.Length() > 0 && UpdateURL[UpdateURL.Length() - 1] != '/')
        UpdateURL += "/";

    for (int idx = 0;; idx++) {
        wxString key = ini->getkey("Snippets", idx);
        if (key.Length() == 0)
            break;
        wxString expansion = ini->gets("Snippets", key);
        if (expansion.Length() == 0)
            break;
        expansion.Replace("\\n", "\n");
        expansion.Replace("\\t", "\t");
        expansion.Replace("\\s", "\v");
        expansion.Replace("\\\\", "\\");
        SnippetList.insert(std::make_pair(key, expansion));
    }

    for (int idx = 0; idx < MAX_RECENTFILES; idx++) {
        wxString item = wxString::Format("File%d", idx + 1);
        wxString text = ini->gets("Recent Files", item);
        if (text.Length() == 0)
            break;
        RecentFiles.Add(text);
    }
    for (int idx = 0; idx < MAX_RECENTWORKSPACES; idx++) {
        wxString item = wxString::Format("Workspace%d", idx + 1);
        wxString text = ini->gets("Recent Workspaces", item);
        if (text.Length() == 0)
            break;
        RecentWorkspaces.Add(text);
    }

    for (int index = 0; index < Shortcuts.Count(); index++) {
        KbdShortcut* key = Shortcuts.GetItem(index);
        wxASSERT(key);
        wxString shortcut = ini->gets("Keyboard", key->GetLabel(), key->GetShortcut());
        key->SetShortcut(shortcut);
    }

    long width = 600, height = 440;
    wxString buffer = ini->gets("Position", "Frame");
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
    static const char* colournames[] = { "Background", "Text",
        "Keywords", "Strings", "Active", "Comments",
        "Operators", "Numbers", "Preprocessor", "StringEOL",
        "Highlight" };

    ini->put("Editor", "TabWidth", EditTabWidth);
    ini->put("Editor", "HardTabs", EditUseTabs);
    ini->put("Editor", "AutoIndent", EditAutoIndent);
    ini->put("Editor", "IndentClip", EditIndentPaste);
    ini->put("Editor", "TrailingSpaces", EditTrailingSpaces);
    ini->put("Editor", "Font", CodeFont.GetNativeFontInfoDesc());

    for (int idx = 0; idx < CLR_COUNT; idx++) {
        long clr = (EditColours[idx].Red() << 16) | (EditColours[idx].Green() << 8) | EditColours[idx].Blue();
        wxString str = wxString::Format("%06X", clr);
        ini->put("Colours", colournames[idx], str);
    }

    ini->put("Search", "Advanced", SearchAdvanced);
    ini->put("Search", "Flags", SearchFlags);
    ini->put("Search", "Scope", SearchScope);
    for (unsigned idx = 0; idx < MAX_SEARCHES && idx < SearchRecent.Count(); idx++) {
        wxString item = wxString::Format("Recent%d", idx + 1);
        ini->put("Search", item, SearchRecent[idx]);
    }
    for (unsigned idx = 0; idx < MAX_SEARCHES && idx < ReplaceRecent.Count(); idx++) {
        wxString item = wxString::Format("Replace%d", idx + 1);
        ini->put("Search", item, ReplaceRecent[idx]);
    }

    ini->put("Options", "PDFReader", UserPDFReaderPath);
    ini->put("Options", "PDFReaderActive", UserPDFReaderActive);

    std::map<wxString,wxString>::iterator iter = SnippetList.begin();
    while (iter != SnippetList.end()) {
        wxString key = iter->first;
        wxString expansion = iter->second;
        if (expansion.Length() > 0) {
            expansion.Replace("\\", "\\\\");
            expansion.Replace("\n", "\\n");
            expansion.Replace("\t", "\\t");
            expansion.Replace("\v", "\\s");
            ini->put("Snippets", key, expansion);
        } else {
            ini->del("Snippets", key);
        }
        ++iter;
    }

    for (unsigned idx = 0; idx < RecentFiles.Count(); idx++) {
        wxString item = wxString::Format("File%d", idx + 1);
        ini->put("Recent Files", item, RecentFiles[idx]);
    }
    for (unsigned idx = 0; idx < RecentWorkspaces.Count(); idx++) {
        wxString item = wxString::Format("Workspace%d", idx + 1);
        ini->put("Recent Workspaces", item, RecentWorkspaces[idx]);
    }

    for (int index = 0; index < Shortcuts.Count(); index++) {
        KbdShortcut* key = Shortcuts.GetItem(index);
        wxASSERT(key);
        ini->put("Keyboard", key->GetLabel(), key->GetShortcut());
    }

    wxString strPos;
    strPos.Printf("%d %d", size.GetWidth(), size.GetHeight());
    ini->put("Position", "Frame", strPos);
    ini->put("Position", "Splitter", splitterpos);
}

