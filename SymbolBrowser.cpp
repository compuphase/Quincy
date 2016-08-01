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
 *  Version: $Id: SymbolBrowser.cpp 5519 2016-05-26 08:33:38Z  $
 */
#include "wxQuincy.h"
#include "SymbolBrowser.h"
#include "tinyxml/tinyxml2.h"

bool CSymbolEntry::Add(const wxString &xmlfile, const wxString &symname,
					   const wxString &syntax, const wxString &summary,
					   const wxString &source, int line)
{
	CSymbolEntry *item;

	/* first see whether the item exists already (same name, same source file,
	 * same syntax); if so, only update the line number
	 */
	for (item = this; item; item = item->Next) {
		if (item->Source.CmpNoCase(source) == 0
			&& item->SymbolName.Cmp(symname) == 0
			&& item->Syntax.Cmp(syntax) == 0)
		{
			item->Line = line;
			return true;
		}
	}

	/* special case, ignore anonymous types */
	if (symname.CmpNoCase(wxT("t:anonymous")) == 0)
		return false;
	/* also ignore any prefefind constant, these do not have a location */
	if (source.Length() == 0)
		return false;

	/* item does not exist, create & insert */
	item = new CSymbolEntry;
	if (!item)
		return false;

	item->XMLfile = xmlfile;
	item->SymbolName = symname;
	if (syntax.Length() == 0)
		item->Syntax = symname.Mid(2);
	else
		item->Syntax = syntax;
	item->Summary = summary;
	item->Source = source;
	item->Line = line;

	CSymbolEntry *root = this;
	while (root->Next && root->Next->SymbolName.Cmp(item->SymbolName)<0)
		root = root->Next;
	item->Next = root->Next;
	root->Next = item;

	return true;
}

void CSymbolEntry::Remove(const wxString &xmlfile)
{
	CSymbolEntry *item, *next;
	item = this;
	while (item->Next) {
		next = item->Next;
		if (xmlfile.IsEmpty() || next->XMLfile.CmpNoCase(xmlfile) == 0) {
			item->Next = next->Next;
			delete next;
		} else {
			item = next;
		}
	}
}

bool CSymbolList::LoadReportFile(const wxString& file)
{
	SymbolList.Remove(file);

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(file.utf8_str()) != tinyxml2::XML_NO_ERROR)
		return false;
	tinyxml2::XMLHandle docHandle(doc);
	tinyxml2::XMLHandle child = docHandle.FirstChildElement("doc").FirstChildElement("members").FirstChildElement("member");
	if (!child.ToElement())
		return false;
	while (child.ToElement()) {
		tinyxml2::XMLElement* summary = child.FirstChildElement("summary").ToElement();
		tinyxml2::XMLElement* location = child.FirstChildElement("location").ToElement();
		if (location) {
			wxString symname = wxString::FromUTF8(child.ToElement()->Attribute("name"));	/* for type and sorting */
			wxString source = wxString::FromUTF8(location->Attribute("file"));
			int line = (int)strtol(location->Attribute("line"), NULL, 10);
			wxString syntax = wxEmptyString;	/* syntax or value, for display */
			if (symname[1] == wxT(':')) {
				if (symname[0] == wxT('M') || symname[0] == wxT('F'))
					syntax = wxString::FromUTF8(child.ToElement()->Attribute("syntax"));
				else if (symname[0] == wxT('C'))
					syntax = symname.Mid(2) + wxT(" (") + wxString::FromUTF8(child.ToElement()->Attribute("value")) + wxT(")");
			}
			wxString descr = wxEmptyString;
			if (summary) {
				descr = wxString::FromUTF8(summary->GetText());
				descr.Trim(false);
				descr.Trim(true);
			}
			/* make a full path, if needed */
			if (source[0] != DIRSEP_CHAR && source.Length() > 1 && source[1] != ':')
				source = file.BeforeLast(DIRSEP_CHAR) + wxT(DIRSEP_STR) + source;
			SymbolList.Add(file, symname, syntax, descr, source, line);
		}
		child = child.NextSiblingElement("member");
	}

	return true;
}

/* Lookup()
 * Looks up a symbol in the "browse" information.
 * If "skip" is greater than zero, the first "skip" matches are ignored. You can
 * use this when there are multiple symbols with the same name.
 * If "partial" is true, only the length of "symbol" is matched; a symbol that is
 * longer than parameter "symbol" but match it up to that length, is returned. 
 */
const CSymbolEntry* CSymbolList::Lookup(const wxString& symbol, int skip, bool partial) const
{
	CSymbolEntry *item = SymbolList.Next;
	while (item) {
		wxString symname = item->SymbolName.Mid(2);
		if (partial)
			symname = symname.Left(symbol.Length());
		if (symname.Cmp(symbol) == 0) {
			if (--skip < 0)
				return item;
		}
		item = item->Next;
	}
	return 0;
}

