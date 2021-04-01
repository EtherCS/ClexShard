/* example */
#include <user_types.h>

#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__
struct OutputObject ok_obj = {
    {NULL},
    "ok"
};
struct OutputObject ko_obj = {
    {NULL},
    "ko"
};
struct Trace fun1_trace = {
    "C1", /* ContractID */
    {NULL}, /* Dependencies */
    {NULL}, /* InputObjectVersionIDs */
    {NULL}, /* InputReferenceVersionIDs */
    {&ok_obj,NULL}, /* OutputObjects */
    {NULL}, /* Parameters */
    "func1", /* Procedure */
    {NULL}, /* Returns */
};
struct Trace fun2_trace = {
    "C2", /* ContractID */
    {NULL}, /* Dependencies */
    {NULL}, /* InputObjectVersionIDs */
    {NULL}, /* InputReferenceVersionIDs */
    {NULL}, /* OutputObjects */
    {NULL}, /* Parameters */
    "func2", /* Procedure */
    {NULL}, /* Returns */
};
struct Transaction fun2_tx = {
    {NULL}, /* Mapping_keys */
    {NULL}, /* Mapping_vals */
    {}, /* Signature_keys */
	{NULL}, /* Signature_vals */
	{
        &fun2_trace,
        NULL
    } /* Traces */
};

#endif