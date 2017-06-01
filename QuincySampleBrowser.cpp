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
 *  Version: $Id: QuincySettingsDlg.h 5591 2016-10-28 15:57:11Z  $
 */
#include "wxQuincy.h"
#include "QuincySampleBrowser.h"


QuincySampleBrowser::QuincySampleBrowser(wxWindow* parent)
    : SampleBrowser(parent)
{
	Parent = static_cast<QuincyFrame*>(parent);
    filelist.Clear();
    ini = 0;
}

void QuincySampleBrowser::OnInitDialog(wxInitDialogEvent& event)
{
    /* get the sample index file */
 	wxString path = theApp->GetExamplesPath() + wxT(DIRSEP_STR);
    wxString host = Parent->GetTargetHost();
    if (host.Length() > 0)
        path += host + wxT(DIRSEP_STR);
    path += wxT("samples.idx");
	if (!wxFileExists(path)) {
        m_htmlSample->SetPage(wxT("<h1>No samples...</h1><p>No index with descriptions of the samples is available (for the current target or for this distribution)."));
        return;
	}
	ini = new minIni(path);
    wxASSERT(ini);

    /* fill the listbox */    
    int idx = 0;
    for ( ;; ) {
        wxString item = ini->getsection(idx);
        if (item.Length() == 0)
            break;
        m_listSamples->Append(item);
        idx++;
    }

    /* set an introductory text in the HTML control */
    m_htmlSample->SetPage(wxT("<h1>Sample browser</h1><p>Select one of the items in the list at the left to see a description of the sample.<p>Clicking 'OK' opens the selected sample."));
}

void QuincySampleBrowser::OnCancel(wxCommandEvent& /* event */)
{
	EndModal(wxID_CANCEL);
}

void QuincySampleBrowser::OnOK(wxCommandEvent& /* event */)
{
    int idx = m_listSamples->GetSelection();
    if (idx >= 0 && ini) {
        wxString section = m_listSamples->GetString(idx);
        wxString files = ini->gets(section, wxT("files"));
        filelist = wxSplit(files, wxT(' '));
	    EndModal(wxID_OK);
    }
}

void QuincySampleBrowser::OnSelect(wxCommandEvent& event)
{
    int idx = m_listSamples->GetSelection();
    if (idx >= 0 && ini) {
        wxString section = m_listSamples->GetString(idx);
        wxString description = ini->gets(section, wxT("description"));
        m_htmlSample->SetPage(description);
    }
}
