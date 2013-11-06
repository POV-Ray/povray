/*******************************************************************************
 * StdStringList.h
 *
 * This file is part of the CodeMax editor support code.
 *
 * Author: Christopher J. Cason.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/windows/cmedit/StdStringList.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __STDSTRINGLIST_H__
#define __STDSTRINGLIST_H__

typedef std::deque<CStdString> StdStringList ;
typedef StdStringList::iterator StdStringListIterator ;

class CStdStringList
{
public:
  CStdStringList(void);
  ~CStdStringList(void);
  bool LoadFromFile (LPCSTR FileName) ;
  int ItemCount (void) ;
  bool DeleteItem (int index) ;
  CStdString& InsertItem (LPCSTR text) ;
  CStdString& InsertItem (int index, LPCSTR text) ;
  CStdString& AppendItem (LPCSTR text) ;
  CStdString& operator [] (int index) ;
  void Clear (void) ;
private:
  CStdString m_EmptyStr ;
  StdStringList m_Items ;
};

#endif
