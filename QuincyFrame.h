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
 *  Version: $Id: QuincyFrame.h 7113 2024-02-25 21:29:31Z thiadmer $
 */
#ifndef _QUINCYFRAME_H
#define _QUINCYFRAME_H
#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/dnd.h>
#include <wx/event.h>
#include <wx/fdrepdlg.h>
#include <wx/icon.h>
#include <wx/listctrl.h>
#include <wx/process.h>
#include <wx/regex.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>
#include <wx/timer.h>
#include <wx/treectrl.h>
#include <wx/utils.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
#include "HelpIndex.h"
#include "SymbolBrowser.h"

#define MAX_EDITORS 32

#define UI_UNDO     0x0001
#define UI_REDO     0x0002
#define UI_CUTCOPY  0x0004
#define UI_PASTE    0x0008
#define UI_RUN      0x0010
#define UI_ABORT    0x0020
#define UI_TRANSFER 0x0040
#define UI_DBGTOOLS 0x0080
#define UI_FINDNEXT 0x0100

#define CTX_RESTART 0x01    /* start scanning from top (change was detected) */
#define CTX_RESET   0x02    /* like CTX_RESTART, but also clears the list before starting the scan */
#define CTX_FULL    0x04    /* like CTX_RESTART, but scans the complete file before returning */
#define CTX_LINES   1
class ContextParse {
public:
    ContextParse() {
        choicectrl = NULL;
        Names.Clear();
        Ranges.Clear();
        currentselection = -1;
        activeedit = NULL;
        startline = 0;
        nestlevel = 0;
        incomment = inparamlist = false;
        context = wxEmptyString;
        topline = btmline = -1;
    }
    void SetControl(wxChoice* ctrl) { choicectrl = ctrl; }
    void ShowContext(int linenr); /* sets the context name in the control */
    bool ScanContext(wxStyledTextCtrl* edit, int flags = 0);
    int Lookup(const wxString& name);

private:
    wxRegEx re;

    wxArrayString WorkNames;
    wxArrayLong WorkRanges;
    wxStyledTextCtrl* activeedit; /* active editor */
    int startline;          /* line that the previous scan left off */
    bool incomment;
    bool inparamlist;
    int nestlevel;
    wxString context;
    int topline, btmline;

    wxArrayString Names;
    wxArrayLong Ranges;
    wxChoice* choicectrl;   /* control with the updated choice lists */
    int currentselection;
};

#define DEBUG_NONE      0
#define DEBUG_LOCAL     0x01
#define DEBUG_REMOTE    0x02
#define DEBUG_BOTH      (DEBUG_LOCAL | DEBUG_REMOTE)

class QuincyFrame : public wxFrame
{
    friend class DragAndDropFile;

public:
    QuincyFrame(const wxString& title, const wxSize& size);

    void AdjustTitle();
    virtual void OnCloseWindow(wxCloseEvent& event);
    virtual void OnActivate(wxActivateEvent& event);

    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnNewFile(wxCommandEvent& event);
    virtual void OnOpen(wxCommandEvent& event);
    virtual void OnSave(wxCommandEvent& event);
    virtual void OnSaveAs(wxCommandEvent& event);
    virtual void OnSaveAll(wxCommandEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnLoadWorkSpace(wxCommandEvent& event);
    virtual void OnSaveWorkSpace(wxCommandEvent& event);
    virtual void OnCloseWorkSpace(wxCommandEvent& event);
    virtual void OnPrint(wxCommandEvent& event);
    virtual void OnRecentFile(wxCommandEvent& event);
    virtual void OnRecentWorkspace(wxCommandEvent& event);
    virtual void OnUndo(wxCommandEvent& event);
    virtual void OnRedo(wxCommandEvent& event);
    virtual void OnCut(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnPaste(wxCommandEvent& event);
    virtual void OnBookmarkToggle(wxCommandEvent& event);
    virtual void OnBookmarkNext(wxCommandEvent& event);
    virtual void OnBookmarkPrevious(wxCommandEvent& event);
    virtual void OnFindDlg(wxCommandEvent& event);
    virtual void OnFindNext(wxCommandEvent& event);
    virtual void OnReplaceDlg(wxCommandEvent& event);
    virtual void OnGotoDlg(wxCommandEvent& event);
    virtual void OnGotoSymbol(wxCommandEvent& event);
    virtual void OnMatchBrace(wxCommandEvent& event);
    virtual void OnFillColumn(wxCommandEvent& event);
    virtual void OnViewWhiteSpace(wxCommandEvent& event);
    virtual void OnViewIndentGuides(wxCommandEvent& event);
    virtual void OnCompile(wxCommandEvent& event);
    virtual void OnTransfer(wxCommandEvent& event);
    virtual void OnDebug(wxCommandEvent& event);
    virtual void OnRun(wxCommandEvent& event);
    virtual void OnAbort(wxCommandEvent& event);
    virtual void OnStepInto(wxCommandEvent& event);
    virtual void OnStepOver(wxCommandEvent& event);
    virtual void OnStepOut(wxCommandEvent& event);
    virtual void OnRunToCursor(wxCommandEvent& event);
    virtual void OnBreakpointToggle(wxCommandEvent& event);
    virtual void OnBreakpointClear(wxCommandEvent& event);
    virtual void OnSettings(wxCommandEvent& event);
    virtual void OnSampleBrowser(wxCommandEvent& event);
    virtual void OnTabsToSpaces(wxCommandEvent& event);
    virtual void OnSpacesToTabs(wxCommandEvent& event);
    virtual void OnIndentsToTabs(wxCommandEvent& event);
    virtual void OnTrimTrailing(wxCommandEvent& event);
    virtual void OnDeviceTool(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnHelp(wxCommandEvent& event);
    virtual void OnContextHelp(wxCommandEvent& event);
    virtual void OnAutoComplete(wxCommandEvent& event);
    virtual void OnIdle(wxIdleEvent& event);
    virtual void OnTerminateApp(wxProcessEvent& event);
    virtual void OnSelectContext(wxCommandEvent& event);

    virtual void OnUIWorkSpace(wxUpdateUIEvent& event);
    virtual void OnUIUndo(wxUpdateUIEvent& event);
    virtual void OnUICutCopy(wxUpdateUIEvent& event);
    virtual void OnUIPaste(wxUpdateUIEvent& event);
    virtual void OnUIFind(wxUpdateUIEvent& event);
    virtual void OnUIRun(wxUpdateUIEvent& event);
    virtual void OnUIStepInto(wxUpdateUIEvent& event);

    virtual void OnTabChange(wxAuiNotebookEvent& event);
    virtual void OnTabClose(wxAuiNotebookEvent& event);

    virtual void OnErrorSelect(wxListEvent& event);
    virtual void OnWatchEdited(wxListEvent& event);
    virtual void OnWatchActivated(wxListEvent& event);
    virtual void OnWatchDelete(wxListEvent& event);
    virtual void OnSymbolSelect(wxTreeEvent& event);
    virtual void OnTerminalChar(wxKeyEvent& event);
    virtual void OnSearchSelect(wxTreeEvent& event);

    virtual void OnFindAction(wxFindDialogEvent& event);
    virtual void OnFindClose(wxFindDialogEvent& event);
    virtual void OnReplace(wxFindDialogEvent& event);
    virtual void OnReplaceAll(wxFindDialogEvent& event);

    virtual void OnEditorChange(wxStyledTextEvent& event);
    virtual void OnEditorCharAdded(wxStyledTextEvent& event);
    virtual void OnEditorPosition(wxStyledTextEvent& event);
    virtual void OnEditorDwellStart(wxStyledTextEvent& event);
    virtual void OnEditorDwellEnd(wxStyledTextEvent& event);

    void LoadSourceFile(const wxString& path);
    void LoadWorkspace(const wxString& path);
    void RebuildRecentMenus();
    void SetEditorsStyle(wxStyledTextCtrl *edit);

    bool LoadHostConfiguration(const wxString& host);
    int  GetDebuggerEnabled() const                 { return DebuggerEnabled; }
    bool GetDebuggerEnabled(int mask) const         { return (DebuggerEnabled & mask) != 0; }
    int  GetDefaultDebugLevel() const               { return DefaultDebugLevel; }
    void SetDefaultDebugLevel(int level)            { DefaultDebugLevel = level; }
    int  GetDefaultOptimize() const                 { return DefaultOptimize; }
    void SetDefaultOptimize(int level)              { DefaultOptimize = level; }
    int  GetDebugLevel() const                      { return DebugLevel; }
    void SetDebugLevel(int level)                   { DebugLevel = level; }
    int  GetDebuggerSelected() const                { return DebuggerSelected; }
    void SetDebuggerSelected(int option)            { DebuggerSelected = option; }
    bool GetDebugLogEnabled() const                 { return DebugLogEnabled; }
    void SetDebugLogEnabled(bool enable)            { DebugLogEnabled = enable; }
    bool GetTransferEnabled() const                 { return (DebuggerEnabled & DEBUG_REMOTE) != 0 || UploadTool.length() > 0; }
    bool GetAutoTransferEnabled() const             { return AutoTransfer && GetTransferEnabled(); }
    void SetAutoTransferEnabled(bool option)        { AutoTransfer = option && GetTransferEnabled(); }
    int  GetOptimizationLevel() const               { return OptimizationLevel; }
    void SetOptimizationLevel(int level)            { OptimizationLevel = level; }
    bool GetOverlayEnabled() const                  { return OverlayEnabled; }
    void SetOverlayEnabled(bool enable)             { OverlayEnabled = enable; }
    bool GetOverlayCode() const                     { return OverlayCode; }
    void SetOverlayCode(bool enable)                { OverlayCode = enable; }
    bool GetVerboseBuild() const                    { return VerboseBuild; }
    void SetVerboseBuild(bool enable)               { VerboseBuild = enable; }
    bool GetCreateReport() const                    { return CreateReport; }
    void SetCreateReport(bool enable)               { CreateReport = enable; }
    long GetDebugBaudrate() const                   { return DebugBaudrate; }
    void SetDebugBaudrate(long baud)                { DebugBaudrate = baud; }
    wxString GetDebugPort() const                   { return DebugPort; }
    void     SetDebugPort(const wxString& port)     { DebugPort = port; }

    wxString GetTargetHost() const                  { return strTargetHost; }
    void     SetTargetHost(const wxString& name)    { strTargetHost = name; }
    wxString GetDefineString() const                { return strDefines; }
    void     SetDefineString(const wxString& string){ strDefines = string; }
    wxString GetIncludePath() const                 { return strIncludePath; }
    void     SetIncludePath(const wxString& path)   { strIncludePath = path; }
    wxString GetCompilerPath() const                { return strCompilerPath; }
    void     SetCompilerPath(const wxString& path)  { strCompilerPath = path; }
    wxString GetOutputPath() const                  { return strOutputPath; }
    void     SetOutputPath(const wxString& path)    { strOutputPath = path; }
    wxString GetPreBuild() const                    { return strPreBuild; }
    void     SetPreBuild(const wxString& command)   { strPreBuild = command; }
    wxString GetMiscCmdOptions() const              { return strMiscCmdOptions; }
    void     SetMiscCmdOptions(const wxString& opt) { strMiscCmdOptions = opt; }
    wxString GetTargetPath() const                  { return strTargetPath; }

    wxString GetFixedAMXName() const                { return strFixedAMXName; }
    bool     HasFixedAMXName() const                { return UseFixedAMXName; }
    void     SetFixedAMXName(bool enable)           { UseFixedAMXName = enable; }

private:
    wxMenuBar* menuBar;
    wxMenu* menuFile;
    wxMenu* menuEdit;
    wxMenu* menuView;
    wxMenu* menuBuild;
    wxMenu* menuTools;
    wxMenu* menuHelp;
    wxMenu* menuRecentFiles;
    wxMenu* menuRecentWorkspaces;
    wxMenu* menuBookmarks;
    wxMenu* menuBreakpoints;
    wxMenu* menuTabSpace;
    wxAuiToolBar* ToolBar;
    wxChoice* FunctionList;

    wxSplitterWindow* SplitterFrame;
    wxPanel* pnlEdit;
    wxPanel* pnlPane;

    wxAuiNotebook* EditTab;

    wxAuiNotebook* PaneTab;
    wxListView* BuildLog;   /* Build */
    wxListView* ErrorLog;   /* Messages */
    wxTreeCtrl* BrowserTree;/* Symbols */
    wxListView* WatchLog;   /* Watches */
    wxTextCtrl* Terminal;   /* Output */
    wxTreeCtrl* SearchLog;  /* Search results */

    wxStyledTextCtrl* Editor[MAX_EDITORS];
    wxString Filename[MAX_EDITORS];
    bool EditorDirty[MAX_EDITORS];
    time_t FileTimeStamp[MAX_EDITORS];
    bool AddEditor(const wxString& name = wxEmptyString);
    bool RemoveEditor(int index = -1, bool deletecontrol = true);
    bool ScanUTF8(const wxString& filename);
    bool Latin1ToUTF8(const wxString& filename);
    bool LoadFile(const wxString& filename, wxStyledTextCtrl* edit);
    void SetChanged(int index = -1, bool changed = true);

    bool CheckSaveFile(bool force_save = false, bool force_prompt = false, wxStyledTextCtrl *edit = NULL);
    bool SaveAllFiles(bool prompt = false);
    void CloseCurrentFile(bool deletetab = true);
    bool LoadSession();
    bool SaveSession();
    void StripTrailingSpaces(wxStyledTextCtrl *edit);
    wxString OptionallyQuoteString(const wxString& string);
    bool IsPawnFile(const wxString& path, bool allow_inc = true);
    void PrepareSearchLog();
    void SpaceToTab(bool indent_only);
    bool CompileSource(const wxString& script);
    bool TransferScript(const wxString& path);
    bool RunCurrentScript(bool debug = false);
    void HandleDebugResponse(const wxString& cmd);
    void SendDebugCommand(const wxString& cmd);
    void SendWatchList();
    void BuildBreakpointList();
    void SendBreakpointList();
    bool GotoSymbol(const CSymbolEntry* symbol);

    bool IgnoreChangeEvent;     /* ignore any "change" event of an editor, because the change is forced */
    long MatchBracePos[2];      /* brace matching should be */
    int PendingFlags;           /* flags for operations that are pending */
    wxString strCurrentDirectory;
    bool VisibleWhiteSpace;
    wxString strWorkspace;      /* current workspace (or empty) */
    wxString strTargetHost;     /* current target host (or empty) */
    int DebugLevel;             /* current debug level */
    int OptimizationLevel;      /* current optimization level */
    bool OverlayCode;           /* whether overlays are generated */
    bool UseFixedAMXName;       /* whether a standard name is to be used */
    bool VerboseBuild;          /* whether to show a memory summary on compile */
    bool CreateReport;          /* whether to create an XML file with a report */
    wxString strDefines;        /* project definitions */
    wxString strIncludePath;    /* path(s) to include files */
    wxString strCompilerPath;   /* path to compiler binaries */
    wxString strOutputPath;     /* path to store the compiled scripts */
    wxString strPreBuild;       /* optional command to run before the build */
    wxString strMiscCmdOptions; /* other options to pass to the Pawn compiler on the command line */
    wxString strTargetPath;     /* path to target host files */
    bool RunTimeIsInstalled;    /* whether the standard run-time is installed (it is often absent in embedded systems) */
    bool DebuggerIsInstalled;   /* whether the standard debugger is installed */
    wxString DebugPort;         /* port name for remote debugging */
    long DebugBaudrate;         /* baud rate for remote debugging */
    bool DebugLogEnabled;       /* whether data received over the serial line is logged to the output pane */

    wxString strRecentAMXName;  /* most recently compiled script (or empty on failure to build) */
    wxString strFixedAMXName;   /* name of the fixed compiled script (or empty) */
    wxString UploadTool;        /* program to use for transferring the AMX file to the target */
    wxString DeviceTool;        /* device-specific configuration tool */
    int DefaultOptimize;        /* default optimization level, depending on the target host */
    int MaxOptimize;            /* maximum optimization level supported by the target host */
    int DefaultDebugLevel;      /* default debug level */
    bool OverlayEnabled;        /* whether overlays are allowed (by the target host) */
    bool RunTimeEnabled;        /* whether the run-time is enabled */
    int DebuggerEnabled;        /* whether the debugger is enabled, for local and/or remote debugging */
    int DebuggerSelected;       /* either local or remote (but never both) */
    bool AutoTransfer;          /* whether automatic transfer after build is selected */
    long ExecPID;               /* process ID of running program/debugger */
    wxProcess *ExecProcess;     /* I/O redirection */
    wxString ExecInputQueue;    /* queue with text typed in the console pane */
    int DebugHoldback;          /* number characters held back, for recognizing the debugger output */
    bool DebugMode;             /* whether we are currently debugging */
    bool DebugRunning;
    wxString DebugCurrentFile;  /* file that the execution point is currently at */
    long DebugCurrentLine;      /* line number that the execution point is at */
    wxArrayLong WatchUpdateList;/* whether the watches list has been edited */
    long LastWatchIndex;        /* need to know the number of watches according to the debugger */
    wxArrayString BreakpointList;
    bool ChangedBreakpoints;    /* if true, the breakpoints must be reset */
    bool BuiltBreakpoints;      /* if true, the breakpoint list is ready to be sent */
    long CalltipPos;            /* position to use for the calltip (for "delayed" calltips)*/

    int UIDisabledTools;        /* whether any of the toolbar buttons and menu items are disabled (if these items do not change state, there is no need to update the UI) */

    wxTimer* Timer;             /* for delayed actions */
    virtual void OnTimer(wxTimerEvent& event);

    unsigned long RectSelectChkSum; /* checksum to detect paste of rectangular selection */
    unsigned long CalcClipboardChecksum();

    ContextParse context;

    wxFindReplaceDialog *FindDlg;
    wxFindReplaceData FindData;
    wxString WordUnderCursor();
    bool FindNextItem(bool warnnotfound);
    void FindAllInEditor(wxStyledTextCtrl* edit, const wxString& fullpath = wxEmptyString);

    bool UpdateSymBrowser(const wxString& filename = wxEmptyString);

    std::map<wxString, wxString> InfoTipList;
    bool ReadInfoTips();
    void RebuildHelpMenu();
    void RebuildToolsMenu();
    wxString LookUpInfoTip(const wxString& keyword, int flags);

    CHelpIndex* HelpIndex;
    CSymbolList SymbolList;
};

class DragAndDropFile: public wxFileDropTarget {
public:
    DragAndDropFile(QuincyFrame* pOwner) { m_pOwner = pOwner; }
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
private:
    QuincyFrame* m_pOwner;
};

#define PEND_INDENTBRACE    0x01    /* closing brace just inserted, indentation is still pending */
#define PEND_MATCHBRACE     0x02    /* caret on a brace/bracket, brace matching must still be performed */
#define PEND_DELETE_BM      0x04    /* caret on a line with a temporary bookmark */
#define PEND_SWITCHEDIT     0x08    /* TAB is active, but should set focus to the active editor */

class BrowserItemData : public wxTreeItemData {
public:
    BrowserItemData(const CSymbolEntry* symbol) { m_symbol = symbol; }
    const CSymbolEntry* Symbol() const { return m_symbol; }
private:
    const CSymbolEntry* m_symbol;
};

#define TIP_FUNCTION    0x01
#define TIP_VARIABLE    0x02
#define TIP_CONSTANT    0x04
#define TIP_ALL         (TIP_FUNCTION | TIP_VARIABLE | TIP_CONSTANT)

#define MAX_RECENTFILES         10
#define MAX_RECENTWORKSPACES    10
#define MAX_HELPFILES           10

enum {
    MARKER_BOOKMARK,
    MARKER_NAVIGATE,
    MARKER_BREAKPOINT,
    MARKER_CURRENTLINE,
};

enum {
    /* the order in this enum must be the same as the order in which the TABs are created */
    TAB_BUILD,
    TAB_MESSAGES,
    TAB_SYMBOLS,
    TAB_WATCHES,
    TAB_OUTPUT,
    TAB_SEARCH,
};

enum {
    IDM_LOADWORKSPACE = 1000,
    IDM_SAVEWORKSPACE,
    IDM_CLOSEWORKSPACE,
    IDM_SAVEALL,
    IDM_FILLCOLUMN,
    IDM_FINDNEXT,
    IDM_MATCHBRACE,
    IDM_GOTOSYMBOL,
    IDM_BOOKMARKTOGGLE,
    IDM_BOOKMARKNEXT,
    IDM_BOOKMARKPREV,
    IDM_BOOKMARKLIST,
    IDM_AUTOCOMPLETE,
    IDM_VIEWPANE,
    IDM_VIEWWHITESPACE,
    IDM_VIEWINDENTGUIDES,
    IDM_COMPILE,
    IDM_TRANSFER,
    IDM_DEBUG,
    IDM_RUN,
    IDM_ABORT,
    IDM_STEPINTO,
    IDM_STEPOVER,
    IDM_STEPOUT,
    IDM_RUNTOCURSOR,
    IDM_BREAKPOINTTOGGLE,
    IDM_BREAKPOINTCLEAR,
    IDM_BREAKPOINTLIST,
    IDM_TABSTOSPACES,
    IDM_SPACESTOTABS,
    IDM_INDENTSTOTABS,
    IDM_TRIMTRAILING,
    IDM_DEVICETOOL,
    IDM_CONTEXTHELP,
    IDM_SELECTCONTEXT,
    IDM_SAMPLEBROWSER,
    //-----
    IDM_RECENTFILE1,
    IDM_RECENTWORKSPACE1 = IDM_RECENTFILE1 + MAX_RECENTFILES,
    IDM_HELP1 = IDM_RECENTWORKSPACE1 + MAX_RECENTWORKSPACES,
    IDM_TIMER = IDM_HELP1 + MAX_HELPFILES,
    //-----
    IDC_EDIT,   /* must remain last */
};

#endif /* _QUINCYFRAME_H */
