/**************************************************************************\
 * 
 *  FILE: LayerTable.cpp
 *
 *  This source file is part of DIME.
 *  Copyright (C) 1998-1999 by Systems In Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License, version 2, as
 *  published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License (the accompanying file named COPYING) for more
 *  details.
 *
 **************************************************************************
 *
 *  If you need DIME for a non-GPL project, contact Systems In Motion
 *  to acquire a Professional Edition License:
 *
 *  Systems In Motion                                   http://www.sim.no/
 *  Prof. Brochs gate 6                                       sales@sim.no
 *  N-7030 Trondheim                                   Voice: +47 22114160
 *  NORWAY                                               Fax: +47 67172912
 *
\**************************************************************************/

/*!
  \class dimeLayerTable dime/tables/LayerTable.h
  \brief The dimeLayerTable class reads and writes LAYER \e tables.
*/

#include <dime/tables/LayerTable.h>
#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/util/MemHandler.h>
#include <dime/Model.h>
#include <dime/records/Record.h>
#include <string.h>

static const char tableName[] = "LAYER";

/*!
  Constructor.
*/

dimeLayerTable::dimeLayerTable()
  : colorNumber( 0 ), layerName( NULL ), layerInfo( NULL )
{
}

//!

dimeTableEntry *
dimeLayerTable::copy(dimeModel * const model) const
{
  dimeMemHandler *memh = model->getMemHandler();
  dimeLayerTable *l = new(memh) dimeLayerTable;
  l->colorNumber = this->colorNumber;
  if (this->layerInfo) {
    l->layerInfo = (dimeLayer*)model->addLayer(this->layerInfo->getLayerName(), 
					      DXFABS(this->colorNumber));
  }
  if (!copyRecords(l, model)) {
    // check if allocated on heap.
    if (!memh) delete l;
    l = NULL;
  }
  return l;
}

//!

const char *
dimeLayerTable::getTableName() const
{
  return tableName;
}

//!

bool 
dimeLayerTable::read(dimeInput * const file)
{
  this->layerName = NULL;
  bool ret = dimeTableEntry::read(file);
  if (ret && this->layerName) {
    this->layerInfo = (dimeLayer*) 
      file->getModel()->addLayer(this->layerName, DXFABS(this->colorNumber));
    delete [] this->layerName;
    this->layerName = NULL;
  }
  return ret;
}

//!

bool 
dimeLayerTable::write(dimeOutput * const file)
{
  bool ret = dimeTableEntry::preWrite(file);
  
  if (this->layerInfo) { 
    ret = file->writeGroupCode(2); 
    file->writeString(this->layerInfo->getLayerName());
  }
  file->writeGroupCode(62);
  file->writeInt16(this->colorNumber);

  ret = dimeTableEntry::write(file);
  return ret;
}

//!

int 
dimeLayerTable::typeId() const
{
  return dimeBase::dimeLayerTableType;
}

//!

bool 
dimeLayerTable::handleRecord(const int groupcode,
			    const dimeParam &param,
			    dimeMemHandler * const memhandler)
{
  switch(groupcode) {
  case 2:
    {
      const char *str = param.string_data;
      if (str) {
	// a small hack -- deleted in read() very soon
	this->layerName = new char[strlen(str)+1];
	if (this->layerName)
	  strcpy(this->layerName, str);
      }
      return true;
    }
  case 62:
    this->colorNumber = param.int16_data;
    return true;
  }
  return dimeTableEntry::handleRecord(groupcode, param, memhandler);
}

//!

int
dimeLayerTable::countRecords() const
{
  int cnt = 1; // header
  if (this->layerInfo) cnt++;
  cnt++; // colorNumber
  return cnt + dimeTableEntry::countRecords();
}

