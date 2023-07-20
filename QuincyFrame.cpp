/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright CompuPhase, 2009-2023
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
 *  Version: $Id: QuincyFrame.cpp 6967 2023-07-20 20:15:38Z thiadmer $
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
#include "res/tb_transfer.xpm"
#include "res/tb_compile.xpm"
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
                                     wxT("stock switch tagof while true false");


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
    AppendIconItem(menuFile, wxID_NEW, MENU_ENTRY(wxT("New")), tb_new);
    AppendIconItem(menuFile, wxID_OPEN, MENU_ENTRY(wxT("Open")), tb_open);
    AppendIconItem(menuFile, wxID_SAVE, MENU_ENTRY(wxT("Save")), tb_save);
    AppendIconItem(menuFile, wxID_SAVEAS, MENU_ENTRY(wxT("SaveAs")), tb_saveas);
    AppendIconItem(menuFile, IDM_SAVEALL, MENU_ENTRY(wxT("SaveAll")), tb_saveall);
    AppendIconItem(menuFile, wxID_CLOSE, MENU_ENTRY(wxT("Close")), tb_close);
    menuFile->AppendSeparator();
    menuFile->Append(IDM_LOADWORKSPACE, MENU_ENTRY(wxT("OpenWorkspace")));
    menuFile->Append(IDM_SAVEWORKSPACE, MENU_ENTRY(wxT("SaveWorkspace")));
    menuFile->Append(IDM_CLOSEWORKSPACE, MENU_ENTRY(wxT("CloseWorkspace")));
    menuFile->AppendSeparator();
    AppendIconItem(menuFile, wxID_PRINT, MENU_ENTRY(wxT("Print")), tb_print);
    menuFile->AppendSeparator();
    menuRecentFiles = new wxMenu;   /* the "recent files/workspaces" menus are filled later */
    menuRecentWorkspaces = new wxMenu;
    menuFile->Append(-1, wxT("Recent files"), menuRecentFiles);
    menuFile->Append(-1, wxT("Recent workspaces"), menuRecentWorkspaces);
    menuFile->AppendSeparator();
    AppendIconItem(menuFile, wxID_EXIT, MENU_ENTRY(wxT("Quit")), tb_quit);
    menuBar->Append(menuFile, wxT("&File"));

    menuEdit = new wxMenu;
    AppendIconItem(menuEdit, wxID_UNDO, MENU_ENTRY(wxT("Undo")), tb_undo);
    AppendIconItem(menuEdit, wxID_REDO, MENU_ENTRY(wxT("Redo")), tb_redo);
    AppendIconItem(menuEdit, wxID_CUT, MENU_ENTRY(wxT("Cut")), tb_cut);
    AppendIconItem(menuEdit, wxID_COPY, MENU_ENTRY(wxT("Copy")), tb_copy);
    AppendIconItem(menuEdit, wxID_PASTE, MENU_ENTRY(wxT("Paste")), tb_paste);
    menuEdit->AppendSeparator();
    AppendIconItem(menuEdit, wxID_FIND, MENU_ENTRY(wxT("Find")), tb_find);
    AppendIconItem(menuEdit, IDM_FINDNEXT, MENU_ENTRY(wxT("FindNext")), tb_findnext);
    AppendIconItem(menuEdit, wxID_REPLACE,  MENU_ENTRY(wxT("Replace")), tb_replace);
    menuEdit->AppendSeparator();
    menuEdit->Append(wxID_INDEX, MENU_ENTRY(wxT("GotoLine")));
    menuEdit->Append(IDM_GOTOSYMBOL, MENU_ENTRY(wxT("GotoSymbol")));
    AppendIconItem(menuEdit, IDM_MATCHBRACE,  MENU_ENTRY(wxT("MatchBrace")), tb_bracematch);
    menuBookmarks = new wxMenu;
    menuBookmarks->Append(IDM_BOOKMARKTOGGLE, MENU_ENTRY(wxT("ToggleBookmark")));
    menuBookmarks->Append(IDM_BOOKMARKNEXT, MENU_ENTRY(wxT("NextBookmark")));
    menuBookmarks->Append(IDM_BOOKMARKPREV, MENU_ENTRY(wxT("PrevBookmark")));
    //??? list bookmarks (in all files)
    menuEdit->Append(-1, wxT("Bookmarks"), menuBookmarks);
    menuEdit->AppendSeparator();
    menuEdit->Append(IDM_AUTOCOMPLETE, MENU_ENTRY(wxT("Autocomplete")));
    AppendIconItem(menuEdit, IDM_FILLCOLUMN,  MENU_ENTRY(wxT("FillColumn")), tb_columnfill);
    menuBar->Append(menuEdit, wxT("&Edit"));

    menuView = new wxMenu;
    menuView->AppendCheckItem(IDM_VIEWWHITESPACE, MENU_ENTRY(wxT("ViewWhitespace")));
    menuView->AppendCheckItem(IDM_VIEWINDENTGUIDES, MENU_ENTRY(wxT("ViewIndentGuides")));
    menuBar->Append(menuView, wxT("&View"));

    menuBuild = new wxMenu;
    AppendIconItem(menuBuild, IDM_COMPILE, MENU_ENTRY(wxT("Compile")), tb_compile);
    AppendIconItem(menuBuild, IDM_TRANSFER, MENU_ENTRY(wxT("Transfer")), tb_transfer);
    menuBuild->AppendSeparator();
    AppendIconItem(menuBuild, IDM_DEBUG, MENU_ENTRY(wxT("Debug")), tb_debug);
    AppendIconItem(menuBuild, IDM_RUN, MENU_ENTRY(wxT("Run")), tb_run);
    AppendIconItem(menuBuild, IDM_ABORT, MENU_ENTRY(wxT("Stop")), tb_abort);
    menuBuild->AppendSeparator();
    AppendIconItem(menuBuild, IDM_STEPINTO, MENU_ENTRY(wxT("StepInto")), tb_stepinto);
    AppendIconItem(menuBuild, IDM_STEPOVER, MENU_ENTRY(wxT("StepOver")), tb_stepover);
    AppendIconItem(menuBuild, IDM_STEPOUT, MENU_ENTRY(wxT("StepOut")), tb_stepout);
    AppendIconItem(menuBuild, IDM_RUNTOCURSOR, MENU_ENTRY(wxT("RunToCursor")), tb_runtocursor);
    menuBuild->AppendSeparator();
    menuBreakpoints = new wxMenu;
    AppendIconItem(menuBreakpoints, IDM_BREAKPOINTTOGGLE, MENU_ENTRY(wxT("ToggleBreakpoint")), tb_breakpoint);
    menuBreakpoints->Append(IDM_BREAKPOINTCLEAR, MENU_ENTRY(wxT("ClearBreakpoints")));
    //??? list all breakpoints
    menuBuild->Append(-1, wxT("Breakpoints"), menuBreakpoints);
    menuBar->Append(menuBuild, wxT("&Build/Run"));

    menuTools = new wxMenu;
    AppendIconItem(menuTools, wxID_PROPERTIES, MENU_ENTRY(wxT("Options")), tb_settings);
    AppendIconItem(menuTools, IDM_SAMPLEBROWSER, MENU_ENTRY(wxT("SampleBrowser")), tb_pawn);
    menuTabSpace = new wxMenu;
    menuTabSpace->Append(IDM_TABSTOSPACES, MENU_ENTRY(wxT("TabToSpace")));
    menuTabSpace->Append(IDM_INDENTSTOTABS, MENU_ENTRY(wxT("IndentToTab")));
    menuTabSpace->Append(IDM_SPACESTOTABS, MENU_ENTRY(wxT("SpaceToTab")));
    menuTabSpace->AppendSeparator();
    menuTabSpace->Append(IDM_TRIMTRAILING, MENU_ENTRY(wxT("TrimTrailing")));
    menuTools->Append(-1, wxT("Whitespace conversions"), menuTabSpace);
    menuBar->Append(menuTools, wxT("&Tools"));

    menuHelp = new wxMenu;
    AppendIconItem(menuHelp, wxID_HELP, MENU_ENTRY(wxT("GeneralHelp")), tb_help);
    AppendIconItem(menuHelp, IDM_CONTEXTHELP, MENU_ENTRY(wxT("ContextHelp")), tb_contexthelp);
    menuHelp->AppendSeparator();
    menuHelp->Append(wxID_ABOUT/*, wxT("&About")*/);
    menuBar->Append(menuHelp, wxT("&Help"));

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
    ToolBar->AddTool(wxID_NEW, wxT("New"), tb_new, wxT("New file") + TB_SHORTCUT(wxT("New")));
    ToolBar->AddTool(wxID_OPEN, wxT("Open"), tb_open, wxT("Open file") + TB_SHORTCUT(wxT("Open")));
    ToolBar->AddTool(wxID_SAVE, wxT("Save"), tb_save, wxT("Save active file") + TB_SHORTCUT(wxT("Save")));
    ToolBar->AddTool(IDM_SAVEALL, wxT("Save all"), tb_saveall, wxT("Save all modified files") + TB_SHORTCUT(wxT("SaveAll")));
    ToolBar->AddSeparator();
    ToolBar->AddTool(wxID_PRINT, wxT("Print"), tb_print, wxT("Print file") + TB_SHORTCUT(wxT("Print")));
    ToolBar->AddSeparator();
    ToolBar->AddTool(wxID_CUT, wxT("Cut"), tb_cut, wxT("Cut to clipboard") + TB_SHORTCUT(wxT("Cut")));
    ToolBar->AddTool(wxID_COPY, wxT("Copy"), tb_copy, wxT("Copy to clipboard") + TB_SHORTCUT(wxT("Copy")));
    ToolBar->AddTool(wxID_PASTE, wxT("Paste"), tb_paste, wxT("Paste from clipboard") + TB_SHORTCUT(wxT("Paste")));
    ToolBar->AddSeparator();
    ToolBar->AddTool(wxID_UNDO, wxT("Undo"), tb_undo, wxT("Undo last operation") + TB_SHORTCUT(wxT("Undo")));
    ToolBar->AddTool(wxID_REDO, wxT("Redo"), tb_redo, wxT("Redo last undo") + TB_SHORTCUT(wxT("Redo")));
    ToolBar->AddSeparator();
    ToolBar->AddTool(IDM_COMPILE, wxT("Build"), tb_compile, wxT("Compile the current script") + TB_SHORTCUT(wxT("Compile")));
    ToolBar->AddTool(IDM_TRANSFER, wxT("Transfer"), tb_transfer, wxT("Transfer the compiled script to an external host") + TB_SHORTCUT(wxT("Transfer")));
    ToolBar->AddTool(IDM_DEBUG, wxT("Debug"), tb_debug, wxT("Debug the script") + TB_SHORTCUT(wxT("Debug")));
    ToolBar->AddTool(IDM_RUN, wxT("Run"), tb_run, wxT("Run the script") + TB_SHORTCUT(wxT("Run")));
    ToolBar->AddTool(IDM_ABORT, wxT("Stop"), tb_abort, wxT("Abort the running script") + TB_SHORTCUT(wxT("Stop")));
    ToolBar->AddTool(IDM_STEPINTO, wxT("Step Into"), tb_stepinto, wxT("Step into functions") + TB_SHORTCUT(wxT("StepInto")));
    ToolBar->AddTool(IDM_STEPOVER, wxT("Step Over"), tb_stepover, wxT("Step over functions") + TB_SHORTCUT(wxT("StepOver")));
    ToolBar->AddTool(IDM_STEPOUT, wxT("Step Out"), tb_stepout, wxT("Run up to the function return") + TB_SHORTCUT(wxT("StepOut")));
    ToolBar->AddTool(IDM_RUNTOCURSOR, wxT("Run to cursor"), tb_runtocursor, wxT("Run up to the current line") + TB_SHORTCUT(wxT("RunToCursor")));
    ToolBar->AddSeparator();
    FunctionList = new wxChoice(ToolBar, IDM_SELECTCONTEXT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SORT);
    FunctionList->SetMinSize(wxSize(200, -1));
    FunctionList->SetToolTip(wxT("The function name at the current cursor position."));
    ToolBar->AddControl(FunctionList, wxT("Context"));
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
        wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New"));
    #else
        wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Monospace"));
    #endif
    BuildLog = new wxListView(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL);
    BuildLog->SetFont(font);
    BuildLog->InsertColumn(0, wxEmptyString);
    PaneTab->AddPage(BuildLog, wxT("Build"), false);    /* TAB_BUILD */
    ErrorLog = new wxListView(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL);
    ErrorLog->SetFont(font);
    ErrorLog->InsertColumn(0, wxEmptyString);
    ErrorLog->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(QuincyFrame::OnErrorSelect), NULL, this);
    PaneTab->AddPage(ErrorLog, wxT("Messages"), false); /* TAB_MESSAGES */
    BrowserTree = new wxTreeCtrl(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_NO_LINES|wxTR_SINGLE|wxTR_DEFAULT_STYLE);
    BrowserTree->SetFont(font);
    BrowserTree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(QuincyFrame::OnSymbolSelect), NULL, this);
    PaneTab->AddPage(BrowserTree, wxT("Symbols"), false); /* TAB_SYMBOLS */
    WatchLog = new wxListView(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_EDIT_LABELS);
    WatchLog->SetFont(font);
    WatchLog->InsertColumn(0, wxEmptyString);
    WatchLog->InsertColumn(1, wxEmptyString);
    WatchLog->Connect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(QuincyFrame::OnWatchEdited), NULL, this);
    WatchLog->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(QuincyFrame::OnWatchActivated), NULL, this);
    WatchLog->Connect(wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler(QuincyFrame::OnWatchDelete), NULL, this);
    PaneTab->AddPage(WatchLog, wxT("Watches"), false);  /* TAB_WATCHES */
    Terminal = new wxTextCtrl(PaneTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY);
    Terminal->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    Terminal->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    Terminal->SetFont(font);
    Terminal->Connect(wxEVT_CHAR, wxKeyEventHandler(QuincyFrame::OnTerminalChar), NULL, this);
    PaneTab->AddPage(Terminal, wxT("Output"), false);   /* TAB_OUTPUT */
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
    long pos = ini->getl(wxT("Position"), wxT("Splitter"), 0);
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
    strWorkspace = ini->gets(wxT("Session"), wxT("Workspace"));
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
                    wxString msg = wxT("The file ") + path.AfterLast(DIRSEP_CHAR)
                                   + wxT(" was changed outside the editor.\nReload?");
                    if (EditorDirty[index])
                        msg += wxT("\n\nNota Bene: the file has local changes. These are lost on a reload.");
                    int reply = wxMessageBox(msg, wxT("Notice"), wxYES_NO);
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
    wxString title = wxT("Pawn");

    /* add current target */
    if (strTargetHost.Length() > 0) {
        title += wxT(" [") + strTargetHost + wxT("]");
    }

    /* add current file */
    wxASSERT(EditTab);
    int sel = EditTab->GetSelection();
    if (sel >= 0)
        title += wxT(" - ") + EditTab->GetPageText(sel);
    if (strWorkspace.Length() > 0) {
        wxString project = strWorkspace;
        if (project.Find(DIRSEP_CHAR) >= 0)
            project = project.AfterLast(DIRSEP_CHAR);
        title += wxT(" (") + project + wxT(")");
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
            if (filenames[n].Right(4).CmpNoCase(wxT(".prj"))==0) {
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
    wxString filename = line.BeforeFirst(wxT('('));
    line = line.AfterFirst(wxT('('));
    wxString linenrs = line.BeforeFirst(wxT(')'));
    long first, last;
    linenrs.ToLong(&first);
    linenrs = linenrs.AfterLast(wxT('-'));
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
        wxMessageBox(wxT("Could not open ") + filename, wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        int reply = wxMessageBox(wxT("The file ") + name.AfterLast(DIRSEP_CHAR)
                                 + wxT(" uses characters outside the ASCII range and needs to be converted to UTF-8.\n")
                                   wxT("Do you wish to do that now?"), wxT("Notice"), wxYES_NO);
        if (reply != wxYES)
            return false;
        if (!Latin1ToUTF8(name)) {
            wxMessageBox(wxT("Conversion of ") + name.AfterLast(DIRSEP_CHAR) + wxT(" to UTF-8 has failed."),
                         wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        edit->SetProperty(wxT("lexer.cpp.track.preprocessor"), wxT("0"));   /* this is too complex with Pawn, because Pawn can also check definitions of variables & functions */
        edit->SetKeyWords(0, PawnKeyWords);
        edit->SetWordChars(wxT("_@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"));
    }

    edit->SetCodePage(wxSTC_CP_UTF8);   /* required for the Unicode build of wxWidgets, which is now standard */
    edit->SetTabIndents(true);
    edit->SetBackSpaceUnIndents(true);
    edit->SetMouseDwellTime(500);
    edit->AutoCompStops(wxT(" ()[]{}<>`~!#$%^&*-+|\\;:'\",./?"));
    edit->AutoCompSetDropRestOfWord(true);
    edit->AutoCompSetChooseSingle(true);
    edit->AutoCompSetSeparator(wxT('|'));
    /* make sure special keys are not not handled by scintilla */
    edit->CmdKeyClear(wxT('['), wxSTC_SCMOD_CTRL);  /* CmdKeyClear blocks the key from propagating */
    edit->CmdKeyClear(wxT(']'), wxSTC_SCMOD_CTRL);
    for (int i = 0; i < theApp->Shortcuts.Count(); i++) {
        KbdShortcut* shortcut = theApp->Shortcuts.GetItem(i);
        wxAcceleratorEntry entry;
        if (entry.FromString(shortcut->GetShortcut())) {
            int flags = wxSTC_SCMOD_NORM;   /* wxWidgets flags and Scintilla flags are not the same */
            if (entry.GetFlags() &  wxACCEL_SHIFT)
                flags |= wxSTC_SCMOD_SHIFT;
            if (entry.GetFlags() &  wxACCEL_CTRL)
                flags |= wxSTC_SCMOD_CTRL;
            if (entry.GetFlags() &  wxACCEL_ALT)
                flags |= wxSTC_SCMOD_ALT;
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
        str = wxT("(new)");
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
            wxMessageBox(wxT("Failed to load file ") + str, wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
            wxFileDialog * saveFileDialog = new wxFileDialog(this, wxT("Save file..."),
                                                            strCurrentDirectory, path,
                                                            wxT("Pawn scripts|*.p;*.pwn;*.pawn|Pawn include files|*.i;*.inc|All files|*"),
                                                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
            if (saveFileDialog->ShowModal() != wxID_OK)
                return false;
            /* get the name, set the default extension */
            path = saveFileDialog->GetPath();
            wxString ext = path.AfterLast(wxT('.'));
            if (ext.Length() == 0 || ext.Length() > 4) {
                int type = saveFileDialog->GetFilterIndex();
                if (type == 0)
                    path += wxT(".p");
                else if (type == 1)
                    path += wxT(".i");
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
            wxMessageBox(wxT("Failed to save the file."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
                filenames += Filename[index] + wxT("\n");
        }
        if (filenames.Length() > 0) {
            wxString msg = wxT("The following file(s) was/were modified:\n")
                           + filenames + wxT("\nSave changes?");
            int reply = wxMessageBox(msg, wxT("Pawn IDE"), wxYES_NO | wxCANCEL);
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
            int reply = wxMessageBox(wxT("Save changes in ") + name, wxT("Pawn IDE"), wxYES_NO | wxCANCEL);
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
            edit->ReplaceTarget(wxT(""));
        }
    }
}

wxString QuincyFrame::OptionallyQuoteString(const wxString& string)
{
    if (string.Find(wxT(' ')) >= 0)
        return wxT("\"") + string + wxT("\"");
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
    strCurrentDirectory = ini->gets(wxT("Session"), wxT("Directory"), strCurrentDirectory);

    int idx;
    wxString item;
    wxString path;
    for (idx = 0; idx < MAX_EDITORS; idx++) {
        item.Printf(wxT("File%d"), idx + 1);
        path = ini->gets(wxT("Session"), item);
        if (path.Length() == 0)
            break;
        AddEditor(path);
    }

    /* load workspace settings */
    strTargetHost = ini->gets(wxT("Options"), wxT("TargetHost"), wxT(""));
    DebugLevel = (int)ini->getl(wxT("Options"), wxT("Debugging"), 1);
    OptimizationLevel = (int)ini->getl(wxT("Options"), wxT("Optimize"), 1);
    OverlayCode = ini->getbool(wxT("Options"), wxT("Overlays"));
    UseFixedAMXName = ini->getbool(wxT("Options"), wxT("FixedAMXName"));
    VerboseBuild = ini->getbool(wxT("Options"), wxT("Verbose"));
    CreateReport = ini->getbool(wxT("Options"), wxT("Report"));
    DebuggerSelected = ini->getl(wxT("Options"), wxT("Debugger"));
    DebugPort = ini->gets(wxT("Options"), wxT("DebugPort"));
    DebugBaudrate = ini->getl(wxT("Options"), wxT("DebugBaudrate"));
    DebugLogEnabled = ini->getbool(wxT("Options"), wxT("DebugLogging"));
    strDefines = ini->gets(wxT("Options"), wxT("Defines"));
    strPreBuild = ini->gets(wxT("Options"), wxT("PreBuild"));
    strMiscCmdOptions = ini->gets(wxT("Options"), wxT("CmdOptions"));
    strIncludePath = ini->gets(wxT("Directories"), wxT("Include"));
    strCompilerPath = ini->gets(wxT("Directories"), wxT("Compiler"), theApp->GetBinPath());
    strOutputPath = ini->gets(wxT("Directories"), wxT("TargetPath"));
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
    if (strTargetPath.Right(4).CmpNoCase(wxT(DIRSEP_STR) wxT("bin")) == 0)
        strTargetPath = strTargetPath.Left(strTargetPath.Length() - 4); /* strip off "/bin" */
    strTargetPath += wxT(DIRSEP_STR) wxT("target");

    if (close_ini)
        delete ini;

    LoadHostConfiguration(strTargetHost);
    if (strFixedAMXName.length() == 0)
        UseFixedAMXName = false;    /* force false if no fixed name is defined */
    else if (ini->getl(wxT("Options"), wxT("FixedAMXName"), -1) == -1)
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
    ini->put(wxT("Session"), wxT("Workspace"), strWorkspace);

    /* the other data may be stored in the workspace file */
    bool close_ini = false;
    if (strWorkspace.Length() > 0) {
        ini = new minIni(strWorkspace);
        close_ini = true;
    }
    wxASSERT(ini);
    ini->put(wxT("Session"), wxT("Directory"), strCurrentDirectory);
    int page, idx;
    wxString item;
    wxString fullpath;
    for (page = 0; page < (int)EditTab->GetPageCount(); page++) {
        item.Printf(wxT("File%d"), page + 1);
        wxStyledTextCtrl *edit = dynamic_cast<wxStyledTextCtrl*>(EditTab->GetPage(page));
        wxASSERT(edit);
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        fullpath = Filename[idx];
        ini->put(wxT("Session"), item, fullpath);
    }
    /* remove any files not currently open */
    for ( ;; ) {
        item.Printf(wxT("File%d"), page + 1);
        wxString name = ini->gets(wxT("Session"), item);
        if (name.Length() == 0)
            break;
        ini->del(wxT("Session"), item);
        page++;
    }

    /* save workspace settings */
    ini->put(wxT("Options"), wxT("TargetHost"), strTargetHost);
    ini->put(wxT("Options"), wxT("Debugging"), DebugLevel);
    ini->put(wxT("Options"), wxT("Optimize"), OptimizationLevel);
    ini->put(wxT("Options"), wxT("Overlays"), OverlayCode);
    ini->put(wxT("Options"), wxT("FixedAMXName"), UseFixedAMXName);
    ini->put(wxT("Options"), wxT("Verbose"), VerboseBuild);
    ini->put(wxT("Options"), wxT("Report"), CreateReport);
    ini->put(wxT("Options"), wxT("Debugger"), DebuggerSelected);
    ini->put(wxT("Options"), wxT("DebugPort"), DebugPort);
    ini->put(wxT("Options"), wxT("DebugBaudrate"), DebugBaudrate);
    ini->put(wxT("Options"), wxT("DebugLogging"), DebugLogEnabled);
    ini->put(wxT("Options"), wxT("Defines"), strDefines);
    ini->put(wxT("Options"), wxT("PreBuild"), strPreBuild);
    ini->put(wxT("Options"), wxT("CmdOptions"), strMiscCmdOptions);
    ini->put(wxT("Directories"), wxT("Include"), strIncludePath);
    ini->put(wxT("Directories"), wxT("Compiler"), strCompilerPath);
    ini->put(wxT("Directories"), wxT("TargetPath"), strOutputPath);

    if (close_ini)
        delete ini;

    return true;
}

bool QuincyFrame::LoadHostConfiguration(const wxString& host)
{
    #define START_OPTION(pos,line)  ((pos) == 0 || ((pos) > 0 && (line)[(pos)-1] == ' '))

    strTargetHost = host;

    /* verify environment */
    RunTimeIsInstalled = wxFileExists(strCompilerPath + wxT(DIRSEP_STR) wxT("pawnrun") wxT(EXE_EXT));
    DebuggerIsInstalled = wxFileExists(strCompilerPath + wxT(DIRSEP_STR) wxT("pawndbg") wxT(EXE_EXT));

    /* preset with compiler defaults */
    strFixedAMXName = wxEmptyString;
    DefaultOptimize = 1;
    DefaultDebugLevel = 1;
    MaxOptimize = 3;
    OverlayEnabled = true;
    RunTimeEnabled = RunTimeIsInstalled;    /* by default, what is installed is enabled */
    DebuggerEnabled = DebuggerIsInstalled ? DEBUG_BOTH : DEBUG_NONE;

    wxString HostFile;
    if (strTargetHost.length() == 0)
        HostFile = wxT("default");
    else
        HostFile = strTargetHost;
    HostFile = strTargetPath + wxT(DIRSEP_STR) + HostFile + wxT(".cfg");
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
        pos = line.Find(wxT("-o:"));
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
        pos = line.Find(wxT("-O:"));
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 3).ToLong(&val);
            DefaultOptimize = (int)val;
        }
        /* default debugging level */
        pos = line.Find(wxT("-d:"));
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 3).ToLong(&val);
            DefaultDebugLevel = (int)val;
        }
        /* Quincy: run-time enabled */
        pos = line.Find(wxT("#runtime:"));
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 9).ToLong(&val);
            RunTimeEnabled = RunTimeEnabled && (val > 0);
        }
        /* Quincy: debug enabled */
        pos = line.Find(wxT("#debug:"));
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 7).ToLong(&val);
            DebuggerEnabled = DebuggerEnabled & val;
        }
        /* Quincy: max. optimization level supported */
        pos = line.Find(wxT("#optlevel:"));
        if (START_OPTION(pos, line)) {
            line.Mid(pos + 10).ToLong(&val);
            MaxOptimize = (int)val;
        }
        /* Quincy: overlays allowed */
        pos = line.Find(wxT("#overlay:"));
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
    wxString ext = path.AfterLast(wxT('.'));
    return (ext.CmpNoCase(wxT("p")) == 0
            || ext.CmpNoCase(wxT("pawn")) == 0
            || ext.CmpNoCase(wxT("pwn")) == 0
            || ((ext.CmpNoCase(wxT("i")) == 0 || ext.CmpNoCase(wxT("inc")) == 0) && allow_inc));
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
        wxMessageBox(wxT("Failed to load the file."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
    }
    AdjustTitle();
}

void QuincyFrame::OnOpen(wxCommandEvent& /* event */)
{
    wxFileDialog * openFileDialog = new wxFileDialog(this, wxT("Open file..."),
                                                    strCurrentDirectory, wxEmptyString,
                                                    wxT("Pawn scripts|*.p;*.pwn;*.pawn;*.i;*.inc|All files|*"),
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
        wxMessageBox(wxT("The workspace could not be opened."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
    wxFileDialog * openFileDialog = new wxFileDialog(this, wxT("Open workspace..."),
                                                    strCurrentDirectory, wxEmptyString,
                                                    wxT("Workspace files|*.qws|All files|*"),
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
        wxMessageBox(wxT("The workspace could not be opened."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
        theApp->RemoveRecentFile(path);
        RebuildRecentMenus();
    }
}

void QuincyFrame::OnSaveWorkSpace(wxCommandEvent& event)
{
    wxFileDialog * saveFileDialog = new wxFileDialog(this, wxT("Save workspace..."),
                                                    strCurrentDirectory, strWorkspace,
                                                    wxT("Workspace files|*.qws|All files|*"),
                                                    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog->ShowModal() != wxID_OK)
        return;
    strWorkspace = saveFileDialog->GetPath();
    int idx;
    if ((idx = strWorkspace.Find('.', true)) < 0 || strWorkspace.Mid(idx).Find(DIRSEP_CHAR) > 0)
        strWorkspace += wxT(".qws");

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
    static wxHtmlEasyPrinting print(wxT("Pawn Print-out"));

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit) {
        wxMessageBox(wxT("No file is selected (or opened)."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
        return;
    }

    print.SetParentWindow(this);
    for (int idx = 0; idx < MAX_EDITORS; idx++) {
        if (Editor[idx] == edit) {
            print.SetHeader(Filename[idx]);
            break;
        }
    }
    print.SetFooter(wxT("<center>@PAGENUM@ of @PAGESCNT@</center>"));
    int pointsizes[] = { 4, 6, 8, 10, 12, 14, 16 };
    print.SetFonts(wxT("helvetica"), wxT("courier"), pointsizes);
    print.PageSetup();
    wxPageSetupDialogData* options = print.GetPageSetupData();
    if (options->IsOk()) {
        wxString text = edit->GetText();
        text.Replace(wxT("&"), wxT("&amp;"));
        text.Replace(wxT("<"), wxT("&lt;"));
        text.Replace(wxT(">"), wxT("&gt;"));
        print.PreviewText(wxT("<pre>") + text + wxT("</pre"));
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
            int len = clip.Find(wxT('\n'));
            if (len < 0)
                len = (int)clip.Length();
            wxString sect;
            if (len > 0 && clip[len - 1] == wxT('\r'))
                sect = clip.Left(len - 1);
            else
                sect = clip.Left(len);
            clip = clip.Mid(len + 1);
            /* insert it in the editor */
            pos = edit->FindColumn(row, col);
            while (edit->GetColumn(pos) < col) {
                edit->InsertText(pos, wxT(" "));
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
            if (clip.Length() > 0 && clip[clip.Length() - 1] == wxT('\n')) {
                /* create an array of the lines */
                wxArrayString Lines;
                wxStringTokenizer tokenizer(clip, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);
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
                    for (indentDoc = 0; indentDoc < line.Length() && line[indentDoc] <= wxT(' '); indentDoc++)
                        /* nothing */;
                    if (indentDoc < line.Length()) {
                        if (line[indentDoc] == wxT('}'))
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
                        while (indent < line.Length() && line[indent] <= wxT(' '))
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
                    wxChar ch = wxT(' ');
                    if (theApp->GetUseTabs() && (delta % theApp->GetTabWidth()) == 0) {
                        ch = wxT('\t');
                        delta /= theApp->GetTabWidth();
                    }
                    for (unsigned row = 0; row < Lines.Count(); row++)
                        if (Lines[row].Length() > 0)
                            Lines[row].insert((size_t)0, delta, ch);
                }
                /* paste clip */
                clip.Empty();
                for (unsigned row = 0; row < Lines.Count() - 1; row++)
                    clip += Lines[row] + wxT("\n");
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
                if (dir.GetFirst(&fname, wxT("*"), wxDIR_FILES )) {
                    wxWindowDisabler *disableAll = new wxWindowDisabler;
                    #if wxCHECK_VERSION(3, 1, 0)
                        wxBusyInfo *info = new wxBusyInfo(
                            wxBusyInfoFlags()
                                .Parent(this)
                                .Icon(wxArtProvider::GetIcon(wxART_FIND))
                                .Title(wxT("Search ") + path)
                                .Text(wxT("Please wait..."))
                                .Foreground(*wxWHITE)
                                .Background(*wxBLACK)
                                .Transparency(4*wxALPHA_OPAQUE/5));
                    #else
                        /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier version */
                        wxBusyInfo *info = new wxBusyInfo(wxT("Please wait..."), this);
                    #endif
                    do {
                        /* check the file type (by verifing the extension) */
                        if (IsPawnFile(fname)) {
                            /* get the full path of the script/include file, verify whether it exists */
                            fname = path + wxT(DIRSEP_STR) + fname;
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
        FindDlg->Create(this, &FindData, wxT("Find..."));
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
                        int reply = wxMessageBox(wxT("No more matches.\nWrap back to the beginning?"), wxT("Pawn IDE"), wxYES_NO | wxICON_QUESTION);
                        if (reply == wxYES)
                            wrapsearch = true;
                    }
                    if (!wrapsearch) {
                        if (!foundmatch)
                            wxMessageBox(wxT("Search Text not found."), wxT("Pawn IDE"), wxOK | wxICON_INFORMATION);
                        else
                            wxMessageBox(wxT("No more matches found."), wxT("Pawn IDE"), wxOK | wxICON_INFORMATION);
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
        FindDlg->Create(this, &FindData, wxT("Replace..."), wxFR_REPLACEDIALOG);
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
            int reply = wxMessageBox(wxT("No more matches in this file.\nWrap back to the beginning?"), wxT("Pawn IDE"), wxYES_NO | wxICON_QUESTION);
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
            wxMessageBox(wxT("Search Text not found."), wxT("Pawn IDE"), wxOK | wxICON_INFORMATION);
        else if (no_next && warnnotfound)
            wxMessageBox(wxT("No more matches found."), wxT("Pawn IDE"), wxOK | wxICON_INFORMATION);
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
        root = SearchLog->AddRoot(wxT("root"));
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
        wxString string = wxString::Format(wxT("%4d: "), line) + linetext;
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
        wxMessageBox(wxT("Could not open \"") + filename + wxT("\"."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
    wxNumberEntryDialog dlg(this, wxT("Jump to a line in the current file."),
                            wxT("Line number"), wxT("Go to line..."),
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
        wxMessageBox(wxT("Symbol \"") + word + wxT("\"not found."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
                matches.Add(symbollist[idx]->Syntax + wxT(" - ") + symbollist[idx]->Source);
            }
            /* create a dialog that the user can choose from */
            static int dlgwidth = wxDefaultCoord;
            static int dlgheight = wxDefaultCoord;
            wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, wxT("This symbol appears in multiple source files."), wxT("Select source file"), matches);
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
        wxMessageBox(wxT("The file \"") + filename + wxT("\" no longer exists."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        wxMessageBox(wxT("Could not open \"") + filename + wxT("\"."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
    if (name.Length() > 1 && name[1] == wxT(':'))
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
    wxString text = wxGetTextFromUser(wxT("Text to insert/replace in the selection"),
                                      wxT("Fill selection"), wxEmptyString, this);
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
            edit->InsertText(pos, wxT(" "));
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
        int reply = wxMessageBox(wxT("Do you want to abort the running script?"), wxT("Pawn IDE"), wxYES_NO | wxICON_QUESTION);
        if (reply != wxYES)
            return;
        wxProcess::Kill(ExecPID, wxSIGTERM);
        wxProcess::Kill(ExecPID, wxSIGKILL);
        ExecPID = 0;
        DebugMode = false;
    }

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit) {
        wxMessageBox(wxT("No source file to compile."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
        return;
    }
    if (!SaveAllFiles(false))
        return;

    /* check the extension of the file (include files cannot be separately compiled) */
    unsigned page = EditTab->GetSelection();
    wxASSERT(page < MAX_EDITORS);
    wxString name = EditTab->GetPageText(page);
    wxString ext = name.AfterLast(wxT('.'));
    if (ext.CmpNoCase(wxT("i")) == 0 || ext.CmpNoCase(wxT("inc")) == 0) {
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
            wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, wxT("Please select a script to compile."), wxT("Select script"), scripts);
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
            wxMessageBox(wxT("This is not a script; cannot compile."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        wxMessageBox(wxT("Unknown filename for the source file to compile."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
        return;
    }
    CompileSource(fullpath);    /* status bar will show the compilation result */
}

void QuincyFrame::OnTransfer(wxCommandEvent& /* event */)
{
    if (ExecPID == 0 && DebuggerSelected == DEBUG_REMOTE) {
        wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
        if (!edit)
            return;

        wxString fullpath = wxEmptyString;
        int idx;
        for (idx = 0; idx < MAX_EDITORS && Editor[idx] != edit; idx++)
            /* nothing */;
        wxASSERT(idx < MAX_EDITORS);
        fullpath = Filename[idx];
        if (fullpath.Length() == 0)
            return;

        wxString amxname;
        if (strOutputPath.length() > 0) {
            wxString path, basename, ext;
            wxFileName::SplitPath(fullpath, &path, &basename, &ext);
            amxname = strOutputPath + wxT(DIRSEP_STR) + basename + wxT(".amx");
        } else {
            amxname = fullpath.BeforeLast(wxT('.')) + wxT(".amx");
        }

        wxString command = strCompilerPath + wxT(DIRSEP_STR) wxT("pawndbg") wxT(EXE_EXT);
        command += wxT(" ") + amxname;
        command += wxT(" -term=off,");
        command += debug_prefix;
        //??? halt the RS232 reception, if any
        command += wxT(" -rs232=") + DebugPort;
        command += wxString::Format(wxT(",%ld"), DebugBaudrate);
        command += wxT(" -transfer -quit");
        ExecPID = wxExecute(command, wxEXEC_ASYNC, ExecProcess);
        if (ExecPID <= 0) {
            wxMessageBox(wxT("Pawn debugger could not be started.\nPlease check the settings."),
                         wxT("Pawn IDE"), wxOK | wxICON_ERROR);
            delete ExecProcess;
            return;
        }
        PaneTab->SetSelection(TAB_OUTPUT);
        Terminal->Enable(true);
        Terminal->Clear();
        ExecInputQueue.Clear();
        DebugMode = true;
        DebugRunning = true;
        DebugHoldback = 0;
    }
}

void QuincyFrame::OnDebug(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    if (ExecPID != 0 && wxProcess::Exists(ExecPID) && DebugMode && !DebugRunning)
        SendDebugCommand(wxT("g"));
    else
        RunCurrentScript(true);
}

void QuincyFrame::OnRun(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    if (ExecPID != 0 && wxProcess::Exists(ExecPID) && DebugMode && !DebugRunning)
        SendDebugCommand(wxT("g"));
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
    SendDebugCommand(wxT("s"));
}

void QuincyFrame::OnStepOver(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    SendDebugCommand(wxT("n"));
}

void QuincyFrame::OnStepOut(wxCommandEvent& /* event */)
{
    LastWatchIndex = 0;
    SendDebugCommand(wxT("g func"));
}

void QuincyFrame::OnRunToCursor(wxCommandEvent& /* event */)
{
    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit)
        return;
    LastWatchIndex = 0;
    int line = edit->GetCurrentLine();
    wxString cmd = wxString::Format(wxT("g %d"), line + 1);
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
                        } while (ch != '\n' && (ch != ' ' || cmd.Cmp(wxT("dbg> ")) != 0));
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
            Terminal->AppendText(text);
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
        Terminal->AppendText(text);
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
        command.Replace(wxT("%fullname%"), script);
        command.Replace(wxT("%name%"), basename);
        command.Replace(wxT("%ext%"), basename);
        command.Replace(wxT("%path%"), path);
        wxArrayString output;
        long result = wxExecute(command, output, wxEXEC_SYNC);
        prebuild_error = (result < 0 || result >= 255);
    }

    /* check the compiler */
    wxString pgmname = wxT("pawncc") wxT(EXE_EXT);
    wxString command = strCompilerPath + wxT(DIRSEP_STR) + pgmname;
    if (!wxFileExists(command)) {
        QuincyDirPicker dlg(this, wxT("The Pawn compiler is not found in the configured folder.\nPlease choose the folder where the compiler is installed."), strCompilerPath, pgmname);
        if (dlg.ShowModal() == wxID_OK) {
            strCompilerPath = dlg.GetPath();
            /* remove trailing slash, if necessary */
            int len = strCompilerPath.Len();
            if (len > 0 && strCompilerPath[len - 1] == DIRSEP_CHAR)
                strCompilerPath = strCompilerPath.Left(len - 1);
            command = strCompilerPath + wxT(DIRSEP_STR) wxT("pawncc") wxT(EXE_EXT);
        }
    }
    if (!wxFileExists(command))
        return false;

    /* build the command */
    wxString options;
    options.Printf(wxT(" -d%d -O%d"), DebugLevel, OptimizationLevel);
    if (strTargetHost.length() > 0)
        options = wxT(" -T") + strTargetHost + options;
    if (strIncludePath.length() > 0)
        options += wxT(" -i") + strIncludePath;
    if (theApp->GetUseTabs())
        options += wxString::Format(wxT(" -t%d"), theApp->GetTabWidth());
    if (VerboseBuild)
        options += wxT(" -v");
    if (OverlayCode)
        options += wxT(" -V");
    if (strDefines.length() > 0)
        options += wxT(" ") + strDefines;

    wxString basename, path, ext;
    wxFileName::SplitPath(script, &path, &basename, &ext);
    if (strOutputPath.length() > 0)
        path = strOutputPath;
    if (path.Right(1) != wxT(DIRSEP_STR))
        path += wxT(DIRSEP_STR);
    wxString extraoptions = strMiscCmdOptions;
    int namepos = extraoptions.Find(wxT("-o"));
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
    }
    options += wxT(" ") + OptionallyQuoteString(wxT("-o") + path + basename + wxT(".amx"));

    if (CreateReport)
        options += wxT(" -r");
    extraoptions = extraoptions.Trim(false);
    if (extraoptions.length() > 0)
        options += wxT(" ") + extraoptions;
    options += wxT(" ") + OptionallyQuoteString(script);

    wxWindowDisabler *disableAll = new wxWindowDisabler;
    #if wxCHECK_VERSION(3, 1, 0)
        wxBusyInfo *info = new wxBusyInfo(
                            wxBusyInfoFlags()
                                .Parent(this)
                                .Icon(wxArtProvider::GetIcon(wxART_EXECUTABLE_FILE))
                                .Title(wxT("<b>Building ") + basename + wxT("</b>"))
                                .Text(wxT("Please wait..."))
                                .Foreground(*wxWHITE)
                                .Background(*wxBLACK)
                                .Transparency(4*wxALPHA_OPAQUE/5));
    #else
        /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier versions */
        wxBusyInfo *info = new wxBusyInfo(wxT("Please wait..."), this);
    #endif
    wxArrayString output;
    wxArrayString errors;
    long result = wxExecute(command + options, output, errors, wxEXEC_SYNC);
    delete disableAll;
    delete info;
    if (result < 0 || result >= 255) {
        wxMessageBox(wxT("Pawn compiler could not be started.\nPlease check the settings."),
                     wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        ErrorLog->InsertItem(0, wxT("Pre-build step return with an error"));
    ErrorLog->SetColumnWidth(0, wxLIST_AUTOSIZE);

    if (errors.Count() == 0 && !prebuild_error) {
        BuildLog->EnsureVisible(BuildLog->GetItemCount() - 1);
        PaneTab->SetSelection(TAB_BUILD);
        SetStatusText(wxT("Build completed successfully"), 0);
    } else {
        PaneTab->SetSelection(TAB_MESSAGES);
        wxString msg = wxString::Format(wxT("%d errors / warnings"), errors.Count());
        SetStatusText(msg, 0);
    }
    UpdateSymBrowser(script);   /* always update (even after errors) because we want to update after warnings */
    return errors.Count() == 0;
}

bool QuincyFrame::RunCurrentScript(bool debug)
{
    /* check whether already running */
    if (ExecPID != 0 && wxProcess::Exists(ExecPID)) {
        wxMessageBox(wxT("A script is already running."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
        return false;
    }
    ExecPID = 0;    /* if not running, preset to an invalid value */

    wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
    if (!edit) {
        wxMessageBox(wxT("No source file to run."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        wxMessageBox(wxT("Unknown filename for the source file to run."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
        return false;
    }

    wxString amxname;
    if (strOutputPath.length() > 0) {
        wxString path, basename, ext;
        wxFileName::SplitPath(fullpath, &path, &basename, &ext);
        /* check whether the file is a script (i.e. the extension is .p, .pwn or .pawn) */
        if (ext.CmpNoCase(wxT("p")) != 0 && ext.CmpNoCase(wxT("pawn")) != 0 && ext.CmpNoCase(wxT("pwn")) != 0) {
            wxString msg = wxT("The file \"") + basename + wxT(".") + ext + wxT("\" appears not to be a script.\nRun anyway?");
            int reply = wxMessageBox(msg, wxT("Pawn IDE"), wxYES_NO | wxICON_QUESTION);
            if (reply != wxYES)
                return false;
        }
        amxname = strOutputPath + wxT(DIRSEP_STR) + basename + wxT(".amx");
    } else {
        amxname = fullpath.BeforeLast(wxT('.')) + wxT(".amx");
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
        ModifiedPrompt = wxT("The source file must be compiled before it can run.\nDo you wish to compile it now?");
    }
    /* to be really sure, also check the time stamps of source and target */
    if (wxFileExists(fullpath)) {
        srctime = wxFileModificationTime(fullpath);
        if (amxtime < srctime)
            ModifiedPrompt = wxT("The current source file was modified since last build\nDo you wish to compile it first?");
    }

    if (ModifiedPrompt.length() > 0) {
        wxMessageDialog *dial = new wxMessageDialog(NULL, ModifiedPrompt, wxT("Script was modified"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
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
            wxMessageBox(wxT("The script cannot run, because it fails to load."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
            return false;
        }
        /* read size & signature of the AMX header */
        long size = 0;
        fread(&size, sizeof(size), 1, fp);
        unsigned short magic = 0;
        fread(&magic, sizeof(magic), 1, fp);
        if (size == 0 || (magic != AMX_MAGIC_16 && magic != AMX_MAGIC_32 && magic != AMX_MAGIC_64)) {
            wxMessageBox(wxT("Invalid or unsupported run-time file format."), wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
            wxMessageBox(wxT("No debug information is present.\nPlease rebuild the script\n(or run without debugging)."),
                         wxT("Pawn IDE"), wxOK | wxICON_ERROR);
            return false;
        }
    }

    /* create a process instance for redirected input and output */
    ExecProcess = new wxProcess(this);
    if (ExecProcess)
        ExecProcess->Redirect();

    wxString command = strCompilerPath + wxT(DIRSEP_STR);
    if (debug)
        command += wxT("pawndbg") wxT(EXE_EXT);
    else
        command += wxT("pawnrun") wxT(EXE_EXT);
    command += wxT(" ") + OptionallyQuoteString(amxname);
    if (debug) {
        command += wxT(" -term=off,");
        command += debug_prefix;
        if (DebuggerSelected == DEBUG_REMOTE) {
            //??? halt the RS232 reception, if any
            command += wxT(" -rs232=") + DebugPort;
            command += wxString::Format(wxT(",%ld"), DebugBaudrate);
        }
    }
    ExecPID = wxExecute(command, wxEXEC_ASYNC, ExecProcess);
    if (ExecPID <= 0) {
        wxMessageBox(wxT("Pawn run-time could not be started.\nPlease check the settings."),
                     wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
    if (cmd.Left(4).Cmp(wxT("file")) == 0) {
        DebugCurrentFile = cmd.Mid(4);
        DebugCurrentFile.Trim();
        DebugCurrentFile.Trim(false);
    } else if (cmd.Left(5).Cmp(wxT("watch")) == 0) {
        wxString text = cmd.Mid(5);
        wxStringTokenizer tokenizer(text, wxT(" \t\r\n"), wxTOKEN_STRTOK);
        wxString word = tokenizer.GetNextToken();
        word.ToLong(&LastWatchIndex);
        wxString name = tokenizer.GetNextToken();
        wxString value = tokenizer.GetNextToken();
        while (tokenizer.HasMoreTokens())
            value += wxT(" ") + tokenizer.GetNextToken();
        if (LastWatchIndex <= WatchLog->GetItemCount())
            WatchLog->SetItemText(LastWatchIndex - 1, name);
        else
            WatchLog->InsertItem(LastWatchIndex - 1, name);
        WatchLog->SetItem(LastWatchIndex - 1, 1, value);
        WatchLog->SetColumnWidth(0, wxLIST_AUTOSIZE);
        WatchLog->SetColumnWidth(1, wxLIST_AUTOSIZE);
    } else if (cmd.Left(3).Cmp(wxT("loc")) == 0 || cmd.Left(3).Cmp(wxT("glb")) == 0) {
        wxStyledTextCtrl *edit = GetActiveEdit(EditTab);
        if (edit) {
            wxString tip = cmd.Mid(3);
            tip.Trim(false);
            tip.Trim();
            tip = tip.AfterFirst(wxT('\t'));
            edit->CallTipShow(CalltipPos, tip);
        }
    } else if (cmd.Left(4).Cmp(wxT("info")) == 0) {
        wxString msg = cmd.Mid(4);
        msg.Trim(false);
        msg.Trim();
        SetStatusText(msg, 0);
    } else if (cmd.Left(4).Cmp(wxT("dbg>")) == 0) {
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
            wxMessageBox(wxT("Could not open ") + DebugCurrentFile, wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
            ostream->PutC(wxT('\r'));
        }
    }
    DebugRunning = true;
}

void QuincyFrame::SendWatchList()
{
    wxASSERT(WatchUpdateList.Count() > 0);
    long line = WatchUpdateList[0];
    WatchUpdateList.RemoveAt(0);    /* do not update again */
    wxString name = WatchLog->GetItemText(line);
    wxString cmd;
    if (name.length() > 0)
        cmd = wxString::Format(wxT("w %d "), line + 1) + name;
    else
        cmd = wxString::Format(wxT("cw %d"), line + 1);
    SendDebugCommand(cmd);
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
            BreakpointList.Add(Filename[idx] + wxString::Format(wxT(":%d"), line));
        }
    }
    BuiltBreakpoints = true;
}

void QuincyFrame::SendBreakpointList()
{
    /* before sending the first breakpoint, clear the entire stack */
    if (ChangedBreakpoints) {
        SendDebugCommand(wxT("cbreak *"));
        ChangedBreakpoints = false;
        BuiltBreakpoints = false;
    } else if (BreakpointList.Count() > 0) {
        wxString bp = BreakpointList[0];
        BreakpointList.RemoveAt(0);
        SendDebugCommand(wxT("break ") + bp);
    }
}

void QuincyFrame::PrepareSearchLog()
{
    if (theApp->SearchAdvanced) {
        if (!SearchLog) {
            wxASSERT(PaneTab->GetPageCount() == TAB_SEARCH);
            #if defined _WIN32
                wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New"));
            #else
                wxFont font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Monospace"));
            #endif
            SearchLog = new wxTreeCtrl(PaneTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_NO_LINES|wxTR_SINGLE|wxTR_DEFAULT_STYLE);
            SearchLog->SetFont(font);
            SearchLog->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(QuincyFrame::OnSearchSelect), NULL, this);
            PaneTab->AddPage(SearchLog, wxT("Search"), false); /* TAB_SEARCH */
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
    wxStringTokenizer tokenizer(orgtext, wxT('\n'), wxTOKEN_RET_DELIMS);
    while (tokenizer.HasMoreTokens()) {
        wxString line = tokenizer.GetNextToken();
        int pos = 0;
        for (unsigned idx = 0; idx < line.length(); idx++) {
            if (line[idx] == wxT('\t'))
                pos = ((pos / theApp->GetTabWidth()) + 1) * theApp->GetTabWidth();
            else
                pos += 1;
            if (line[idx] == wxT(' ') && pos % theApp->GetTabWidth() == 0) {
                /* see how many spaces we can remove */
                int start = idx;
                while (start > 0 && line[start - 1] == wxT(' '))
                    start--;
                wxASSERT(idx - start < (unsigned)theApp->GetTabWidth());
                /* we wish to avoid converting a single space between two words
                 * to a TAB (in fact, this cannot happen if converting indent
                 * spacing only)
                 */
                if (idx - start > 0) {
                    /* replace the subrange of spaces with a TAB */
                    line.erase(start, (idx - start + 1));
                    line.insert(start, wxT('\t'));
                    idx = start;    /* adjust current position and string length */
                    changed = true;
                }
            } else if (indent_only && line[idx] > wxT(' ')) {
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
    wxStringTokenizer tokenizer(orgtext, wxT('\n'), wxTOKEN_RET_DELIMS);
    while (tokenizer.HasMoreTokens()) {
        wxString line = tokenizer.GetNextToken();
        int pos;
        while ((pos = line.Find(wxT('\t'))) >= 0) {
            int spaces = theApp->GetTabWidth() - (pos % theApp->GetTabWidth());
            line.erase(pos, 1);
            line.insert(pos, spaces, wxT(' '));
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

void QuincyFrame::OnSampleBrowser(wxCommandEvent& /* event */)
{
    QuincySampleBrowser* dlg = new QuincySampleBrowser(this);
    if (dlg->ShowModal() == wxID_OK) {
        /* open the files on selection */
        for (unsigned idx = 0; idx < dlg->GetFileCount(); idx++) {
            wxString path = theApp->GetExamplesPath() + wxT(DIRSEP_STR);
            wxString host = GetTargetHost();
            if (host.Length() > 0)
                path += host + wxT(DIRSEP_STR);
            path += dlg->GetFile(idx);
            LoadSourceFile(path);
        }
    }
}

void QuincyFrame::OnAbout(wxCommandEvent& /* event */)
{
    wxIcon icon(Quincy48_xpm);

    wxAboutDialogInfo info;
    info.SetName(wxT("Pawn IDE"));
    info.SetVersion(wxT("0.6.") wxT(SVN_REVSTR));
    info.SetDescription(wxT("A tiny IDE for Pawn."));
    info.SetCopyright(wxT("(C) 2009-2017 ITB CompuPhase"));
    info.SetIcon(icon);
    info.SetWebSite(wxT("http://www.compuphase.com/pawn/"));
    wxAboutBox(info);
}

void QuincyFrame::OnHelp(wxCommandEvent& event)
{
    wxString filename;
    if (event.GetId() == wxID_HELP) {
        filename = theApp->GetDocPath() + wxT(DIRSEP_STR) + wxT("Quincy.pdf");
    } else if (menuHelp) {
        wxMenuItem *item = menuHelp->FindItem(event.GetId());
        if (item) {
            filename = item->GetItemLabelText();
            filename.Replace(wxT(" "), wxT("_"));
            wxString HostDocPath = theApp->GetDocPath();
            if (strTargetHost.Length() > 0)
                HostDocPath += wxT(DIRSEP_STR) + strTargetHost;
            filename = HostDocPath + wxT(DIRSEP_STR) + filename + wxT(".pdf");
        }
    }

    wxString Command;
    if (theApp->GetReaderPathValid()) {
        Command = theApp->GetReaderPath();
        /* remove page and label options */
        int idx;
        #if defined _WIN32
            idx = Command.Find(wxT('/'));
        #else
            idx = Command.Find(wxT('-'));
        #endif
        if (idx > 0)
            Command = Command.Left(idx);
        Command += wxT(" ") + filename;
    } else {
        #if wxCHECK_VERSION(2, 9, 0)
        #else
            wxTheMimeTypesManager->ReadMailcap(wxT("/etc/mailcap"));    /* for wxWidgets 2.8 */
        #endif
        wxFileType *FileType = wxTheMimeTypesManager->GetFileTypeFromMimeType(wxT("application/pdf"));
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
        label = wxT("at.") + word.Mid(1);
        filenames = HelpIndex->LookUp(label.utf8_str());
    }
    /* - if there are zero matches, give an error
       - if there is one match, use that
       - if there are more than one matches, pop up a selection dialog */
    wxString filename;
    int page = 0;
    if (filenames == NULL || filenames->size() == 0) {
        wxMessageBox(wxT("No help is available for \"") + word + wxT("\"."),
                     wxT("Pawn IDE"), wxOK | wxICON_ERROR);
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
        wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, wxT("This keyword appears in multiple documents. Please select the one to open."), wxT("Select document"), documents);
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
        if ((idx = Command.Find(wxT("{path}"))) >= 0)
            Command.Replace(wxT("{path}"), filename);
        else
            Command += wxT(" ") + filename;
        /* replace other options */
        Command.Replace(wxT("{page}"), wxString::Format(wxT("%d"), page));
        Command.Replace(wxT("{label}"), wxT("p.") + label);
    } else {
        #if wxCHECK_VERSION(2, 9, 0)
        #else
            wxTheMimeTypesManager->ReadMailcap(wxT("/etc/mailcap"));    /* for wxWidgets 2.8 */
        #endif
        wxFileType *FileType = wxTheMimeTypesManager->GetFileTypeFromMimeType(wxT("application/pdf"));
        Command = FileType->GetOpenCommand(filename);
        #if defined _WIN32
            Command += wxString::Format(wxT(" /A \"page=%d\""), page);  /* for Adobe Acrobat */
        #else
            Command += wxString::Format(wxT(" -p %d"), page);   /* for Evince and Okular */
        #endif
    }
    wxExecute(Command);
}

void QuincyFrame::OnEditorChange(wxStyledTextEvent& /* event */)
{
    if (!IgnoreChangeEvent) {
        SetStatusText(wxT("Source file changed since last compile"), 0);
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

    if (theApp->GetAutoIndent() && chr == wxT('\n')) {
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
                for (start = 0; start < text.Length() && text[start] <= wxT(' '); start++)
                    /* nothing */;
                for (stop = start + 1; stop < text.Length() && text[stop] > wxT(' ') && text[stop] != wxT('('); stop++)
                    /* nothing */;
                if (start < text.Length() && stop < text.Length()) {
                    wxString word = text.Mid(start, stop - start);
                    if (word.Cmp(wxT("if")) == 0 || word.Cmp(wxT("for")) == 0
                        || word.Cmp(wxT("while")) == 0 || word.Cmp(wxT("do")) == 0)
                    {
                        /* check the last character on the line */
                        text.Trim();
                        stop = text.Length();
                        wxASSERT(stop > 0);
                        if (text[stop - 1] == wxT(')') || text[stop - 1] == wxT('{'))
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
    } else if (chr == wxT('}') && theApp->GetAutoIndent() && edit->GetLexer() == wxSTC_LEX_CPP) {
        /* test whether there is any text on the line */
        int currentLine = edit->GetCurrentLine();
        wxString text = edit->GetLine(currentLine);
        unsigned idx, count = 0;
        for (idx = 0; idx < text.Length() && (text[idx] <= wxT(' ') || text[idx] == wxT('}')) && count <= 1; idx++)
            if (text[idx] == wxT('}'))
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
    if ((PendingFlags & PEND_INDENTBRACE) && pos > 0 && edit->GetCharAt(pos - 1) == wxT('}')) {
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
    wxString field = wxString::Format(wxT("Line %d Col %d"), line + 1, col + 1);
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
    if (!isalnum(text[col]) && text[col] != wxT('_') && text[col] != wxT('@'))
        return;     /* not pointing at a word */
    int start = col, end = col;
    while (start > 0 && (isalnum(text[start - 1]) || text[start] == wxT('_') || text[start] == wxT('@')))
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
        int bold = tip.Find(wxT("\n"));
        if (bold > 0)
            edit->CallTipSetHighlight(0, bold);
    } else if (ExecPID != 0 && DebugMode && !DebugRunning) {
        /* if debugging and waiting at a prompt, send a command to show the value */
        SendDebugCommand(wxT("d ") + word);
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
    if (!isalnum(text[col]) && text[col] != wxT('_') && text[col] != wxT('@') && text[col] != wxT('.'))
        return;     /* not pointing at a word */
    int start = col;
    while (start > 0 && (isalnum(text[start - 1]) || text[start - 1] == wxT('_') || text[start - 1] == wxT('@') || text[start - 1] == wxT('/') || text[start - 1] == wxT('*')))
        start--;
    int length = col - start + 1;
    if (length == 0)
        return;     /* not a word */
    wxString prefix = text.Mid(start, length);

    bool dotfield = (start > 0 && text[start - 1] == wxT('.'));
    if (!dotfield && prefix.Cmp(wxT(".")) == 0) {
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
        if (text[start] == wxT(',')) {
            /* function parameter, but other parameters come before this one */
            /* move back lines as long as a line ends with a comma */
            while (line > 0) {
                wxString temp = edit->GetLine(line - 1);
                temp.Trim();
                size_t len = temp.Length();
                if (len > 0 && temp[len - 1] == wxT(',')) {
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
                while (start < (int)text.Length() && !isalnum(text[start]) && text[start] != wxT('_') && text[start] != wxT('@'))
                    start++;
                end = start;
                while (end < (int)text.Length() && (isalnum(text[end]) || text[end] == wxT('_') || text[end] == wxT('@')))
                    end++;
                contextsymbol = text.Mid(start, end - start);
            } while (contextsymbol.Cmp(wxT("if")) == 0 || contextsymbol.Cmp(wxT("for")) == 0
                     || contextsymbol.Cmp(wxT("while")) == 0 || contextsymbol.Cmp(wxT("do")) == 0);
        } else {
            if (text[start] == wxT('(')) {
                /* function parameter (first in the list), skip '(' */
                start--;
                while (start > 0 && isspace(text[start - 1]))
                    start--;
            }
            int end = start;
            while (start > 0 && (isalnum(text[start - 1]) || text[start - 1] == wxT('_') || text[start - 1] == wxT('@')))
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
            case wxT('\n'):
                edit->NewLine();
                break;
            case wxT('\t'):
                edit->Tab();
                break;
            case wxT('\v'):
                edit->BackTab();
                break;
            case wxT('^'):
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
            if ((int)name.Length() > length + 2 && name[1] == wxT(':')) {
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
                    definition = definition.BeforeFirst(wxT('\n'));
                }
            }
        }
        /* go through the definition to find the matching parameters */
        start = definition.Find(wxT('('));
        wxASSERT(start > 0 && start < (int)definition.Length());
        for ( ;; ) {
            start++;    /* skip '(' or ',' */
            while (start < (int)definition.Length() && (definition[start] <= wxT(' ') || definition[start] <= wxT('&')))
                start++;
            if (start >= (int)definition.Length())
                break;
            for (pos = start; pos < (int)definition.Length() && (isalnum(definition[pos]) || definition[pos] == wxT('_')); pos++)
                /* nothing */;
            if (pos > start) {
                wxString param = definition.Mid(start, pos - start);
                if (param.Cmp(wxT("const")) == 0) {
                    start = pos;
                    continue;
                }
                if (length == 0 || param.Left(length).Cmp(prefix) == 0)
                    list.Add(param);
            }
            for (start = pos; start < (int)definition.Length() && definition[start] != wxT(','); start++)
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
            if ((int)name.Length() > length + 2 && name[1] == wxT(':')) {
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
            wxChar nxt = (idx + 1 < textlen) ? fulltext[idx + 1] : wxT('\0');
            if (cur == wxT('/') && nxt == wxT('/')) {
                /* wipe out until '\r' or '\n' */
                while (idx < textlen && fulltext[idx] != wxT('\r') && fulltext[idx] != wxT('\n'))
                    fulltext[idx++] = wxT(' ');
                if (idx < textlen && fulltext[idx] != wxT('\n'))
                    fulltext[idx++] = wxT(' ');
            } else if (cur == wxT('/') && nxt == wxT('*')) {
                fulltext[idx++] = wxT(' '); /* erase '/*' */
                fulltext[idx++] = wxT(' ');
                wxChar prv = wxT('\0');
                while (idx < textlen && (fulltext[idx] != wxT('/') || prv != wxT('*'))) {
                    prv = fulltext[idx];
                    fulltext[idx++] = wxT(' ');
                }
                if (idx < textlen)
                    fulltext[idx++] = wxT(' '); /* also erase final '/' the comment */
            } else if (cur == wxT('"') || cur == wxT('\'')) {
                fulltext[idx++] = wxT(' ');     /* erase initial '"' */
                while (idx < textlen && fulltext[idx] != cur) {
                    if (idx + 1 < textlen && fulltext[idx] == wxT('\\'))
                        fulltext[idx++] = wxT(' '); /* erase '\' plus the character following it */
                    fulltext[idx++] = wxT(' ');
                }
                if (idx < textlen)
                    fulltext[idx++] = wxT(' '); /* also erase final '"' */
            } else {
                idx++;  /* none of the above: normal character */
            }
        }
        wxStringTokenizer tokenizer(fulltext, wxT(" \t\r\n()[]{}<>`~!#$%^&*-+|\\;:'\",./?"), wxTOKEN_STRTOK);
        while (tokenizer.HasMoreTokens()) {
            wxString word = tokenizer.GetNextToken();
            /* only include tokens that are alphanumeric (but start alphabetic) */
            if (!isalpha(word[0]) && word[0] != wxT('_') && word[0] != wxT('@'))
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
            items += wxT("|");
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
    bool enable_run;
    if (enable_abort && DebugMode && DebugRunning)
        enable_run = !DebugRunning;
    else
        enable_run = RunTimeEnabled;
    bool enable_transfer = !enable_abort
                           && (DebuggerEnabled & DEBUG_REMOTE) != 0
                           && DebuggerSelected == DEBUG_REMOTE;
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
        if (dir.GetFirst(&fname, wxT("*.xml"), wxDIR_FILES )) {
            do {
                /* get the full path of the matching PDF file, verify whether it exists */
                fname = path + wxT(DIRSEP_STR) + fname;
                wxASSERT(wxFileExists(fname));
                if (SymbolList.LoadReportFile(fname))
                    count += 1;
            } while (dir.GetNext(&fname));
        }
        result = (count > 0);
    } else {
        result = SymbolList.LoadReportFile(filename.BeforeLast(wxT('.')) + wxT(".xml"));
    }

    wxASSERT(BrowserTree);
    if (result) {
        BrowserTree->DeleteAllItems();
        wxTreeItemId root = BrowserTree->AddRoot(wxT("root"));
        wxTreeItemId sectionConstants = BrowserTree->AppendItem(root, wxT("Constants"));
        wxTreeItemId sectionGlobals = BrowserTree->AppendItem(root, wxT("Global variables"));
        wxTreeItemId sectionFunctions = BrowserTree->AppendItem(root, wxT("Functions"));
        wxTreeItemId section;
        for (const CSymbolEntry* item = SymbolList.Root(); item; item = item->Next) {
            wxASSERT(item->SymbolName[1] == wxT(':'));
            if (item->SymbolName[0] == wxT('C'))
                section = sectionConstants;
            else if (item->SymbolName[0] == wxT('F'))
                section = sectionGlobals;
            else if (item->SymbolName[0] == wxT('M'))
                section = sectionFunctions;
            else
                continue;
            BrowserTree->AppendItem(section, item->Syntax + wxT(" - ") + item->Source, -1, -1, new BrowserItemData(item));
        }
    } else {
        if (BrowserTree->GetCount() == 0) {
            wxTreeItemId root = BrowserTree->AddRoot(wxT("root"));
            BrowserTree->AppendItem(root, wxT("No symbols loaded"));
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
        pathname += wxT(DIRSEP_STR) + strTargetHost;
    pathname += wxT(DIRSEP_STR) wxT("infotips.lst");
    /* the target host name may be upper case while the direcory name is lower case */
    if (!wxFileExists(pathname) && strTargetHost.Length() > 0) {
        wxString name = strTargetHost;
        pathname = theApp->GetDocPath() + wxT(DIRSEP_STR) + name.MakeLower() + wxT(DIRSEP_STR) wxT("infotips.lst");
    }

    wxTextFile flst;
    if (!flst.Open(pathname))
        return false;
    for (long idx = 0; idx < (long)flst.GetLineCount(); idx++) {
        wxString line = flst.GetLine(idx);
        line = line.Trim(false);
        line = line.Trim(true);
        if (line.length() != 0 && line[0] != wxT('#')) {
            /* get the keyword from the line */
            int namelength;
            int openparen = line.Find(wxT('('));
            if (line[0] == '@') {
                namelength = line.Find(wxT(')'));   /* use complete function definition for public functions */
                if (namelength > 0)
                    namelength++;
            } else {
                namelength = openparen; /* use only the function name */
            }
            if (namelength < 0) {
                namelength = line.Find(wxT(' '));
                if (namelength > 0 && line[namelength - 1] == wxT(':'))
                    namelength += line.Mid(namelength + 1).Find(wxT(' ')) + 1;
            }
            if (namelength > 0) {
                /* remove the tag name in front of the function name */
                wxString keyword;
                int skip = line.Find(wxT(':'));
                if (skip >= 0 && skip < openparen) {
                    while (line[++skip] == wxT(' '))
                        /* nothing */;
                } else {
                    skip = 0;
                }
                keyword = line.Mid(skip, namelength - skip);
                /* reformat the line somewhat */
                if (line[namelength] == wxT('(')) {
                    int closing = line.Mid(namelength).Find(')');
                    if (closing > 0)
                        namelength += closing + 1;
                }
                wxString def = line.Left(namelength);
                wxString descr = line.Mid(namelength).Trim(false);
                /* add it to the list */
                InfoTipList.insert(std::make_pair(keyword, def + wxT("\n") + descr));
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
            if (key[0] == wxT('@')) {
                int paren = key.Find(wxT('('));
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
        if (sym->SymbolName.Length() > 1 && sym->SymbolName[1] == wxT(':')
            && keyword.Cmp(sym->SymbolName.Mid(2)) == 0 && sym->Syntax.Length() > 0)
        {
            if (((flags & TIP_FUNCTION) && sym->SymbolName[0] == 'M')
                || ((flags & TIP_VARIABLE) && sym->SymbolName[0] == 'F')
                || ((flags & TIP_CONSTANT) && sym->SymbolName[0] == 'C'))
            {
                wxString item = sym->Syntax;
                if (sym->Summary.Length() > 0)
                    item += wxT("\n") + sym->Summary;
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
        HostDocPath += wxT(DIRSEP_STR) + strTargetHost;
    wxDir dir(HostDocPath);
    if (!dir.IsOpened())
        return;
    wxString filename;
    if (dir.GetFirst(&filename, wxT("*.aux"), wxDIR_FILES )) {
        int count = 0;
        do {
            /* get the full path of the matching PDF file, verify whether it exists */
            wxString PDFFile = HostDocPath + wxT(DIRSEP_STR) + filename.BeforeLast(wxT('.')) + wxT(".pdf");
            if (!wxFileExists(PDFFile))
                continue;   /* not found, skip this file */
            /* process the index */
            wxString AuxFile = HostDocPath + wxT(DIRSEP_STR) + filename;
            HelpIndex->ScanFile(AuxFile.utf8_str(), IDM_HELP1 + count, PDFFile.utf8_str());
            /* set this in the menu */
            if (menuHelp) {
                filename = filename.BeforeLast(wxT('.'));
                filename.Replace(wxT("_"), wxT(" "));
                menuHelp->Insert(count + 1, IDM_HELP1 + count, filename);
                Connect(IDM_HELP1 + count, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuincyFrame::OnHelp));
            }
            count += 1;
        } while (dir.GetNext(&filename) && count < MAX_HELPFILES);
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
        re.Compile(wxT("^[[:blank:]]*(static[[:blank:]]+|)([A-Za-z@_][A-Za-z@_0-9]*:[[:blank:]]*|)([A-Za-z@_][A-Za-z@_0-9]*)[[:blank:]]*\\("), wxRE_EXTENDED);

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
            if (line.Left(12).Cmp(wxT("timer_second"))==0)
                lastline = idx + 1;
            /* strip line comments */
            int pos;
            if ((pos = line.Find(wxT("//"))) >= 0)
                line = line.Left(pos);
            if (nestlevel == 0 && !incomment && re.Matches(line)) {
                context = re.GetMatch(line, 3);
                topline = idx;
                inparamlist = true;
            }
            bool instring = false;
            bool escaped = false;
            for (unsigned c = 0; c < line.Length(); c++) {
                if (line[c] == wxT('{') && !incomment && !instring && !inparamlist) {
                    if (nestlevel == 0 && context.Length() > 0) {
                        WorkNames.Add(context);
                        wxASSERT(topline >= 0);
                    }
                    nestlevel++;
                } else if (line[c] == wxT('}') && !incomment && !instring && !inparamlist) {
                    nestlevel--;
                    if (nestlevel < 0)
                        nestlevel = 0;
                    if (nestlevel == 0  && context.Length() > 0) {
                        wxASSERT(topline >= 0);
                        btmline = idx;
                        WorkRanges.Add((long)topline | ((long)btmline << 16));
                        wxASSERT(WorkNames.Count() == WorkRanges.Count());
                        context = wxEmptyString;
                        topline = -1;
                    }
                } else if (line[c] == wxT(')') && !incomment && !instring) {
                    inparamlist = false;
                } else if (line[c] == wxT('"') && !incomment && !escaped) {
                    instring = !instring;
                } else if (line[c] == wxT('/') && (c + 1) < line.Length() && line[c + 1] == wxT('*') && !instring) {
                    incomment = true;
                } else if (line[c] == wxT('*') && (c + 1) < line.Length() && line[c + 1] == wxT('/') && !instring) {
                    incomment = false;
                }
                if (line[c] == wxT('\\'))
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
            int newsel = (newidx >= 0) ? choicectrl->FindString(Names[newidx]) : wxNOT_FOUND;
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