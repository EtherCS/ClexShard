/* User defined types */

#ifndef __USER_TYPES_H__
#define __USER_TYPES_H__

// Dependency ...
typedef struct Trace Dependency;

typedef struct Object {
	char Status[100];
	char Value[100];
	char VersionID[100];
} Object;

typedef struct OutputObject  {
	char* Labels[100];
	char* Object;
} OutputObject;

// Trace ...
typedef struct Trace{
	char*         ContractID;
	Dependency*   Dependencies[100];
	char*         InputObjectVersionIDs[100];
	char*         InputReferenceVersionIDs[100];
	OutputObject* OutputObjects[100];
	char*         Parameters[100];
	char*         Procedure;
	char*         Returns[100];
} Trace;

// Transaction ...
typedef struct Transaction{
	char*  Mapping_keys[100];
	char*  Mapping_vals[100];
	int    Signature_keys[100];
	char*  Signature_vals[100];
	Trace* Traces[100];
} Transaction;

#endif