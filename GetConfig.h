/**********************************************************************
* 版权所有 (C)2016, Zhou Zhaoxiong。
*
* 文件名称：GetConfig.h
* 文件标识：无
* 内容摘要：读取配置项值的头文件
* 其它说明：无
* 当前版本：V1.0
* 作    者：Zhou Zhaoxiong
* 完成日期：20160427
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 重定义数据类型
typedef unsigned char   UINT8;
typedef signed   int    INT32;
typedef unsigned int    UINT32;

// 函数声明
void GetStringContentValue(FILE *fp, UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pszOutput, UINT32 iOutputLen);
void GetConfigFileStringValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pDefaultVal, UINT8 *pszOutput, UINT32 iOutputLen, UINT8 *pszConfigFileName);
INT32 GetConfigFileIntValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT32 iDefaultVal, UINT8 *pszConfigFileName);
