/**********************************************************************
* 版权所有 (C)2016, Zhou Zhaoxiong。
*
* 文件名称：FileGather.c
* 文件标识：无
* 内容摘要：将各个目录中前缀相同的文件集中到一个目录中
* 其它说明：无
* 当前版本：V1.0
* 作    者：Zhou Zhaoxiong
* 完成日期：20160427
*
**********************************************************************/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
#include <time.h>
#include <unistd.h>

#include "GetConfig.h"

// 结构体定义
// 用于保存扫描目录
typedef struct
{
    UINT8  szScanedDir[256];
} T_ScanedDirInfo;

// 全局配置
typedef struct
{
    UINT32          iScanedDirNum;        // 扫描目录个数
    T_ScanedDirInfo tScanedDirInfo[20];   // 扫描目录具体信息(最大支持20个)
    UINT8           szGatherDir[256];     // 汇总目录具体信息
    UINT32          iScanDirInterval;     // 扫描目录的间隔
    UINT8           szFilePrefix[20];     // 需要进行汇总的文件的前缀
    UINT8           szFileSuffix[20];     // 需要进行汇总的文件的后缀
} T_ConfigInfo;

// 全局变量
T_ConfigInfo g_tConfigInfo = {0};


// 函数声明
INT32 SelectFlies(struct dirent *pDir);
void ScanDirAndGather(void);
void Sleep(UINT32 iCountMs);
INT32 ReadIniFile(UINT8 *pszIniFile);


/****************************************************************
* 功能描述: 主函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0-执行完成
* 其他说明: 无
* 修改日期       版本号        修改人        修改内容
* -------------------------------------------------------------
* 20160427        V1.0     Zhou Zhaoxiong     创建
****************************************************************/
INT32 main(void)
{
    INT32  iRetValue      = 0;
    UINT8  szIniFile[128] = {0};
    UINT8 *pszHomePath    = NULL;

    pszHomePath = (UINT8 *)getenv("HOME");     // 获取当前用户所在的主目录
    if (pszHomePath == NULL)
    {
        return -1;
    }

    // 获取带路径的配置文件名
    snprintf(szIniFile, sizeof(szIniFile)-1, "%s/zhouzx/Test/FileGather/etc/Config.ini", pszHomePath);

    // 循环读取配置文件及汇总文件
    while (1)
    {
        iRetValue = ReadIniFile(szIniFile);
        if (iRetValue != 0)   // 读取配置失败
        {
            printf("Exec ReadIniFile to get config values failed!\n");
            return -1;
        }

        ScanDirAndGather();          // 扫描目录并汇总前缀相同的文件

        Sleep(g_tConfigInfo.iScanDirInterval * 1000);    // 每隔一段时间执行一次文件的汇总
    }

    return 0;
}


/**********************************************************************
* 功能描述：根据前缀和后缀选择文件
* 输入参数：dir-目录
* 输出参数：无
* 返 回 值：0-失败   1-成功
* 其它说明：无
* 修改日期         版本号      修改人          修改内容
* --------------------------------------------------------------------
* 20160427         V1.0    ZhouZhaoxiong        创建
***********************************************************************/
INT32 SelectFlies(struct dirent *pDir)
{
    INT32 iLoopFlag     = 0;
    INT32 iSelectResult = 0;

    if (pDir == NULL)
    {
        printf("SelectFlies:input parameter is NULL!\n");
        return 0;
    }

    // 匹配文件前缀和后缀
    iSelectResult = ((0 == strncmp(pDir->d_name, g_tConfigInfo.szFilePrefix, strlen(g_tConfigInfo.szFilePrefix))) 
                     && ((strncmp(&pDir->d_name[strlen(pDir->d_name) - strlen(g_tConfigInfo.szFileSuffix)], g_tConfigInfo.szFileSuffix, strlen(g_tConfigInfo.szFileSuffix)) == 0)));

    if (iSelectResult == 1)            // 找到了匹配前缀的文件
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**********************************************************************
* 功能描述：扫描目录并汇总前缀相同的文件
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期         版本号      修改人          修改内容
* --------------------------------------------------------------------
* 20160427         V1.0    ZhouZhaoxiong        创建
***********************************************************************/
void ScanDirAndGather(void)
{
    INT32  iScanDirRet           = 0;
    UINT32 iDirIdx               = 0;
    UINT32 iFileIdx              = 0;
    UINT32 iFileCount            = 0;
    UINT32 iScanedNoFileDirCount = 0;
    UINT32 iFileSize             = 0;
    UINT8  szScanedFile[512]     = {0};
    UINT8  szGatherFile[512]     = {0};
    UINT8  szCmdBuf[256]         = {0};
    FILE  *fp                    = NULL;
    struct dirent **ppDirEnt     = NULL;

    // 依次扫描各个目录, 并汇总文件
    for (iDirIdx = 0; iDirIdx < g_tConfigInfo.iScanedDirNum; iDirIdx ++)
    {
        iScanDirRet = scandir(g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir, &ppDirEnt, SelectFlies, alphasort);
        if (iScanDirRet < 0)   // 扫描目录出错
        {
            printf("ScanDirAndGather:exec scandir failed, path=%s\n", g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir);
            return;
        }
        else if (iScanDirRet == 0)   // 目录下无文件
        {
            printf("ScanDirAndGather:no satisfied file in directory %s\n", g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir);

            iScanedNoFileDirCount ++;
            if (iScanedNoFileDirCount >= g_tConfigInfo.iScanedDirNum)    // 表示所有目录下均无满足条件的文件
            {
                printf("ScanDirAndGather:scaned no satisfied files in all %d dirs\n", iScanedNoFileDirCount);
                return;
            }
        }
        else          // 将满足条件的文件移动到汇总目录中
        {
            for (iFileIdx = 0; iFileIdx < iScanDirRet; iFileIdx ++)
            {
                // 先判断扫描到的文件是否为空文件, 是则直接删除, 不是才执行移动的操作
                memset(szScanedFile, 0x00, sizeof(szScanedFile));
                snprintf(szScanedFile, sizeof(szScanedFile) - 1, "%s/%s", g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir, ppDirEnt[iFileIdx]->d_name);
                fp = fopen(szScanedFile, "r");
                if (fp == NULL)          // 打开文件失败, 直接返回
                {
                    printf("ScanDirAndGather:open file %s failed, please check!\n", szScanedFile);
                    return;
                }
                fseek(fp, 0, SEEK_END);
                iFileSize = ftell(fp);
                if (iFileSize == 0)     // 该文件为空文件
                {
                    printf("ScanDirAndGather:%s is an empty file, so delete it directly!\n", szScanedFile);
                    memset(szCmdBuf, 0x00, sizeof(szCmdBuf));
                    snprintf(szCmdBuf, sizeof(szCmdBuf) - 1, "rm %s", szScanedFile);
                    system(szCmdBuf);
                }
                else
                {
                    // 判断要移动的文件在汇总目录下是否已经存在, 是则直接删除, 不是才移动
                    memset(szGatherFile, 0x00, sizeof(szGatherFile));
                    snprintf(szGatherFile, sizeof(szGatherFile) - 1, "%s/%s", g_tConfigInfo.szGatherDir, ppDirEnt[iFileIdx]->d_name);
                    if ((access(szGatherFile, 0)) != 0)      // 汇总目录下文件不存在
                    {
                        memset(szCmdBuf, 0x00, sizeof(szCmdBuf));
                        snprintf(szCmdBuf, sizeof(szCmdBuf) - 1, "mv %s %s", szScanedFile, g_tConfigInfo.szGatherDir);
                        system(szCmdBuf);
                
                        printf("ScanDirAndGather:now, %s\n", szCmdBuf);
            
                        iFileCount ++;
                    }
                    else
                    {
                        printf("ScanDirAndGather:%s has already existed in %s, so delete it directly!\n", ppDirEnt[iFileIdx]->d_name, g_tConfigInfo.szGatherDir);
                        memset(szCmdBuf, 0x00, sizeof(szCmdBuf));
                        snprintf(szCmdBuf, sizeof(szCmdBuf) - 1, "rm %s", szScanedFile);
                        system(szCmdBuf);
                    }
                }
            }
        }
    }

    printf("ScanDirAndGather:this time,totally moved %d file(s) to %s\n", iFileCount, g_tConfigInfo.szGatherDir);

    return;
}


/*******************************************************************
* 功能描述： 程序休眠
* 输入参数： iCountMs-休眠时间(单位:ms)
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期      版本号       修改人        修改内容
* ------------------------------------------------------------------
* 20160427       V1.0     Zhou Zhaoxiong     创建
********************************************************************/ 
void Sleep(UINT32 iCountMs)
{
    struct timeval t_timeout = {0};

    if (iCountMs < 1000)
    {
        t_timeout.tv_sec  = 0;
        t_timeout.tv_usec = iCountMs * 1000;
    }
    else
    {
        t_timeout.tv_sec  = iCountMs / 1000;
        t_timeout.tv_usec = (iCountMs % 1000) * 1000;
    }
    select(0, NULL, NULL, NULL, &t_timeout);    // 调用select函数阻塞程序
}


/********************************************************************
* 功能描述： 读取配置文件中的各个配置项的值
* 输入参数： pszIniFile-配置文件
* 输出参数： 无
* 返 回 值： 0-成功 -1-失败
* 其它说明： 无
* 修改日期      版本号       修改人        修改内容
* ------------------------------------------------------------------
* 20160427       V1.0     Zhou Zhaoxiong     创建
********************************************************************/ 
INT32 ReadIniFile(UINT8 *pszIniFile)
{
    UINT32 iDirIdx        = 0;
    UINT8  szSectName[50] = {0};
    
    if (pszIniFile == NULL)
    {
        printf("ReadIniFile:input parameter is NULL!\n");
        return -1;
    }

    memset(&g_tConfigInfo, 0x00, sizeof(T_ConfigInfo));

    // 扫描目录个数
    g_tConfigInfo.iScanedDirNum = GetConfigFileIntValue("DIRINFO", "ScanedDirNum", 20, pszIniFile);
    if (g_tConfigInfo.iScanedDirNum > 20)    // 最多只有20个
    {
        printf("ReadIniFile:ScanedDirNum(%d) is more than 20, set it to 20 now!\n", g_tConfigInfo.iScanedDirNum);
        g_tConfigInfo.iScanedDirNum = 20;
    }
    if (g_tConfigInfo.iScanedDirNum == 0)
    {
        printf("ReadIniFile:ScanedDirNum is 0, please check!\n");
        return -1;
    }

    // 读取扫描目录具体信息
    for (iDirIdx = 0; iDirIdx < g_tConfigInfo.iScanedDirNum; iDirIdx ++)
    {
        memset(szSectName, 0x00, sizeof(szSectName));
        snprintf(szSectName, sizeof(szSectName) - 1, "ScanedDir%d", iDirIdx+1);

        GetConfigFileStringValue("DIRINFO", szSectName, "", g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir, sizeof(g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir), pszIniFile);
        if (strlen(g_tConfigInfo.tScanedDirInfo[iDirIdx].szScanedDir) == 0)     // 扫描目录未配置
        {
            printf("ReadIniFile:%s is not filled, please check!\n", szSectName);
            return -1;
        }
    }

    // 读取汇总目录具体信息
    GetConfigFileStringValue("DIRINFO", "GatherDir", "", g_tConfigInfo.szGatherDir, sizeof(g_tConfigInfo.szGatherDir), pszIniFile);
    if (strlen(g_tConfigInfo.szGatherDir) == 0)     // 汇总目录未配置
    {
        printf("ReadIniFile:GatherDir is not filled, please check!\n");
        return -1;
    }

    // 读取扫描目录的间隔, 默认60s
    g_tConfigInfo.iScanDirInterval = GetConfigFileIntValue("DIRINFO", "ScanDirInterval", 60, pszIniFile);
    if (g_tConfigInfo.iScanDirInterval == 0)
    {
        printf("ReadIniFile:ScanDirInterval is 0, please check!\n");
        return -1;
    }
    
    // 读取需要进行汇总的文件的前缀信息
    GetConfigFileStringValue("FILEINFO", "FilePrefix", "", g_tConfigInfo.szFilePrefix, sizeof(g_tConfigInfo.szFilePrefix), pszIniFile);
    if (strlen(g_tConfigInfo.szFilePrefix) == 0)     // 前缀信息未配置
    {
        printf("ReadIniFile:FilePrefix is not filled, please check!\n");
        return -1;
    }

    // 读取需要进行汇总的文件的后缀信息
    GetConfigFileStringValue("FILEINFO", "FileSuffix", "", g_tConfigInfo.szFileSuffix, sizeof(g_tConfigInfo.szFileSuffix), pszIniFile);
    if (strlen(g_tConfigInfo.szFileSuffix) == 0)     // 前缀信息未配置
    {
        printf("ReadIniFile:FileSuffix is not filled, please check!\n");
        return -1;
    }
    
    return 0;
}

