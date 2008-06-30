// ObjList.cpp
#include "stdafx.h"
#include "ObjList.h"
#include "MarkedList.h"

ObjList::ObjList(const ObjList& objlist): HeeksObj(objlist) {operator=(objlist);}

void ObjList::Clear()
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		(*It)->m_owner = NULL;
		delete *It;
	}
	m_objects.clear();
}

const ObjList& ObjList::operator=(const ObjList& objlist)
{
	HeeksObj::operator=(objlist);
	ClearUndoably();
	std::list<HeeksObj*>::const_iterator It;
	for (It=objlist.m_objects.begin();It!=objlist.m_objects.end();It++)
	{
		HeeksObj* new_op = (*It)->MakeACopy();
		if(new_op)Add(new_op, NULL);
	}
	return *this;
}

void ObjList::ClearUndoably(void)
{
	if (m_objects.size() == 0) return;
	std::list<HeeksObj*> objects_to_delete = m_objects;
	std::list<HeeksObj*>::iterator It;
	for (It=objects_to_delete.begin();It!=objects_to_delete.end();It++)wxGetApp().DeleteUndoably(*It);
	m_objects.clear();
	LoopItStack.clear();
}

HeeksObj* ObjList::MakeACopy(void) const { return new ObjList(*this); }

void ObjList::GetBox(CBox &box)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		(*It)->GetBox(box);
	}
}

void ObjList::glCommands(bool select, bool marked, bool no_color)
{
	HeeksObj::glCommands(select, marked, no_color);
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		if(select)glPushName((unsigned int)(*It));
		(*It)->glCommands(select, marked || (wxGetApp().m_marked_list->ObjectMarked(*It) != 0), no_color);
		if(select)glPopName();
	}
}

HeeksObj* ObjList::GetFirstChild()
{
	if (m_objects.size()==0) return NULL;
	LoopIt = m_objects.begin();
	return *LoopIt;
}

HeeksObj* ObjList::GetNextChild()
{
	if (m_objects.size()==0 || LoopIt==m_objects.end()) return NULL;
	LoopIt++;
	if (LoopIt==m_objects.end()) return NULL;
	return *LoopIt;
}

bool ObjList::Add(HeeksObj* object, HeeksObj* prev_object)
{
	if (object==NULL) return false;
	if (!CanAdd(object)) return false;
	if (m_objects.size()==0 || prev_object==NULL)
	{
		m_objects.push_back(object);
		LoopIt = m_objects.end();
		LoopIt--;
	}
	else
	{
		for(LoopIt = m_objects.begin(); LoopIt != m_objects.end(); LoopIt++) { if (*LoopIt==prev_object) break; }
		m_objects.insert(LoopIt, object);
	}
	HeeksObj::Add(object, prev_object);
	return true;
}


void ObjList::Remove(HeeksObj* object)
{
	if (object==NULL) return;
	for(LoopIt = m_objects.begin(); LoopIt != m_objects.end(); LoopIt++){
		if(*LoopIt==object)break;
	}
	if(LoopIt != m_objects.end())m_objects.erase(LoopIt);
	HeeksObj::Remove(object);
}

void ObjList::KillGLLists(void)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++) (*It)->KillGLLists();
}
