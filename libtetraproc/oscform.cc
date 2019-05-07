// ----------------------------------------------------------------------------
//
//  Copyright (C) 2006-2008 Fons Adriaensen <fons@linuxaudio.org>
//    
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ----------------------------------------------------------------------------


#include <string.h>
#include <netinet/in.h>
#include "oscform.h"


//------------------------------------------------------------------------------------


Oscform::Oscform (int size) : _size (size)
{
    _data = new char [size];
}


Oscform::~Oscform (void)
{
    delete[] _data;
}


//------------------------------------------------------------------------------------


int Oscform::put_command (const char *v)
{
    int k;

    _idata = 0;
    _inext = 4;
    _isize = 0;
    _pform = 0;

    if (*v == 0) return -1;
    k = strlen (v);
    if (k + _inext >= _size) return -1;
    _pcomm = _data + _inext;
    strcpy (_pcomm, v);
    _iform = 0;
    _inext += k + 1;
    while (_inext & 3) _data [_inext++] = 0;

    return 0;
}


int Oscform::put_format (const char *v)
{
    int k;

    if (*v != ',') return -1;
    k = strlen (v);
    if (k + _inext >= _size) return -1;
    _pform = _data + _inext;
    strcpy (_pform, v);
    _iform = 1;
    _inext += k + 1;
    _idata = _inext;
    while (_inext & 3) _data [_inext++] = 0;

    return 0;
}


int Oscform::put_int (int v)
{
    if (!_pform || (_pform [_iform] != 'i')) return -1;
    if (4 + _inext > _size) return -1;
    *((int *)(_data + _inext)) = htonl (v);
    _iform++;
    _inext += 4;

    return 0;
}


int Oscform::put_float (float v)
{
    union { int i; float f; } u;
    
    if (!_pform || (_pform [_iform] != 'f')) return -1;
    if (4 + _inext > _size) return -1;
    u.f = v;
    *((int *)(_data + _inext)) = htonl (u.i);
    _iform++;
    _inext += 4;

    return 0;
}


int Oscform::put_string (const char *v)
{
    int k;

    if (!_pform || (_pform [_iform] != 's')) return -1;
    k = strlen (v);
    if (k + _inext >= _size) return -1;
    strcpy (_data + _inext,  v);
    _iform++;
    _inext += k + 1;
    while (_inext & 3) _data [_inext++] = 0;

    return 0;
}


int Oscform::write (FILE *F)
{
    if (!_pcomm || (_pform &&  _pform [_iform])) return -1;
    _iform = 1;
    _isize = _inext - 4;
    _inext = _idata;
    *((int *)_data) = htonl (_isize);
    if ((int) fwrite (_data, 1, _isize + 4, F) != _isize + 4) return -2;

    return 0;
}


//------------------------------------------------------------------------------------


int Oscform::read (FILE *F)
{
    int i, j;

    _pcomm = 0;
    _pform = 0;
    _iform = 0;
    _idata = 0;
    _inext = 0;
    _isize = 0;
    
    if ((int) fread (_data, 1, 4, F) != 4) return -2;
    _isize = ntohl (*((int *)_data));
    if (_isize + 4 > _size) return -1;
    if ((int) fread (_data + 4, 1, _isize, F) != _isize) return -2;

    i = j = 4;
    while ((j < _size) && _data [j]) j++;
    if (j == _size) return -1;
    _pcomm = _data + i;
    j++;
    while (j & 3) j++;

    i = j;
    while ((j < _size) && _data [j]) j++;
    if (j == _size) return 0;
    _pform = _data + i;
    j++;
    while (j & 3) j++;

    if (*_pform != ',') return -1;
    _idata = _inext = j;
    _iform = 1;

    return 0;
}


int Oscform::get_int (int *v)
{
    if (!_pform || (_pform [_iform] != 'i')) return -1;
    if (4 + _inext > _size) return -1;
    *v = ntohl (*((int *)(_data + _inext)));
    _iform++;
    _inext += 4;

    return 0;
}


int Oscform::get_float (float *v)
{
    if (!_pform || (_pform [_iform] != 'f')) return -1;
    if (4 + _inext > _size) return -1;
    *((int *) v) = ntohl (*((int *)(_data + _inext)));
    _iform++;
    _inext += 4;

    return 0;
}


int Oscform::get_float1 (float *v)
{
    if (!_pform || (4 + _inext > _size)) return -1;
    if (_pform [_iform] == 'f')
    {
        *((int *) v) = ntohl (*((int *)(_data + _inext)));
    }
    else if (_pform [_iform] == 'i')
    {
	*v = ntohl (*((int *)(_data + _inext)));
    }
    else return -1;
    _iform++;
    _inext += 4;

    return 0;
}


int Oscform::get_string (const char **p, int *n)
{
    int j;

    if (!_pform || (_pform [_iform] != 's')) return -1;
    j = _inext;
    while ((j < _size) && _data [j]) j++;
    if (j == _size) return -1;
    *p = _data + _inext;
    *n = j - _inext;
    j++;
    while (j & 3) j++;
    _iform++;
    _inext = j;

    return 0;
}


//------------------------------------------------------------------------------------







