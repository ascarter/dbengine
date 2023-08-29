// ================================================================================
//
//	File:
//      dbstruct.h
//
//	Component:
//      Database Engine
//
//	Description:
//      Database internal structures
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __DBSTRUCT_H__
#define __DBSTRUCT_H__

// --------------------------------------------------------------------------------
//	SMART POINTER DEFINITIONS
// --------------------------------------------------------------------------------
SmartPointer(CDbFile);
SmartPointer(CDbTable);

// --------------------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------------------
const int	DB_MAX_OBJECT_NAME			= 10;
const UINT	DB_DEFAULT_TABLES			= 10;
const UINT	DB_DEFAULT_INDEXES			= 2 * DB_DEFAULT_TABLES;
const UINT	DB_DEFAULT_SLOTS			= 1000;
const UINT	DB_DEFAULT_GROWTH_FACTOR	= 500;
const UINT	DB_DEFAULT_REC_BUFFER_SIZE	= 100;

// --------------------------------------------------------------------------------
//	TYPEDEFS
// --------------------------------------------------------------------------------
typedef UINT DBRECID;

// --------------------------------------------------------------------------------
// Structure:
//      DbCatalog
//
//  Description:
//      Catalog information.  Used for tracking catalog metadata.
// --------------------------------------------------------------------------------
struct DbCatalog
{
	FILEOFFSET	Offset;			// Offset to catalog data
	UINT		Size;			// Catalog record size
	UINT		Entries;		// Count of active entries
	UINT		Slots;			// Count of total available slots
	UINT		GrowthFactor;	// Catalog growth factor
	UINT		LastId;			// Last id value used
};

// --------------------------------------------------------------------------------
// Structure:
//      DbFileInfo
//
//  Description:
//      File information.  Used for managing the catalogs and identifying the file
//		version.
// --------------------------------------------------------------------------------
struct DbFileInfo
{
	// VERSION HEADER
	UINT		MajorVersion;		// Major version
	UINT		MinorVersion;		// Minor version

	// CATALOGS
	DbCatalog	Tables;				// Table catalog
	DbCatalog	Indexes;			// Index catalog

	// FILE OFFSET INFORMATION
	FILEOFFSET	DataOffsetStart;	// Offset to beginning of data (incl. catalog)
	FILEOFFSET	DataOffsetEnd;		// Offset to end of data (including catalog)

	// OTHER METADATA
	time_t		Created;			// Date/time created
	time_t		LastUpdated;		// Date/time of last update
};

// --------------------------------------------------------------------------------
// Structure:
//      DbObjectInfo
//
//  Description:
//      Catalog information.  Base structure shared by metadata tracking records
//
//		Id == 0 -> unused table slot
// --------------------------------------------------------------------------------
struct DbObjectInfo
{
	UINT		Id;									// Unique id value (0 == free)
	char		Name[DB_MAX_OBJECT_NAME + 1];		// Object name
	FILEOFFSET	Offset;								// Offset to beginning of data area
	UINT		Size;								// Object size (bytes)
	UINT		Entries;							// Slots in use
	UINT		Slots;								// Total slots
	UINT		GrowthFactor;						// Growth factor (slots)
};

// --------------------------------------------------------------------------------
// Structure:
//      DbTable
//
//  Description:
//      Table information.  Used for tracking the table catalog in a CDbFile object
//
//		TableId == 0 -> unused table slot
// --------------------------------------------------------------------------------
struct DbTableInfo : public DbObjectInfo
{
	UINT	LastRecordId;							// Last unique record id used
};

// --------------------------------------------------------------------------------
// Structure:
//      DbIndexInfo
//
//  Description:
//      Index information.  Used for tracking the index catalog in a CDbFile object
//
//		IndexId == 0 -> unused index slot
// --------------------------------------------------------------------------------
struct DbIndexInfo : public DbObjectInfo
{
	UINT	TableId;								// Corresponding table id
};

// --------------------------------------------------------------------------------
// Structure:
//      DbRecord
//
//  Description:
//      Database record.  Standardizes the structure of records by enforcing a
//		record id.  All other record structures should derive from this base.
// --------------------------------------------------------------------------------
struct DbRecord
{
	DBRECID	RID;									// Record id
};

#endif // __DBSTRUCT_H__


