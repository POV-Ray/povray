//******************************************************************************
///
/// @file windows/cmedit/StdStringList.h
///
/// This file is part of the CodeMax editor support code.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

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
