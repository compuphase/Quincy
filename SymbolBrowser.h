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
 *  Version: $Id: SymbolBrowser.h 5691 2017-06-09 19:48:22Z thiadmer $
 */
#ifndef _SYMBOLBROWSER_H
#define _SYMBOLBROWSER_H

#include <wx/wx.h>

class CSymbolEntry
{
public:
    CSymbolEntry() : Next(0), Line(0) {}
    bool Add(const wxString &xmlfile, const wxString &symname, const wxString &syntax,
             const wxString &summary, const wxString &source, int line);
    void Remove(const wxString &xmlfile = wxEmptyString);

public:
    wxString SymbolName;    // name plus type
    wxString Syntax;        // name plus decoration
    wxString Source;        // source file where the symbol is defined
    wxString Summary;       // symbol documentation (summary)
    int Line;

    wxString XMLfile;       // XML report file from which the declaration comes
    CSymbolEntry *Next;
};

class CSymbolList
{
public:
    ~CSymbolList() { Clear(); }
    void Clear() { SymbolList.Remove(); }
    bool LoadReportFile(const wxString& file);
    const CSymbolEntry* Lookup(const wxString& symbol, int skip = 0, bool partial = false) const;
    const CSymbolEntry* Root() const { return SymbolList.Next; }
private:
    CSymbolEntry SymbolList;
};

#endif /* _SYMBOLBROWSER_H */

