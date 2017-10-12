//******************************************************************************
///
/// @file windows/cmedit/StdStringList.cpp
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

#include "cmedit.h"
#include "ccodemax.h"
#include "settings.h"
#include "menusupport.h"
#include "eventhandlers.h"
#include "editorinterface.h"
#include "dialogs.h"

CStdStringList::CStdStringList(void)
{
}

CStdStringList::~CStdStringList(void)
{
}

bool CStdStringList::LoadFromFile (LPCSTR FileName)
{
  FILE        *f ;

  m_Items.clear () ;
  int size = GetFileLength (FileName) ;
  if (size == -1 || size > 1024 * 1024)
    return (false) ;
  if ((f = fopen (FileName, "rb")) == NULL)
    return (false) ;
  char *buffer = new char [size + 1] ;
  int nread = (int) fread (buffer, size, 1, f) ;
  fclose (f) ;
  if (nread != 1)
  {
    delete [] buffer ;
    return (false) ;
  }
  char *s1 = buffer ;
  char *s2 = buffer + size ;
  char *s3 = buffer ;
  while (s1 < s2)
  {
    if (*s1 == '\r' || *s1 == '\n')
    {
      *s1++ = '\0' ;
      AppendItem (s3) ;
      if (s1 < s2 && *s1 == '\n')
        *s1++ ;
      s3 = s1 ;
    }
    else
      s1++ ;
  }
  if (s3 < s2 && s1 > s3)
  {
    *s2 = '\0' ;
    AppendItem (s3) ;
  }
  delete [] buffer ;
  return (true) ;
}

int CStdStringList::ItemCount (void)
{
  return ((int) m_Items.size ()) ;
}

CStdString& CStdStringList::operator [] (int index)
{
  if (index >= (int) m_Items.size ())
    return (m_EmptyStr) ;
  return (m_Items.at (index)) ;
}

bool CStdStringList::DeleteItem (int index)
{
  if (index == 0)
    m_Items.pop_front () ;
  else if (index == m_Items.size () - 1)
    m_Items.pop_back () ;
  else if (index < (int) m_Items.size ())
    m_Items.erase (m_Items.begin () + index) ;
  else
    return (false) ;
  return (true) ;
}

CStdString& CStdStringList::InsertItem (LPCSTR text)
{
  m_Items.push_front (text) ;
  return (m_Items.front ()) ;
}

CStdString& CStdStringList::InsertItem (int index, LPCSTR text)
{
  if (index == 0)
    return (InsertItem (text)) ;
  m_Items.insert (m_Items.begin () + index, text) ;
  return (m_Items [index]) ;
}

CStdString& CStdStringList::AppendItem (LPCSTR text)
{
  m_Items.push_back (text) ;
  return (m_Items.back ()) ;
}

void CStdStringList::Clear (void)
{
  m_Items.clear () ;
}
