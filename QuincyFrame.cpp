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
 *  Version: $Id: QuincyFrame.cpp 7205 2024-08-02 18:38:54Z thiadmer $
 */
#define _CRT_SECURE_NO_DEPRECATE
#include "wxQuincy.h"
#include <wx/busyinfo.h>
#include <wx/clipbrd.h>
#include <wx/filename.h>
#include <wx/mimetype.h>
#include <wx/numdlg.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/html/htmprint.h>
#include "QuincyFrame.h"
#include "QuincyReplaceDlg.h"
#include "QuincyReplacePrompt.h"
#include "QuincySearchDlg.h"
#include "QuincySampleBrowser.h"
#include "QuincySettingsDlg.h"
#include "QuincyDirPicker.h"
#include <amx.h>
#include <amxdbg.h>
#include "svnrev.h"

#if defined __GNUC__
    #pragma GCC diagnostic ignored "-Wunused-result"
#endif

#if !defined wxSTC_MARK_BOOKMARK
    #if defined SC_MARK_BOOKMARK
        #define wxSTC_MARK_BOOKMARK SC_MARK_BOOKMARK
    #else
        #define wxSTC_MARK_BOOKMARK wxSTC_MARK_ARROW
    #endif
#endif
#if wxCHECK_VERSION(2, 9, 0)
    #define GetActiveEdit(ctrl)     dynamic_cast<wxStyledTextCtrl*>(((ctrl)->GetSelection() >= 0 && (ctrl)->GetSelection() < (int)(ctrl)->GetPageCount()) ? (ctrl)->GetCurrentPage() : 0)
#else
    #define GetActiveEdit(ctrl)     dynamic_cast<wxStyledTextCtrl*>(((ctrl)->GetSelection() >= 0 && (ctrl)->GetSelection() < (int)(ctrl)->GetPageCount()) ? (ctrl)->GetPage((ctrl)->GetSelection()) : 0)
#endif


#include "res/Quincy16.xpm"
#include "res/Quincy32.xpm"
#include "res/Quincy48.xpm"
#include "res/filemodified.xpm"
#include "res/tb_new.xpm"
#include "res/tb_open.xpm"
#include "res/tb_save.xpm"
#include "res/tb_saveas.xpm"
#include "res/tb_saveall.xpm"
#include "res/tb_close.xpm"
#include "res/tb_print.xpm"
#include "res/tb_quit.xpm"
#include "res/tb_undo.xpm"
#include "res/tb_redo.xpm"
#include "res/tb_cut.xpm"
#include "res/tb_copy.xpm"
#include "res/tb_paste.xpm"
#include "res/tb_columnfill.xpm"
#include "res/tb_find.xpm"
#include "res/tb_findnext.xpm"
#include "res/tb_replace.xpm"
#include "res/tb_bracematch.xpm"
#include "res/tb_compile.xpm"
#include "res/tb_transfer.xpm"
#include "res/tb_device.xpm"
#include "res/tb_debug.xpm"
#include "res/tb_run.xpm"
#include "res/tb_stop.xpm"
#include "res/tb_stepover.xpm"
#include "res/tb_stepinto.xpm"
#include "res/tb_stepout.xpm"
#include "res/tb_runtocursor.xpm"
#include "res/tb_breakpoint.xpm"
#include "res/tb_settings.xpm"
#include "res/tb_help.xpm"
#include "res/tb_contexthelp.xpm"
#include "res/tb_pawn.xpm"


#if !defined wxStyledTextEventHandler
    #define wxStyledTextEventHandler(func) \
        (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxStyledTextEventFunction, &func)
#endif

#if !defined sizearray
    #define sizearray(a)    (sizeof(a) / sizeof((a)[0]))
#endif

#if defined _WIN32
    #define MIN_PANE_SIZE   24
#else
    #define MIN_PANE_SIZE   32
#endif


static const wxChar debug_prefix[] = wxT("}%`");
static const wxChar PawnKeyWords[] = wxT("assert break case const continue default defined ")
                                     wxT("do else exit for forward goto if native new ")
                                     wxT("operator public return sizeof sleep state static ")
                                     wxT("stock switch tagof var while true false");


QuincyFrame::QuincyFrame(const wxString& title, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, size)
{
    int idx;
    for (idx = 0; idx < MAX_EDITORS; idx++) {
        Editor[idx] = NULL;
        Filename[idx] = wxEmptyString;
        EditorDirty[idx] = false;
        FileTimeStamp[idx] = 0;
    }

    /* default "current" directory is the one with the examples */
    strCurrentDirectory = theApp->GetExamplesPath();

    SetSizeHints(wxDefaultSize, wxDefaultSize);
    SetIcon(wxIcon(Quincy32_xpm));
    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(QuincyFrame::OnCloseWindow));
    Connect(wxEVT_ACTIVATE, wxActivateEventHandler(QuincyFrame::OnActivate));

    /* create the bitmaps that are shared for the menu and the toolbar */
    wxBitmap tb_new(tb_new_xpm);
    wxBitmap tb_open(tb_open_xpm);
    wxBitmap tb_save(tb_save_xpm);
    wxBitmap tb_saveas(tb_saveas_xpm);
    wxBitmap tb_saveall(tb_saveall_xpm);
    wxBitmap tb_close(tb_close_xpm);
    wxBitmap tb_print(tb_print_xpm);
    wxBitmap tb_quit(tb_quit_xpm);
    wxBitmap tb_undo(tb_undo_xpm);
    wxBitmap tb_redo(tb_redo_xpm);
    wxBitmap tb_cut(tb_cut_xpm);
    wxBitmap tb_copy(tb_copy_xpm);
    wxBitmap tb_paste(tb_paste_xpm);
    wxBitmap tb_columnfill(tb_columnfill_xpm);
    wxBitmap tb_find(tb_find_xpm);
    wxBitmap tb_findnext(tb_findnext_xpm);
    wxBitmap tb_replace(tb_replace_xpm);
    wxBitmap tb_bracematch(tb_bracematch_xpm);
    wxBitmap tb_compile(tb_compile_xpm);
    wxBitmap tb_transfer(tb_transfer_xpm);
    wxBitmap tb_devicetool(tb_device_xpm);
    wxBitmap tb_debug(tb_debug_xpm);
    wxBitmap tb_run(tb_run_xpm);
    wxBitmap tb_abort(tb_stop_xpm);
    wxBitmap tb_stepinto(tb_stepinto_xpm);
    wxBitmap tb_stepover(tb_stepover_xpm);
    wxBitmap tb_stepout(tb_stepout_xpm);
    wxBitmap tb_runtocursor(tb_runtocursor_xpm);
    wxBitmap tb_breakpoint(tb_breakpoint_xpm);
    wxBitmap tb_settings(tb_settings_xpm);
    wxBitmap tb_help(tb_help_xpm);
    wxBitmap tb_contexthelp(tb_contexthelp_xpm);

    #define AppendIconItem(menu,_id,label,bitmap) \
        do { \
            wxMenuItem *item = new wxMenuItem((menu), (_id), (label)); \
            item->SetBitmap(bitmap); \
            (menu)->Append(item); \
        } while (0)

    /* create the menu */
    #define MENU_ENTRY(label)   theApp->Shortcuts.FormatShortCut(label, true)
    menuBar = new wxMenuBar;
    menuFile = new wxMenu;
    AppendIconItem(menuFile, wxID_NEW, MENU_ENTRY("New"), tb_new);
    AppendIconItem(menuFile, wxID_OPEN, MENU_ENTRY("Open"), tb_open);
    AppendIconItem(menuFile, wxID_SAVE, MENU_ENTRY("Save"), tb_save);
    AppendIconItem(menuFile, wxID_SAVEAS, MENU_ENTRY("SaveAs"), tb_saveas);
    AppendIconItem(menuFile, IDM_SAVEALL, MENU_ENTRY("SaveAll"), tb_saveall);
    AppendIconItem(menuFile, wxID_CLOSE, MENU_ENTRY("Close"), tb_close);
    menuFile->AppendSeparator();
    menuFile->Append(IDM_LOADWORKSPACE, MENU_ENTRY("OpenWorkspace"));
    menuFile->Append(IDM_SAVEWORKSPACE, MENU_ENTRY("SaveWorkspace"));
    menuFile->Append(IDM_CLOSEWORKSPACE, MENU_ENTRY("CloseWorkspace"));
    menuFile->AppendSeparator();
    AppendIconItem(menuFile, wxID_PRINT, MENU_ENTRY("Print"), tb_print);
    menuFile->AppendSeparator();
    menuRecentFiles = new wxMenu;   /* the "recent files/workspaces" menus are filled later */
    menuRecentWorkspaces = new wxMenu;
    menuFile->Append(-1, "Recent files", menuRecentFiles);
    menuFile->Append(-1, "Recent workspaces", menuRecentWorkspaces);
    menuFile->AppendSeparator();
    AppendIconItem(menuFile, wxID_EXIT, MENU_ENTRY("Quit"), tb_quit);
    menuBar->Append(menuFile, "&File");

    menuEdit = new wxMenu;
    AppendIconItem(menuEdit, wxID_UNDO, MENU_ENTRY("Undo"), tb_undo);
    AppendIconItem(menuEdit, wxID_REDO, MENU_ENTRY("Redo"), tb_redo);
    AppendIconItem(menuEdit, wxID_CUT, MENU_ENTRY("Cut"), tb_cut);
    AppendIconItem(menuEdit, wxID_COPY, MENU_ENTRY("Copy"), tb_copy);
    AppendIconItem(menuEdit, wxID_PASTE, MENU_ENTRY("Paste"), tb_paste);
    menuEdit->AppendSeparator();
    AppendIconItem(menuEdit, wxID_FIND, MENU_ENTRY("Find"), tb_find);
    AppendIconItem(menuEdit, IDM_FINDNEXT, MENU_ENTRY("FindNext"), tb_findnext);
    AppendIconItem(menuEdit, wxID_REPLACE,  MENU_ENTRY("Replace"), tb_replace);
    menuEdit->AppendSeparator();
    menuEdit->Append(wxID_INDEX, MENU_ENTRY("GotoLine"));
    menuEdit->Append(IDM_GOTOSYMBOL, MENU_ENTRY("GotoSymbol"));
    AppendIconItem(menuEdit, IDM_MATCHBRACE,  MENU_ENTRY("MatchBrace"), tb_bracematch);
    menuBookmarks = new wxMenu;
    menuBookmarks->Append(IDM_BOOKMARKTOGGLE, MENU_ENTRY("ToggleBookmark"));
    menuBookmarks->Append(IDM_BOOKMARKNEXT, MENU_ENTRY("NextBookmark"));
    menuBookmarks->Append(IDM_BOOKMARKPREV, MENU_ENTRY("PrevBookmark"));
    //??? list bookmarks (in all files)
    menuEdit->Append(-1, "Bookmarks", menuBookmarks);
    menuEdit->AppendSeparator();
    menuEdit->Append(IDM_AUTOCOMPLETE, MENU_ENTRY("Autocomplete"));
    AppendIconItem(menuEdit, IDM_FILLCOLUMN,  MENU_ENTRY("FillColumn"), tb_columnfill);
    menuBar->Append(menuEdit, "&Edit");

    menuView = new wxMenu;
    menuView->AppendCheckItem(IDM_VIEWWHITESPACE, MENU_ENTRY("ViewWhitespace"));
    menuView->AppendCheckItem(IDM_VIEWINDENTGUIDES, MENU_ENTRY("ViewIndentGuides"));
    menuBar->Append(menuView, "&View");

    menuBuild = new wxMenu;
    AppendIconItem(menuBuild, IDM_COMPILE, MENU_ENTRY("Compile"), tb_compile);
    AppendIconItem(menuBuild, IDM_TRANSFER, MENU_ENTRY("Transfer"), tb_transfer);
    menuBuild->AppendSeparator();
    AppendIconItem(menuBuild, IDM_DEBUG, MENU_ENTRY("Debug"), tb_debug);
    AppendIconItem(menuBuild, IDM_RUN, MENU_ENTRY("Run"), tb_run);
    AppendIconItem(menuBuild, IDM_ABORT, MENU_ENTRY("Stop"), tb_abort);
    menuBuild->AppendSeparator();
    AppendIconItem(menuBuild, IDM_STEPINTO, MENU_ENTRY("StepInto"), tb_stepinto);
    AppendIconItem(menuBuild, IDM_STEPOVER, MENU_ENTRY("StepOver"), tb_stepover);
    AppendIconItem(menuBuild, IDM_STEPOUT, MENU_ENTRY("StepOut"), tb_stepout);
    AppendIconItem(menuBuild, IDM_RUNTOCURSOR, MENU_ENTRY("RunToCursor"), tb_runtocursor);
    menuBuild->AppendSeparator();
    menuBreakpoints = new wxMenu;
    AppendIconItem(menuBreakpoints, IDM_BREAKPOINTTOGGLE, MENU_ENTRY("ToggleBreakpoint"), tb_breakpoint);
    menuBreakpoints->Append(IDM_BREAKPOINTCLEAR, MENU_ENTRY("ClearBreakpoints"));
    //??? list all breakpoints
    menuBuild->Append(-1, "Breakpoints", menuBreakpoints);
    menuBar->Append(menuBuild, "&Build/Run");

    menuTools = new wxMenu;
    AppendIconItem(menuTools, wxID_PROPERTIES, MENU_ENTRY("Options"), tb_settings);
    AppendIconItem(menuTools, IDM_SAMPLEBROWSER, MENU_ENTRY("SampleBrowser"), tb_pawn);
    menuTabSpace = new wxMenu;
    menuTabSpace->Append(IDM_TABSTOSPACES, MENU_ENTRY("TabToSpace"));
    menuTabSpace->Append(IDM_INDENTSTOTABS, MENU_ENTRY("IndentToTab"));
    menuTabSpace->Append(IDM_SPACESTOTABS, MENU_ENTRY("SpaceToTab"));
    menuTabSpace->AppendSeparator();
    menuTabSpace->Append(IDM_TRIMTRAILING, MENU_ENTRY("TrimTrailing"));
    menuTools->Append(-1, "Whitespace conversions", menuTabSpace);
    menuBar->Append(menuTools, "&Tools");

    menuHelp = new wxMenu;
    AppendIconItem(menuHelp, wxID_HELP, MENU_ENTRY("GeneralHelp"), tb_help);
    AppendIconItem(menuHelp, IDM_CONTEXTHELP, MENU_ENTRY("ContextHelp"), tb_contexthelp);
    menuHelp->AppendSeparator();
    menuHelp->Append(wxID_ABOUT/*, "&About"*/);
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);
    RebuildRecentMenus();

    /* selection events */
    Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnNewFile));
    Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnOpen));
    Connect(wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSave));
    Connect(wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSaveAs));
    Connect(IDM_SAVEALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSaveAll));
    Connect(wxID_CLOSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnClose));
    Connect(IDM_LOADWORKSPACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnLoadWorkSpace));
    Connect(IDM_SAVEWORKSPACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSaveWorkSpace));
    Connect(IDM_CLOSEWORKSPACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnCloseWorkSpace));
    Connect(wxID_PRINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnPrint));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnQuit));
    Connect(wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnUndo));
    Connect(wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRedo));
    Connect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnCut));
    Connect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnCopy));
    Connect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnPaste));
    Connect(wxID_FIND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnFindDlg));
    Connect(IDM_FINDNEXT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnFindNext));
    Connect(wxID_REPLACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnReplaceDlg));
    Connect(wxID_INDEX, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnGotoDlg));
    Connect(IDM_GOTOSYMBOL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnGotoSymbol));
    Connect(IDM_BOOKMARKTOGGLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnBookmarkToggle));
    Connect(IDM_BOOKMARKNEXT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnBookmarkNext));
    Connect(IDM_BOOKMARKPREV, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnBookmarkPrevious));
    Connect(IDM_MATCHBRACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnMatchBrace));
    Connect(IDM_FILLCOLUMN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnFillColumn));
    Connect(IDM_VIEWWHITESPACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnViewWhiteSpace));
    Connect(IDM_VIEWINDENTGUIDES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnViewIndentGuides));
    Connect(IDM_COMPILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnCompile));
    Connect(IDM_TRANSFER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnTransfer));
    Connect(IDM_DEBUG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnDebug));
    Connect(IDM_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRun));
    Connect(IDM_ABORT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnAbort));
    Connect(IDM_STEPINTO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnStepInto));
    Connect(IDM_STEPOVER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnStepOver));
    Connect(IDM_STEPOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnStepOut));
    Connect(IDM_RUNTOCURSOR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRunToCursor));
    Connect(IDM_BREAKPOINTTOGGLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnBreakpointToggle));
    Connect(IDM_BREAKPOINTCLEAR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnBreakpointClear));
    Connect(wxID_PROPERTIES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSettings));
    Connect(IDM_SAMPLEBROWSER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSampleBrowser));
    Connect(IDM_TABSTOSPACES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnTabsToSpaces));
    Connect(IDM_SPACESTOTABS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnSpacesToTabs));
    Connect(IDM_INDENTSTOTABS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnIndentsToTabs));
    Connect(IDM_TRIMTRAILING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnTrimTrailing));
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnAbout));
    Connect(wxID_HELP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnHelp));
    Connect(IDM_CONTEXTHELP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnContextHelp));
    Connect(IDM_AUTOCOMPLETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnAutoComplete));
    /* UI update events */
    Connect(IDM_CLOSEWORKSPACE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIWorkSpace));
    Connect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIUndo));
    Connect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIUndo));
    Connect(wxID_CUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUICutCopy));
    Connect(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUICutCopy));
    Connect(wxID_PASTE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIPaste));
    Connect(IDM_FINDNEXT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIFind));
    Connect(IDM_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIRun));
    Connect(IDM_STEPINTO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuincyFrame::OnUIStepInto));
    /* find dialog events */
    Connect(wxID_ANY, wxEVT_COMMAND_FIND, wxFindDialogEventHandler(QuincyFrame::OnFindAction));
    Connect(wxID_ANY, wxEVT_COMMAND_FIND_NEXT, wxFindDialogEventHandler(QuincyFrame::OnFindAction));
    Connect(wxID_ANY, wxEVT_COMMAND_FIND_REPLACE, wxFindDialogEventHandler(QuincyFrame::OnReplace));
    Connect(wxID_ANY, wxEVT_COMMAND_FIND_REPLACE_ALL, wxFindDialogEventHandler(QuincyFrame::OnReplaceAll));
    Connect(wxID_ANY, wxEVT_COMMAND_FIND_CLOSE, wxFindDialogEventHandler(QuincyFrame::OnFindClose));
    /* frame events */
    Connect(wxEVT_IDLE, wxIdleEventHandler(QuincyFrame::OnIdle));
    Connect(wxEVT_END_PROCESS, wxProcessEventHandler(QuincyFrame::OnTerminateApp));

    /* add a status bar */
    CreateStatusBar(2);
    int widths[2] = { -1, 180 };
    SetStatusWidths(2, widths);

    /* main view (toolbar is part of the main view) */
    wxBoxSizer* bSizerFrame = new wxBoxSizer(wxVERTICAL);
    #define TB_SHORTCUT(label)  theApp->Shortcuts.FormatShortCut(label, false, true)
    ToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT);
    ToolBar->SetToolBitmapSize(wxSize(16, 16));
    ToolBar->AddTool(wxID_NEW, "New", tb_new, "New file" + TB_SHORTCUT("New"));
    ToolBar->AddTool(wxID_OPEN, "Open", tb_open, "Open file" + TB_SHORTCUT("Open"));
    ToolBar->AddTool(wxID_SAVE, "Save", tb_save, "Save active file" + TB_SHORTCUT("Save"));
    ToolBar->AddTool(IDM_SAVEALL, "Save all", tb_saveall, "Save all modified files" + TB_SHORTCUT("SaveAll"));
    ToolBar->AddSeparator();
    ToolBar->AddTool(wxID_PRINT, "Print", tb_print, "Print file" + TB_SHORTCUT("Print"));
    ToolBar->AddSeparator();
    ToolBar->AddTool(wxID_CUT, "Cut", tb_cut, "Cut to clipboard" + TB_SHORTCUT("Cut"));
    ToolBar->AddTool(wxID_COPY, "Copy", tb_copy, "Copy to clipboard" + TB_SHORTCUT("Copy"));
    ToolBar->AddTool(wxID_PASTE, "Paste", tb_paste, "Paste from clipboard" + TB_SHORTCUT("Paste"));
    ToolBar->AddSeparator();
    ToolBar->AddTool(wxID_UNDO, "Undo", tb_undo, "Undo last operation" + TB_SHORTCUT("Undo"));
    ToolBar->AddTool(wxID_REDO, "Redo", tb_redo, "Redo last undo" + TB_SHORTCUT("Redo"));
    ToolBar->AddSeparator();
    ToolBar->AddTool(IDM_COMPILE, "Build", tb_compile, "Compile the current script" + TB_SHORTCUT("Compile"));
    ToolBar->AddTool(IDM_TRANSFER, "Transfer", tb_transfer, "Transfer the compiled script to an external host" + TB_SHORTCUT("Transfer"));
    ToolBar->AddTool(IDM_DEBUG, "Debug", tb_debug, "Debug the script" + TB_SHORTCUT("Debug"));
    ToolBar->AddTool(IDM_RUN, "Run", tb_run, "Run the script" + TB_SHORTCUT("Run"));
    ToolBar->AddTool(IDM_ABORT, "Stop", tb_abort, "Abort the running script" + TB_SHORTCUT("Stop"));
    ToolBar->AddTool(IDM_STEPINTO, "Step Into", tb_stepinto, "Step into functions" + TB_SHORTCUT("StepInto"));
    ToolBar->AddTool(IDM_STEPOVER, "Step Over", tb_stepover, "Step over functions" + TB_SHORTCUT("StepOver"));
    ToolBar->AddTool(IDM_STEPOUT, "Step Out", tb_stepout, "Run up to the function return" + TB_SHORTCUT("StepOut"));
    ToolBar->AddTool(IDM_RUNTOCURSOR, "Run to cursor", tb_runtocursor, "Run up to the current line" + TB_SHORTCUT("RunToCursor"));
    ToolBar->AddSeparator();
    ToolBar->AddTool(IDM_DEVICETOOL, "Device", tb_devicetool, "Configure the device" + TB_SHORTCUT("DeviceTool"));
    ToolBar->AddSeparator();
    FunctionList = new wxChoice(ToolBar, IDM_SELECTCONTEXT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SORT);
    FunctionList->SetMinSize(wxSize(200, -1));
    FunctionList->SetToolTip("The function name at the current cursor position.");
    ToolBar->AddControl(FunctionList, "Context");
    ToolBar->Realize();
    ToolBar->Refresh(false);
    bSizerFrame->Add(ToolBar, 0, wxEXPAND, 5);

    Connect(wxID_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnNewFile));
    Connect(wxID_OPEN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnOpen));
    Connect(wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnSave));
    Connect(IDM_SAVEALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnSaveAll));
    Connect(wxID_PRINT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnPrint));
    Connect(wxID_CUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnCut));
    Connect(wxID_COPY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnCopy));
    Connect(wxID_PASTE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnPaste));
    Connect(wxID_UNDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnUndo));
    Connect(wxID_REDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnRedo));
    Connect(IDM_COMPILE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnCompile));
    Connect(IDM_TRANSFER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnTransfer));
    Connect(IDM_DEBUG, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnDebug));
    Connect(IDM_RUN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnRun));
    Connect(IDM_ABORT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnAbort));
    Connect(IDM_STEPINTO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnStepInto));
    Connect(IDM_STEPOVER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnStepOver));
    Connect(IDM_STEPOUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnStepOut));
    Connect(IDM_RUNTOCURSOR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnRunToCursor));
    Connect(IDM_DEVICETOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(QuincyFrame::OnDeviceTool));
    Connect(IDM_SELECTCONTEXT, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(QuincyFrame::OnSelectContext));

    /* splitter window */
    SplitterFrame = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    SplitterFrame->SetMinimumPaneSize(MIN_PANE_SIZE);

    /* add a notebook control with tabs for the editors */
    pnlEdit = new wxPanel(SplitterFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizerEdit = new wxBoxSizer(wxVERTICAL);
    EditTab = new wxAuiNotebook(pnlEdit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_TOP);
    bSizerEdit->Add(EditTab, 1, wxEXPAND | wxTOP, 5);
    pnlEdit->SetSizer(bSizerEdit);
    pnlEdit->Layout();
    bSizerEdit->Fit(pnlEdit);
    EditTab->Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(QuincyFrame::OnTabChange), NULL, this );
    EditTab->Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(QuincyFrame::OnTabClose), NULL, this );

    /* add a notebook with tabs for compiler information and log windows
       note: the pages must be added in the order of the TAB_xxx constants */
    pnlPane = new wxPanel(SplitterFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizerPane = new wxBoxSizer(wxVERTICAL);
    PaneTab = new wxAuiNotebook(pnlPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_BOTTOM);
    #if defined _WIN32
        wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier New");
    #else
        wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Monospace");
    #endif
    BuildLog = new wxListView(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL);
    BuildLog->SetFont(font);
    BuildLog->InsertColumn(0, wxEmptyString);
    PaneTab->AddPage(BuildLog, "Build", false);    /* TAB_BUILD */
    ErrorLog = new wxListView(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL);
    ErrorLog->SetFont(font);
    ErrorLog->InsertColumn(0, wxEmptyString);
    ErrorLog->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(QuincyFrame::OnErrorSelect), NULL, this);
    PaneTab->AddPage(ErrorLog, "Messages", false); /* TAB_MESSAGES */
    BrowserTree = new wxTreeCtrl(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_NO_LINES|wxTR_SINGLE|wxTR_DEFAULT_STYLE);
    BrowserTree->SetFont(font);
    BrowserTree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(QuincyFrame::OnSymbolSelect), NULL, this);
    PaneTab->AddPage(BrowserTree, "Symbols", false); /* TAB_SYMBOLS */
    WatchLog = new wxListView(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_EDIT_LABELS);
    WatchLog->SetFont(font);
    WatchLog->InsertColumn(0, "Symbol");
    WatchLog->InsertColumn(1, "Value");
    WatchLog->Connect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(QuincyFrame::OnWatchEdited), NULL, this);
    WatchLog->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(QuincyFrame::OnWatchActivated), NULL, this);
    WatchLog->Connect(wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler(QuincyFrame::OnWatchDelete), NULL, this);
    PaneTab->AddPage(WatchLog, "Watches", false);  /* TAB_WATCHES */
    Terminal = new wxTextCtrl(PaneTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY);
    Terminal->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    Terminal->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    Terminal->SetFont(font);
    Terminal->Connect(wxEVT_CHAR, wxKeyEventHandler(QuincyFrame::OnTerminalChar), NULL, this);
    PaneTab->AddPage(Terminal, "Output", false);   /* TAB_OUTPUT */
    PaneTab->Layout();
    bSizerPane->Add(PaneTab, 1, wxEXPAND | wxBOTTOM, 5);
    pnlPane->SetSizer(bSizerPane);
    pnlPane->Layout();
    bSizerPane->Fit(pnlPane);
    SearchLog = 0;  /* this panel is optionally set */

    SplitterFrame->SplitHorizontally(pnlEdit, pnlPane, 0);
    bSizerFrame->Add(SplitterFrame, 1, wxEXPAND);
    this->SetSizer(bSizerFrame);
    this->Layout();

    /* set initial splitter position, centre the application window */
    minIni* ini = theApp->GetConfigFile();
    long pos = ini->getl("Position", "Splitter", 0);
    SplitterFrame->SetSashPosition(pos);
    SplitterFrame->SetSashGravity(1.0);
    Centre();

    /* Find settings */
    FindDlg = NULL;
    unsigned flags = wxFR_DOWN | (theApp->SearchFlags & (wxFR_MATCHCASE | wxFR_WHOLEWORD));
    FindData.SetFlags(flags);
    PrepareSearchLog();

    /* drag & drop target */
    SetDropTarget(new DragAndDropFile(this));

    /* a timer for delayed events */
    Timer = new wxTimer(this, IDM_TIMER);
    wxASSERT(Timer);
    Connect(IDM_TIMER, wxEVT_TIMER, wxTimerEventHandler(QuincyFrame::OnTimer));

    /* see whether there are initial files to load */
    RectSelectChkSum = 0;
    HelpIndex = 0;
    strWorkspace = ini->gets("Session", "Workspace");
    LoadSession();
    if (EditTab->GetPageCount() == 0)
        AddEditor();    /* no files in the session, create an empty file */
    context.SetControl(FunctionList);
    UIDisabledTools = 0;
    VisibleWhiteSpace = false;
    IgnoreChangeEvent = false;
    PendingFlags = 0;
    ExecPID = 0;
    ExecProcess = 0;
    DebugMode = false;
    WatchLog->Enable(DebugMode);
    WatchUpdateList.Clear();
}

void QuincyFrame::OnCloseWindow(wxCloseEvent& /* event */)
{
    if (!SaveAllFiles(true))
        return;
    Disconnect(wxEVT_ACTIVATE, wxActivateEventHandler(QuincyFrame::OnActivate));
    if (ExecPID != 0 && wxProcess::Exists(ExecPID)) {
        wxProcess::Kill(ExecPID, wxSIGTERM);
        wxProcess::Kill(ExecPID, wxSIGKILL);
    }
    SaveSession();              /* save all options */
    IgnoreChangeEvent = true;
    /* optionally copy search options from the "FindData" structure to the main
       options */
    if (!theApp->SearchAdvanced)
        theApp->SearchFlags = FindData.GetFlags();
    /* save application window size and other options */
    wxSize size = GetSize();
    long splitterpos =  SplitterFrame->GetSashPosition();
    theApp->SaveSettings(size, splitterpos);
    this->Destroy();
}

void QuincyFrame::OnActivate(wxActivateEvent& event)
{
    static bool recurse = false;
    if (!recurse && event.GetActive()) {
        for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
            wxStyledTextCtrl *edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page));
            wxASSERT(edit);
            int index;
            for (index = 0; index < MAX_EDITORS && Editor[index] != edit; index++)
                /* nothing */;
            wxASSERT(index < MAX_EDITORS);
            if (index < MAX_EDITORS) {
                wxString path = Filename[index];
                bool exists = wxFileExists(path);
                time_t t = exists ? wxFileModificationTime(path) : 0;
                if (exists && t != FileTimeStamp[index]) {
                    recurse = true;
                    wxString msg = "The file " + path.AfterLast(DIRSEP_CHAR)
                                   + " was changed outside the editor.\nReload?";
                    if (EditorDirty[index])
                        msg += "\n\nNota Bene: the file has local changes. These are lost on a reload.";
                    int reply = wxMessageBox(msg, "Notice", wxYES_NO);
                    if (reply == wxYES) {
                        LoadFile(path, edit);
                        SetChanged(index, false);
                    }
                    FileTimeStamp[index] = t; /* so it isn't asked again */
                    recurse = false;
                }
            }
        }
    }
}

void QuincyFrame::AdjustTitle()
{
    wxString title = "Pawn";

    /* add current target */
    if (strTargetHost.Length() > 0) {
        title += " [" + strTargetHost + "]";
    }

    /* add current file */
    wxASSERT(EditTab);
    int sel = EditTab->GetSelection();
    if (sel >= 0)
        title += " - " + EditTab->GetPageText(sel);
    if (strWorkspace.Length() > 0) {
        wxString project = strWorkspace;
        if (project.Find(DIRSEP_CHAR) >= 0)
            project = project.AfterLast(DIRSEP_CHAR);
        title += " (" + project + ")";
    }
    SetTitle(title);
}

void QuincyFrame::OnQuit(wxCommandEvent& /* event */)
{
    if (!SaveAllFiles(true))
        return;
    Disconnect(wxEVT_ACTIVATE, wxActivateEventHandler(QuincyFrame::OnActivate));
    SaveSession();

    IgnoreChangeEvent = true;
    while (EditTab->GetPageCount() > 0)
        RemoveEditor(0, true);

    Close(true);
}

void QuincyFrame::OnTabChange(wxAuiNotebookEvent& /* event */)
{
    /* user has clicked on a TAB to select a different file */
    AdjustTitle();
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    context.ScanContext(edit, CTX_RESET);
}

void QuincyFrame::OnTabClose(wxAuiNotebookEvent& event)
{
    CloseCurrentFile(false);
    event.Skip();
}

bool DragAndDropFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
{
    size_t nFiles = filenames.GetCount();
    for (size_t n = 0; n < nFiles; n++) {
        if (wxFile::Exists(filenames[n])) {
            /* check whether this is a project file, if so, change to the project */
            wxASSERT(m_pOwner->EditTab);
            if (filenames[n].Right(4).CmpNoCase(".prj")==0) {
                /* close all current files (query for save), also save the session */
                if (!m_pOwner->SaveAllFiles(true))
                    return true;
                m_pOwner->SaveSession();
                while (m_pOwner->EditTab->GetPageCount() > 0)
                    m_pOwner->RemoveEditor(0);
                /* load the new workspace */
                m_pOwner->strWorkspace = filenames[n];
                m_pOwner->LoadSession();
                /* no files in the session, create an empty file */
                if (m_pOwner->EditTab->GetPageCount() == 0)
                    m_pOwner->AddEditor();
            } else {
                /* if there is only one editor, with a default filename, close that one */
                if (m_pOwner->EditTab->GetPageCount() == 1 && m_pOwner->Filename[0].Length() == 0)
                    m_pOwner->RemoveEditor(0);
                if (m_pOwner->AddEditor(filenames[n]))
                    m_pOwner->AdjustTitle();
            }
        }
    }
    return true;
}

void QuincyFrame::OnErrorSelect(wxListEvent& event)
{
    wxString line = ErrorLog->GetItemText(event.GetIndex());
    line.Trim(false);

    /* line is filename(line1[-line2]): warning|error|fatal error errno: text */
    wxString filename = line.BeforeFirst('(');
    line = line.AfterFirst('(');
    wxString linenrs = line.BeforeFirst(')');
    long first, last;
    linenrs.ToLong(&first);
    linenrs = linenrs.AfterLast('-');
    if (linenrs.length() == 0)
        last = first;
    else
        linenrs.ToLong(&last);

    /* find the file, or load it */
    wxStyledTextCtrl* edit = 0;
    filename.Trim();
    for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
        if (Filename[idx].Cmp(filename) == 0)
            edit = Editor[idx];
    if (!edit) {
        /* compare the base names only if no full path on the match is found */
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++) {
            wxString basename = Filename[idx].AfterLast(DIRSEP_CHAR);
            if (basename.Cmp(filename) == 0)
                edit = Editor[idx];
        }
    }
    if (!edit) {
        /* try to open the file (and find it) */
        AddEditor(filename);
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
            if (Filename[idx].Cmp(filename) == 0)
                edit = Editor[idx];
    }
    if (!edit) {
        wxMessageBox("Could not open " + filename, "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    }

    /* find the TAB page to activate */
    for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
        if (EditTab->GetPage(page) == edit) {
            EditTab->SetSelection(page);
            break;
        }
    }
    /* scroll to the position */
    long firstpos = edit->PositionFromLine(first - 1);
    long lastpos = edit->PositionFromLine(last);
    edit->GotoPos(firstpos);
    edit->SetSelection(firstpos, lastpos);
    PendingFlags |= PEND_SWITCHEDIT;
    if (!Timer->IsRunning())    /* delayed switch focus (because calling edit->SetFocus() here does not work) */
        Timer->Start(100, true);
}

void QuincyFrame::OnSymbolSelect(wxTreeEvent& event)
{
    /* see which item it is */
    BrowserItemData* data = dynamic_cast<BrowserItemData*>(BrowserTree->GetItemData(event.GetItem()));
    if (!data)
        return;
    const CSymbolEntry* symbol = data->Symbol();
    if (!symbol)
        return;

    /* set a temporary bookmark on the current position */
    wxStyledTextCtrl* edit = GetActiveEdit(EditTab);
    if (edit) {
        int line = edit->GetCurrentLine();
        if ((edit->MarkerGet(line) & ((1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE))) == 0) {
            IgnoreChangeEvent = true;
            edit->MarkerAdd(line, MARKER_NAVIGATE);
            IgnoreChangeEvent = false;
        }
    }

    GotoSymbol(symbol);
}

bool QuincyFrame::AddEditor(const wxString &name)
{
    /* Scintilla only supports UTF-8 when compiled for Unicode, and this
       is the default for the most wxWidgets distributions */
    if (name.length() > 0 && wxFileExists(name) && !ScanUTF8(name)) {
        int reply = wxMessageBox("The file " + name.AfterLast(DIRSEP_CHAR)
                                 + " uses characters outside the ASCII range and needs to be converted to UTF-8.\n"
                                   "Do you wish to do that now?", "Notice", wxYES_NO);
        if (reply != wxYES)
            return false;
        if (!Latin1ToUTF8(name)) {
            wxMessageBox("Conversion of " + name.AfterLast(DIRSEP_CHAR) + " to UTF-8 has failed.",
                         "Pawn IDE", wxOK | wxICON_ERROR);
            return false;
        }
    }

    /* find an empty spot */
    int idx;
    for (idx = 0; idx < MAX_EDITORS && Editor[idx] != NULL; idx++)
        /* nothing */;
    if (idx >= MAX_EDITORS)
        return false;

    /* the editor */
    wxStyledTextCtrl *edit = new wxStyledTextCtrl(EditTab, IDC_EDIT + idx, wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);
    Editor[idx] = edit;
    SetEditorsStyle(edit);
    edit->SetIndentationGuides(false);
    edit->SetCaretLineVisible(true);
    edit->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    edit->SetMarginWidth(1, 16);
    edit->SetMarginMask(1,~wxSTC_MASK_FOLDERS);
    edit->MarkerDefine(MARKER_BOOKMARK, wxSTC_MARK_BOOKMARK, wxColour(0, 0, 0), wxColour(0, 160, 0));
    edit->MarkerDefine(MARKER_NAVIGATE, wxSTC_MARK_BOOKMARK, wxColour(0, 0, 0), wxColour(0, 0, 160));
    edit->MarkerDefine(MARKER_BREAKPOINT, wxSTC_MARK_CIRCLE, wxColour(192, 192, 192), wxColour(160, 0, 0));
    edit->MarkerDefine(MARKER_CURRENTLINE, wxSTC_MARK_SHORTARROW, wxColour(0, 0, 0), wxColour(240, 192, 0));

    if (name.Length() == 0 || IsPawnFile(name)) {
        edit->SetLexer(wxSTC_LEX_CPP);
        edit->SetProperty("lexer.cpp.track.preprocessor", "0");   /* this is too complex with Pawn, because Pawn can also check definitions of variables & functions */
        edit->SetKeyWords(0, PawnKeyWords);
        edit->SetWordChars("_@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    }

    edit->SetCodePage(wxSTC_CP_UTF8);   /* required for the Unicode build of wxWidgets, which is now standard */
    edit->SetTabIndents(true);
    edit->SetBackSpaceUnIndents(true);
    edit->SetMouseDwellTime(500);
    edit->AutoCompStops("[]{}<>`~!#$%^&*-+|\\;:'\",./?");
    edit->AutoCompSetDropRestOfWord(true);
    edit->AutoCompSetChooseSingle(true);
    edit->AutoCompSetSeparator('|');
    /* make sure special keys are not not handled by scintilla */
    edit->CmdKeyClear('[', wxSTC_KEYMOD_CTRL);  /* CmdKeyClear blocks the key from propagating */
    edit->CmdKeyClear(']', wxSTC_KEYMOD_CTRL);
    for (int i = 0; i < theApp->Shortcuts.Count(); i++) {
        KbdShortcut* shortcut = theApp->Shortcuts.GetItem(i);
        wxAcceleratorEntry entry;
        if (entry.FromString(shortcut->GetShortcut())) {
            int flags = wxSTC_KEYMOD_NORM;   /* wxWidgets flags and Scintilla flags are not the same */
            if (entry.GetFlags() &  wxACCEL_SHIFT)
                flags |= wxSTC_KEYMOD_SHIFT;
            if (entry.GetFlags() &  wxACCEL_CTRL)
                flags |= wxSTC_KEYMOD_CTRL;
            if (entry.GetFlags() &  wxACCEL_ALT)
                flags |= wxSTC_KEYMOD_ALT;
            edit->CmdKeyAssign(entry.GetKeyCode(), flags, 0);   /* propagate to parent window */
        }
    }

    Connect(IDC_EDIT + idx, wxEVT_STC_CHANGE, wxStyledTextEventHandler(QuincyFrame::OnEditorChange));
    Connect(IDC_EDIT + idx, wxEVT_STC_CHARADDED, wxStyledTextEventHandler(QuincyFrame::OnEditorCharAdded));
    Connect(IDC_EDIT + idx, wxEVT_STC_UPDATEUI, wxStyledTextEventHandler(QuincyFrame::OnEditorPosition));
    Connect(IDC_EDIT + idx, wxEVT_STC_DWELLSTART, wxStyledTextEventHandler(QuincyFrame::OnEditorDwellStart));
    Connect(IDC_EDIT + idx, wxEVT_STC_DWELLEND, wxStyledTextEventHandler(QuincyFrame::OnEditorDwellEnd));

    edit->SetDropTarget(new DragAndDropFile(this));

    wxASSERT(EditTab);
    wxString str;
    if (name.Length() == 0) {
        Filename[idx] = wxEmptyString;
        FileTimeStamp[idx] = 0;
        str = "(new)";
    } else {
        Filename[idx] = name;
        FileTimeStamp[idx] = wxFileModificationTime(name);
        str = name.AfterLast(DIRSEP_CHAR);
    }
    EditTab->AddPage(edit, str, false);
    unsigned count = EditTab->GetPageCount();
    EditTab->SetSelection(count - 1);   /* activate the page just added */
    EditTab->Layout();
    this->Layout();
    this->Refresh();

    if (name.Length() > 0) {
        if (!LoadFile(name, edit))
            wxMessageBox("Failed to load file " + str, "Pawn IDE", wxOK | wxICON_ERROR);
    } else {
        edit->ClearAll();
    }
    edit->SetFocus();
    edit->EnsureCaretVisible();
    SetChanged(idx, false);
    SetStatusText(wxEmptyString, 0);

    return true;
}

bool QuincyFrame::RemoveEditor(int index, bool deletecontrol)
{
    wxASSERT(EditTab);
    if (EditTab->GetPageCount() == 0)
        return false;

    wxStyledTextCtrl *edit;
    if (index == -1) {
        /* get the active editor */
        edit = GetActiveEdit(EditTab);
    } else {
        wxASSERT(index >= 0 && index < MAX_EDITORS);
        edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(index));
    }
    wxASSERT(edit);
    /* find the editor in the list */
    for (index = 0; index < MAX_EDITORS && Editor[index] != edit; index++)
        /* nothing */;
    if (index >= MAX_EDITORS || edit == NULL)
        return false;
    Editor[index] = NULL;
    Filename[index] = wxEmptyString;
    FileTimeStamp[index] = 0;

    /* optionally find the tab page and delete it */
    if (deletecontrol) {
        wxASSERT(EditTab);
        for (index = 0; index < (int)EditTab->GetPageCount(); index++)
            if (EditTab->GetPage(index) == edit)
                EditTab->DeletePage(index);
    }

    return true;
}

/* adapted from the functions in the Pawn compiler */
bool QuincyFrame::ScanUTF8(const wxString& filename)
{
    FILE *fp = fopen(filename.utf8_str(), "r");
    if (!fp)
        return false;
    bool utf8 = true;
    bool bom_found = false;
    bool firstchar = true;
    unsigned char line[512];
    while (utf8 && fgets((char*)line, sizeof(line), fp) != NULL) {
        const unsigned char *ptr = line;
        if (firstchar) {
            /* check whether the very first character on the very first line
               starts with a byte order mark (BOM) */
            if (line[0] == 0xef && line[1] == 0xbb && line[2] == 0xbf) {
                bom_found = true;
                ptr += 3;
            }
            firstchar = false;
        }
        /* run over one line */
        long lowmark = 0;
        int follow = 0;
        long code = 0;
        while (utf8 && *ptr != '\0') {
            unsigned char ch = *ptr++;
            if (follow > 0 && (ch & 0xc0) == 0x80) {
                /* leader code is active, combine with earlier code */
                code = (code << 6) | (ch & 0x3f);
                if (--follow == 0) {
                    /* encoding a character in more bytes than is strictly
                       needed, is not really valid UTF-8; if no BOM is found,
                       we are strict in order to increase the chance of heuristic
                       dectection of non-UTF-8 text (JAVA writes zero bytes as
                       a 2-byte code UTF-8, which is invalid) */
                    if (code < lowmark && !bom_found)
                        utf8 = false;
                    /* the code positions 0xd800--0xdfff and 0xfffe & 0xffff do
                       not exist in UCS-4 (and hence, they do not exist in Unicode) */
                    if (code >= 0xd800 && code <= 0xdfff || code == 0xfffe || code == 0xffff)
                        utf8 = false;
                }
            } else if (follow == 0 && (ch & 0x80) == 0x80) {
                /* UTF-8 leader code */
                if ((ch & 0xe0) == 0xc0) {
                    /* 110xxxxx 10xxxxxx */
                    follow = 1;
                    lowmark = 0x80L;
                    code = ch & 0x1f;
                } else if ((ch & 0xf0) == 0xe0) {
                    /* 1110xxxx 10xxxxxx 10xxxxxx (16 bits, BMP plane) */
                    follow = 2;
                    lowmark = 0x800L;
                    code = ch & 0x0f;
                } else if ((ch & 0xf8) == 0xf0) {
                    /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
                    follow = 3;
                    lowmark = 0x10000L;
                    code = ch & 0x07;
                } else if ((ch & 0xfc) == 0xf8) {
                    /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
                    follow = 4;
                    lowmark = 0x200000L;
                    code = ch & 0x03;
                } else if ((ch & 0xfe) == 0xfc) {
                    /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (32 bits) */
                    follow = 5;
                    lowmark = 0x4000000L;
                    code = ch & 0x01;
                } else {
                    /* this is invalid UTF-8 */
                    utf8 = false;
                } /* if */
            } else if (follow == 0 && (ch & 0x80) == 0x00) {
                /* 0xxxxxxx (US-ASCII), which is valid UTF8 */
                code = ch;
            } else {
                /* this is invalid UTF-8 */
                utf8 = false;
            }
        }
    }
    fclose(fp);
    return utf8;
}

bool QuincyFrame::Latin1ToUTF8(const wxString& filename)
{
    /* Nota Bene:
     * 1) no check is made to verify whether the file is already in UTF8
     * 2) worse: the file is simply assumed to be in Latin-1
     */

    FILE *fp = fopen(filename.utf8_str(), "r");
    if (!fp)
        return false;

    /* get the file length, then read it in memory completely */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    unsigned char *buffer = (unsigned char*)malloc(size * sizeof(char));
    if (!buffer) {
        fclose(fp);
        return false;
    }
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), size, fp);
    fclose(fp);

    /* re-open the file for writing */
    fp = fopen(filename.utf8_str(), "w");
    if (!fp) {
        free(buffer);
        return false;
    }
    for (int idx = 0; idx < size; idx++) {
        if (buffer[idx] < 0x80) {
            fputc(buffer[idx], fp);
        } else {
            /* simplified since our range is only 8-bits */
            fputc(0xc0 | (buffer[idx] >> 6), fp);
            fputc(0x80 | (buffer[idx] & 0x3f), fp);
        }

    }
    free(buffer);
    fclose(fp);

    return true;
}

void QuincyFrame::SetChanged(int index, bool changed)
{
    if (EditTab && EditTab->GetPageCount() > 0) {
        unsigned page;
        if (index < 0) {
            page = EditTab->GetSelection();
            for (index = 0; index < MAX_EDITORS; index++)
                if (EditTab->GetPage(page) == Editor[index])
                    break;
            wxASSERT(index >= 0 && index < MAX_EDITORS);
        } else {
            for (page = 0; page < EditTab->GetPageCount(); page++)
                if (EditTab->GetPage(page) == Editor[index])
                    break;
            wxASSERT(page < EditTab->GetPageCount());
        }
        if (page < EditTab->GetPageCount() && index >= 0 && index < MAX_EDITORS) {
            if (changed && !EditorDirty[index]) {
                wxBitmap dirty(filemodified_xpm);
                EditTab->SetPageBitmap(page, dirty);
            } else if (!changed && EditorDirty[index]) {
                EditTab->SetPageBitmap(page, wxNullBitmap);
            }
            EditorDirty[index] = changed;
        }
    }
}

bool QuincyFrame::LoadFile(const wxString& filename, wxStyledTextCtrl* edit)
{
    /* load the file ourselves instead of relying on wxStyledTextCtrl::LoadFile()
       to be sure that the file is properly converted from UTF8 */
    FILE *fp = fopen(filename.utf8_str(), "rt");
    if (!fp)
        return false;

    char* data = (char*)malloc(65536 * sizeof(char));
    if (!data) {
        fclose(fp);
        return false;
    }

    /* read line by line, to do proper CR/LF conversion */
    edit->ClearAll();
    IgnoreChangeEvent = true;   /* do not respond to events while loading */
    bool first = true;
    while (fgets(data, 65536 - 1, fp) != NULL) {
        char* ptr = strchr(data, '\0');
        while (ptr > data && (*(ptr - 1) == '\r' || *(ptr - 1) == '\n'))
            *--ptr = '\0';  /* delete all CR/LF that fgets may have read */
        wxString text(data, wxConvUTF8);
        if (!first)
            edit->NewLine();
        edit->AddText(text);
        first = false;
    }
    IgnoreChangeEvent = false;
    free(data);
    fclose(fp);

    edit->SetSelection(0, 0);
    edit->EmptyUndoBuffer();
    edit->SetSavePoint();
    return true;
}

bool QuincyFrame::CheckSaveFile(bool force_save, bool force_prompt, wxStyledTextCtrl* edit)
{
    if (force_prompt)
        force_save = true;

    if (!edit)
        edit = GetActiveEdit(EditTab);
    if (!edit)
        return false;

    int idx;
    for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
        /* nothing */;
    wxASSERT(idx < MAX_EDITORS);

    if (idx < MAX_EDITORS && EditorDirty[idx] || force_save) {
        wxString path = wxEmptyString;
        path = Filename[idx];
        if (path.Length() == 0 || force_prompt) {
            wxFileDialog * saveFileDialog = new wxFileDialog(this, "Save file...",
                                                            strCurrentDirectory, path,
                                                            "Pawn scripts|*.p;*.pwn;*.pawn|Pawn include files|*.i;*.inc|All files|*",
                                                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
            if (saveFileDialog->ShowModal() != wxID_OK)
                return false;
            /* get the name, set the default extension */
            path = saveFileDialog->GetPath();
            wxString ext = path.AfterLast('.');
            if (ext.Length() == 0 || ext.Length() > 4) {
                int type = saveFileDialog->GetFilterIndex();
                if (type == 0)
                    path += ".p";
                else if (type == 1)
                    path += ".i";
            }
            Filename[idx] = path;
            strCurrentDirectory = wxPathOnly(Filename[idx]);
            /* find the tab, change the text */
            wxString name = Filename[idx].AfterLast(DIRSEP_CHAR);
            wxASSERT(EditTab);
            unsigned page;
            for (page = 0; page < EditTab->GetPageCount(); page++)
                if (EditTab->GetPage(page) == Editor[idx])
                    break;
            wxASSERT(page < EditTab->GetPageCount());
            EditTab->SetPageText(page, name);
            EditTab->Layout();
            AdjustTitle();
            /* may need to change the lexer after a file rename */
            if (IsPawnFile(name)) {
                edit->SetLexer(wxSTC_LEX_CPP);
                edit->SetKeyWords(0, PawnKeyWords);
            } else {
                edit->SetLexer(wxSTC_LEX_NULL);
                edit->SetKeyWords(0, wxEmptyString);
            }
        }
        wxASSERT(edit);
        if (theApp->GetStripTrailing())
            StripTrailingSpaces(edit);
        /* save the file ourselves instead of relying on wxStyledTextCtrl::SaveFile()
           to be sure that the file is properly converted to UTF8 */
        FILE *fp = fopen(Filename[idx].utf8_str(), "wt");
        if (!fp) {
            wxMessageBox("Failed to save the file.", "Pawn IDE", wxOK | wxICON_ERROR);
            return false;
        }
        /* store the file line by line, to enforce clean line ends */
        int maxLines = edit->GetLineCount();
        for (int line = 0; line < maxLines; line++) {
            wxString text = edit->GetLine(line);
            int len = text.Length();
            while (len > 0 && (text[len - 1] == '\r' || text[len - 1] == '\n'))
                len--;
            text = text.Left(len);
            #if wxCHECK_VERSION(2, 9, 0)
                wxScopedCharBuffer buf = text.mb_str(wxConvUTF8);
            #else
                const wxCharBuffer buf = text.mb_str(wxConvUTF8);
            #endif
            fprintf(fp, "%s\n", (const char*)buf);
        }
        fclose(fp);
        edit->SetSavePoint();   /* clear the modification flag */
        FileTimeStamp[idx] = wxFileModificationTime(Filename[idx]);
    }
    return true;
}

bool QuincyFrame::SaveAllFiles(bool prompt)
{
    wxASSERT(EditTab);
    if (prompt) {
        /* collect the names of the modified files */
        wxString filenames;
        for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
            wxStyledTextCtrl *edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page));
            wxASSERT(edit);
            int index;
            for (index = 0; index < MAX_EDITORS && Editor[index] != edit; index++)
                /* nothing */;
            wxASSERT(index < MAX_EDITORS);
            if (index < MAX_EDITORS && EditorDirty[index])
                filenames += Filename[index] + "\n";
        }
        if (filenames.Length() > 0) {
            wxString msg = "The following file(s) was/were modified:\n"
                           + filenames + "\nSave changes?";
            int reply = wxMessageBox(msg, "Pawn IDE", wxYES_NO | wxCANCEL);
            if (reply == wxCANCEL)
                return false;
            if (reply == wxNO)
                return true;    /* no error, but no saving either */
        }
    }

    for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
        wxStyledTextCtrl *edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page));
        wxASSERT(edit);
        if (!CheckSaveFile(false, false, edit))
            return false;
        int idx;
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        SetChanged(idx, false);
    }
    return true;
}

void QuincyFrame::CloseCurrentFile(bool deletetab)
{
    wxASSERT(EditTab);
    int index = -1;
    if (EditTab->GetPageCount() > 0) {
        wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
        wxASSERT(edit);
        /* find the editor in the list */
        for (index = 0; index < MAX_EDITORS && Editor[index] != edit; index++)
            /* nothing */;
        wxASSERT(index < MAX_EDITORS);
        if (index < MAX_EDITORS && EditorDirty[index]) {
            /* ask whether the changes in the file must be saved */
            wxString name = Filename[index];
            if (name.Length() == 0) {
                int idx = EditTab->GetSelection();
                if (idx >= 0 && idx < (int)EditTab->GetPageCount())
                    name = EditTab->GetPageText(idx);
            }
            int reply = wxMessageBox("Save changes in " + name, "Pawn IDE", wxYES_NO | wxCANCEL);
            if (reply == wxCANCEL || (reply == wxYES && !CheckSaveFile(false, false, edit)))
                return;
        }
    }

    if (deletetab) {
        RemoveEditor();
        if (EditTab->GetPageCount() == 0)
            AddEditor();    /* no files in the session, create an empty file */
    } else if (index >= 0) {
        wxASSERT(index < MAX_EDITORS);
        Editor[index] = NULL;
        Filename[index] = wxEmptyString;
        FileTimeStamp[index] = 0;
    }

    AdjustTitle();
}

void QuincyFrame::StripTrailingSpaces(wxStyledTextCtrl *edit)
{
    /* adapted from SciTE source code */
    wxASSERT(edit);
    int maxLines = edit->GetLineCount();
    for (int line = 0; line < maxLines; line++) {
        int lineStart = edit->PositionFromLine(line);
        int lineEnd = edit->GetLineEndPosition(line);
        int i = lineEnd - 1;
        char ch = static_cast<char>(edit->GetCharAt(i));
        while (i >= lineStart && ch <= ' ') {
            i--;
            ch = static_cast<char>(edit->GetCharAt(i));
        }
        if (i < lineEnd - 1) {
            edit->SetTargetStart(i + 1);
            edit->SetTargetEnd(lineEnd);
            edit->ReplaceTarget("");
        }
    }
}

wxString QuincyFrame::OptionallyQuoteString(const wxString& string)
{
    if (string.Find(' ') >= 0)
        return "\"" + string + "\"";
    return string;
}

bool QuincyFrame::LoadSession()
{
    if (strWorkspace.Length() > 0 && !wxFileExists(strWorkspace))
        strWorkspace = wxEmptyString;
    minIni *ini;
    bool close_ini = false;
    if (strWorkspace.Length() == 0) {
        ini = theApp->GetConfigFile();
    } else {
        ini = new minIni(strWorkspace);
        close_ini = true;
    }
    wxASSERT(ini);
    strCurrentDirectory = ini->gets("Session", "Directory", strCurrentDirectory);

    int idx;
    wxString item;
    wxString path;
    for (idx = 0; idx < MAX_EDITORS; idx++) {
        item.Printf("File%d", idx + 1);
        path = ini->gets("Session", item);
        if (path.Length() == 0)
            break;
        AddEditor(path);
    }

    /* load workspace settings */
    strTargetHost = ini->gets("Options", "TargetHost", "");
    DebugLevel = (int)ini->getl("Options", "Debugging", 1);
    OptimizationLevel = (int)ini->getl("Options", "Optimize", 1);
    OverlayCode = ini->getbool("Options", "Overlays");
    UseFixedAMXName = ini->getbool("Options", "FixedAMXName");
    VerboseBuild = ini->getbool("Options", "Verbose");
    CreateReport = ini->getbool("Options", "Report");
    AutoTransfer = ini->getbool("Options", "AutoTransfer");
    DebuggerSelected = ini->getl("Options", "Debugger");
    DebugPort = ini->gets("Options", "DebugPort");
    DebugBaudrate = ini->getl("Options", "DebugBaudrate");
    DebugLogEnabled = ini->getbool("Options", "DebugLogging");
    strDefines = ini->gets("Options", "Defines");
    strPreBuild = ini->gets("Options", "PreBuild");
    strMiscCmdOptions = ini->gets("Options", "CmdOptions");
    strIncludePath = ini->gets("Directories", "Include");
    strCompilerPath = ini->gets("Directories", "Compiler", theApp->GetBinPath());
    strOutputPath = ini->gets("Directories", "TargetPath");
    /* remove trailing slash, if necessary */
    int len = strCompilerPath.Len();
    if (len == 0)
        strCompilerPath = theApp->GetBinPath();
    else if (len > 0 && strCompilerPath[len - 1] == DIRSEP_CHAR)
        strCompilerPath = strCompilerPath.Left(len - 1);
    len = strOutputPath.Len();
    if (len > 0 && strOutputPath[len - 1] == DIRSEP_CHAR)
        strOutputPath = strOutputPath.Left(len - 1);

    strTargetPath = strCompilerPath;
    if (strTargetPath.Right(4).CmpNoCase(DIRSEP_STR "bin") == 0)
        strTargetPath = strTargetPath.Left(strTargetPath.Length() - 4); /* strip off "/bin" */
    strTargetPath += DIRSEP_STR "target";

    if (close_ini)
        delete ini;

    LoadHostConfiguration(strTargetHost);
    if (strFixedAMXName.length() == 0)
        UseFixedAMXName = false;    /* force false if no fixed name is defined */
    else if (ini->getl("Options", "FixedAMXName", -1) == -1)
        UseFixedAMXName = true;     /* set to "true" by default if the option is not yet in the INI file/workspace */
    if (!OverlayEnabled)
        OverlayCode = false;        /* force to false if the host does not support overlays */
    if (OptimizationLevel > MaxOptimize)
        OptimizationLevel = MaxOptimize;
    if ((DebuggerSelected & DebuggerEnabled) == 0)
        DebuggerSelected = DEBUG_NONE;

    UpdateSymBrowser();
    ReadInfoTips();
    RebuildHelpMenu();
    RebuildToolsMenu();
    AdjustTitle();

    return true;
}

/* SaveSession() saves the session information, but not the files themselves */
bool QuincyFrame::SaveSession()
{
    /* protection: if there are no files at all, assume that the session does
       not exist (this protects against double save of the session, which is
       harmful because the files get removed from the workspace file) */
    if (EditTab->GetPageCount() == 0)
        return false;

    minIni *ini;
    /* store workspace always in global INI */
    ini = theApp->GetConfigFile();
    ini->put("Session", "Workspace", strWorkspace);

    /* the other data may be stored in the workspace file */
    bool close_ini = false;
    if (strWorkspace.Length() > 0) {
        ini = new minIni(strWorkspace);
        close_ini = true;
    }
    wxASSERT(ini);
    ini->put("Session", "Directory", strCurrentDirectory);
    int page, idx;
    wxString item;
    wxString fullpath;
    for (page = 0; page < (int)EditTab->GetPageCount(); page++) {
        item.Printf("File%d", page + 1);
        wxStyledTextCtrl *edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page));
        wxASSERT(edit);
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        fullpath = Filename[idx];
        ini->put("Session", item, fullpath);
    }
    /* remove any files not currently open */
    for ( ;; ) {
        item.Printf("File%d", page + 1);
        wxString name = ini->gets("Session", item);
        if (name.Length() == 0)
            break;
        ini->del("Session", item);
        page++;
    }

    /* save workspace settings */
    ini->put("Options", "TargetHost", strTargetHost);
    ini->put("Options", "Debugging", DebugLevel);
    ini->put("Options", "Optimize", OptimizationLevel);
    ini->put("Options", "Overlays", OverlayCode);
    ini->put("Options", "FixedAMXName", UseFixedAMXName);
    ini->put("Options", "Verbose", VerboseBuild);
    ini->put("Options", "Report", CreateReport);
    ini->put("Options", "AutoTransfer", AutoTransfer);
    ini->put("Options", "Debugger", DebuggerSelected);
    ini->put("Options", "DebugPort", DebugPort);
    ini->put("Options", "DebugBaudrate", DebugBaudrate);
    ini->put("Options", "DebugLogging", DebugLogEnabled);
    ini->put("Options", "Defines", strDefines);
    ini->put("Options", "PreBuild", strPreBuild);
    ini->put("Options", "CmdOptions", strMiscCmdOptions);
    ini->put("Directories", "Include", strIncludePath);
    ini->put("Directories", "Compiler", strCompilerPath);
    ini->put("Directories", "TargetPath", strOutputPath);

    if (close_ini)
        delete ini;

    return true;
}

bool QuincyFrame::LoadHostConfiguration(const wxString& host)
{
    #define START_OPTION(pos,line)  ((pos) == 0 || ((pos) > 0 && (line)[(pos)-1] == ' '))

    strTargetHost = host;

    /* verify environment */
    RunTimeIsInstalled = wxFileExists(strCompilerPath + DIRSEP_STR "pawnrun" EXE_EXT);
    DebuggerIsInstalled = wxFileExists(strCompilerPath + DIRSEP_STR "pawndbg" EXE_EXT);

    /* preset with compiler defaults */
    strFixedAMXName = wxEmptyString;
    UploadTool = wxEmptyString;
    DeviceTool = wxEmptyString;
    DefaultOptimize = 1;
    DefaultDebugLevel = 1;
    MaxOptimize = 3;
    OverlayEnabled = true;
    RunTimeEnabled = RunTimeIsInstalled;    /* by default, what is installed is enabled */
    DebuggerEnabled = DebuggerIsInstalled ? DEBUG_BOTH : DEBUG_NONE;

    wxString HostFile;
    if (strTargetHost.length() == 0)
        HostFile = "default";
    else
        HostFile = strTargetHost;
    HostFile = strTargetPath + DIRSEP_STR + HostFile + ".cfg";
    if (!wxFileExists(HostFile))
        return false;
    /* parse through the target host file (or default.cfg) to find an optional
       required output name and other options */
    wxTextFile ifile;
    if (!ifile.Open(HostFile))
        return false;
    for (long idx = 0; idx < (long)ifile.GetLineCount(); idx++) {
        wxString line = ifile.GetLine(idx);
        line = line.Trim(false);
        line = line.Trim(true);
        int pos;
        /* fixed output file */
        pos = line.Find("-o:");
        if (START_OPTION(pos, line)) {
            pos += 3;
            unsigned i2;
            for (i2 = pos; i2 < line.length() && line[i2] > ' '; i2++)
                /* nothing */;
            strFixedAMXName = line.Mid(pos, i2 - pos);
            pos = strFixedAMXName.Find('.', true);
            if (pos > 0)
                strFixedAMXName = strFixedAMXName.Left(pos);
        } /* if */
        /* default optimization */
        long val;
        pos = line.Find("-O:");
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 3).ToLong(&val);
            DefaultOptimize = (int)val;
        }
        /* default debugging level */
        pos = line.Find("-d:");
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 3).ToLong(&val);
            DefaultDebugLevel = (int)val;
        }
        /* Quincy: run-time enabled */
        pos = line.Find("#runtime:");
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 9).ToLong(&val);
            RunTimeEnabled = RunTimeEnabled && (val > 0);
        }
        /* Quincy: debug enabled */
        pos = line.Find("#debug:");
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 7).ToLong(&val);
            DebuggerEnabled = DebuggerEnabled & val;
        }
        /* Quincy: upload tool */
        pos = line.Find("#upload:");
        if (START_OPTION(pos, line))
            UploadTool = line.Mid(pos + 8).Trim(true).Trim(false);
        /* Quincy: device-specific tool */
        pos = line.Find("#tool:");
        if (START_OPTION(pos, line))
            DeviceTool = line.Mid(pos + 6).Trim(true).Trim(false);
        /* Quincy: max. optimization level supported */
        pos = line.Find("#optlevel:");
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 10).ToLong(&val);
            MaxOptimize = (int)val;
        }
        /* Quincy: overlays allowed */
        pos = line.Find("#overlay:");
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 9).ToLong(&val);
            OverlayEnabled = (val > 0);
        }
    }
    ifile.Close();
    return true;
}

/* IsPawnFile() only looks at the file extension */
bool QuincyFrame::IsPawnFile(const wxString& path, bool allow_inc)
{
    wxString ext = path.AfterLast('.');
    return (ext.CmpNoCase("p") == 0
            || ext.CmpNoCase("pawn") == 0
            || ext.CmpNoCase("pwn") == 0
            || ((ext.CmpNoCase("i") == 0 || ext.CmpNoCase("inc") == 0) && allow_inc));
}

void QuincyFrame::OnNewFile(wxCommandEvent& /* event */)
{
    AddEditor();
    AdjustTitle();
}

void QuincyFrame::LoadSourceFile(const wxString& path)
{
    strCurrentDirectory = wxPathOnly(path);

    // if there is only one editor, with a default filename, close that one
    wxASSERT(EditTab);
    if (EditTab->GetPageCount() == 1 && Filename[0].Length() == 0)
        RemoveEditor(0);

    if (AddEditor(path)) {
        theApp->PushRecentFile(path);
        RebuildRecentMenus();
    } else {
        wxMessageBox("Failed to load the file.", "Pawn IDE", wxOK | wxICON_ERROR);
    }
    AdjustTitle();
}

void QuincyFrame::OnOpen(wxCommandEvent& /* event */)
{
    wxFileDialog * openFileDialog = new wxFileDialog(this, "Open file...",
                                                    strCurrentDirectory, wxEmptyString,
                                                    "Pawn scripts|*.p;*.pwn;*.pawn;*.i;*.inc|All files|*",
                                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog->ShowModal() == wxID_OK)
        LoadSourceFile(openFileDialog->GetPath());
}

void QuincyFrame::OnRecentFile(wxCommandEvent& event)
{
    unsigned idx = event.GetId() - IDM_RECENTFILE1;
    wxString path = theApp->GetRecentFile(idx);
    if (wxFileExists(path)) {
        LoadSourceFile(path);
    } else {
        wxMessageBox("The workspace could not be opened.", "Pawn IDE", wxOK | wxICON_ERROR);
        theApp->RemoveRecentWorkspace(path);
        RebuildRecentMenus();
    }
}

void QuincyFrame::OnSave(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl* edit = GetActiveEdit(EditTab);
    if (edit) {
        int idx;
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        CheckSaveFile();    // save only if file has changed
        SetChanged(idx, false);
    }
}

void QuincyFrame::OnSaveAs(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl* edit = GetActiveEdit(EditTab);
    if (edit) {
        int idx;
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        CheckSaveFile(true, true, edit);
        SetChanged(idx, false);
    }
}

void QuincyFrame::OnSaveAll(wxCommandEvent& /* event */)
{
    SaveAllFiles(false);
}

void QuincyFrame::OnClose(wxCommandEvent& /* event */)
{
    CloseCurrentFile();
}

void QuincyFrame::LoadWorkspace(const wxString& path)
{
    /* close all current files (query for save) */
    if (!SaveAllFiles(true))
        return;
    SaveSession();  /* save the session, so that the project settings are also stored */
    wxASSERT(EditTab);
    while (EditTab->GetPageCount() > 0)
        RemoveEditor(0);

    /* load the new workspace */
    strWorkspace = path;
    LoadSession();
    if (EditTab->GetPageCount() == 0)
        AddEditor();    /* no files in the session, create an empty file */

    theApp->PushRecentWorkspace(strWorkspace);
    RebuildRecentMenus();
}

void QuincyFrame::OnLoadWorkSpace(wxCommandEvent& event)
{
    wxFileDialog * openFileDialog = new wxFileDialog(this, "Open workspace...",
                                                    strCurrentDirectory, wxEmptyString,
                                                    "Workspace files|*.qws|All files|*",
                                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog->ShowModal() == wxID_OK)
        LoadWorkspace(openFileDialog->GetPath());
}

void QuincyFrame::OnRecentWorkspace(wxCommandEvent& event)
{
    unsigned idx = event.GetId() - IDM_RECENTWORKSPACE1;
    wxString path = theApp->GetRecentWorkspace(idx);
    if (wxFileExists(path)) {
        LoadWorkspace(path);
    } else {
        wxMessageBox("The workspace could not be opened.", "Pawn IDE", wxOK | wxICON_ERROR);
        theApp->RemoveRecentFile(path);
        RebuildRecentMenus();
    }
}

void QuincyFrame::OnSaveWorkSpace(wxCommandEvent& event)
{
    wxFileDialog * saveFileDialog = new wxFileDialog(this, "Save workspace...",
                                                    strCurrentDirectory, strWorkspace,
                                                    "Workspace files|*.qws|All files|*",
                                                    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog->ShowModal() != wxID_OK)
        return;
    strWorkspace = saveFileDialog->GetPath();
    int idx;
    if ((idx = strWorkspace.Find('.', true)) < 0 || strWorkspace.Mid(idx).Find(DIRSEP_CHAR) > 0)
        strWorkspace += ".qws";

    SaveAllFiles(false);
    SaveSession();
}

void QuincyFrame::OnCloseWorkSpace(wxCommandEvent& event)
{
    /* close all current files (query for save) */
    if (!SaveAllFiles(true))
        return;
    SaveSession();  /* save the session, so that the project settings are also stored */
    wxASSERT(EditTab);
    while (EditTab->GetPageCount() > 0)
        RemoveEditor(0);
    strWorkspace = wxEmptyString;

    /* load default workspace */
    LoadSession();
    if (EditTab->GetPageCount() == 0)
        AddEditor();    /* no files in the session, create an empty file */
}

void QuincyFrame::OnPrint(wxCommandEvent& /* event */)
{
    static wxHtmlEasyPrinting print("Pawn Print-out");

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit) {
        wxMessageBox("No file is selected (or opened.)", "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    }

    print.SetParentWindow(this);
    for (int idx = 0; idx < MAX_EDITORS; idx++) {
        if (Editor[idx] == edit) {
            print.SetHeader(Filename[idx]);
            break;
        }
    }
    print.SetFooter("<center>@PAGENUM@ of @PAGESCNT@</center>");
    int pointsizes[] = { 4, 6, 8, 10, 12, 14, 16 };
    print.SetFonts("helvetica", "courier", pointsizes);
    print.PageSetup();
    wxPageSetupDialogData* options = print.GetPageSetupData();
    if (options->IsOk()) {
        wxString text = edit->GetText();
        text.Replace("&", "&amp;");
        text.Replace("<", "&lt;");
        text.Replace(">", "&gt;");
        print.PreviewText("<pre>" + text + "</pre");
    }
}

void QuincyFrame::OnUndo(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit) {
        edit->Undo();
        SetChanged(-1, edit->GetModify());
    }
}

void QuincyFrame::OnRedo(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit) {
        edit->Redo();
        SetChanged(-1, edit->GetModify());
    }
}

unsigned long QuincyFrame::CalcClipboardChecksum()
{
    unsigned long crc = 0;
    if (wxTheClipboard->Open()) {
        if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            wxString clip = data.GetText();

            static const unsigned long crcTable[256] = {
                0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,
                0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,
                0xE7B82D07,0x90BF1D91,0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,
                0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,
                0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,0x3B6E20C8,0x4C69105E,0xD56041E4,
                0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,
                0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,0x26D930AC,
                0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
                0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,
                0xB6662D3D,0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,
                0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,
                0x086D3D2D,0x91646C97,0xE6635C01,0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,
                0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,
                0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,0x4DB26158,0x3AB551CE,
                0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,
                0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
                0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,
                0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,
                0xB7BD5C3B,0xC0BA6CAD,0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,
                0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,
                0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,0xF00F9344,0x8708A3D2,0x1E01F268,
                0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,
                0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,0xD6D6A3E8,
                0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
                0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,
                0x4669BE79,0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,
                0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,
                0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,
                0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,
                0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,0x86D3D2D4,0xF1D4E242,
                0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,
                0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
                0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,
                0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,
                0x47B2CF7F,0x30B5FFE9,0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,
                0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,
                0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D };
            crc = 0xffffffffuL;
            for (size_t idx = 0; idx < clip.Length(); idx++) {
                #if wxCHECK_VERSION(2, 9, 0)
                    crc = (crc >> 8) ^ crcTable[(crc ^ clip[idx].GetValue()) & 0xff];
                #else
                    crc = (crc >> 8) ^ crcTable[(crc ^ clip[idx]) & 0xff];
                #endif
            }
            crc ^= 0xffffffffuL;
        }
        wxTheClipboard->Close();
    }
    return crc;
}

void QuincyFrame::OnCut(wxCommandEvent& /* event */)
{
    RectSelectChkSum = 0;
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit) {
        /* see OnCopy() for details  */
        bool isrectangle = edit->SelectionIsRectangle();
        edit->Cut();
        if (isrectangle)
            RectSelectChkSum = CalcClipboardChecksum();
    }
}

void QuincyFrame::OnCopy(wxCommandEvent& /* event */)
{
    RectSelectChkSum = 0;
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit) {
        /* first copy the contents to the clipboard, then see what the clipboard
           contents have become (in case of a rectangular selection) */
        bool isrectangle = edit->SelectionIsRectangle();
        edit->Copy();
        if (isrectangle)
            RectSelectChkSum = CalcClipboardChecksum();
    }
}

void QuincyFrame::OnPaste(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    /* delete the original selection, because we handle pasting ourselves */
    if (edit->GetSelectionStart() != edit->GetSelectionEnd())
        edit->ReplaceSelection(wxEmptyString);

    bool handled = false;

    /* check for a rectangular selection */
    if (RectSelectChkSum != 0 && RectSelectChkSum == CalcClipboardChecksum()) {
        wxString clip = wxEmptyString;
        if (wxTheClipboard->Open()) {
            if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                clip = data.GetText();
            }
            wxTheClipboard->Close();
        }
        /* get current position (line, column) */
        edit->BeginUndoAction();
        int pos = edit->GetCurrentPos();
        int row = edit->LineFromPosition(pos);
        int col = edit->GetColumn(pos);
        while (clip.Length() > 0) {
            /* extract a row from the clipboard data */
            int len = clip.Find('\n');
            if (len < 0)
                len = (int)clip.Length();
            wxString sect;
            if (len > 0 && clip[len - 1] == '\r')
                sect = clip.Left(len - 1);
            else
                sect = clip.Left(len);
            clip = clip.Mid(len + 1);
            /* insert it in the editor */
            pos = edit->FindColumn(row, col);
            while (edit->GetColumn(pos) < col) {
                edit->InsertText(pos, " ");
                pos = edit->FindColumn(row, col);
            }
            edit->InsertText(pos, sect);
            row++;
        }
        edit->EndUndoAction();
        handled = true;
    }

    /* if no rectangular selection, check for automatic adjustment of the indent */
    if (!handled && theApp->GetIndentBlocks()) {
        /* check whether we are at the start of a line */
        int pos = edit->GetCurrentPos();
        int col = edit->GetColumn(pos);
        if (col == 0) {
            /* check whether the clipboard data ends with a newline */
            wxString clip = wxEmptyString;
            if (wxTheClipboard->Open()) {
                if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
                    wxTextDataObject data;
                    wxTheClipboard->GetData(data);
                    clip = data.GetText();
                }
                wxTheClipboard->Close();
            }
            if (clip.Length() > 0 && clip[clip.Length() - 1] == '\n') {
                /* create an array of the lines */
                wxArrayString Lines;
                wxStringTokenizer tokenizer(clip, "\n", wxTOKEN_RET_EMPTY_ALL);
                while (tokenizer.HasMoreTokens()) {
                    wxString line = tokenizer.GetNextToken();
                    line.Trim();
                    Lines.Add(line);
                }
                wxASSERT(Lines.Count() >= 2);   /* since we tokenize on \n and there is at least one \n, there must be at least 2 tokens */
                /* determine indentation level of next and previous line(s) */
                unsigned indentDoc = 0;
                int curline = edit->GetCurrentLine();
                wxASSERT(curline >= 0 && curline < edit->GetLineCount());
                bool emptybelow = false;
                bool bracebelow = false;
                for (int row = curline; row < edit->GetLineCount(); row++) {
                    wxString line = edit->GetLine(row);
                    for (indentDoc = 0; indentDoc < line.Length() && line[indentDoc] <= ' '; indentDoc++)
                        /* nothing */;
                    if (indentDoc < line.Length()) {
                        if (line[indentDoc] == '}')
                            bracebelow = true;  /* closing brace found on the next line */
                        break;
                    }
                    emptybelow = true;
                }
                if (curline >= 1) {
                    bool emptyabove = false;
                    bool braceabove = false;
                    unsigned indent = indentDoc;
                    for (int row = curline - 1; row >= 0; row--) {
                        wxString line = edit->GetLine(row);
                        indent = 0;
                        while (indent < line.Length() && line[indent] <= ' ')
                            indent = (line[indent] == '\t') ? (indent / theApp->GetTabWidth() + 1) * theApp->GetTabWidth() : indent + 1;
                        if (indent < line.Length()) {
                            if (line[indent] == '}')
                                braceabove = true;  /* closing brace found on the previous line */
                            break;
                        }
                        emptyabove = true;
                    }
                    /* if a closing brace was found, use the indentation of
                       the first line below the insertion position
                       otherwise, if the line above has an indentation that
                       is one TAB level deeper than the line below, and
                       either there is an empty line below or a non-empty
                       line above, use the identation of the top line */
                    if (indent == indentDoc + theApp->GetTabWidth()
                        && (emptybelow || !emptyabove) && bracebelow && !braceabove)
                        indentDoc = indent;
                }
                /* check the indent of the clip */
                unsigned indentClip = INT_MAX;
                for (unsigned row = 0; row < Lines.Count() && indentClip > 0; row++) {
                    wxString line = Lines[row];
                    if (line.length() > 0) {
                        unsigned indent = 0;
                        while (indent < line.Length() && line[indent] <= ' ')
                            indent = (line[indent] == '\t') ? (indent / theApp->GetTabWidth() + 1) * theApp->GetTabWidth() : indent + 1;
                        if (indent < indentClip && line[indent] != '\0')
                            indentClip = indent;
                    }
                }
                /* erase the indent from the pasted block */
                for (unsigned row = 0; row < Lines.Count(); row++) {
                    wxString line = Lines[row];
                    unsigned indent = 0;
                    while (indent < indentClip && line.Length() > 0) {
                        if (line[0] == '\t')
                            indent = (indent / theApp->GetTabWidth() + 1) * theApp->GetTabWidth();
                        else
                            indent += 1;
                        line.Remove(0, 1);
                    }
                    Lines[row] = line;
                }
                /* adjust indentation of the block to the destination indent
                   (optionally inserting either TABs or spaces) */
                if (indentDoc > 0) {
                    unsigned delta = indentDoc;
                    wxChar ch = ' ';
                    if (theApp->GetUseTabs() && (delta % theApp->GetTabWidth()) == 0) {
                        ch = '\t';
                        delta /= theApp->GetTabWidth();
                    }
                    for (unsigned row = 0; row < Lines.Count(); row++)
                        if (Lines[row].Length() > 0)
                            Lines[row].insert((size_t)0, delta, ch);
                }
                /* paste clip */
                clip.Empty();
                for (unsigned row = 0; row < Lines.Count() - 1; row++)
                    clip += Lines[row] + "\n";
                edit->InsertText(pos, clip);
                handled = true;
            }
        }
    }
    if (!handled)
        edit->Paste();
}

void QuincyFrame::OnFindDlg(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    wxString word = WordUnderCursor();
    if (theApp->SearchAdvanced) {
        QuincySearchDlg* dlg = new QuincySearchDlg(this);
        dlg->SetFlags(theApp->SearchFlags | wxFR_DOWN);
        dlg->SetScope(0);   /* by default, search in the current file only */ //??? make it an option to use theApp->SearchScope
        dlg->SetRecentList(theApp->SearchRecent);
        dlg->SetSearchText(word);
        if (dlg->ShowModal() == wxID_OK) {
            /* save scope & flags */
            theApp->SearchFlags = dlg->GetFlags();
            theApp->SearchScope = dlg->GetScope();
            FindData.SetFindString(dlg->GetSearchText());   /* for FindNextItem() */
            /* add the text to the recent searches list */
            if (theApp->SearchRecent.Index(dlg->GetSearchText()) != wxNOT_FOUND)
                theApp->SearchRecent.Remove(dlg->GetSearchText());
            theApp->SearchRecent.Insert(dlg->GetSearchText(), 0);
            /* find all matches */
            wxASSERT(SearchLog);
            SearchLog->DeleteAllItems();
            switch (theApp->SearchScope) {
            case 0:
                FindAllInEditor(edit);
                break;
            case 1:
                for (unsigned page = 0; page < EditTab->GetPageCount(); page++)
                    FindAllInEditor(dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page)));
                break;
            case 2:
                wxString path;
                if (strWorkspace.Length() > 0)
                    path = strWorkspace.BeforeLast(DIRSEP_CHAR);
                else if (Filename[0].Length() > 0 && Filename[0].Find(DIRSEP_CHAR, true) > 0)
                    path = Filename[0].BeforeLast(DIRSEP_CHAR);
                else
                    path = wxGetCwd();
                wxDir dir(path);
                if (!dir.IsOpened())
                    return;
                wxString fname;
                if (dir.GetFirst(&fname, "*", wxDIR_FILES )) {
                    wxWindowDisabler *disableAll = new wxWindowDisabler;
                    #if wxCHECK_VERSION(3, 1, 0)
                        wxBusyInfo *info = new wxBusyInfo(
                            wxBusyInfoFlags()
                                .Parent(this)
                                .Icon(wxArtProvider::GetIcon(wxART_FIND))
                                .Title("Search " + path)
                                .Text("Please wait...")
                                .Foreground(*wxWHITE)
                                .Background(*wxBLACK)
                                .Transparency(4*wxALPHA_OPAQUE/5));
                    #else
                        /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier version */
                        wxBusyInfo *info = new wxBusyInfo("Please wait...", this);
                    #endif
                    do {
                        /* check the file type (by verifing the extension) */
                        if (IsPawnFile(fname)) {
                            /* get the full path of the script/include file, verify whether it exists */
                            fname = path + DIRSEP_STR + fname;
                            wxASSERT(wxFileExists(fname));
                            /* load the file in a temporary editor */
                            wxStyledTextCtrl* tmp = new wxStyledTextCtrl(EditTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTRANSPARENT_WINDOW);
                            if (tmp) {
                                tmp->Hide();
                                if (LoadFile(fname, tmp))
                                    FindAllInEditor(tmp, fname);
                                delete tmp;
                            }
                        }
                    } while (dir.GetNext(&fname));
                    delete disableAll;
                    delete info;
                }
                break;
            }
            PaneTab->SetSelection(TAB_SEARCH);
            /* jump to the first match below the current line */
            FindNextItem(theApp->SearchScope == 0);
        }
    } else if (!FindDlg) {
        FindData.SetFindString(word);
        FindDlg = new wxFindReplaceDialog();
        FindDlg->Create(this, &FindData, "Find...");
        FindDlg->Show();
        int line = edit->GetCurrentLine();
        if ((edit->MarkerGet(line) & ((1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE))) == 0) {
            IgnoreChangeEvent = true;
            edit->MarkerAdd(line, MARKER_NAVIGATE);
            IgnoreChangeEvent = false;
        }
    }
}

void QuincyFrame::OnReplaceDlg(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    wxString word = WordUnderCursor();
    if (theApp->SearchAdvanced) {
        QuincyReplaceDlg* dlg = new QuincyReplaceDlg(this);
        dlg->SetFlags(theApp->SearchFlags | wxFR_DOWN);
        dlg->SetRecentList(theApp->SearchRecent);
        dlg->SetReplaceList(theApp->ReplaceRecent);
        dlg->SetSearchText(word);
        if (dlg->ShowModal() == wxID_OK) {
            /* save flags */
            theApp->SearchFlags = dlg->GetFlags();
            FindData.SetFindString(dlg->GetSearchText());   /* for FindNextItem() */
            /* add the texts to the recent searches list */
            if (theApp->SearchRecent.Index(dlg->GetSearchText()) != wxNOT_FOUND)
                theApp->SearchRecent.Remove(dlg->GetSearchText());
            theApp->SearchRecent.Insert(dlg->GetSearchText(), 0);
            if (theApp->ReplaceRecent.Index(dlg->GetReplaceText()) != wxNOT_FOUND)
                theApp->ReplaceRecent.Remove(dlg->GetReplaceText());
            theApp->ReplaceRecent.Insert(dlg->GetReplaceText(), 0);
            QuincyReplacePrompt *prompt = new QuincyReplacePrompt(this);
            /* find the matches, starting from the current position */
            int length = dlg->GetSearchText().Length();
            bool foundmatch = false;
            int pos;
            for ( ;; ) {
                if (edit->GetSelectionStart() != edit->GetSelectionEnd())
                    pos = edit->GetSelectionEnd();
                else
                    pos = edit->GetCurrentPos();
                int result = edit->FindText(pos, edit->GetLength() - 1, dlg->GetSearchText(), dlg->GetFlags());
                if (result < 0) {
                    /* see whether there is a match from the beginning */
                    result = edit->FindText(0, edit->GetLength() - 1, dlg->GetSearchText(), dlg->GetFlags());
                    bool wrapsearch = false;
                    if (result >= 0) {
                        int reply = wxMessageBox("No more matches.\nWrap back to the beginning?", "Pawn IDE", wxYES_NO | wxICON_QUESTION);
                        if (reply == wxYES)
                            wrapsearch = true;
                    }
                    if (!wrapsearch) {
                        if (!foundmatch)
                            wxMessageBox("Search Text not found.", "Pawn IDE", wxOK | wxICON_INFORMATION);
                        else
                            wxMessageBox("No more matches found.", "Pawn IDE", wxOK | wxICON_INFORMATION);
                        break;  /* done with search/replace */
                    }
                }
                /* set the selection on the matched word and make sure that the
                   entire "search text" is visible */
                edit->GotoPos(result + length);
                edit->GotoPos(result);
                edit->SetSelection(result, result + length);
                /* show the prompt at the optimal location */
                wxPoint pt = edit->PointFromPosition(result);
                pt = edit->ClientToScreen(pt);
                pt.y += 20;
                prompt->SetPosition(pt);
                int reply = prompt->ShowModal();
                if (reply == wxID_YES || reply == wxID_LAST || reply == wxID_YESTOALL) {
                    if (reply == wxID_YESTOALL)
                        edit->BeginUndoAction();
                    edit->ReplaceSelection(dlg->GetReplaceText());
                    edit->GotoPos(result + length);
                    if (reply == wxID_YESTOALL) {
                        for ( ;; ) {
                            result = edit->FindText(0, edit->GetLength() - 1, dlg->GetSearchText(), dlg->GetFlags());
                            if (result < 0)
                                break;
                            edit->GotoPos(result);
                            edit->SetSelection(result, result + length);
                            edit->ReplaceSelection(dlg->GetReplaceText());
                            edit->GotoPos(result + length);
                        }
                        edit->BeginUndoAction();
                    }
                }
                if (reply == wxID_CANCEL || reply == wxID_LAST || reply == wxID_YESTOALL)
                    break;
            }
        }
    } else if (FindDlg) {
        FindData.SetFindString(word);
        FindDlg = new wxFindReplaceDialog();
        FindDlg->Create(this, &FindData, "Replace...", wxFR_REPLACEDIALOG);
        FindDlg->Show();
        int line = edit->GetCurrentLine();
        if ((edit->MarkerGet(line) & ((1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE))) == 0) {
            IgnoreChangeEvent = true;
            edit->MarkerAdd(line, MARKER_NAVIGATE);
            IgnoreChangeEvent = false;
        }
    }
}

void QuincyFrame::OnFindAction(wxFindDialogEvent& /* event */)
{
    FindNextItem(true);
}

void QuincyFrame::OnReplace(wxFindDialogEvent& /* event */)
{
    wxString replace = FindData.GetReplaceString();
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit && edit->GetSelectionStart() != edit->GetSelectionEnd()) {
        edit->ReplaceSelection(replace);
        FindNextItem(true);
    }
}

void QuincyFrame::OnReplaceAll(wxFindDialogEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    wxString replace = FindData.GetReplaceString();
    wxString text = FindData.GetFindString();
    int flags = FindData.GetFlags();
    edit->SetCurrentPos(0);
    while (edit->SearchNext(flags, text) >= 0) {
        edit->SearchAnchor();
        edit->ReplaceSelection(replace);
        edit->CharRight();
    }
}

void QuincyFrame::OnFindNext(wxCommandEvent& /* event */)
{
    FindNextItem(true);
}

void QuincyFrame::OnFindClose(wxFindDialogEvent& /* event */)
{
    wxASSERT(FindDlg);
    FindDlg->Destroy();
    FindDlg = 0;
}

/** WordUnderCursor() returns the word that the text cursor points at (in the
 *  active editor); if a marked area exists, it will return that marked area
 *  instead
 */
wxString QuincyFrame::WordUnderCursor()
{
    wxStyledTextCtrl* edit = GetActiveEdit(EditTab);
    if (!edit)
        return wxEmptyString;

    /* check for a marked area */
    int start = edit->GetSelectionStart();
    int end = edit->GetSelectionEnd();
    if (start == end) {
        /* there is no marked area */
        int pos = edit->GetCurrentPos();
        start = edit->WordStartPosition(pos, true);
        end = edit->WordEndPosition(pos, true);
    }
    wxString word = edit->GetTextRange(start, end);
    return word;
}

bool QuincyFrame::FindNextItem(bool warnnotfound)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return false;

    int currentpos = edit->GetCurrentPos();
    if (FindData.GetFlags() & wxFR_DOWN) {
        /* see whether there is a selection, if there is, move the cursor, so
         * that we will not find the same word again
         */
        if (edit->GetSelectionStart() != edit->GetSelectionEnd())
            edit->CharRight();
    }
    edit->SearchAnchor();

    int result;
    int flags = theApp->SearchAdvanced ? theApp->SearchFlags | wxFR_DOWN : FindData.GetFlags();
    wxString text = FindData.GetFindString();
    if (FindData.GetFlags() & wxFR_DOWN)
        result = edit->SearchNext(flags, text);
    else
        result = edit->SearchPrev(flags, text);

    if (result < 0) {
        /* check whether there exists a match elsewhere in the file */
        bool notfound = false;
        bool no_next = false;
        bool restore_cursor = true;
        if (FindData.GetFlags() & wxFR_DOWN)
            edit->SetCurrentPos(0);
        else
            edit->SetCurrentPos(edit->GetLength() - 1);
        edit->SearchAnchor();
        if (FindData.GetFlags() & wxFR_DOWN)
            result = edit->SearchNext(flags, text);
        else
            result = edit->SearchPrev(flags, text);
        if (result < 0) {
            notfound = true;            /* no matches exists at all */
        } else if (result == currentpos) {
            no_next = true;             /* this is the only match in the file */
            result = -1;
        } else {
            /* there are no more matches after/before the current cursor
               position, but there is a match before it; give an option to wrap
               around (actually, the wrap-around has already happened; the user
               only needs to confirm it) */
            int reply = wxMessageBox("No more matches in this file.\nWrap back to the beginning?", "Pawn IDE", wxYES_NO | wxICON_QUESTION);
            if (reply == wxYES) {
                edit->EnsureCaretVisible();
                restore_cursor = false;
            }
        }
        if (restore_cursor) {
            edit->SetCurrentPos(currentpos);            /* restore the position */
            edit->SetSelection(currentpos, currentpos); /* clear any selection */
        }
        if (notfound && warnnotfound)
            wxMessageBox("Search Text not found.", "Pawn IDE", wxOK | wxICON_INFORMATION);
        else if (no_next && warnnotfound)
            wxMessageBox("No more matches found.", "Pawn IDE", wxOK | wxICON_INFORMATION);
    } else {
        edit->EnsureCaretVisible();
    }
    edit->SetFocus();
    return result >= 0;
}

void QuincyFrame::FindAllInEditor(wxStyledTextCtrl* edit, const wxString& fullpath)
{
    wxASSERT(edit);
    wxASSERT(SearchLog);

    wxString filename;
    if (fullpath.Length() > 0) {
        filename = fullpath;
    } else {
        int idx;
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        filename = Filename[idx];
    }
    wxTreeItemId root = SearchLog->GetRootItem();
    if (!root.IsOk())
        root = SearchLog->AddRoot("root");
    int flags = theApp->SearchAdvanced ? theApp->SearchFlags | wxFR_DOWN : FindData.GetFlags();
    wxTreeItemId file;
    int start = 0;
    for ( ;; ) {
        int result = edit->FindText(start, edit->GetLength() - 1, FindData.GetFindString(), flags);
        if (result < 0)
            break;
        int line = edit->LineFromPosition(result);
        wxString linetext = edit->GetLine(line);
        linetext.Trim(false);
        linetext.Trim(true);
        if (!file.IsOk())
            file = SearchLog->AppendItem(root, filename);
        wxString string = wxString::Format("%4d: ", line) + linetext;
        SearchLog->AppendItem(file, string);
        start = result + 1;
    }
    if (file.IsOk())
        SearchLog->Expand(file);
}

void QuincyFrame::OnSearchSelect(wxTreeEvent& event)
{
    /* see which item it is */
    wxString string = SearchLog->GetItemText(event.GetItem());
    wxTreeItemId parent = SearchLog->GetItemParent(event.GetItem());
    wxString filename = SearchLog->GetItemText(parent);

    /* find the file, or load it */
    wxStyledTextCtrl* edit = 0;
    for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
        if (Filename[idx].Cmp(filename) == 0)
            edit = Editor[idx];
    if (!edit) {
        /* try to open the file (and find it) */
        AddEditor(filename);
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
            if (Filename[idx].Cmp(filename) == 0)
                edit = Editor[idx];
    }
    if (!edit) {
        wxMessageBox("Could not open \"" + filename + "\".", "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    }

    /* find the TAB page to activate */
    for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
        if (EditTab->GetPage(page) == edit) {
            EditTab->SetSelection(page);
            break;
        }
    }

    /* scroll to the position */
    long line;
    string.ToLong(&line);
    edit->GotoPos(edit->PositionFromLine((int)line));
    PendingFlags |= PEND_SWITCHEDIT;    /* delayed switch focus (because calling edit->SetFocus() here does not work) */
    if (!Timer->IsRunning())
        Timer->Start(100, true);
}

void QuincyFrame::OnGotoDlg(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    static long line = 1;
    wxNumberEntryDialog dlg(this, "Jump to a line in the current file.",
                            "Line number", "Go to line...",
                            line, 1, edit->GetLineCount());
    if (dlg.ShowModal() == wxID_OK) {
        line = dlg.GetValue();
        long pos = edit->PositionFromLine(line - 1);
        edit->GotoPos(pos);
        edit->SetFocus();
    }
}

void QuincyFrame::OnGotoSymbol(wxCommandEvent& /* event */)
{
    /* find the word that the text cursor points at */
    wxString word = WordUnderCursor();
    if (word.IsEmpty())
        return;
    const CSymbolEntry* symbol = SymbolList.Lookup(word);
    if (!symbol) {
        wxMessageBox("Symbol \"" + word + "\"not found.", "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    }

    /* see whether there are more matches
       if there are more matches (not a likely scenario), collect them and let
       the user choose */
    int count = 1;
    while (SymbolList.Lookup(word, count) != NULL)
        count++;
    if (count > 1) {
        const CSymbolEntry** symbollist = new const CSymbolEntry*[count];
        if (symbollist) {
            wxArrayString matches;
            for (int idx = 0; idx < count; idx++) {
                symbollist[idx] = SymbolList.Lookup(word, idx);
                matches.Add(symbollist[idx]->Syntax + " - " + symbollist[idx]->Source);
            }
            /* create a dialog that the user can choose from */
            static int dlgwidth = wxDefaultCoord;
            static int dlgheight = wxDefaultCoord;
            wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, "This symbol appears in multiple source files.", "Select source file", matches);
            dlg->SetSize(wxDefaultCoord, wxDefaultCoord, dlgwidth, dlgheight, wxSIZE_AUTO);
            int result = dlg->ShowModal();
            dlg->GetSize(&dlgwidth, &dlgheight);
            if (result == wxID_OK) {
                int idx = dlg->GetSelection();
                wxASSERT(idx >= 0 && idx < count);
                symbol = symbollist[idx];
            } else {
                symbol = NULL;
            }
            delete[] symbollist;
        }
    }
    if (!symbol)
        return;

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    wxASSERT(edit); /* otherwise a valid word could never have been found */
    int line = edit->GetCurrentLine();
    if ((edit->MarkerGet(line) & ((1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE))) == 0) {
        IgnoreChangeEvent = true;
        edit->MarkerAdd(line, MARKER_NAVIGATE);
        IgnoreChangeEvent = false;
    }

    GotoSymbol(symbol);
}

bool QuincyFrame::GotoSymbol(const CSymbolEntry* symbol)
{
    wxASSERT(symbol);

    /* find the file, or load it (note: the symbol browser always has the full
       paths of the filenames) */
    wxString filename = symbol->Source;
    if (!wxFileExists(filename)) {
        wxMessageBox("The file \"" + filename + "\" no longer exists.", "Pawn IDE", wxOK | wxICON_ERROR);
        return false;
    }
    wxStyledTextCtrl* edit = 0;
    for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
        if (Filename[idx].Cmp(filename) == 0)
            edit = Editor[idx];
    if (!edit) {
        /* try to open the file (and find it) */
        AddEditor(filename);
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
            if (Filename[idx].Cmp(filename) == 0)
                edit = Editor[idx];
    }
    if (!edit) {
        wxMessageBox("Could not open \"" + filename + "\".", "Pawn IDE", wxOK | wxICON_ERROR);
        return false;
    }

    /* find the TAB page to activate */
    for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
        if (EditTab->GetPage(page) == edit) {
            EditTab->SetSelection(page);
            break;
        }
    }

    /* search up and down for the best match (because the file may have been
       edited since last compile) */
    int line = symbol->Line - 1;
    int pos = edit->PositionFromLine(line);
    wxString name = symbol->SymbolName;
    if (name.Length() > 1 && name[1] == ':')
        name = name.Mid(2);
    //??? if the type is F, also look for the word "new" on the same line; if the
    //    type is C, als look for the word "const" on the same line
    int fwd = edit->FindText(pos, edit->GetLength(), name, wxSTC_FIND_WHOLEWORD | wxSTC_FIND_MATCHCASE);
    int bck = edit->FindText(pos, 0, name, wxSTC_FIND_WHOLEWORD | wxSTC_FIND_MATCHCASE);
    if (fwd < 0 || (bck >= 0 && (pos - bck) < (fwd - pos)))
        pos = bck;
    else if (fwd >= 0)
        pos = fwd;

    /* scroll to the position */
    edit->GotoPos(pos);
    PendingFlags |= PEND_SWITCHEDIT;
    Timer->Start(100, true);
    return true;
}

void QuincyFrame::OnMatchBrace(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    char chr = '\0';
    int pos = edit->GetCurrentPos();
    if (pos >= 0)
        chr = edit->GetCharAt(pos);
    if (pos > 0 && !(chr == '{' || chr == '}' || chr == '(' || chr == ')' || chr == '[' || chr == ']')) {
        pos--;
        chr = edit->GetCharAt(pos);
    }
    if (chr == '{' || chr == '}' || chr == '(' || chr == ')' || chr == '[' || chr == ']') {
        int brace = edit->BraceMatch(pos);
        if (brace >= 0)
            edit->GotoPos(brace);
    }
}

void QuincyFrame::OnBookmarkToggle(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    IgnoreChangeEvent = true;
    int line = edit->GetCurrentLine();
    if (edit->MarkerGet(line) & (1 << MARKER_BOOKMARK))
        edit->MarkerDelete(line, MARKER_BOOKMARK);
    else if (edit->MarkerGet(line) & (1 << MARKER_NAVIGATE))
        edit->MarkerDelete(line, MARKER_NAVIGATE);
    else
        edit->MarkerAdd(line, MARKER_BOOKMARK);
    IgnoreChangeEvent = false;
}

void QuincyFrame::OnBookmarkNext(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    int line = edit->GetCurrentLine();
    if (line < edit->GetLineCount()) {
        int next = edit->MarkerNext(line + 1, (1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE));
        if (next < 0)
            next = edit->MarkerNext(0, (1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE));
        if (next >= 0) {
            long pos = edit->PositionFromLine(next);
            edit->GotoPos(pos);
        }
    }

}

void QuincyFrame::OnBookmarkPrevious(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    int line = edit->GetCurrentLine();
    if (line > 0) {
        int prev = edit->MarkerPrevious(line - 1, (1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE));
        if (prev < 0)
            edit->MarkerPrevious(edit->GetLineCount() - 1, (1 << MARKER_BOOKMARK) | (1 << MARKER_NAVIGATE));
        if (prev >= 0) {
            long pos = edit->PositionFromLine(prev);
            edit->GotoPos(pos);
        }
    }
}

void QuincyFrame::OnFillColumn(wxCommandEvent& /* event */)
{
    /* check that the selection is a rectangle (and that there is a selection) */
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit || !edit->SelectionIsRectangle())
        return;
    int p1 = edit->GetSelectionStart();
    int p2 = edit->GetSelectionEnd();
    if (p1 == p2)
        return;

    /* pop up a dialog */
    wxString text = wxGetTextFromUser("Text to insert/replace in the selection",
                                      "Fill selection", wxEmptyString, this);
    if (text.IsEmpty())
        return;
    /* get the row start/end and starting column of the selection */
    int top = edit->LineFromPosition(p1);
    int bot = edit->LineFromPosition(p2);
    if (bot < top) {
        int t = top;
        top = bot;
        bot = t;
    }
    int col = edit->GetColumn(p1);
    if (col > edit->GetColumn(p2))
        col = edit->GetColumn(p2);
    /* insert the text */
    edit->BeginUndoAction();
    edit->DeleteBack();         /* this deletes the selection */
    for (int row = top; row <= bot; row++) {
        /* insert it in the editor */
        int pos = edit->FindColumn(row, col);
        while (edit->GetColumn(pos) < col) {
            edit->InsertText(pos, " ");
            pos = edit->FindColumn(row, col);
        }
        edit->InsertText(pos, text);
    }
    edit->EndUndoAction();
}

void QuincyFrame::OnViewWhiteSpace(wxCommandEvent& /* event */)
{
    #if !defined SCWS_INVISIBLE
        #define SCWS_INVISIBLE      0
        #define SCWS_VISIBLEALWAYS  1
    #endif
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    VisibleWhiteSpace = !VisibleWhiteSpace;
    edit->SetViewWhiteSpace(VisibleWhiteSpace ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    if (menuView)
        menuView->Check(IDM_VIEWWHITESPACE, VisibleWhiteSpace);
}

void QuincyFrame::OnViewIndentGuides(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    bool enabled = edit->GetIndentationGuides() == 0;   /* inverts the current state */
    edit->SetIndentationGuides(enabled);
    if (menuView)
        menuView->Check(IDM_VIEWINDENTGUIDES, enabled);
}

void QuincyFrame::OnCompile(wxCommandEvent& /* event */)
{
    if (ExecPID != 0 && wxProcess::Exists(ExecPID)) {
        int reply = wxMessageBox("Do you want to abort the running script?", "Pawn IDE", wxYES_NO | wxICON_QUESTION);
        if (reply != wxYES)
            return;
        wxProcess::Kill(ExecPID, wxSIGTERM);
        wxProcess::Kill(ExecPID, wxSIGKILL);
        ExecPID = 0;
        DebugMode = false;
    }

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit) {
        wxMessageBox("No source file to compile.", "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    }
    if (!SaveAllFiles(false))
        return;

    /* check the extension of the file (include files cannot be separately compiled) */
    unsigned page = EditTab->GetSelection();
    wxASSERT(page < MAX_EDITORS);
    wxString name = EditTab->GetPageText(page);
    wxString ext = name.AfterLast('.');
    if (ext.CmpNoCase("i") == 0 || ext.CmpNoCase("inc") == 0) {
        /* collect all scripts */
        wxString scriptname;
        wxArrayString scripts;
        for (page = 0; page < EditTab->GetPageCount(); page++) {
            name = EditTab->GetPageText(page);
            if (IsPawnFile(name, false))
                scripts.Add(name);
        }
        static int dlgwidth = wxDefaultCoord;
        static int dlgheight = wxDefaultCoord;
        if (scripts.Count() > 1) {
            wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, "Please select a script to compile.", "Select script", scripts);
            dlg->SetSize(wxDefaultCoord, wxDefaultCoord, dlgwidth, dlgheight, wxSIZE_AUTO);
            int result = dlg->ShowModal();
            dlg->GetSize(&dlgwidth, &dlgheight);
            if (result == wxID_OK) {
                int index = dlg->GetSelection();
                scriptname = scripts[index];
            }
        } else if (scripts.Count() == 1) {
            scriptname = scripts[0];
        } else {
            wxMessageBox("This is not a script; cannot compile.", "Pawn IDE", wxOK | wxICON_ERROR);
            return;
        }
        /* find the edit control for the named script */
        for (page = 0; page < EditTab->GetPageCount(); page++) {
            name = EditTab->GetPageText(page);
            if (name.Cmp(scriptname) == 0) {
                edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page));
                break;
            }
        }
    }
    wxASSERT(edit != NULL);

    wxString fullpath = wxEmptyString;
    int idx;
    for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
        /* nothing */;
    wxASSERT(idx < MAX_EDITORS);
    fullpath = Filename[idx];
    if (fullpath.Length() == 0) {
        wxMessageBox("Unknown filename for the source file to compile.", "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    }
    CompileSource(fullpath);  /* status bar will show the compilation result */
    if (GetAutoTransferEnabled() && strRecentAMXName.length() > 0)
        TransferScript(strRecentAMXName);
}

void QuincyFrame::OnTransfer(wxCommandEvent& /* event */)
{
    if (ExecPID == 0 && (DebuggerSelected == DEBUG_REMOTE || UploadTool.length() > 0)) {
        if (strRecentAMXName.length() == 0) {
            wxMessageBox("No recent compiled file to transfer. Build the script first",
                         "Pawn IDE", wxOK | wxICON_ERROR);
        } else {
            TransferScript(strRecentAMXName);
        }
    }
}

void QuincyFrame::OnDebug(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    if (ExecPID != 0 && wxProcess::Exists(ExecPID) && DebugMode && !DebugRunning)
        SendDebugCommand("g");
    else
        RunCurrentScript(true);
}

void QuincyFrame::OnRun(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    if (ExecPID != 0 && wxProcess::Exists(ExecPID) && DebugMode && !DebugRunning)
        SendDebugCommand("g");
    else
        RunCurrentScript();
}

void QuincyFrame::OnAbort(wxCommandEvent& /* event */)
{
    if (ExecPID != 0 && wxProcess::Exists(ExecPID)) {
        wxProcess::Kill(ExecPID, wxSIGTERM);
        wxProcess::Kill(ExecPID, wxSIGKILL);
    }
    ExecPID = 0;
    DebugMode = false;
}

void QuincyFrame::OnStepInto(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    SendDebugCommand("s");
}

void QuincyFrame::OnStepOver(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    SendDebugCommand("n");
}

void QuincyFrame::OnStepOut(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    SendDebugCommand("g func");
}

void QuincyFrame::OnRunToCursor(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    LastWatchIndex = 0;
    int line = edit->GetCurrentLine();
    wxString cmd = wxString::Format("g %d", line + 1);
    SendDebugCommand(cmd);
}

void QuincyFrame::OnBreakpointToggle(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    IgnoreChangeEvent = true;
    int line = edit->GetCurrentLine();
    if (edit->MarkerGet(line) & (1 << MARKER_BREAKPOINT))
        edit->MarkerDelete(line, MARKER_BREAKPOINT);
    else
        edit->MarkerAdd(line, MARKER_BREAKPOINT);
    IgnoreChangeEvent = false;
    ChangedBreakpoints = true;

    /* see whether we can send the update immediately */
    if (ExecPID != 0 && wxProcess::Exists(ExecPID) && DebugMode && !DebugRunning) {
        BuildBreakpointList();
        SendBreakpointList();   /* sends only the first, the others are sent in response */
    }
}

void QuincyFrame::OnBreakpointClear(wxCommandEvent& /* event */)
{
    for (unsigned tab = 0; tab < EditTab->GetPageCount(); tab++) {
        wxStyledTextCtrl* edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(tab));
        wxASSERT(edit);
        int line = 0;
        for ( ;; ) {
            int next = edit->MarkerNext(line, (1 << MARKER_BREAKPOINT));
            if (next < 0)
                break;
            edit->MarkerDelete(next, MARKER_BREAKPOINT);
            line = next + 1;
        }
    }
    ChangedBreakpoints = true;

    /* see whether we can send the update immediately */
    if (ExecPID != 0 && wxProcess::Exists(ExecPID) && DebugMode && !DebugRunning) {
        BuildBreakpointList();  /* make sure to build an empty list */
        SendBreakpointList();   /* sends the "clear" command */
    }
}

void QuincyFrame::OnIdle(wxIdleEvent& event)
{
    if (ExecPID != 0 && !wxProcess::Exists(ExecPID))
        ExecPID = 0;

    if (ExecPID != 0) {
        wxASSERT(wxProcess::Exists(ExecPID) && ExecProcess);
        wxInputStream* istream = ExecProcess->GetInputStream();
        if (istream) {
            wxString text;
            while (istream->CanRead()) {
                wxChar ch = (wxChar)istream->GetC();
                wxASSERT(DebugHoldback < sizearray(debug_prefix));
                if (DebugMode && ch == debug_prefix[DebugHoldback]) {
                    if (debug_prefix[++DebugHoldback] == '\0') {
                        /* gobble up the complete command line, then process it */
                        wxString cmd;
                        do {
                            ch = (wxChar)istream->GetC();
                            cmd += ch;
                        } while (ch != '\n' && (ch != ' ' || cmd.Cmp("dbg> ") != 0));
                        DebugHoldback = 0;
                        HandleDebugResponse(cmd);
                    }
                } else {
                    /* first add any characters held back */
                    for (int i = 0; i < DebugHoldback; i++)
                        text += debug_prefix[i];
                    DebugHoldback = 0;
                    /* add the new character */
                    if (ch == '\b') {
                        int len = text.length();
                        if (len > 0)
                            text = text.Left(len - 1);
                    } else if (ch != EOF) {
                        text += ch;
                    }
                }
            }
            if (!text.IsEmpty()) {
                Terminal->AppendText(text);
                if (PaneTab->GetSelection() != TAB_OUTPUT)
                    PaneTab->SetSelection(TAB_OUTPUT);  /* make sure "output" window is visible */
            }
        }

        if (DebugRunning) {
            /* collected text is only sent to the script if not waiting
               at a debugger prompt */
            wxOutputStream* ostream = ExecProcess->GetOutputStream();
            if (ostream) {
                for (unsigned idx = 0; idx < ExecInputQueue.length(); idx++)
                    ostream->PutC(ExecInputQueue[idx]);
                ExecInputQueue.Empty();
            }
        }

        /* while the debugger is running, request for continued idle events */
        event.RequestMore();
    }

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!context.ScanContext(edit, 0))
        event.RequestMore();
}

void QuincyFrame::OnTerminateApp(wxProcessEvent& /* event */)
{
    /* flush the remaining output */
    wxASSERT(ExecProcess);
    wxInputStream* istream = ExecProcess->GetInputStream();
    if (istream) {
        wxString text;
        while (istream->CanRead()) {
            wxChar ch = (wxChar)istream->GetC();
            if (ch != EOF)
                text += ch;
        }
        if (!text.IsEmpty()) {
            Terminal->AppendText(text);
            if (PaneTab->GetSelection() != TAB_OUTPUT)
                PaneTab->SetSelection(TAB_OUTPUT);  /* make sure "output" window is visible */
        }
    }
    /* keep the control enabled, so user can still scroll */
}

bool QuincyFrame::CompileSource(const wxString& script)
{
    /* check whether there is a prebuild step */
    bool prebuild_error = false;
    if (strPreBuild.length() > 0) {
        /* replace any options */
        wxString command = strPreBuild;
        wxString path, basename, ext;
        wxFileName::SplitPath(script, &path, &basename, &ext);
        command.Replace("%fullname%", script);
        command.Replace("%name%", basename);
        command.Replace("%ext%", basename);
        command.Replace("%path%", path);
        wxArrayString output;
        long result = wxExecute(command, output, wxEXEC_SYNC);
        prebuild_error = (result < 0 || result >= 255);
    }

    /* check the compiler */
    wxString pgmname = "pawncc" EXE_EXT;
    wxString command = strCompilerPath + DIRSEP_STR + pgmname;
    if (!wxFileExists(command)) {
        QuincyDirPicker dlg(this, "The Pawn compiler is not found in the configured folder.\nPlease choose the folder where the compiler is installed.", strCompilerPath, pgmname);
        if (dlg.ShowModal() == wxID_OK) {
            strCompilerPath = dlg.GetPath();
            /* remove trailing slash, if necessary */
            int len = strCompilerPath.Len();
            if (len > 0 && strCompilerPath[len - 1] == DIRSEP_CHAR)
                strCompilerPath = strCompilerPath.Left(len - 1);
            command = strCompilerPath + DIRSEP_STR "pawncc" EXE_EXT;
        }
    }
    if (!wxFileExists(command))
        return false;

    /* build the command */
    wxString options;
    options.Printf(" -d%d -O%d", DebugLevel, OptimizationLevel);
    if (strTargetHost.length() > 0)
        options = " -T" + strTargetHost + options;
    if (strIncludePath.length() > 0)
        options += " -i" + strIncludePath;
    if (theApp->GetUseTabs())
        options += wxString::Format(" -t%d", theApp->GetTabWidth());
    if (VerboseBuild)
        options += " -v";
    if (OverlayCode)
        options += " -V";
    if (strDefines.length() > 0)
        options += " " + strDefines;

    wxString basename, path, ext;
    wxFileName::SplitPath(script, &path, &basename, &ext);
    if (strOutputPath.length() > 0)
        path = strOutputPath;
    if (path.Right(1) != DIRSEP_STR)
        path += DIRSEP_STR;
    wxString extraoptions = strMiscCmdOptions;
    int namepos = extraoptions.Find("-o");
    if (namepos >= 0) {
        size_t start = namepos + 2;
        if (extraoptions[start] == ':' || extraoptions[start] == '=')
            start++;
        bool quote = false;
        if (extraoptions[start] == '"') {
            start++;
            quote = true;
        }
        size_t end = start;
        while (end < extraoptions.length()
               && (quote && extraoptions[end] != '"' || !quote && extraoptions[end] != ' '))
           end++;
        basename = extraoptions.SubString(start, end);
        if (quote)
            end++;
        extraoptions = extraoptions.Remove(namepos, end - namepos);
    } else if (UseFixedAMXName && strFixedAMXName.length() > 0) {
        basename = strFixedAMXName;
    }
    strRecentAMXName = path + basename + ".amx";   /* may be reset later (if compile fails) */
    options += " " + OptionallyQuoteString("-o" + strRecentAMXName);

    if (CreateReport)
        options += " -r";
    extraoptions = extraoptions.Trim(false);
    if (extraoptions.length() > 0)
        options += " " + extraoptions;
    options += " " + OptionallyQuoteString(script);

    wxWindowDisabler *disableAll = new wxWindowDisabler;
    #if wxCHECK_VERSION(3, 1, 0)
        wxBusyInfo *info = new wxBusyInfo(
                            wxBusyInfoFlags()
                                .Parent(this)
                                .Icon(wxArtProvider::GetIcon(wxART_EXECUTABLE_FILE))
                                .Title("<b>Building " + basename + "</b>")
                                .Text("Please wait...")
                                .Foreground(*wxWHITE)
                                .Background(*wxBLACK)
                                .Transparency(4*wxALPHA_OPAQUE/5));
    #else
        /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier versions */
        wxBusyInfo *info = new wxBusyInfo("Please wait...", this);
    #endif
    wxArrayString output;
    wxArrayString errors;
    long result = wxExecute(command + options, output, errors, wxEXEC_SYNC);
    delete disableAll;
    delete info;
    if (result < 0 || result >= 255) {
        wxMessageBox("Pawn compiler could not be started.\nPlease check the settings.",
                     "Pawn IDE", wxOK | wxICON_ERROR);
        strRecentAMXName = wxEmptyString;
        return false;
    }

    BuildLog->DeleteAllItems();
    BuildLog->InsertItem(0, command.AfterLast(DIRSEP_CHAR) + options);
    for (unsigned idx = 0; idx < output.Count(); idx++)
        BuildLog->InsertItem(idx + 1, output[idx]);
    BuildLog->SetColumnWidth(0, wxLIST_AUTOSIZE);
    ErrorLog->DeleteAllItems();
    for (unsigned idx = 0; idx < errors.Count(); idx++)
        ErrorLog->InsertItem(idx, errors[idx]);
    if (prebuild_error)
        ErrorLog->InsertItem(0, "Pre-build step return with an error");
    ErrorLog->SetColumnWidth(0, wxLIST_AUTOSIZE);

    if (errors.Count() == 0 && !prebuild_error) {
        BuildLog->EnsureVisible(BuildLog->GetItemCount() - 1);
        PaneTab->SetSelection(TAB_BUILD);
        SetStatusText("Build completed successfully", 0);
    } else {
        PaneTab->SetSelection(TAB_MESSAGES);
        wxString msg = wxString::Format("%d errors / warnings", errors.Count());
        SetStatusText(msg, 0);
        strRecentAMXName = wxEmptyString;
    }
    UpdateSymBrowser(script);   /* always update (even after errors) because we want to update after warnings */
    return errors.Count() == 0;
}

bool QuincyFrame::TransferScript(const wxString& path)
{
    //??? halt the RS232 reception, if any

    bool success = false;
    wxString command;
    if (UploadTool.length() > 0) {
        wxWindowDisabler *disableAll = new wxWindowDisabler;
        #if wxCHECK_VERSION(3, 1, 0)
            wxBusyInfo *info = new wxBusyInfo(
                                wxBusyInfoFlags()
                                    .Parent(this)
                                    .Icon(wxArtProvider::GetIcon(wxART_EXECUTABLE_FILE))
                                    .Title("<b>Transferring " + path.AfterLast(DIRSEP_CHAR) + "</b>")
                                    .Text("Please wait...")
                                    .Foreground(*wxWHITE)
                                    .Background(*wxBLACK)
                                    .Transparency(4*wxALPHA_OPAQUE/5));
        #else
            /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier versions */
            wxBusyInfo *info = new wxBusyInfo("Please wait...", this);
        #endif
        wxArrayString output;
        wxArrayString errors;
        command = strCompilerPath + DIRSEP_STR + UploadTool + EXE_EXT;
        command += " " + path;
        long result = wxExecute(command, output, errors, wxEXEC_SYNC);
        delete disableAll;
        delete info;
        if (result < 0 || result >= 255) {
            wxMessageBox("Transfer could not be started.\nPlease check the settings.",
                         "Pawn IDE", wxOK | wxICON_ERROR);
            return false;
        }
        success = (result == 0);
        wxString msg = success ? "Transferred to target device." : "Failure to transfer the script.";
        int cnt = BuildLog->GetItemCount();
        BuildLog->InsertItem(cnt + 1, msg);
        wxStatusBar* bar = GetStatusBar();
        SetStatusText(bar->GetStatusText(0) + ". " + msg);
    } else {
        command = strCompilerPath + DIRSEP_STR "pawndbg" EXE_EXT;
        command += " " + path;
        command += " -term=off,";
        command += debug_prefix;
        command += " -rs232=" + DebugPort;
        command += wxString::Format(",%ld", DebugBaudrate);
        command += " -transfer -quit";

        ExecPID = wxExecute(command, wxEXEC_ASYNC, ExecProcess);
        if (ExecPID <= 0) {
            wxMessageBox("Pawn debugger could not be started.\nPlease check the settings.",
                         "Pawn IDE", wxOK | wxICON_ERROR);
            delete ExecProcess;
            return success;
        }
        PaneTab->SetSelection(TAB_OUTPUT);
        Terminal->Enable(true);
        Terminal->Clear();
        ExecInputQueue.Clear();
        DebugMode = true;
        DebugRunning = true;
        DebugHoldback = 0;
        success = true;
    }
    return success;
}

bool QuincyFrame::RunCurrentScript(bool debug)
{
    /* check whether already running */
    if (ExecPID != 0 && wxProcess::Exists(ExecPID)) {
        wxMessageBox("A script is already running.", "Pawn IDE", wxOK | wxICON_ERROR);
        return false;
    }
    ExecPID = 0;    /* if not running, preset to an invalid value */

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit) {
        wxMessageBox("No source file to run.", "Pawn IDE", wxOK | wxICON_ERROR);
        return false;
    }
    if (!SaveAllFiles(false))
        return false;

    wxString fullpath = wxEmptyString;
    int idx;
    for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
        /* nothing */;
    wxASSERT(idx < MAX_EDITORS);
    fullpath = Filename[idx];
    if (fullpath.Length() == 0) {
        wxMessageBox("Unknown filename for the source file to run.", "Pawn IDE", wxOK | wxICON_ERROR);
        return false;
    }

    wxString amxname;
    if (strOutputPath.length() > 0) {
        wxString path, basename, ext;
        wxFileName::SplitPath(fullpath, &path, &basename, &ext);
        /* check whether the file is a script (i.e. the extension is .p, .pwn or .pawn) */
        if (ext.CmpNoCase("p") != 0 && ext.CmpNoCase("pawn") != 0 && ext.CmpNoCase("pwn") != 0) {
            wxString msg = "The file \"" + basename + "." + ext + "\" appears not to be a script.\nRun anyway?";
            int reply = wxMessageBox(msg, "Pawn IDE", wxYES_NO | wxICON_QUESTION);
            if (reply != wxYES)
                return false;
        }
        amxname = strOutputPath + DIRSEP_STR + basename + ".amx";
    } else {
        amxname = fullpath.BeforeLast('.') + ".amx";
    }

    /* check whether this script can run (or whether it must be compiled) */
    wxString ModifiedPrompt = wxEmptyString;
    time_t amxtime = (time_t)-1;
    time_t srctime = (time_t)-1;
    if (wxFileExists(amxname)) {
        amxtime = wxFileModificationTime(amxname);
    } else {
        /* if the output file does not exist, assume "source code" modified (but
         * with a different prompt)
         */
        ModifiedPrompt = "The source file must be compiled before it can run.\nDo you wish to compile it now?";
    }
    /* to be really sure, also check the time stamps of source and target */
    if (wxFileExists(fullpath)) {
        srctime = wxFileModificationTime(fullpath);
        if (amxtime < srctime)
            ModifiedPrompt = "The current source file was modified since last build\nDo you wish to compile it first?";
    }

    if (ModifiedPrompt.length() > 0) {
        wxMessageDialog *dial = new wxMessageDialog(NULL, ModifiedPrompt, "Script was modified", wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
        int result = dial->ShowModal();
        if (result == wxID_YES) {
            if (!CompileSource(fullpath))
                return false;   /* if there were errors during compile, do not run */
        }
    }

    if (debug) {
        /* verify that the AMX file has debug info. */
        FILE *fp = fopen(amxname.utf8_str(), "rb");
        if (fp == NULL) {
            wxMessageBox("The script cannot run, because it fails to load.", "Pawn IDE", wxOK | wxICON_ERROR);
            return false;
        }
        /* read size & signature of the AMX header */
        long size = 0;
        fread(&size, sizeof(size), 1, fp);
        unsigned short magic = 0;
        fread(&magic, sizeof(magic), 1, fp);
        if (size == 0 || (magic != AMX_MAGIC_16 && magic != AMX_MAGIC_32 && magic != AMX_MAGIC_64)) {
            wxMessageBox("Invalid or unsupported run-time file format.", "Pawn IDE", wxOK | wxICON_ERROR);
            fclose(fp);
            return false;
        }
        fseek(fp, size, SEEK_SET);
        /* read size & signature of the debug info. */
        size = 0;
        fread(&size, sizeof(size), 1, fp);
        magic = 0;
        fread(&magic, sizeof(magic), 1, fp);
        fclose(fp);
        if (size == 0 || magic != AMX_DBG_MAGIC) {
            wxMessageBox("No debug information is present.\nPlease rebuild the script\n(or run without debugging).",
                         "Pawn IDE", wxOK | wxICON_ERROR);
            return false;
        }
    }

    /* create a process instance for redirected input and output */
    ExecProcess = new wxProcess(this);
    if (ExecProcess)
        ExecProcess->Redirect();

    wxString command = strCompilerPath + DIRSEP_STR;
    if (debug)
        command += "pawndbg" EXE_EXT;
    else
        command += "pawnrun" EXE_EXT;
    command += " " + OptionallyQuoteString(amxname);
    if (debug) {
        command += " -term=off,";
        command += debug_prefix;
        if (DebuggerSelected == DEBUG_REMOTE) {
            //??? halt the RS232 reception, if any
            command += " -rs232=" + DebugPort;
            command += wxString::Format(",%ld", DebugBaudrate);
        }
    }
    ExecPID = wxExecute(command, wxEXEC_ASYNC, ExecProcess);
    if (ExecPID <= 0) {
        wxMessageBox("Pawn run-time could not be started.\nPlease check the settings.",
                     "Pawn IDE", wxOK | wxICON_ERROR);
        delete ExecProcess;
        return false;
    }
    PaneTab->SetSelection(TAB_OUTPUT);
    Terminal->Enable(true);
    Terminal->Clear();
    Terminal->SetFocus();
    ExecInputQueue.Clear();
    DebugMode = debug;
    DebugRunning = true;        /* start assuming "run mode" (wait for prompt) */
    DebugHoldback = 0;
    WatchLog->Enable(DebugMode);
    if (DebugMode) {
        /* copy all rows in the watch log to the update list */
        LastWatchIndex = 0;
        WatchUpdateList.Clear();
        int idx = 0;
        while (idx < WatchLog->GetItemCount()) {
            wxString name = WatchLog->GetItemText(idx);
            if (name.Length() == 0) {
                WatchLog->DeleteItem(idx);
            } else {
                WatchUpdateList.Add(idx);
                idx++;
            }
        }
        ChangedBreakpoints = true;  /* force updating all breakpoints too */
        BuiltBreakpoints = false;
    }
    return true;
}

void QuincyFrame::HandleDebugResponse(const wxString& cmd)
{
    SetStatusText(wxEmptyString, 0);
    if (cmd.Left(4).Cmp("file") == 0) {
        DebugCurrentFile = cmd.Mid(4);
        DebugCurrentFile.Trim();
        DebugCurrentFile.Trim(false);
    } else if (cmd.Left(5).Cmp("watch") == 0) {
        wxString text = cmd.Mid(5);
        wxStringTokenizer tokenizer(text, " \t\r\n", wxTOKEN_STRTOK);
        wxString word = tokenizer.GetNextToken();
        word.ToLong(&LastWatchIndex);
        wxString name = tokenizer.GetNextToken();
        wxString value = tokenizer.GetNextToken();
        while (tokenizer.HasMoreTokens())
            value += " " + tokenizer.GetNextToken();
        if (LastWatchIndex <= WatchLog->GetItemCount())
            WatchLog->SetItemText(LastWatchIndex - 1, name);
        else
            WatchLog->InsertItem(LastWatchIndex - 1, name);
        WatchLog->SetItem(LastWatchIndex - 1, 1, value);
        WatchLog->SetColumnWidth(0, wxLIST_AUTOSIZE);
        WatchLog->SetColumnWidth(1, wxLIST_AUTOSIZE);
    } else if (cmd.Left(3).Cmp("loc") == 0 || cmd.Left(3).Cmp("glb") == 0) {
        wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
        if (edit) {
            wxString tip = cmd.Mid(3);
            tip.Trim(false);
            tip.Trim();
            tip = tip.AfterFirst('\t');
            edit->CallTipShow(CalltipPos, tip);
        }
    } else if (cmd.Left(4).Cmp("info") == 0) {
        wxString msg = cmd.Mid(4);
        msg.Trim(false);
        msg.Trim();
        SetStatusText(msg, 0);
    } else if (cmd.Left(4).Cmp("dbg>") == 0) {
        DebugRunning = false;
        /* set the "current line" marker */
        wxStyledTextCtrl* edit = 0;
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
            if (Filename[idx].Cmp(DebugCurrentFile) == 0)
                edit = Editor[idx];
        if (!edit) {
            /* compare the base names only if no full path on the match is found */
            for (int idx = 0; idx < MAX_EDITORS && !edit; idx++) {
                wxString basename = Filename[idx].AfterLast(DIRSEP_CHAR);
                if (basename.Cmp(DebugCurrentFile) == 0)
                    edit = Editor[idx];
            }
        }
        if (edit) {
            IgnoreChangeEvent = true;
            edit->MarkerAdd(DebugCurrentLine, MARKER_CURRENTLINE);
            IgnoreChangeEvent = false;
        }
        /* send any watches not yet sent */
        if (WatchUpdateList.Count() > 0) {
            SendWatchList();
        } else {
            /* check that there is one (but only one) extra line in the watches
               pane (for the user to add a new watch) */
            int rows = WatchLog->GetItemCount();
            while (rows > LastWatchIndex + 1)
                WatchLog->DeleteItem(--rows - 1);
            if (rows <= LastWatchIndex)
                WatchLog->InsertItem(rows, wxEmptyString);
        }
        /* send any breakpoints not yet sent */
        if (ChangedBreakpoints && !BuiltBreakpoints)
            BuildBreakpointList();
        if (ChangedBreakpoints || BreakpointList.Count() > 0)
            SendBreakpointList();
    } else {
        /* must be a line-change event */
        cmd.ToLong(&DebugCurrentLine);
        DebugCurrentLine -= 1;
        /* find the TAB that this file is loaded in (or load it) */
        wxStyledTextCtrl* edit = 0;
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
            if (Filename[idx].Cmp(DebugCurrentFile) == 0)
                edit = Editor[idx];
        if (!edit) {
            /* compare the base names only if no full path on the match is found */
            for (int idx = 0; idx < MAX_EDITORS && !edit; idx++) {
                wxString basename = Filename[idx].AfterLast(DIRSEP_CHAR);
                if (basename.Cmp(DebugCurrentFile) == 0)
                    edit = Editor[idx];
            }
        }
        if (!edit) {
            /* try to open the file (and find it) */
            AddEditor(DebugCurrentFile);
            for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
                if (Filename[idx].Cmp(DebugCurrentFile) == 0)
                    edit = Editor[idx];
        }
        if (!edit) {
            wxMessageBox("Could not open " + DebugCurrentFile, "Pawn IDE", wxOK | wxICON_ERROR);
            return;
        }
        /* find the TAB page to activate */
        for (unsigned page = 0; page < EditTab->GetPageCount(); page++) {
            if (EditTab->GetPage(page) == edit) {
                EditTab->SetSelection(page);
                break;
            }
        }
        AdjustTitle();
        /* scroll to the position (don't set the marker yet, this is done when
           the debugger prompt is found) */
        long pos = edit->PositionFromLine(DebugCurrentLine);
        edit->GotoPos(pos);
    }
}

void QuincyFrame::SendDebugCommand(const wxString& cmd)
{
    /* remove the "current line" indicator;
       find the edit control again (the user may be opened/closed files
       in between) */
    wxStyledTextCtrl* edit = 0;
    for (int idx = 0; idx < MAX_EDITORS && !edit; idx++)
        if (Filename[idx].Cmp(DebugCurrentFile) == 0)
            edit = Editor[idx];
    if (!edit) {
        /* compare the base names only if no full path on the match is found */
        for (int idx = 0; idx < MAX_EDITORS && !edit; idx++) {
            wxString basename = Filename[idx].AfterLast(DIRSEP_CHAR);
            if (basename.Cmp(DebugCurrentFile) == 0)
                edit = Editor[idx];
        }
    }
    if (edit) {
        IgnoreChangeEvent = true;
        edit->MarkerDelete(DebugCurrentLine, MARKER_CURRENTLINE);
        IgnoreChangeEvent = false;
    }

    if (ExecPID != 0 && wxProcess::Exists(ExecPID)) {
        wxOutputStream* ostream = ExecProcess->GetOutputStream();
        if (ostream) {
            for (unsigned idx = 0; idx < cmd.length(); idx++)
                ostream->PutC(cmd[idx]);
            ostream->PutC('\r');
        }
    }
    DebugRunning = true;
}

void QuincyFrame::SendWatchList()
{
    wxASSERT(WatchUpdateList.Count() > 0);
    long line = WatchUpdateList[0];
    WatchUpdateList.RemoveAt(0);    /* do not update again */
    if (line < WatchLog->GetItemCount()) {
        wxString name = WatchLog->GetItemText(line);
        wxString cmd;
        if (name.length() > 0)
            cmd = wxString::Format("w %d ", line + 1) + name;
        else
            cmd = wxString::Format("cw %d", line + 1);
        SendDebugCommand(cmd);
    }
}

void QuincyFrame::BuildBreakpointList()
{
    BreakpointList.Clear();
    for (unsigned tab = 0; tab < EditTab->GetPageCount(); tab++) {
        wxStyledTextCtrl* edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(tab));
        wxASSERT(edit);
        unsigned idx = 0;
        while (idx < MAX_EDITORS && Editor[idx] != edit)
            idx++;
        wxASSERT(idx < MAX_EDITORS);
        int line = 0;
        for ( ;; ) {
            int next = edit->MarkerNext(line, (1 << MARKER_BREAKPOINT));
            if (next < 0)
                break;
            line = next + 1;
            BreakpointList.Add(Filename[idx] + wxString::Format(":%d", line));
        }
    }
    BuiltBreakpoints = true;
}

void QuincyFrame::SendBreakpointList()
{
    /* before sending the first breakpoint, clear the entire stack */
    if (ChangedBreakpoints) {
        SendDebugCommand("cbreak *");
        ChangedBreakpoints = false;
        BuiltBreakpoints = false;
    } else if (BreakpointList.Count() > 0) {
        wxString bp = BreakpointList[0];
        BreakpointList.RemoveAt(0);
        SendDebugCommand("break " + bp);
    }
}

void QuincyFrame::PrepareSearchLog()
{
    if (theApp->SearchAdvanced) {
        if (!SearchLog) {
            wxASSERT(PaneTab->GetPageCount() == TAB_SEARCH);
            #if defined _WIN32
                wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier New");
            #else
                wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Monospace");
            #endif
            SearchLog = new wxTreeCtrl(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_NO_LINES|wxTR_SINGLE|wxTR_DEFAULT_STYLE);
            SearchLog->SetFont(font);
            SearchLog->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(QuincyFrame::OnSearchSelect), NULL, this);
            PaneTab->AddPage(SearchLog, "Search", false); /* TAB_SEARCH */
        }
    } else {
        if (SearchLog) {
            wxASSERT(PaneTab->GetPageCount() > TAB_SEARCH);
            wxASSERT(PaneTab->GetPage(TAB_SEARCH));
            SearchLog->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(QuincyFrame::OnSearchSelect), NULL, this);
            PaneTab->DeletePage(TAB_SEARCH);    /* also deletes the search log control */
            SearchLog = 0;
        }
    }
}

void QuincyFrame::OnSettings(wxCommandEvent& /* event */)
{
    QuincySettingsDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK) {
        /* modify all editors (colours, settings) */
        wxASSERT(EditTab);
        unsigned idx;
        for (idx = 0; idx < EditTab->GetPageCount(); idx++) {
            wxStyledTextCtrl *edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(idx));
            wxASSERT(edit);
            SetEditorsStyle(edit);
        }
        /* add/remove a page for the search results, depending on which search
           dialog is used */
        PrepareSearchLog();
        /* since the target host may have changed, rescan the help index files
           and menu */
        RebuildHelpMenu();
        AdjustTitle();
    }
}

void QuincyFrame::SpaceToTab(bool indent_only)
{
    if (theApp->GetTabWidth() <= 0)
        return;
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    bool changed = false;
    wxString newtext = wxEmptyString;
    wxString orgtext = edit->GetText();
    wxStringTokenizer tokenizer(orgtext, '\n', wxTOKEN_RET_DELIMS);
    while (tokenizer.HasMoreTokens()) {
        wxString line = tokenizer.GetNextToken();
        int pos = 0;
        for (unsigned idx = 0; idx < line.length(); idx++) {
            if (line[idx] == '\t')
                pos = ((pos / theApp->GetTabWidth()) + 1) * theApp->GetTabWidth();
            else
                pos += 1;
            if (line[idx] == ' ' && pos % theApp->GetTabWidth() == 0) {
                /* see how many spaces we can remove */
                int start = idx;
                while (start > 0 && line[start - 1] == ' ')
                    start--;
                wxASSERT(idx - start < (unsigned)theApp->GetTabWidth());
                /* we wish to avoid converting a single space between two words
                 * to a TAB (in fact, this cannot happen if converting indent
                 * spacing only)
                 */
                if (idx - start > 0) {
                    /* replace the subrange of spaces with a TAB */
                    line.erase(start, (idx - start + 1));
                    line.insert(start, '\t');
                    idx = start;    /* adjust current position and string length */
                    changed = true;
                }
            } else if (indent_only && line[idx] > ' ') {
                break;
            }
        }
        newtext += line;
    }
    if (changed) {
        edit->SetText(newtext);
        SetChanged();
    }
}

void QuincyFrame::OnTabsToSpaces(wxCommandEvent& /* event */)
{
    if (theApp->GetTabWidth() <= 0)
        return;
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    bool changed = false;
    wxString newtext = wxEmptyString;
    wxString orgtext = edit->GetText();
    wxStringTokenizer tokenizer(orgtext, '\n', wxTOKEN_RET_DELIMS);
    while (tokenizer.HasMoreTokens()) {
        wxString line = tokenizer.GetNextToken();
        int pos;
        while ((pos = line.Find('\t')) >= 0) {
            int spaces = theApp->GetTabWidth() - (pos % theApp->GetTabWidth());
            line.erase(pos, 1);
            line.insert(pos, spaces, ' ');
            changed = true;
        }
        newtext += line;
    }
    if (changed) {
        edit->SetText(newtext);
        SetChanged();
    }
}

void QuincyFrame::OnSpacesToTabs(wxCommandEvent& /* event */)
{
    SpaceToTab(false);
}

void QuincyFrame::OnIndentsToTabs(wxCommandEvent& /* event */)
{
    SpaceToTab(true);
}

void QuincyFrame::OnTrimTrailing(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit)
        StripTrailingSpaces(edit);
}

void QuincyFrame::OnDeviceTool(wxCommandEvent& /* event */)
{
    wxString command = strCompilerPath + DIRSEP_STR + DeviceTool + EXE_EXT;
    long pid = wxExecute(command, wxEXEC_ASYNC);
    if (pid <= 0) {
        wxMessageBox("Device configuration tool could not be started.\nPlease check the installation and configuration.",
                     "Pawn IDE", wxOK | wxICON_ERROR);
    }
}

void QuincyFrame::OnSampleBrowser(wxCommandEvent& /* event */)
{
    QuincySampleBrowser* dlg = new QuincySampleBrowser(this);
    if (dlg->ShowModal() == wxID_OK) {
        /* open the files on selection */
        for (unsigned idx = 0; idx < dlg->GetFileCount(); idx++) {
            wxString path = theApp->GetExamplesPath() + DIRSEP_STR;
            wxString host = GetTargetHost();
            if (host.Length() > 0)
                path += host + DIRSEP_STR;
            path += dlg->GetFile(idx);
            LoadSourceFile(path);
        }
    }
}

void QuincyFrame::OnAbout(wxCommandEvent& /* event */)
{
    wxIcon icon(Quincy48_xpm);

    wxAboutDialogInfo info;
    info.SetName("Pawn IDE");
    info.SetVersion("0.7." SVNREV_STR);
    info.SetDescription("A tiny IDE for Pawn.");
    info.SetCopyright("(C) 2009-2024 CompuPhase");
    info.SetIcon(icon);
    info.SetWebSite("https://www.compuphase.com/pawn/");
    wxAboutBox(info);
}

void QuincyFrame::OnHelp(wxCommandEvent& event)
{
    wxString filename;
    if (event.GetId() == wxID_HELP) {
        filename = theApp->GetDocPath() + DIRSEP_STR + "Quincy.pdf";
    } else if (menuHelp) {
        wxMenuItem *item = menuHelp->FindItem(event.GetId());
        if (item) {
            filename = item->GetItemLabelText();
            filename.Replace(" ", "_");
            wxString HostDocPath = theApp->GetDocPath();
            if (strTargetHost.Length() > 0)
                HostDocPath += DIRSEP_STR + strTargetHost;
            filename = HostDocPath + DIRSEP_STR + filename + ".pdf";
        }
    }

    wxString Command;
    if (theApp->GetReaderPathValid()) {
        Command = theApp->GetReaderPath();
        /* remove page and label options */
        int idx;
        #if defined _WIN32
            idx = Command.Find('/');
        #else
            idx = Command.Find('-');
        #endif
        if (idx > 0)
            Command = Command.Left(idx);
        Command += " " + filename;
    } else {
        #if wxCHECK_VERSION(2, 9, 0)
        #else
            wxTheMimeTypesManager->ReadMailcap("/etc/mailcap");    /* for wxWidgets 2.8 */
        #endif
        wxFileType *FileType = wxTheMimeTypesManager->GetFileTypeFromMimeType("application/pdf");
        Command = FileType->GetOpenCommand(filename);
    }
    wxExecute(Command);
}

void QuincyFrame::OnContextHelp(wxCommandEvent& /* event */)
{
    /* get the word that the text cursor points at */
    wxString word = WordUnderCursor();
    if (word.IsEmpty())
        return;
    if (isdigit(word[0]))
        return; /* a number is never a token for which there is help */
    wxString label = word;  /* the label may be the word itself */

    /* find all document files that contain the bookmark */
    wxASSERT(HelpIndex);
    std::map<const char*,int> *filenames = HelpIndex->LookUp(label.utf8_str());
    wxASSERT(filenames);
    if (filenames->size() == 0 && word[0] == '@') {
        /* no exact match found, try a prefix */
        label = "at." + word.Mid(1);
        filenames = HelpIndex->LookUp(label.utf8_str());
    }
    /* - if there are zero matches, give an error
       - if there is one match, use that
       - if there are more than one matches, pop up a selection dialog */
    wxString filename;
    int page = 0;
    if (filenames == NULL || filenames->size() == 0) {
        wxMessageBox("No help is available for \"" + word + "\".",
                     "Pawn IDE", wxOK | wxICON_ERROR);
        return;
    } else if (filenames->size() == 1) {
        std::map<const char*,int>::iterator p = filenames->begin();
        filename = wxString::FromUTF8(p->first);
        page = p->second;
    } else {
        /* pop up a dialog, to have the user select the document */
        wxArrayString documents;
        std::map<const char*,int>::iterator p = filenames->begin();
        while (p != filenames->end()) {
            documents.Add(wxString::FromUTF8(p->first));
            ++p;
        }
        static int dlgwidth = wxDefaultCoord;
        static int dlgheight = wxDefaultCoord;
        wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, "This keyword appears in multiple documents. Please select the one to open.", "Select document", documents);
        dlg->SetSize(wxDefaultCoord, wxDefaultCoord, dlgwidth, dlgheight, wxSIZE_AUTO);
        int result = dlg->ShowModal();
        dlg->GetSize(&dlgwidth, &dlgheight);
        if (result == wxID_OK) {
            int index = dlg->GetSelection();
            filename = documents[index];
        }
    }
    if (filenames) {
        filenames->clear();
        delete filenames;
    }
    if (filename.Length() == 0)
        return; /* no file, quit */

    wxString Command;
    if (theApp->GetReaderPathValid()) {
        Command = theApp->GetReaderPath();
        int idx;
        /* unless there is a {path} keyword, concatenate the file path */
        if ((idx = Command.Find("{path}")) >= 0)
            Command.Replace("{path}", filename);
        else
            Command += " " + filename;
        /* replace other options */
        Command.Replace("{page}", wxString::Format("%d", page));
        Command.Replace("{label}", "p." + label);
    } else {
        #if wxCHECK_VERSION(2, 9, 0)
        #else
            wxTheMimeTypesManager->ReadMailcap("/etc/mailcap");    /* for wxWidgets 2.8 */
        #endif
        wxFileType *FileType = wxTheMimeTypesManager->GetFileTypeFromMimeType("application/pdf");
        Command = FileType->GetOpenCommand(filename);
        #if defined _WIN32
            Command += wxString::Format(" /A \"page=%d\"", page);  /* for Adobe Acrobat */
        #else
            Command += wxString::Format(" -p %d", page);   /* for Evince and Okular */
        #endif
    }
    wxExecute(Command);
}

void QuincyFrame::OnEditorChange(wxStyledTextEvent& /* event */)
{
    if (!IgnoreChangeEvent) {
        SetStatusText("Source file changed since last compile", 0);
        SetChanged();
        /* flag start of re-scan of the context list (in the background, as an idle task */
        wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
        context.ScanContext(edit, CTX_RESTART);
    }
}

void QuincyFrame::OnEditorCharAdded(wxStyledTextEvent& event)
{
    if (IgnoreChangeEvent)
        return;

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    char chr = (char)((wxStyledTextEvent&)event).GetKey();

    if (theApp->GetAutoIndent() && chr == '\n') {
        //??? also act on '\r' if support for Mac is needed
        int lineInd = 0;
        int currentLine = edit->GetCurrentLine();
        if (currentLine > 0)
            lineInd = edit->GetLineIndentation(currentLine - 1);
        if (lineInd > 0) {
            if (edit->GetLexer() == wxSTC_LEX_CPP) {
                /* also check the text on the line; if it starts with a particular
                   keyword and ends with a ')' or a '{', increment the indentation */
                wxASSERT(currentLine > 0);
                wxString text = edit->GetLine(currentLine - 1);
                /* get the first word on the previous line */
                unsigned start, stop;
                for (start = 0; start < text.Length() && text[start] <= ' '; start++)
                    /* nothing */;
                for (stop = start + 1; stop < text.Length() && text[stop] > ' ' && text[stop] != '('; stop++)
                    /* nothing */;
                if (start < text.Length() && stop < text.Length()) {
                    wxString word = text.Mid(start, stop - start);
                    if (word.Cmp("if") == 0 || word.Cmp("for") == 0
                        || word.Cmp("while") == 0 || word.Cmp("do") == 0)
                    {
                        /* check the last character on the line */
                        text.Trim();
                        stop = text.Length();
                        wxASSERT(stop > 0);
                        if (text[stop - 1] == ')' || text[stop - 1] == '{')
                            lineInd += theApp->GetTabWidth();
                    }
                }
            }
            edit->SetLineIndentation(currentLine, lineInd);
            int pos = edit->PositionFromLine(currentLine);
            while (edit->GetColumn(pos) < lineInd)
                pos++;
            edit->GotoPos(pos);
        }
    } else if (chr == '}' && theApp->GetAutoIndent() && edit->GetLexer() == wxSTC_LEX_CPP) {
        /* test whether there is any text on the line */
        int currentLine = edit->GetCurrentLine();
        wxString text = edit->GetLine(currentLine);
        unsigned idx, count = 0;
        for (idx = 0; idx < text.Length() && (text[idx] <= ' ' || text[idx] == '}') && count <= 1; idx++)
            if (text[idx] == '}')
                count++;
        if (idx == text.Length() && count == 1) {
            /* we need to get the indent level of the matching brace, but
               edit->BraceMatch() does not work at this point; so the core
               of this routine is postponed to the OnEditorPosition() event */
            PendingFlags |= PEND_INDENTBRACE;
        }
    }
}

void QuincyFrame::OnEditorPosition(wxStyledTextEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    /* change indentation level of the closing brace */
    int pos = edit->GetCurrentPos();
    int line = edit->LineFromPosition(pos);
    int col = edit->GetColumn(pos);
    if ((PendingFlags & PEND_INDENTBRACE) && pos > 0 && edit->GetCharAt(pos - 1) == '}') {
        int matchPos = edit->BraceMatch(pos - 1);
        if (matchPos >= 0) {
            /* find the indent on the line of the matching brace */
            int matchLine = edit->LineFromPosition(matchPos);
            wxASSERT(matchLine >= 0 && matchLine < line);
            int lineInd = edit->GetLineIndentation(matchLine);
            edit->SetLineIndentation(line, lineInd);
            /* refresh variables that may have changed */
            col = lineInd + 1;
            pos = edit->PositionFromLine(line);
            while (edit->GetColumn(pos) < col)
                pos++;
        }
    }
    PendingFlags &= ~PEND_INDENTBRACE;

    /* cancel any other pending event */
    if (Timer->IsRunning() && !(PendingFlags & PEND_SWITCHEDIT)) {
        Timer->Stop();
        PendingFlags = 0;
    }

    /* update status bar */
    wxString field = wxString::Format("Line %d Col %d", line + 1, col + 1);
    SetStatusText(field, 1);

    /* check for brace highlighting */
    char chr = '\0';
    if (pos >= 0)
        chr = edit->GetCharAt(pos);
    if (pos > 0 && !(chr == '{' || chr == '}' || chr == '(' || chr == ')' || chr == '[' || chr == ']')) {
        pos--;
        chr = edit->GetCharAt(pos);
    }
    if (chr == '{' || chr == '}' || chr == '(' || chr == ')' || chr == '[' || chr == ']') {
        PendingFlags |= PEND_MATCHBRACE;
        MatchBracePos[0] = pos;
        MatchBracePos[1] = edit->BraceMatch(pos);
    } else {
        if (MatchBracePos[0] >= 0)
            PendingFlags |= PEND_MATCHBRACE;
        MatchBracePos[0] = MatchBracePos[1] = wxSTC_INVALID_POSITION;
    }
    if ((PendingFlags & PEND_MATCHBRACE) && !Timer->IsRunning())
        Timer->Start(100, true);

    /* if there is a temporary bookmark on the currently active line, remove it */
    line = edit->GetCurrentLine();
    if (edit->MarkerGet(line) & (1 << MARKER_NAVIGATE)) {
        PendingFlags |= PEND_DELETE_BM;
        if (!Timer->IsRunning())
            Timer->Start(1000, true);
    }

    /* update the current context in the toolbar */
    context.ShowContext(line);
}

void QuincyFrame::OnEditorDwellStart(wxStyledTextEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;

    /* find the word that the cursor points at */
    wxPoint pt = wxGetMousePosition();
    pt = edit->ScreenToClient(pt);
    CalltipPos = edit->PositionFromPointClose(pt.x, pt.y);
    if (CalltipPos == wxSTC_INVALID_POSITION)
        return;
    int line = edit->LineFromPosition(CalltipPos);
    int col = CalltipPos - edit->PositionFromLine(line);    /* so TABs count as 1 */
    wxString text = edit->GetLine(line);
    if (!isalnum(text[col]) && text[col] != '_' && text[col] != '@')
        return;     /* not pointing at a word */
    int start = col, end = col;
    while (start > 0 && (isalnum(text[start - 1]) || text[start] == '_' || text[start] == '@'))
        start--;
    while (end < (int)text.length() && isalnum(text[end]))
        end++;
    if (start == end)
        return;     /* not a word */
    wxString word = text.Mid(start, end - start);

    /* first check whether it is a known function or constant, or (if not
       debugging) a known variable */
    wxString tip;
    if (ExecPID == 0)
        tip = LookUpInfoTip(word, TIP_ALL);
    else
        tip = LookUpInfoTip(word, TIP_FUNCTION | TIP_CONSTANT);
    if (tip.Length() > 0) {
        edit->CallTipShow(CalltipPos, tip);
        int bold = tip.Find("\n");
        if (bold > 0)
            edit->CallTipSetHighlight(0, bold);
    } else if (ExecPID != 0 && DebugMode && !DebugRunning) {
        /* if debugging and waiting at a prompt, send a command to show the value */
        SendDebugCommand("d " + word);
    }
}

void QuincyFrame::OnEditorDwellEnd(wxStyledTextEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit)
        edit->CallTipCancel();
}

void QuincyFrame::OnTimer(wxTimerEvent& event)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (edit) {
        if (PendingFlags & PEND_SWITCHEDIT) {
            PendingFlags &= ~PEND_SWITCHEDIT;
            edit->SetFocus();
        }

        if (PendingFlags & PEND_MATCHBRACE) {
            PendingFlags &= ~PEND_MATCHBRACE;
            if (MatchBracePos[0] >= 0 && MatchBracePos[1] < 0)
                edit->BraceBadLight(MatchBracePos[0]);
            else
                edit->BraceHighlight(MatchBracePos[0], MatchBracePos[1]);
        }

        if (PendingFlags & PEND_DELETE_BM) {
            PendingFlags &= ~PEND_DELETE_BM;
            int line = edit->GetCurrentLine();
            if (edit->MarkerGet(line) & (1 << MARKER_NAVIGATE)) {
                IgnoreChangeEvent = true;
                edit->MarkerDelete(line, MARKER_NAVIGATE);
                IgnoreChangeEvent = false;
            }
        }
    } /* if (edit) */

    if (ExecPID != 0 && DebugMode && !DebugRunning && WatchUpdateList.Count() > 0)
        SendWatchList();
}

void QuincyFrame::OnAutoComplete(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit || edit->AutoCompActive())
        return;

    /* find the part of the word left of the text cursor */
    int pos = edit->GetCurrentPos();
    int line = edit->LineFromPosition(pos);
    int col = pos - edit->PositionFromLine(line);   /* so TABs count as 1 */
    col -= 1;       /* but only characters *before* the cursor count */
    wxString text = edit->GetLine(line);
    if (col < 0 || col >= (int)text.Length())
        return;     /* no characters typed yet */
    if (!isalnum(text[col]) && text[col] != '_' && text[col] != '@' && text[col] != '.')
        return;     /* not pointing at a word */
    int start = col;
    while (start > 0 && (isalnum(text[start - 1]) || text[start - 1] == '_' || text[start - 1] == '@' || text[start - 1] == '/' || text[start - 1] == '*'))
        start--;
    int length = col - start + 1;
    if (length == 0)
        return;     /* not a word */
    wxString prefix = text.Mid(start, length);

    bool dotfield = (start > 0 && text[start - 1] == '.');
    if (!dotfield && prefix.Cmp(".") == 0) {
        dotfield = true;
        prefix = wxEmptyString;
        length = 0;
        start++;    /* undo skipping the '.' */
    }

    wxString contextsymbol = wxEmptyString;
    if (dotfield) {
        /* find the context (array name, function name) */
        start -= 2; /* skip '.' and go back one character more */
        /* move back on the line skipping whitespace, possibly move back a line */
        for ( ;; ) {
            while (start > 0 && isspace(text[start]))
                start--;
            if (!isspace(text[start]) || line == 0)
                break;
            text = edit->GetLine(--line);
            if (text.Length() == 0)
                return;     /* can't find parent function */
            start = text.Length() - 1;
        }
        if (text[start] == ',') {
            /* function parameter, but other parameters come before this one */
            /* move back lines as long as a line ends with a comma */
            while (line > 0) {
                wxString temp = edit->GetLine(line - 1);
                temp.Trim();
                size_t len = temp.Length();
                if (len > 0 && temp[len - 1] == ',') {
                    text = temp;
                    line -= 1;
                } else {
                    break;
                }
            }
            /* find the first word on the line that is not a keyword */
            int end = 0;
            do {
                start = end;
                while (start < (int)text.Length() && !isalnum(text[start]) && text[start] != '_' && text[start] != '@')
                    start++;
                end = start;
                while (end < (int)text.Length() && (isalnum(text[end]) || text[end] == '_' || text[end] == '@'))
                    end++;
                contextsymbol = text.Mid(start, end - start);
            } while (contextsymbol.Cmp("if") == 0 || contextsymbol.Cmp("for") == 0
                     || contextsymbol.Cmp("while") == 0 || contextsymbol.Cmp("do") == 0);
        } else {
            if (text[start] == '(') {
                /* function parameter (first in the list), skip '(' */
                start--;
                while (start > 0 && isspace(text[start - 1]))
                    start--;
            }
            int end = start;
            while (start > 0 && (isalnum(text[start - 1]) || text[start - 1] == '_' || text[start - 1] == '@'))
                start--;
            contextsymbol = text.Mid(start, end - start + 1);
        }
    }

    /* first check for snippets (these are expanded without giving a list of matches) */
    std::map<wxString,wxString>::iterator iter = theApp->SnippetList.find(prefix);
    if (iter != theApp->SnippetList.end() && iter->second.Length() > 0) {
        wxString expansion = iter->second;
        /* erase the prefix, then insert the expansion */
        edit->DelWordLeft();
        long caretpos = -1;
        for (unsigned idx = 0; idx < expansion.Length(); idx++) {
            switch ((int)expansion[idx]) {
            case '\n':
                edit->NewLine();
                break;
            case '\t':
                edit->Tab();
                break;
            case '\v':
                edit->BackTab();
                break;
            case '^':
                caretpos = edit->GetCurrentPos();
                break;
            default:
                edit->AddText(expansion.Mid(idx, 1));
                break;
            }
        }
        if (caretpos >= 0) {
            edit->SetCurrentPos(caretpos);
            edit->SetSelection(caretpos, caretpos); /* clear any selection */
        }
        return;
    }

    /* build the word list */
    wxArrayString list;

    if (contextsymbol.Length() > 0) {
        wxString definition = wxEmptyString;
        /* symbols from the symbol browser */
        for (const CSymbolEntry* sym = SymbolList.Root(); sym && definition.IsEmpty(); sym = sym->Next) {
            wxString name = sym->SymbolName;
            if ((int)name.Length() > length + 2 && name[1] == ':') {
                name = name.Mid(2);
                if (name.Cmp(contextsymbol) == 0)
                    definition = sym->Syntax;
            }
        }
        if (definition.IsEmpty()) {
            /* known (system) functions */
            for (iter = InfoTipList.begin(); iter != InfoTipList.end() && definition.IsEmpty(); ++iter) {
                wxASSERT(iter->first.Length() > 0);
                wxString name = iter->first;
                if ((int)name.Length() > length && name.Cmp(contextsymbol) == 0) {
                    definition = iter->second;
                    definition = definition.BeforeFirst('\n');
                }
            }
        }
        /* go through the definition to find the matching parameters */
        start = definition.Find('(');
        wxASSERT(start > 0 && start < (int)definition.Length());
        for ( ;; ) {
            start++;    /* skip '(' or ',' */
            while (start < (int)definition.Length() && (definition[start] <= ' ' || definition[start] <= '&'))
                start++;
            if (start >= (int)definition.Length())
                break;
            for (pos = start; pos < (int)definition.Length() && (isalnum(definition[pos]) || definition[pos] == '_'); pos++)
                /* nothing */;
            if (pos > start) {
                wxString param = definition.Mid(start, pos - start);
                if (param.Cmp("const") == 0) {
                    start = pos;
                    continue;
                }
                if (length == 0 || param.Left(length).Cmp(prefix) == 0)
                    list.Add(param);
            }
            for (start = pos; start < (int)definition.Length() && definition[start] != ','; start++)
                /* nothing */;
        }
    } else {
        /* known (system) functions */
        for (iter = InfoTipList.begin(); iter != InfoTipList.end(); ++iter) {
            wxASSERT(iter->first.Length() > 0);
            wxString name = iter->first;
            if ((int)name.Length() > length && name.Left(length).Cmp(prefix) == 0)
                list.Add(name);
        }
        /* symbols from the symbol browser */
        for (const CSymbolEntry* sym = SymbolList.Root(); sym; sym = sym->Next) {
            wxString name = sym->SymbolName;
            if ((int)name.Length() > length + 2 && name[1] == ':') {
                name = name.Mid(2);
                if (name.Left(length).Cmp(prefix) == 0 && list.Index(name) == wxNOT_FOUND)
                    list.Add(name);
            }
        }
        /* symbols from the current document */
        wxString fulltext = edit->GetText();
        /* parse through the text, wipe out comments and literal strings */
        size_t textlen = fulltext.Length();
        size_t idx = 0;
        while (idx < textlen)   {
            wxChar cur = fulltext[idx];
            wxChar nxt = (idx + 1 < textlen) ? fulltext[idx + 1] : '\0';
            if (cur == '/' && nxt == '/') {
                /* wipe out until '\r' or '\n' */
                while (idx < textlen && fulltext[idx] != '\r' && fulltext[idx] != '\n')
                    fulltext[idx++] = ' ';
                if (idx < textlen && fulltext[idx] != '\n')
                    fulltext[idx++] = ' ';
            } else if (cur == '/' && nxt == '*') {
                fulltext[idx++] = ' '; /* erase '/*' */
                fulltext[idx++] = ' ';
                wxChar prv = '\0';
                while (idx < textlen && (fulltext[idx] != '/' || prv != '*')) {
                    prv = fulltext[idx];
                    fulltext[idx++] = ' ';
                }
                if (idx < textlen)
                    fulltext[idx++] = ' '; /* also erase final '/' the comment */
            } else if (cur == '"' || cur == '\'') {
                fulltext[idx++] = ' ';     /* erase initial '"' */
                while (idx < textlen && fulltext[idx] != cur) {
                    if (idx + 1 < textlen && fulltext[idx] == '\\')
                        fulltext[idx++] = ' '; /* erase '\' plus the character following it */
                    fulltext[idx++] = ' ';
                }
                if (idx < textlen)
                    fulltext[idx++] = ' '; /* also erase final '"' */
            } else {
                idx++;  /* none of the above: normal character */
            }
        }
        wxStringTokenizer tokenizer(fulltext, " \t\r\n()[]{}<>`~!#$%^&*-+|\\;:'\",./?", wxTOKEN_STRTOK);
        while (tokenizer.HasMoreTokens()) {
            wxString word = tokenizer.GetNextToken();
            /* only include tokens that are alphanumeric (but start alphabetic) */
            if (!isalpha(word[0]) && word[0] != '_' && word[0] != '@')
                continue;
            /* only include words that are longer than what is typed already, and
               limit the list to the words that start with the same prefix (and
               avoid duplicates) */
            if ((int)word.Length() > length && word.Left(length).Cmp(prefix) == 0
                && list.Index(word) == wxNOT_FOUND)
                list.Add(word);
        }

        list.Sort();
    }

    wxString items;
    for (unsigned idx = 0; idx < list.GetCount(); idx++) {
        if (items.length () > 0)
            items += "|";
        items += list[idx];
    }
    if (items.Length() > 0)
        edit->AutoCompShow(length, items);
}

void QuincyFrame::OnWatchActivated(wxListEvent& event)
{
    WatchLog->EditLabel(event.GetIndex());
}

void QuincyFrame::OnWatchEdited(wxListEvent& event)
{
    /* add this row to the list to update */
    WatchUpdateList.Add(event.GetIndex());
    /* set a timer to update the watches */
    Timer->Start(200, true);
}

void QuincyFrame::OnWatchDelete(wxListEvent& event)
{
    /* clear the name of this watch */
    WatchLog->SetItem(event.GetIndex(), 0, wxEmptyString);
    /* add this row to the list to update */
    WatchUpdateList.Add(event.GetIndex());
    /* set a timer to update the watches */
    Timer->Start(200, true);
}

void QuincyFrame::OnTerminalChar(wxKeyEvent& event)
{
    if (ExecPID != 0)
        ExecInputQueue += (wxChar)event.GetUnicodeKey();
    event.Skip();
}

void QuincyFrame::OnUIWorkSpace(wxUpdateUIEvent& /* event */)
{
    if (menuFile) {
        wxMenuItem *item = menuFile->FindItem(IDM_CLOSEWORKSPACE);
        if (item)
            item->Enable(strWorkspace.Length() > 0);
    }
}

void QuincyFrame::OnUIUndo(wxUpdateUIEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    int newflags = UIDisabledTools;
    newflags = (edit && edit->CanUndo()) ? newflags & ~UI_UNDO : newflags | UI_UNDO;
    newflags = (edit && edit->CanRedo()) ? newflags & ~UI_REDO : newflags | UI_REDO;
    if (newflags != UIDisabledTools) {
        if (menuEdit) {
            wxMenuItem* itemUndo = menuEdit->FindItem(wxID_UNDO);
            wxMenuItem* itemRedo = menuEdit->FindItem(wxID_REDO);
            wxASSERT(itemUndo && itemRedo);
            if (edit) {
                itemUndo->Enable(edit->CanUndo());
                itemRedo->Enable(edit->CanRedo());
            }
            else {
                itemUndo->Enable(false);
                itemRedo->Enable(false);
            }
        }
        if (ToolBar) {
            ToolBar->EnableTool(wxID_UNDO, edit && edit->CanUndo());
            ToolBar->EnableTool(wxID_REDO, edit && edit->CanRedo());
            ToolBar->Refresh(false);
        }
        UIDisabledTools = newflags;
    }
}

void QuincyFrame::OnUICutCopy(wxUpdateUIEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    int s1 = 0, s2 = 0;
    if (edit) {
        s1 = edit->GetSelectionStart();
        s2 = edit->GetSelectionEnd();
    }
    int newflags = (s1 != s2) ? UIDisabledTools & ~UI_CUTCOPY : UIDisabledTools | UI_CUTCOPY;
    if (newflags != UIDisabledTools) {
        if (menuEdit) {
            wxMenuItem *item;
            if ((item = menuEdit->FindItem(wxID_CUT)) != NULL)
                item->Enable(s1 != s2);
            if ((item = menuEdit->FindItem(wxID_COPY)) != NULL)
                item->Enable(s1 != s2);
            if ((item = menuEdit->FindItem(IDM_FILLCOLUMN)) != NULL)
                item->Enable(s1 != s2 && edit->SelectionIsRectangle());
        }
        if (ToolBar) {
            ToolBar->EnableTool(wxID_CUT, s1 != s2);
            ToolBar->EnableTool(wxID_COPY, s1 != s2);
            ToolBar->Refresh(false);
        }
        UIDisabledTools = newflags;
    }
}

void QuincyFrame::OnUIPaste(wxUpdateUIEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    bool enable = (edit != 0);
    /* check the clipboard, as Scintilla's CanPaste() does not behave well if
       the clipboard holds only a single line */
    if (enable && wxTheClipboard->Open()) {
        enable = wxTheClipboard->IsSupported(wxDF_TEXT);
        wxTheClipboard->Close();
    }
    int newflags = enable ? UIDisabledTools & ~UI_PASTE : UIDisabledTools | UI_PASTE;
    if (newflags != UIDisabledTools) {
        if (menuEdit) {
            wxMenuItem *item = menuEdit->FindItem(wxID_PASTE);
            if (item)
                item->Enable(enable);
        }
        if (ToolBar) {
            ToolBar->EnableTool(wxID_PASTE, enable);
            ToolBar->Refresh(false);
        }
        UIDisabledTools = newflags;
    }
}

void QuincyFrame::OnUIFind(wxUpdateUIEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (menuEdit) {
        wxMenuItem *item = menuEdit->FindItem(IDM_FINDNEXT);
        if (item)
            item->Enable(edit && !FindData.GetFindString().IsEmpty());
    }
}

void QuincyFrame::OnUIRun(wxUpdateUIEvent& /* event */)
{
    bool enable_abort = ExecPID != 0 && wxProcess::Exists(ExecPID);
    bool enable_run = (enable_abort && DebugMode) || RunTimeEnabled;
    bool enable_transfer = !enable_abort
                           && (((DebuggerEnabled & DEBUG_REMOTE) != 0 && DebuggerSelected == DEBUG_REMOTE)
                               || UploadTool.length() > 0);
    int newflags = UIDisabledTools;
    newflags = enable_abort ? newflags & ~UI_ABORT : newflags | UI_ABORT;
    newflags = enable_abort ? newflags & ~UI_RUN : newflags | UI_RUN;
    newflags = enable_abort ? newflags & ~UI_TRANSFER : newflags | UI_TRANSFER;
    if (newflags != UIDisabledTools) {
        if (ToolBar) {
            ToolBar->EnableTool(IDM_TRANSFER, enable_transfer);
            ToolBar->EnableTool(IDM_DEBUG, DebuggerEnabled != DEBUG_NONE);
            ToolBar->EnableTool(IDM_RUN, enable_run);
            ToolBar->EnableTool(IDM_ABORT, enable_abort);
            ToolBar->EnableTool(IDM_DEVICETOOL, DeviceTool.length() > 0);
            wxAuiToolBarItem *item = ToolBar->FindTool(IDM_RUN);
            wxASSERT(item);
            if (DebugMode)
                item->SetShortHelp("Continue" + TB_SHORTCUT("Debug"));
            else
                item->SetShortHelp("Run the script" + TB_SHORTCUT("Run"));
            ToolBar->Refresh(false);
        }
        if (menuBuild) {
            wxMenuItem *item;
            if ((item = menuBuild->FindItem(IDM_TRANSFER)) != NULL)
                item->Enable(enable_transfer);
            if ((item = menuBuild->FindItem(IDM_DEBUG)) != NULL)
                item->Enable(DebuggerEnabled != DEBUG_NONE);
            if ((item = menuBuild->FindItem(IDM_RUN)) != NULL)
                item->Enable(enable_run);
            if ((item = menuBuild->FindItem(IDM_ABORT)) != NULL)
                item->Enable(enable_abort);
        }
        UIDisabledTools = newflags;
    }
}

void QuincyFrame::OnUIStepInto(wxUpdateUIEvent& /* event */)
{
    bool enable = ExecPID != 0 && wxProcess::Exists(ExecPID) && !DebugRunning;
    int newflags = enable ? UIDisabledTools & ~UI_DBGTOOLS : UIDisabledTools | UI_DBGTOOLS;
    if (newflags != UIDisabledTools) {
        if (ToolBar) {
            ToolBar->EnableTool(IDM_STEPINTO, enable);
            ToolBar->EnableTool(IDM_STEPOVER, enable);
            ToolBar->EnableTool(IDM_STEPOUT, enable);
            ToolBar->EnableTool(IDM_RUNTOCURSOR, enable);
            ToolBar->Refresh(false);
        }
        if (menuBuild) {
            wxMenuItem *item;
            if ((item = menuBuild->FindItem(IDM_STEPINTO)) != NULL)
                item->Enable(enable);
            if ((item = menuBuild->FindItem(IDM_STEPOVER)) != NULL)
                item->Enable(enable);
            if ((item = menuBuild->FindItem(IDM_STEPOUT)) != NULL)
                item->Enable(enable);
            if ((item = menuBuild->FindItem(IDM_RUNTOCURSOR)) != NULL)
                item->Enable(enable);
        }
        UIDisabledTools = newflags;
    }
}

bool QuincyFrame::UpdateSymBrowser(const wxString& filename)
{
    bool result;
    if (filename.Length() == 0) {
        wxString path;
        if (strOutputPath.Length() > 0)
            path = strOutputPath;
        else if (strWorkspace.Length() > 0)
            path = strWorkspace.BeforeLast(DIRSEP_CHAR);
        else if (Filename[0].Length() > 0 && Filename[0].Find(DIRSEP_CHAR, true) > 0)
            path = Filename[0].BeforeLast(DIRSEP_CHAR);
        else
            path = wxGetCwd();
        wxDir dir(path);
        if (!dir.IsOpened())
            return false;
        int count = 0;
        wxString fname;
        if (dir.GetFirst(&fname, "*.xml", wxDIR_FILES )) {
            do {
                /* get the full path of the matching PDF file, verify whether it exists */
                fname = path + DIRSEP_STR + fname;
                wxASSERT(wxFileExists(fname));
                if (SymbolList.LoadReportFile(fname))
                    count += 1;
            } while (dir.GetNext(&fname));
        }
        result = (count > 0);
    } else {
        result = SymbolList.LoadReportFile(filename.BeforeLast('.') + ".xml");
    }

    wxASSERT(BrowserTree);
    if (result) {
        BrowserTree->DeleteAllItems();
        wxTreeItemId root = BrowserTree->AddRoot("root");
        wxTreeItemId sectionConstants = BrowserTree->AppendItem(root, "Constants");
        wxTreeItemId sectionGlobals = BrowserTree->AppendItem(root, "Global variables");
        wxTreeItemId sectionFunctions = BrowserTree->AppendItem(root, "Functions");
        wxTreeItemId section;
        for (const CSymbolEntry* item = SymbolList.Root(); item; item = item->Next) {
            wxASSERT(item->SymbolName[1] == ':');
            if (item->SymbolName[0] == 'C')
                section = sectionConstants;
            else if (item->SymbolName[0] == 'F')
                section = sectionGlobals;
            else if (item->SymbolName[0] == 'M')
                section = sectionFunctions;
            else
                continue;
            BrowserTree->AppendItem(section, item->Syntax + " - " + item->Source, -1, -1, new BrowserItemData(item));
        }
    } else {
        if (BrowserTree->GetCount() == 0) {
            wxTreeItemId root = BrowserTree->AddRoot("root");
            BrowserTree->AppendItem(root, "No symbols loaded");
        }
    }
    return result;
}

bool QuincyFrame::ReadInfoTips()
{
    InfoTipList.clear();    /* delete any current contents */

    /* build the filename */
    wxString pathname;
    pathname = theApp->GetDocPath();
    if (strTargetHost.Length() > 0)
        pathname += DIRSEP_STR + strTargetHost;
    pathname += DIRSEP_STR "infotips.lst";
    /* the target host name may be upper case while the direcory name is lower case */
    if (!wxFileExists(pathname) && strTargetHost.Length() > 0) {
        wxString name = strTargetHost;
        pathname = theApp->GetDocPath() + DIRSEP_STR + name.MakeLower() + DIRSEP_STR "infotips.lst";
    }

    wxTextFile flst;
    if (!flst.Open(pathname))
        return false;
    for (long idx = 0; idx < (long)flst.GetLineCount(); idx++) {
        wxString line = flst.GetLine(idx);
        line = line.Trim(false);
        line = line.Trim(true);
        if (line.length() != 0 && line[0] != '#') {
            /* get the keyword from the line */
            int namelength;
            int openparen = line.Find('(');
            if (line[0] == '@') {
                namelength = line.Find(')');   /* use complete function definition for public functions */
                if (namelength > 0)
                    namelength++;
            } else {
                namelength = openparen; /* use only the function name */
            }
            if (namelength < 0) {
                namelength = line.Find(' ');
                if (namelength > 0 && line[namelength - 1] == ':')
                    namelength += line.Mid(namelength + 1).Find(' ') + 1;
            }
            if (namelength > 0) {
                /* remove the tag name in front of the function name */
                wxString keyword;
                int skip = line.Find(':');
                if (skip >= 0 && skip < openparen) {
                    while (line[++skip] == ' ')
                        /* nothing */;
                } else {
                    skip = 0;
                }
                keyword = line.Mid(skip, namelength - skip);
                /* reformat the line somewhat */
                if (line.length() > (size_t)namelength && line[namelength] == '(') {
                    int closing = line.Mid(namelength).Find(')');
                    if (closing > 0)
                        namelength += closing + 1;
                }
                wxString def = line.Left(namelength);
                wxString descr = line.Mid(namelength).Trim(false);
                /* add it to the list */
                InfoTipList.insert(std::make_pair(keyword, def + "\n" + descr));
            }
        }
    }
    flst.Close();
    return true;
}

wxString QuincyFrame::LookUpInfoTip(const wxString& keyword, int flags)
{
    if (flags & TIP_FUNCTION) {
        std::map<wxString,wxString>::iterator iter = InfoTipList.begin();
        while (iter != InfoTipList.end()) {
            wxString key = iter->first;
            if (key[0] == '@') {
                int paren = key.Find('(');
                if (paren > 0)
                    key = key.Left(paren);
            }
            if (key.Cmp(keyword) == 0)
                return iter->second;
            ++iter;
        }
    }

    /* not found in the "info" files, see whether the symbol browser has it */
    for (const CSymbolEntry* sym = SymbolList.Root(); sym; sym = sym->Next) {
        if (sym->SymbolName.Length() > 1 && sym->SymbolName[1] == ':'
            && keyword.Cmp(sym->SymbolName.Mid(2)) == 0 && sym->Syntax.Length() > 0)
        {
            if (((flags & TIP_FUNCTION) && sym->SymbolName[0] == 'M')
                || ((flags & TIP_VARIABLE) && sym->SymbolName[0] == 'F')
                || ((flags & TIP_CONSTANT) && sym->SymbolName[0] == 'C'))
            {
                wxString item = sym->Syntax;
                if (sym->Summary.Length() > 0)
                    item += "\n" + sym->Summary;
                return item;
            }
        }
    }

    return wxEmptyString;
}

void QuincyFrame::RebuildRecentMenus()
{
    if (menuRecentFiles) {
        unsigned idx = 0;
        while (menuRecentFiles->FindItem(IDM_RECENTFILE1 + idx) != NULL) {
            Disconnect(IDM_RECENTFILE1 + idx, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRecentFile));
            menuRecentFiles->Destroy(IDM_RECENTFILE1 + idx);
            idx++;
        }
        for (idx = 0; !theApp->GetRecentFile(idx).IsEmpty(); idx++) {
            menuRecentFiles->Append(IDM_RECENTFILE1 + idx, theApp->GetRecentFile(idx));
            Connect(IDM_RECENTFILE1 + idx, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRecentFile));
        }
    }

    if (menuRecentWorkspaces) {
        unsigned idx = 0;
        while (menuRecentWorkspaces->FindItem(IDM_RECENTWORKSPACE1 + idx) != NULL) {
            Disconnect(IDM_RECENTWORKSPACE1 + idx, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRecentWorkspace));
            menuRecentWorkspaces->Destroy(IDM_RECENTWORKSPACE1 + idx);
            idx++;
        }
        for (idx = 0; !theApp->GetRecentWorkspace(idx).IsEmpty(); idx++) {
            menuRecentWorkspaces->Append(IDM_RECENTWORKSPACE1 + idx, theApp->GetRecentWorkspace(idx));
            Connect(IDM_RECENTWORKSPACE1 + idx, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnRecentWorkspace));
        }
    }
}

void QuincyFrame::RebuildHelpMenu()
{
    /* delete all non-standard help files from the menu */
    if (menuHelp) {
        int count = 0;
        while (menuHelp->FindItem(IDM_HELP1 + count) != NULL) {
            Disconnect(IDM_HELP1 + count, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnHelp));
            menuHelp->Destroy(IDM_HELP1 + count);
            count++;
        }
    }

    /* create or re-create the index */
    if (HelpIndex)
        delete HelpIndex;
    HelpIndex = new CHelpIndex;

    wxString HostDocPath = theApp->GetDocPath();
    if (strTargetHost.Length() > 0)
        HostDocPath += DIRSEP_STR + strTargetHost;
    wxDir dir(HostDocPath);
    if (!dir.IsOpened())
        return;
    wxString filename;
    if (dir.GetFirst(&filename, "*.aux", wxDIR_FILES )) {
        int count = 0;
        do {
            /* get the full path of the matching PDF file, verify whether it exists */
            wxString PDFFile = HostDocPath + DIRSEP_STR + filename.BeforeLast('.') + ".pdf";
            if (!wxFileExists(PDFFile))
                continue;   /* not found, skip this file */
            /* process the index */
            wxString AuxFile = HostDocPath + DIRSEP_STR + filename;
            HelpIndex->ScanFile(AuxFile.utf8_str(), IDM_HELP1 + count, PDFFile.utf8_str());
            /* set this in the menu */
            if (menuHelp) {
                filename = filename.BeforeLast('.');
                filename.Replace("_", " ");
                menuHelp->Insert(count + 1, IDM_HELP1 + count, filename);
                Connect(IDM_HELP1 + count, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnHelp));
            }
            count += 1;
        } while (dir.GetNext(&filename) && count < MAX_HELPFILES);
    }
}

void QuincyFrame::RebuildToolsMenu()
{
    if (menuTools) {
        if (menuTools->FindItem(IDM_DEVICETOOL) != NULL) {
            Disconnect(IDM_DEVICETOOL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnDeviceTool));
            menuTools->Destroy(IDM_DEVICETOOL);
            size_t pos = menuTools->GetMenuItemCount();
            if (pos > 0) {
                wxMenuItem* item = menuTools->FindItemByPosition(pos - 1);
                if (item->GetKind() == wxITEM_SEPARATOR)
                    menuTools->Destroy(item);
            }
        }
    }

    if (DeviceTool.length() > 0) {
        wxBitmap tb_devicetool(tb_device_xpm);
        menuTools->AppendSeparator();
        AppendIconItem(menuTools, IDM_CONTEXTHELP, MENU_ENTRY("DeviceTool"), tb_devicetool);
        Connect(IDM_DEVICETOOL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnDeviceTool));
    }
}

void QuincyFrame::SetEditorsStyle(wxStyledTextCtrl *edit)
{
    wxASSERT(edit);
    edit->StyleSetFont(wxSTC_STYLE_DEFAULT, theApp->CodeFont);
    for (int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; i++)
        edit->StyleSetFont(i, theApp->CodeFont);

    edit->SetCaretLineBackground(theApp->EditColours[CLR_ACTIVE]);
    edit->SetIndentationGuides(false);

    edit->StyleSetBackground(wxSTC_STYLE_DEFAULT, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_STYLE_DEFAULT, theApp->EditColours[CLR_TEXT]);
    edit->StyleSetBackground(wxSTC_C_DEFAULT, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_DEFAULT, theApp->EditColours[CLR_TEXT]);
    edit->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(80, 80, 40));
    edit->StyleSetBackground(wxSTC_C_IDENTIFIER, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_IDENTIFIER, theApp->EditColours[CLR_TEXT]);
    edit->StyleSetBackground(wxSTC_C_WORD, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_WORD, theApp->EditColours[CLR_KEYWORDS]);
    edit->StyleSetBackground(wxSTC_C_WORD2, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_WORD2, theApp->EditColours[CLR_KEYWORDS]);
    edit->StyleSetBackground(wxSTC_C_STRING, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_STRING, theApp->EditColours[CLR_STRINGS]);
    edit->StyleSetBackground(wxSTC_C_CHARACTER, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_CHARACTER, theApp->EditColours[CLR_STRINGS]);
    edit->StyleSetBackground(wxSTC_C_COMMENT, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_COMMENT, theApp->EditColours[CLR_COMMENTS]);
    edit->StyleSetBackground(wxSTC_C_COMMENTLINE, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_COMMENTLINE, theApp->EditColours[CLR_COMMENTS]);
    edit->StyleSetBackground(wxSTC_C_COMMENTDOC, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_COMMENTDOC, theApp->EditColours[CLR_COMMENTS]);
    edit->StyleSetBackground(wxSTC_C_COMMENTDOCKEYWORD, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD, theApp->EditColours[CLR_COMMENTS]);
    edit->StyleSetBackground(wxSTC_C_NUMBER, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_NUMBER, theApp->EditColours[CLR_NUMBERS]);
    edit->StyleSetBackground(wxSTC_C_OPERATOR, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_OPERATOR, theApp->EditColours[CLR_OPERATORS]);
    edit->StyleSetBackground(wxSTC_C_PREPROCESSOR, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_PREPROCESSOR, theApp->EditColours[CLR_PREPROCESSOR]);
    edit->StyleSetBackground(wxSTC_C_STRINGEOL, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_C_STRINGEOL, theApp->EditColours[CLR_STRINGEOL]);
    edit->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, theApp->EditColours[CLR_HIGHLIGHT]);
    edit->StyleSetBackground(wxSTC_STYLE_BRACEBAD, theApp->EditColours[CLR_BACKGROUND]);
    edit->StyleSetForeground(wxSTC_STYLE_BRACEBAD, theApp->EditColours[CLR_STRINGEOL]);

    edit->StyleSetBold(wxSTC_C_OPERATOR, true);
    edit->StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);
    edit->StyleSetBold(wxSTC_STYLE_BRACEBAD, true);
    edit->StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);
    edit->StyleSetUnderline(wxSTC_C_STRINGEOL, true);
    edit->StyleSetUnderline(wxSTC_STYLE_BRACEBAD, true);

    edit->SetTabWidth(theApp->GetTabWidth());
    edit->SetIndent(theApp->GetTabWidth());
    edit->SetUseTabs(theApp->GetUseTabs());

    edit->CallTipUseStyle(30);
    wxFont tipfont = theApp->CodeFont;
    wxSize sz = tipfont.GetPixelSize();
    sz.SetHeight((sz.GetHeight() * 8) / 10);
    sz.SetWidth((sz.GetWidth() * 8) / 10);
    tipfont.SetPixelSize(sz);
    edit->StyleSetFont(wxSTC_STYLE_CALLTIP, tipfont);
}

void QuincyFrame::OnSelectContext(wxCommandEvent& event)
{
    int idx = FunctionList->GetSelection();
    if (idx != wxNOT_FOUND) {
        wxString name = FunctionList->GetString(idx);
        int linenr = context.Lookup(name);
        wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
        if (linenr != wxNOT_FOUND && edit) {
            long pos = edit->PositionFromLine(linenr);
            edit->GotoPos(pos);
            edit->SetFocus();
        } else {
            FunctionList->SetSelection(wxNOT_FOUND);
        }
    }
}

/** ScanContext() scans the current editor file in small chunks. It returns true
 *  when it has completed, or false when it needs to be called again to finish.
 */
bool ContextParse::ScanContext(wxStyledTextCtrl* edit, int flags)
{
    if (edit != activeedit)
        flags |= CTX_RESET;
    if ((flags & CTX_RESET) || (flags & CTX_FULL))
        flags |= CTX_RESTART;
    bool updatectrl = false;
    if (flags & CTX_RESTART) {
        WorkNames.Clear();
        WorkRanges.Clear();
        startline = 0;
        activeedit = edit;
        incomment = false;
        inparamlist = false;
        nestlevel = 0;
        topline = -1;
        context = wxEmptyString;
    }
    if (flags & CTX_RESET) {
        Names.Clear();
        Ranges.Clear();
        updatectrl = true;
    }

    if (!re.IsValid())
        re.Compile("^[[:blank:]]*(static[[:blank:]]+|)([A-Za-z@_][A-Za-z@_0-9]*:[[:blank:]]*|)([A-Za-z@_][A-Za-z@_0-9]*)[[:blank:]]*\\(", wxRE_EXTENDED);

    /* run through the source code of the current document */
    wxASSERT(edit == activeedit);
    bool result = false;    /* assume not yet completed */
    if (edit) {
        int startline_save = startline;
        int lastline;
        if (flags & CTX_FULL) {
            lastline = edit->GetLineCount();
        } else {
            lastline = startline + CTX_LINES;
            if (lastline > edit->GetLineCount())
                lastline = edit->GetLineCount();
        }
        wxASSERT(re.IsValid());
        for (int idx = startline; idx < lastline; idx++) {
            wxString line = edit->GetLine(idx);
            if (line.Left(12).Cmp("timer_second")==0)
                lastline = idx + 1;
            /* strip line comments */
            int pos;
            if ((pos = line.Find("//")) >= 0)
                line = line.Left(pos);
            if (nestlevel == 0 && !incomment && re.Matches(line)) {
                context = re.GetMatch(line, 3);
                topline = idx;
                inparamlist = true;
            }
            bool instring = false;
            bool escaped = false;
            for (unsigned c = 0; c < line.Length(); c++) {
                if (line[c] == '{' && !incomment && !instring && !inparamlist) {
                    if (nestlevel == 0 && context.Length() > 0) {
                        WorkNames.Add(context);
                        wxASSERT(topline >= 0);
                    }
                    nestlevel++;
                } else if (line[c] == '}' && !incomment && !instring && !inparamlist) {
                    nestlevel--;
                    if (nestlevel < 0)
                        nestlevel = 0;
                    if (nestlevel == 0  && context.Length() > 0) {
                        wxASSERT(topline >= 0);
                        btmline = idx;
                        if (WorkRanges.Count() == WorkNames.Count() - 1)
                            WorkRanges.Add((long)topline | ((long)btmline << 16));  /* with unbalanced braces, parsing fails */
                        wxASSERT(WorkNames.Count() == WorkRanges.Count());
                        context = wxEmptyString;
                        topline = -1;
                    }
                } else if (line[c] == ')' && !incomment && !instring) {
                    inparamlist = false;
                } else if (line[c] == '"' && !incomment && !escaped) {
                    instring = !instring;
                } else if (line[c] == '/' && (c + 1) < line.Length() && line[c + 1] == '*' && !instring) {
                    incomment = true;
                } else if (line[c] == '*' && (c + 1) < line.Length() && line[c + 1] == '/' && !instring) {
                    incomment = false;
                }
                if (line[c] == '\\')
                    escaped = !escaped;
                else
                    escaped = false;
            }
        } /* for (idx) */
        startline = lastline;
        if (startline >= edit->GetLineCount()) {
            if (startline != startline_save) {
                /* first see whether the work names are identical to the saved ones
                   (the choice control does not need to be updated if they are) */
                updatectrl = false;     /* preset */
                if (Names.Count() != WorkNames.Count())
                    updatectrl = true;
                for (unsigned i =0; !updatectrl && i < WorkNames.Count(); i++)
                    if (WorkNames[i].Cmp(Names[i]) != 0)
                        updatectrl = true;
                if (updatectrl)
                    Names = WorkNames;
                Ranges = WorkRanges;
                WorkNames.Clear();
                WorkRanges.Clear();
            }
            result = true;
        }
    } else {
        if (Names.Count() > 0) {
            Names.Clear();
            Ranges.Clear();
            updatectrl = true;
        }
        result = true;
    }

    if (choicectrl && updatectrl) {
        choicectrl->SetSelection(wxNOT_FOUND);
        choicectrl->Set(Names);
        if (edit) {
            int pos = edit->GetCurrentPos();
            int line = edit->LineFromPosition(pos);
            currentselection = -1;  /* make invalid, to force ShowContext() to update */
            ShowContext(line);
        }
    }

    return result;
}

void ContextParse::ShowContext(int linenr)
{
    if (choicectrl) {
        int newidx = -1;
        for (unsigned idx = 0; idx < Ranges.Count() && newidx < 0; idx++)
            if (linenr >= (Ranges[idx] & 0xffff) && linenr <= ((Ranges[idx] >> 16) & 0xffff))
                newidx = idx;
        if (newidx != currentselection) {
            currentselection = newidx;
            int newsel = (newidx >= 0 && newidx < (int)Names.Count()) ? choicectrl->FindString(Names[newidx]) : wxNOT_FOUND;
            choicectrl->SetSelection(newsel);
        }
    }
}

int ContextParse::Lookup(const wxString& name)
{
    unsigned idx;
    for (idx = 0; idx < Names.Count() && Names[idx] != name; idx++)
        /* nothing */;
    if (idx >= Names.Count())
        return wxNOT_FOUND;
    wxASSERT(idx < Ranges.Count());
    return (int)(Ranges[idx] & 0xffff);
}