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
 *  Version: $Id: HelpIndex.h 5504 2016-05-15 13:42:30Z  $
 */
#ifndef _HELPINDEX_H
#define _HELPINDEX_H

#include <map>

class CPageRef {
public:
	CPageRef(int id = 0, int page = 0)
		{ m_id = id; m_page = page; }
	void Set(int id, int page)
		{ m_id = id; m_page = page; }
	int GetPage() const
		{ return m_page; }
	int GetId() const
		{ return m_id; }
	int Valid() const
		{ return m_id >0 && m_page > 0; }
private:
	int m_page;
	int m_id;
};

class CHelpIndex {
public:
	CHelpIndex();
	~CHelpIndex();
	void AddPage(const char *key, int id, int page);
	void AddFile(const char *filename, int id);
	bool ScanFile(const char *indexfile, int id, const char *docfile = NULL);
	std::map<const char*,int> *LookUp(const char *key);	// returns filename/page pairs
	const char *LookUp(int id);	// returns filename
private:
	std::multimap<std::string, CPageRef> m_index;
	std::multimap<int, std::string> m_filetable;
};

#endif /* _HELPINDEX_H */
