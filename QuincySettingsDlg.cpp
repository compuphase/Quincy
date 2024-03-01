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
 *  distributed under the License is distributed on an "AS IS" basis, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 *
 *  Version: $Id: QuincySettingsDlg.cpp 7113 2024-02-25 21:29:31Z thiadmer $
 */
#include <wx/busyinfo.h>
#include <wx/sstream.h>
#include <wx/url.h>
#include <wx/variant.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include "wxQuincy.h"
#include "QuincySettingsDlg.h"
#include "portscan.h"

QuincySettingsDlg::QuincySettingsDlg(wxWindow* parent)
    : SettingsDlg(parent)
{
    Parent = static_cast<QuincyFrame*>(parent);
    NeedRestart = false;
    InitData();
}

void QuincySettingsDlg::OnCancel(wxCommandEvent& /* event */)
{
    EndModal(wxID_CANCEL);
}

void QuincySettingsDlg::OnOK(wxCommandEvent& /* event */)
{
    CopyData();
    if (NeedRestart)
        wxMessageBox(wxT("The new settings will take effect on the next start of the Pawn IDE."));
    EndModal(wxID_OK);
}

void QuincySettingsDlg::OnTargetHost(wxCommandEvent& /* event */)
{
    wxString host = m_TargetHost->GetStringSelection();

    /* see whether the list must be refreshed */
    if (host.Cmp(wxT("(install target)")) == 0) {
        /* first check for write permissions in the "targets" directory, if you
           cannot write, the configuration must be manually installed */
        wxString dir = Parent->GetTargetPath();
        if (!wxDirExists(dir)) {
            #if defined _MSC_VER && wxMAJOR_VERSION < 3
                wxMkDir(dir);
            #else
                wxMkDir(dir.utf8_str(), 0777);
            #endif
        }
        wxFileName tgtdir = wxFileName::DirName(dir, wxPATH_NATIVE);
        if (!tgtdir.IsOk() || !tgtdir.DirExists() || !tgtdir.IsDirWritable()) {
            wxMessageBox(wxT("No access rights to install new target hosts in\n")
                         wxT("the installation directory.\n")
                         wxT("Run the Pawn IDE as administrator (root).\n"),
                         wxT("Pawn IDE"), wxOK | wxICON_ERROR);
            host = wxEmptyString;
            m_TargetHost->SetSelection(0);
        } else {
            #if wxCHECK_VERSION(3, 1, 0)
                wxBusyInfo *info = new wxBusyInfo(
                                    wxBusyInfoFlags()
                                        .Parent(this)
                                        .Icon(wxArtProvider::GetIcon(wxART_EXECUTABLE_FILE))
                                        .Title(wxT("<b>Collecting supported targets</b>"))
                                        .Text(wxT("Please wait..."))
                                        .Foreground(*wxWHITE)
                                        .Background(*wxBLACK)
                                        .Transparency(4*wxALPHA_OPAQUE/5));
            #else
                /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier versions */
                wxBusyInfo *info = new wxBusyInfo(wxT("Please wait..."), this);
            #endif
            wxFileSystem fs;
            wxArrayString targets;
            wxString path = theApp->GetUpdateURL() + wxT("quincy_targets.txt");
            //??? wxWebRequest 
            wxFSFile* file = fs.OpenFile(path);
            if (file) {
                wxInputStream *in = file->GetStream();
                if (in && in->IsOk()) {
                    wxString htmldata;
                    wxStringOutputStream html_stream(&htmldata);
                    in->Read(html_stream);
                    while (htmldata.Length() > 0) {
                        wxString line = htmldata.BeforeFirst(wxT('\n'));
                        line.Trim();
                        targets.Add(line);
                        htmldata = htmldata.AfterFirst(wxT('\n'));
                    }
                    delete in;
                }
            }
            delete info;
            if (targets.Count() == 0) {
                wxMessageBox(wxT("No targets found on server ") + theApp->GetUpdateURL() + wxT("\nPlease check your internet connection."),
                             wxT("Pawn IDE"), wxOK | wxICON_ERROR);
                host = wxEmptyString;
                m_TargetHost->SetSelection(0);
            } else {
                wxSingleChoiceDialog *dlg = new wxSingleChoiceDialog(this, wxT("Select the target host to download and install."), wxT("Select target host"), targets);
                if (dlg->ShowModal() != wxID_OK) {
                    host = wxEmptyString;
                    m_TargetHost->SetSelection(0);
                } else {
                    /* get the host name before showing the new busy box */
                    int idx = dlg->GetSelection();
                    host = targets[idx];
                    host = host.BeforeFirst(wxT('\t'));
                    host = host.BeforeFirst(wxT(' '));
                    #if wxCHECK_VERSION(3, 1, 0)
                        wxBusyInfo *info = new wxBusyInfo(
                                            wxBusyInfoFlags()
                                                .Parent(this)
                                                .Icon(wxArtProvider::GetIcon(wxART_EXECUTABLE_FILE))
                                                .Title(wxT("<b>Installing ") + host + wxT("</b>"))
                                                .Text(wxT("Please wait..."))
                                                .Foreground(*wxWHITE)
                                                .Background(*wxBLACK)
                                                .Transparency(4*wxALPHA_OPAQUE/5));
                    #else
                        /* wxBusyInfoFlags was introduced in 3.1, fall back to uglier boxes on earlier versions */
                        wxBusyInfo *info = new wxBusyInfo(wxT("Please wait..."), this);
                    #endif
                    /* download the target host file */
                    path = path.BeforeLast(wxT('/')) + wxT("/") + host + wxT(".zip");   /* assume that if the index file is redirected, the host files are too */
                    file = fs.OpenFile(path);
                    path = theApp->GetUserDataPath() + wxT(DIRSEP_STR) + host + wxT(".zip");
                    if (file) {
                        wxInputStream *in = file->GetStream();
                        if (in && in->IsOk()) {
                            wxRemoveFile(path); /* delete it, because otherwise the download may fail */
                            wxFileOutputStream bin_stream(path);
                            in->Read(bin_stream);
                            delete in;
                        }
                    }
                    /* make/check the directory structure */
                    wxString docdir = theApp->GetDocPath() + wxT(DIRSEP_STR) + host;
                    if (!wxDirExists(docdir)) {
                        #if defined _MSC_VER && wxMAJOR_VERSION < 3
                            wxMkDir(docdir);
                        #else
                            wxMkDir(docdir.utf8_str(), 0777);
                        #endif
                    }
                    wxString sampledir = theApp->GetExamplesPath() + wxT(DIRSEP_STR) + host;
                    if (!wxDirExists(sampledir)) {
                        #if defined _MSC_VER && wxMAJOR_VERSION < 3
                            wxMkDir(sampledir);
                        #else
                            wxMkDir(sampledir.utf8_str(), 0777);
                        #endif
                    }
                    /* Parent->GetTargetPath() was already made at the start of
                       this routine; also create a string with the path to the
                       system include files */
                    wxString incdir = Parent->GetTargetPath();
                    wxASSERT(incdir.AfterLast(DIRSEP_CHAR).Cmp(wxT("target")) == 0);
                    incdir = incdir.BeforeLast(DIRSEP_CHAR) + wxT(DIRSEP_STR) wxT("include") wxT(DIRSEP_STR) + host;
                    if (!wxDirExists(incdir)) {
                        #if defined _MSC_VER && wxMAJOR_VERSION < 3
                            wxMkDir(incdir);
                        #else
                            wxMkDir(incdir.utf8_str(), 0777);
                        #endif
                    }
                    /* unpack the downloaded file */
                    wxFileInputStream zipfile(path);
                    if (zipfile.IsOk()) {
                        wxZipInputStream zip(zipfile);
                        wxZipEntry* entry;
                        while ((entry = zip.GetNextEntry()) != NULL) {
                            if (!entry->IsDir()) {
                                zip.OpenEntry(*entry);
                                if (zip.CanRead()) {
                                    /* since the files need not be stored to the
                                       same root location, we strip the paths and
                                       select the correct one */
                                    wchar_t DirSep = wxT('/');                  /* ZIP spec. uses slash... */
                                    dir = entry->GetName().BeforeFirst(DirSep);
                                    if (dir.Find(wxT('\\')) != wxNOT_FOUND) {   /* ...but some implementations differ */
                                        DirSep = wxT('\\');
                                        dir = entry->GetName().BeforeFirst(DirSep);
                                    }
                                    if (dir.CmpNoCase(wxT("target")) == 0)
                                        dir = Parent->GetTargetPath();
                                    else if (dir.CmpNoCase(wxT("include")) == 0)
                                        dir = incdir;
                                    else if (dir.CmpNoCase(wxT("doc")) == 0)
                                        dir = docdir;
                                    else if (dir.CmpNoCase(wxT("examples")) == 0)
                                        dir = sampledir;
                                    wxString name = dir + wxT(DIRSEP_STR) + entry->GetName().AfterLast(DirSep);
                                    wxFileOutputStream file(name);
                                    if (file.IsOk())
                                        zip.Read(file);
                                }
                            }
                        }
                    }
                    delete info;
                } /* if (wxChoiceDialog() confirmed) */
                /* check whether the main configuration file exists */
                path = Parent->GetTargetPath() + wxT(DIRSEP_STR) + host + wxT(".cfg");
                if (!wxFileExists(path)) {
                    wxMessageBox(wxT("Failed to install the target host ") + host + wxT("\nPlease check that you have the correct permissions."),
                                 wxT("Pawn IDE"), wxOK | wxICON_ERROR);
                    host = wxEmptyString;
                    m_TargetHost->SetSelection(0);
                } else {
                    /* update the combo-box list, set the combo-box selection to
                       the target just downloaded */
                    CollectTargetHosts();
                    int sel = m_TargetHost->FindString(host);
                    m_TargetHost->SetSelection(sel >= 0 ? sel : 0);
                }
            } /* if (target file downloaded) */
        } /* if (IsDirWritable()) */
    } /* if (install target) */

    /* immediately load the target host, so that other settings may change
       according to the host capabilities */
    if (host.Cmp(wxT("-")) == 0)
        host = wxEmptyString;
    Parent->LoadHostConfiguration(host);
    m_DebugLevel->SetSelection(Parent->GetDefaultDebugLevel());
    m_Optimization->SetSelection(Parent->GetDefaultOptimize());
    m_OverlayCode->Enable(Parent->GetOverlayEnabled());
    if (!Parent->GetOverlayEnabled())
        m_OverlayCode->SetValue(false);
    m_chkStandardAMXname->SetValue(Parent->GetFixedAMXName().length() > 0);
    m_chkStandardAMXname->Enable(Parent->GetFixedAMXName().length() > 0);

    /* may also need to enable/disable local/remote debugger options */
    m_optDebugLocal->Enable(Parent->GetDebuggerEnabled(DEBUG_LOCAL));
    if (!Parent->GetDebuggerEnabled(DEBUG_LOCAL))
        m_optDebugLocal->SetValue(false);
    m_optDebugRS232->Enable(Parent->GetDebuggerEnabled(DEBUG_REMOTE));
    if (!Parent->GetDebuggerEnabled(DEBUG_REMOTE))
        m_optDebugRS232->SetValue(false);
    m_chkAutoTransfer->Enable(Parent->GetTransferEnabled());
    if (!Parent->GetTransferEnabled())
        m_chkAutoTransfer->SetValue(false);
}

void QuincySettingsDlg::OnDebuggerLocal(wxCommandEvent& /* event */)
{
    m_ctrlPort->Enable(false);
    m_ctrlBaudRate->Enable(false);
    m_chkEnableLogging->Enable(false);
}

void QuincySettingsDlg::OnDebuggerRS232(wxCommandEvent& /* event */)
{
    m_ctrlPort->Enable(true);
    m_ctrlBaudRate->Enable(true);
    m_chkEnableLogging->Enable(true);
}

void QuincySettingsDlg::OnSnippetEdit(wxGridEvent& event)
{
    int row = event.GetRow();
    if (row + 1 == m_gridSnippets->GetNumberRows())
        m_gridSnippets->AppendRows();   /* user changed the bottom row, add a new bottom row */
    //m_gridSnippets->AutoSizeColumns(false);   /* apparently causes recursion (and a crash) */
}

void QuincySettingsDlg::OnKbdChanged(wxPropertyGridEvent& event)
{
    NeedRestart = true;
}

void QuincySettingsDlg::OnUserPDFReader(wxCommandEvent& /* event */)
{
    bool enable = m_UserPDFReader->GetValue();
    m_UserReaderPath->Enable(enable);
    m_UserReaderBrowse->Enable(enable);
}

void QuincySettingsDlg::OnUserReaderBrowse(wxCommandEvent& /* event */)
{
    wxString filter;
    #if defined _WIN32
        filter = wxT("Applications (*.exe)|*.exe");
    #else
        filter = wxT("Applications|*");
    #endif
    wxFileDialog FileDialog(this, wxT("Select PDF reader"), wxT(""), wxT(""),
                            filter, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (FileDialog.ShowModal() == wxID_OK)
        m_UserReaderPath->SetValue(FileDialog.GetPath());
}

void QuincySettingsDlg::CollectTargetHosts()
{
    /* accumulate the list of target hosts */
    wxArrayString list;
    wxDir dir(Parent->GetTargetPath());
    if (dir.IsOpened()) {
        wxString filename;
        bool cont = dir.GetFirst(&filename, wxT("*.cfg"), wxDIR_FILES);
        while (cont) {
            /* skip names that start with "uncrustify_", also ignore "default.cfg" */
            wxString prefix = filename.Left(11);
            if (prefix.CmpNoCase(wxT("uncrustify_")) != 0 && filename.CmpNoCase(wxT("default.cfg")) != 0) {
                /* remove extension */
                int len = filename.Length();
                filename = filename.Left(len - 4);
                list.Add(filename);
            }
            cont = dir.GetNext(&filename);
        }
    }
    list.Sort();

    /* refresh the combo-box list */
    m_TargetHost->Clear();
    m_TargetHost->Append(wxT("-"));
    for (size_t idx = 0; idx < list.Count(); idx++)
        m_TargetHost->Append(list[idx]);
    m_TargetHost->Append(wxT("(install target)"));
}

void QuincySettingsDlg::InitData()
{
    /* accumulate the list of target hosts */
    CollectTargetHosts();
    int sel = m_TargetHost->FindString(Parent->GetTargetHost());
    m_TargetHost->SetSelection(sel >= 0 ? sel : 0);

    m_DebugLevel->SetSelection(Parent->GetDebugLevel());
    m_Optimization->SetSelection(Parent->GetOptimizationLevel());
    m_OverlayCode->SetValue(Parent->GetOverlayCode());
    m_OverlayCode->Enable(Parent->GetOverlayEnabled());
    m_chkStandardAMXname->SetValue(Parent->HasFixedAMXName());
    m_chkStandardAMXname->Enable(Parent->GetFixedAMXName().length() > 0);
    m_chkVerbose->SetValue(Parent->GetVerboseBuild());
    m_chkAutoTransfer->Enable(Parent->GetTransferEnabled());
    m_chkAutoTransfer->SetValue(Parent->GetAutoTransferEnabled());
    m_chkCreateReport->SetValue(Parent->GetCreateReport());
    m_Defines->SetValue(Parent->GetDefineString());
    m_IncludePath->SetValue(Parent->GetIncludePath());
    m_BinPath->SetValue(Parent->GetCompilerPath());
    m_OutputPath->SetValue(Parent->GetOutputPath());
    m_PreBuild->SetValue(Parent->GetPreBuild());
    m_MiscCmdOptions->SetValue(Parent->GetMiscCmdOptions());

    m_TabWidth->SetValue(theApp->GetTabWidth());
    m_UseTabs->SetValue(!theApp->GetUseTabs());
    m_AutoIndent->SetValue(theApp->GetAutoIndent());
    m_IndentPastedBlocks->SetValue(theApp->GetIndentBlocks());
    m_EditFont->SetSelectedFont(theApp->CodeFont);

    m_clrBackground->SetColour(theApp->EditColours[CLR_BACKGROUND]);
    m_clrNormalText->SetColour(theApp->EditColours[CLR_TEXT]);
    m_clrKeywords->SetColour(theApp->EditColours[CLR_KEYWORDS]);
    m_clrStrings->SetColour(theApp->EditColours[CLR_STRINGS]);
    m_clrStringEOL->SetColour(theApp->EditColours[CLR_STRINGEOL]);
    m_clrHighlight->SetColour(theApp->EditColours[CLR_HIGHLIGHT]);
    m_clrActiveLine->SetColour(theApp->EditColours[CLR_ACTIVE]);
    m_clrComments->SetColour(theApp->EditColours[CLR_COMMENTS]);
    m_clrOperators->SetColour(theApp->EditColours[CLR_OPERATORS]);
    m_clrNumbers->SetColour(theApp->EditColours[CLR_NUMBERS]);
    m_clrPreprocessor->SetColour(theApp->EditColours[CLR_PREPROCESSOR]);

    m_optDebugLocal->SetValue(Parent->GetDebuggerSelected() == DEBUG_LOCAL && Parent->GetDebuggerEnabled(DEBUG_LOCAL));
    m_optDebugRS232->SetValue(Parent->GetDebuggerSelected() == DEBUG_REMOTE && Parent->GetDebuggerEnabled(DEBUG_REMOTE));
    m_optDebugLocal->Enable(Parent->GetDebuggerEnabled(DEBUG_LOCAL));
    m_optDebugRS232->Enable(Parent->GetDebuggerEnabled(DEBUG_REMOTE));
    wxArrayString portlist = EnumeratePortsList();
    m_ctrlPort->Append(portlist);
    m_ctrlPort->SetStringSelection(Parent->GetDebugPort());
    m_ctrlPort->Enable(Parent->GetDebuggerSelected() == DEBUG_REMOTE);
    m_ctrlBaudRate->SetStringSelection(wxString::Format(wxT("%d"), Parent->GetDebugBaudrate()));
    m_ctrlBaudRate->Enable(Parent->GetDebuggerSelected() == DEBUG_REMOTE);
    m_chkEnableLogging->SetValue(Parent->GetDebugLogEnabled());
    m_chkEnableLogging->Enable(Parent->GetDebuggerSelected() == DEBUG_REMOTE);

    int row = 0;
    std::map<wxString,wxString>::iterator iter = theApp->SnippetList.begin();
    while (iter != theApp->SnippetList.end()) {
        wxString key = iter->first;
        wxString expansion = iter->second;
        if (key.Length() > 0 && expansion.Length() > 0) {
            expansion.Replace(wxT("\\"), wxT("\\\\"));
            expansion.Replace(wxT("\n"), wxT("\\n"));
            expansion.Replace(wxT("\t"), wxT("\\t"));
            expansion.Replace(wxT("\v"), wxT("\\s"));
            m_gridSnippets->AppendRows();
            m_gridSnippets->SetCellValue(row, 0, key);
            m_gridSnippets->SetCellValue(row, 1, expansion);
            row++;
        }
        ++iter;
    }
    m_gridSnippets->AppendRows();   /* always one more so user can add new snippets */
    m_gridSnippets->AutoSizeColumns(false);

    wxString cat = wxEmptyString;
    for (int index = 0; index < theApp->Shortcuts.Count(); index++) {
        KbdShortcut* key = theApp->Shortcuts.GetItem(index);
        wxASSERT(key);
        if (cat.Cmp(key->GetCategory()) != 0) {
            cat = key->GetCategory();
            m_KbdShortcuts->Append(new wxPropertyCategory(cat));
        }
        m_KbdShortcuts->Append(new wxStringProperty(key->GetName(false), key->GetLabel(), key->GetShortcut()));
    }

    m_StripTrailingWhitespace->SetValue(theApp->GetStripTrailing());
    m_ModalFind->SetValue(theApp->SearchAdvanced);
    m_LocalIniFile->SetValue(theApp->UseLocalIniFile());
    m_UserPDFReader->SetValue(theApp->GetReaderPathValid());
    m_UserReaderPath->SetValue(theApp->GetReaderPath());
    m_UserReaderPath->Enable(theApp->GetReaderPathValid());
    m_UserReaderBrowse->Enable(theApp->GetReaderPathValid());
}

void QuincySettingsDlg::CopyData()
{
    wxString host = m_TargetHost->GetStringSelection();
    if (host.Cmp(wxT("-")) == 0 || host.Cmp(wxT("(install target)")) == 0)
        host = wxEmptyString;
    Parent->SetTargetHost(host);

    Parent->SetDebugLevel(m_DebugLevel->GetSelection());
    Parent->SetOptimizationLevel(m_Optimization->GetSelection());
    Parent->SetOverlayCode(m_OverlayCode->GetValue());
    Parent->SetFixedAMXName(m_chkStandardAMXname->GetValue());
    Parent->SetVerboseBuild(m_chkVerbose->GetValue());
    Parent->SetAutoTransferEnabled(m_chkAutoTransfer->GetValue());
    Parent->SetCreateReport(m_chkCreateReport->GetValue());
    Parent->SetDefineString(m_Defines->GetValue());
    Parent->SetIncludePath(m_IncludePath->GetValue());
    Parent->SetCompilerPath(m_BinPath->GetValue());
    Parent->SetOutputPath(m_OutputPath->GetValue());
    Parent->SetPreBuild(m_PreBuild->GetValue());
    Parent->SetMiscCmdOptions(m_MiscCmdOptions->GetValue());

    int tab = m_TabWidth->GetValue();
    if (tab > 1 && tab <= 8)
        theApp->SetTabWidth(tab);
    theApp->SetUseTabs(!m_UseTabs->GetValue());
    theApp->SetAutoIndent(m_AutoIndent->GetValue());
    theApp->SetIndentBlocks(m_IndentPastedBlocks->GetValue());
    theApp->CodeFont = m_EditFont->GetSelectedFont();

    theApp->EditColours[CLR_BACKGROUND] = m_clrBackground->GetColour();
    theApp->EditColours[CLR_TEXT] = m_clrNormalText->GetColour();
    theApp->EditColours[CLR_KEYWORDS] = m_clrKeywords->GetColour();
    theApp->EditColours[CLR_STRINGS] = m_clrStrings->GetColour();
    theApp->EditColours[CLR_STRINGEOL] = m_clrStringEOL->GetColour();
    theApp->EditColours[CLR_HIGHLIGHT] = m_clrHighlight->GetColour();
    theApp->EditColours[CLR_ACTIVE] = m_clrActiveLine->GetColour();
    theApp->EditColours[CLR_COMMENTS] = m_clrComments->GetColour();
    theApp->EditColours[CLR_OPERATORS] = m_clrOperators->GetColour();
    theApp->EditColours[CLR_NUMBERS] = m_clrNumbers->GetColour();
    theApp->EditColours[CLR_PREPROCESSOR] = m_clrPreprocessor->GetColour();

    if (m_optDebugRS232->GetValue())
        Parent->SetDebuggerSelected(DEBUG_REMOTE);
    else
        Parent->SetDebuggerSelected(DEBUG_LOCAL);
    Parent->SetDebugPort(m_ctrlPort->GetStringSelection());
    wxString baud = m_ctrlBaudRate->GetStringSelection();
    long val;
    baud.ToLong(&val);
    Parent->SetDebugBaudrate(val);
    Parent->SetDebugLogEnabled(m_chkEnableLogging->GetValue());

    std::map<wxString,wxString>::iterator iter = theApp->SnippetList.begin();
    while (iter != theApp->SnippetList.end()) {
        iter->second.Empty();
        ++iter;
    }
    for (int row = 0; row < m_gridSnippets->GetNumberRows(); row++) {
        wxString key = m_gridSnippets->GetCellValue(row, 0);
        wxString expansion = m_gridSnippets->GetCellValue(row, 1);
        if (key.Length() > 0 && expansion.Length() > 0) {
            expansion.Replace(wxT("\\n"), wxT("\n"));
            expansion.Replace(wxT("\\t"), wxT("\t"));
            expansion.Replace(wxT("\\s"), wxT("\v"));
            expansion.Replace(wxT("\\\\"), wxT("\\"));
            iter = theApp->SnippetList.find(key);
            if (iter != theApp->SnippetList.end())
                iter->second = expansion;
            else
                theApp->SnippetList.insert(std::make_pair(key, expansion));
        }
    }

    theApp->SetStripTrailing(m_StripTrailingWhitespace->GetValue());
    theApp->SearchAdvanced = m_ModalFind->GetValue();
    theApp->SetReaderPathValid(m_UserPDFReader->GetValue());
    theApp->SetReaderPath(m_UserReaderPath->GetValue());

    for (wxPropertyGridIterator it = m_KbdShortcuts->GetIterator(); !it.AtEnd(); it++) {
        wxPGProperty* p = *it;
        int index = theApp->Shortcuts.FindLabel(p->GetName());
        wxASSERT(index >= 0);
        KbdShortcut* key = theApp->Shortcuts.GetItem(index);
        wxASSERT(key);
        wxVariant value = p->GetValue();
        wxString shortcut = value.GetString();
        key->SetShortcut(shortcut);
    }

    bool localini = m_LocalIniFile->GetValue();
    if (localini != theApp->UseLocalIniFile()) {
        wxString lclname = theApp->GetBinPath() + wxT(DIRSEP_STR) + wxT("quincy.ini");
        wxString glbname = theApp->GetUserDataPath() + wxT(DIRSEP_STR) + wxT("quincy.ini");
        if (localini) {
            /* copy the INI file to the local directory */
            if (!wxCopyFile(glbname, lclname, true))
                localini = false;   /* on failure to copy, revert to central INI */
        } else {
            /* move the INI file in the local directory to the application data folder */
            if (wxCopyFile(lclname, glbname, true))
                wxRemoveFile(lclname);
        }
        if (localini != theApp->UseLocalIniFile()) {
            wxString ininame = localini ? lclname : glbname;
            theApp->SetLocalIniFile(localini, ininame); /* closes and re-opens the INI file */
        }
    }
}
