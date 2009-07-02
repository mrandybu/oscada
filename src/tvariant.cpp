
//OpenSCADA system file: tvariant.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>

#include <tsys.h>
#include "tvariant.h"

//*************************************************
//* TVariant                                      *
//*************************************************
TVariant::TVariant( )				{ vl.assign(1,(char)TVariant::Null); }

TVariant::TVariant( char ivl )			{ vl.assign(1,(char)TVariant::Null); setB(ivl); }

TVariant::TVariant( int ivl )			{ vl.assign(1,(char)TVariant::Null); setI(ivl); }

TVariant::TVariant( double ivl )		{ vl.assign(1,(char)TVariant::Null); setR(ivl); }

TVariant::TVariant( const string &ivl )		{ vl.assign(1,(char)TVariant::Null); setS(ivl); }

TVariant::TVariant( const TVarObj &ivl )	{ vl.assign(1,(char)TVariant::Null); setO(ivl); }

bool TVariant::operator==( TVariant &vr )	{ return vr.vl == vl; }

TVariant &TVariant::operator=( TVariant &vr )	{ vl = vr.vl; return *this; }

char TVariant::getB( char def ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_BOOL : (bool)atoi(getS().c_str());
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_BOOL : (bool)getI();
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_BOOL : (bool)getR();
	case TVariant::Boolean:	return vl[1];
	default: return def;
    }
}

int TVariant::getI( int def ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_INT: atoi(getS().c_str());
	case TVariant::Integer:	return *(int*)(vl.data()+1);
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_INT : (int)getR();
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_INT : getB();
	default: return def;
    }
}

double TVariant::getR( double def ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_REAL : atof(getS().c_str());
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_REAL : getI();
	case TVariant::Real:	return *(double*)(vl.data()+1);
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_REAL : getB();
	default: return def;
    }
}

string TVariant::getS( const string &def ) const
{
    switch( type() )
    {
	case TVariant::String:	return vl.substr(1);
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_STR : TSYS::int2str(getI());
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_STR : TSYS::real2str(getR());
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_STR : TSYS::int2str(getB());
	default: return def;
    }
}

TVarObj	&TVariant::getO( ) const
{
    if( type() != TVariant::Object ) throw TError("TVariant",_("Variable not object!"));
    return *(TVarObj*)TSYS::str2addr(vl.substr(1));
}

void TVariant::setB( char ivl )
{
    if( type() == TVariant::Object ) delete &getO( );
    if( type() != TVariant::Boolean )
    {
	vl.assign(1,(char)TVariant::Boolean);
	vl.reserve(1+sizeof(char));
    }
    vl.replace(1,string::npos,(char*)&ivl,sizeof(char));
}

void TVariant::setI( int ivl )
{
    if( type() == TVariant::Object ) delete &getO( );
    if( type() != TVariant::Integer )
    {
	vl.assign(1,(char)TVariant::Integer);
	vl.reserve(1+sizeof(int));
    }
    vl.replace(1,string::npos,(char*)&ivl,sizeof(int));
}

void TVariant::setR( double ivl )
{
    if( type() == TVariant::Object ) delete &getO( );
    if( type() != TVariant::Real )
    {
	vl.assign(1,(char)TVariant::Real);
	vl.reserve(1+sizeof(double));
    }
    vl.replace(1,string::npos,(char*)&ivl,sizeof(double));
}

void TVariant::setS( const string &ivl )
{
    if( type() == TVariant::Object ) delete &getO( );
    if( type() != TVariant::String ) vl.assign(1,(char)TVariant::String);
    vl.replace(1,string::npos,ivl);
}

void TVariant::setO( const TVarObj &val )
{
    if( type() != TVariant::Object ) vl.assign(1,(char)TVariant::Object);
    vl.replace(1,string::npos,TSYS::addr2str(val.dup()));
}

//***********************************************************
//* TVarObj                                                 *
//*   Variable object, by default included properties       *
//***********************************************************
TVarObj::TVarObj( )		{ }

TVarObj::~TVarObj( )		{ }

TVarObj *TVarObj::dup( ) const
{
    TVarObj *obj = new TVarObj();
    *obj = *this;
    return obj;
}

TVarObj &TVarObj::operator=( const TVarObj &obj )
{
    mProps = obj.mProps;
    return *this;
}

TVariant TVarObj::propGet( const string &id, bool onlyCntr )
{
    map<string,TVariant>::iterator vit = mProps.find(id);
    if( vit == mProps.end() ) return TVariant();

    return vit->second;
}

void TVarObj::propSet( const string &id, TVariant val )		{ mProps[id] = val; }

TVariant TVarObj::funcCall( const string &id, vector<TVariant> prms )	{ return TVariant(); }
