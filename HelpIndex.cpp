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
 *  Version: $Id: HelpIndex.cpp 5564 2016-07-26 09:24:04Z  $
 */
#define _CRT_SECURE_NO_WARNINGS
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include "HelpIndex.h"

CHelpIndex::CHelpIndex()
{
}

CHelpIndex::~CHelpIndex()
{
	m_index.clear();
	m_filetable.clear();
}

void CHelpIndex::AddFile(const char *filename, int id)
{
	std::string fname = (filename!=NULL) ? filename : "";
	m_filetable.insert(std::make_pair(id, fname));
}

void CHelpIndex::AddPage(const char *key, int id, int page)
{
	CPageRef ref(id, page);
	m_index.insert(std::make_pair(key, ref));
}

bool CHelpIndex::ScanFile(const char *indexfile, int id, const char *docfile)
{
	FILE *fp = fopen(indexfile, "rt");
	if (!fp)
		return false;

	if (!docfile)
		docfile = indexfile;
	AddFile(docfile, id);

	char line[128];
	while (fgets(line, sizeof line, fp)) {
		int softpage = 0, hardpage = 0;
		char macro[64] = "", label[64] = "", type[64] = "";
		sscanf(line, "%[^{]{%[^}]}{%d}{%[^}]}{%d}", macro, label, &softpage, type, &hardpage);
		if (strcmp(macro, "\\@definelabel") == 0
			&& strlen(label) > 2 && label[0] == 'p' && label[1] == '.' 
			&& (strpbrk(label + 2, " .,:+-/&!()") == NULL || strncmp(label, "p.at.", 5) == 0 && strpbrk(label + 5, " .,:+-/&!()") == NULL)
			&& strcmp(type, "page") == 0 
			&& hardpage > 0)
			AddPage(label + 2, id, hardpage);
	}

	fclose(fp);
	return true;
}

std::map<const char*,int> *CHelpIndex::LookUp(const char *key)
{
	std::map<const char*,int> *filenames;
	filenames = new(std::map<const char*,int>);
	filenames->clear();

	assert(key != NULL);
	std::multimap<std::string, CPageRef>::iterator p;
	for (p = m_index.begin(); p != m_index.end(); ++p) {
		if (p->first.compare(key) == 0) {
			CPageRef pr = p->second;
			const char *name = LookUp(pr.GetId());
			int page = pr.GetPage();
			filenames->insert(std::pair<const char*,int>(name, page));
		}
	}

	return filenames;
}

const char *CHelpIndex::LookUp(int id)
{
	std::multimap<int, std::string>::iterator p;

	if (id == 0)
		p = m_filetable.begin();
	else
		p = m_filetable.find(id);
	return (p == m_filetable.end()) ? NULL : p->second.c_str();
}
