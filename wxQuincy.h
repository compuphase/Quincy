/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright ITB CompuPhase, 2009-2017
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
 *  Version: $Id: wxQuincy.h 7108 2024-02-19 22:02:45Z thiadmer $
 */
#ifndef _QUINCY_H
#define _QUINCY_H

#include <wx/wx.h>
#include <wx/colour.h>
#include <wx/filefn.h>
#include <wx/font.h>
#include <wx/stdpaths.h>
#include <map>
#include "KbdShortcuts.h"
#include "wxMinIni.h"

#if (defined _Windows || defined __WIN32 || defined __WIN32__) && !defined _WIN32
    #define _WIN32  /* so the rest of the code only needs to check for _WIN32 */
#endif

#if defined _WIN32
    #define DIRSEP_CHAR '\\'
    #define DIRSEP_STR  "\\"
    #define EXE_EXT     ".exe"
#else
    #define DIRSEP_CHAR '/'
    #define DIRSEP_STR  "/"
    #define EXE_EXT     ""
#endif

enum {
    CLR_BACKGROUND,
    CLR_TEXT,
    CLR_KEYWORDS,
    CLR_STRINGS,
    CLR_ACTIVE,
    CLR_COMMENTS,
    CLR_OPERATORS,
    CLR_NUMBERS,
    CLR_PREPROCESSOR,
    CLR_STRINGEOL,
    CLR_HIGHLIGHT,
    /* ----- */
    CLR_COUNT
};

#define MAX_SEARCHES    20  /* only 20 recent searches are saved */

class QuincyApp : public wxApp
{
public:
    virtual bool OnInit();

    int  GetTabWidth() const            { return EditTabWidth; }
    void SetTabWidth(int width)         { EditTabWidth = width; }
    bool GetUseTabs() const             { return EditUseTabs; }
    void SetUseTabs(bool enable)        { EditUseTabs = enable; }
    bool GetAutoIndent() const          { return EditAutoIndent; }
    void SetAutoIndent(bool enable)     { EditAutoIndent = enable; }
    bool GetIndentBlocks() const        { return EditIndentPaste; }
    void SetIndentBlocks(bool enable)   { EditIndentPaste = enable; }
    bool GetStripTrailing() const       { return !EditTrailingSpaces; }
    void SetStripTrailing(bool enable)  { EditTrailingSpaces = !enable; }
    bool UseLocalIniFile() const        { return LocalIniFile; }
    void SetLocalIniFile(bool enable, const wxString& path);

    wxString GetReaderPath() const      { return UserPDFReaderPath; }
    void SetReaderPath(const wxString& path)    { UserPDFReaderPath = path; }
    bool GetReaderPathValid() const     { return UserPDFReaderActive; }
    void SetReaderPathValid(bool valid) { UserPDFReaderActive = valid; }

    wxString GetUpdateURL() const       { return (UpdateURL.Length() != 0) ? UpdateURL : (const wxString)wxT("https://www.compuphase.com/update/"); }

    wxString GetRootPath() const        { return RootPath; }
    wxString GetBinPath() const         { return BinPath; }
    wxString GetDocPath() const         { return DocPath; }
    wxString GetExamplesPath() const    { return ExamplesPath; }
    wxString GetUserDataPath() const    { return UserDataPath; }

    wxString GetRecentFile(unsigned idx) const { return (idx < RecentFiles.Count()) ? RecentFiles[idx] : wxT(""); }
    void PushRecentFile(const wxString& path);
    void RemoveRecentFile(const wxString& path);
    wxString GetRecentWorkspace(unsigned idx) const { return (idx < RecentWorkspaces.Count()) ? RecentWorkspaces[idx] : wxT(""); }
    void PushRecentWorkspace(const wxString& path);
    void RemoveRecentWorkspace(const wxString& path);

    minIni* GetConfigFile() const       { return ini; }
    void LoadSettings(wxSize *size);
    void SaveSettings(const wxSize& size, long splitterpos);

    KbdShortcutList Shortcuts;
    std::map<wxString, wxString> SnippetList;

    wxFont CodeFont;
    wxColour EditColours[CLR_COUNT];

    bool SearchAdvanced;
    long SearchFlags;
    int SearchScope;
    wxArrayString SearchRecent;
    wxArrayString ReplaceRecent;

private:
    minIni* ini;
    bool LocalIniFile;

    wxString RootPath;
    wxString BinPath;
    wxString DocPath;
    wxString ExamplesPath;
    wxString UserDataPath;

    int EditTabWidth;
    bool EditUseTabs;
    bool EditAutoIndent;
    bool EditIndentPaste;
    bool EditTrailingSpaces;

    wxString UserPDFReaderPath;
    bool UserPDFReaderActive;

    wxString UpdateURL;

    wxArrayString RecentFiles;
    wxArrayString RecentWorkspaces;
};

extern QuincyApp* theApp;

#endif /* _QUINCY_H */
