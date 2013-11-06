/*******************************************************************************
 * pointer.h
 *
 * This module contains to Pointer class which is a limited version of std::auto_ptr.
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
 * $File: //depot/public/povray/3.x/source/base/pointer.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POINTER_H
#define POINTER_H

#include "configbase.h"

namespace pov_base
{

// TODO FIXME - the Pointer class should be replaced by one of the boost/std smart pointers

template<class X> class Pointer
{
	public:
		explicit Pointer(X *p = NULL)
		{
			ptr = p;
		}

		Pointer(X& a)
		{
			ptr = a.release();
		}

		template<class Y> Pointer(Pointer<Y>& a)
		{
			ptr = a.release();
		}

		~Pointer()
		{
			if(ptr != NULL)
				delete ptr;
			ptr = NULL;
		}

		Pointer& operator=(Pointer& a)
		{
			reset(a.release());
			return *this;
		}

		template<class Y> Pointer& operator=(Pointer<Y>& a)
		{
			reset(a.release());
			return *this;
		}

		X& operator*() const
		{
			return *ptr;
		}

		const X *operator->() const
		{
			return ptr;
		}

		X *operator->()
		{
			return ptr;
		}

		const X *get() const
		{
			return ptr;
		}

		X *release()
		{
			X *t = ptr;
			ptr = NULL;
			return t;
		}

		void reset(X *p = NULL)
		{
			if(ptr != NULL)
				delete ptr;
			ptr = p;
		}

		bool operator==(const void *p) const
		{
			return (ptr == p);
		}

		bool operator!=(const void *p) const
		{
			return (ptr != p);
		}
	private:
		X *ptr;
};

}

#endif
