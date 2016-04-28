/**********************************************************************
* 鐗堟潈鎵€鏈?(C)2015, Zhou Zhaoxiong銆?
*
* 鏂囦欢鍚嶇О锛欸etConfig.c
* 鏂囦欢鏍囪瘑锛氭棤
* 鍐呭鎽樿锛氭紨绀篖inux涓嬮厤缃枃浠剁殑璇诲彇鏂规硶
* 鍏跺畠璇存槑锛氭棤
* 褰撳墠鐗堟湰锛歏1.0
* 浣?   鑰咃細Zhou Zhaoxiong
* 瀹屾垚鏃ユ湡锛?0150507
*
**********************************************************************/
#include "GetConfig.h"


/**********************************************************************
* 鍔熻兘鎻忚堪锛?鑾峰彇鍏蜂綋鐨勫瓧绗︿覆鍊?
* 杈撳叆鍙傛暟锛?fp-閰嶇疆鏂囦欢鎸囬拡
             pszSectionName-娈靛悕, 濡? GENERAL
             pszKeyName-閰嶇疆椤瑰悕, 濡? EmployeeName
             iOutputLen-杈撳嚭缂撳瓨闀垮害
* 杈撳嚭鍙傛暟锛?pszOutput-杈撳嚭缂撳瓨
* 杩?鍥?鍊硷細 鏃?
* 鍏跺畠璇存槑锛?鏃?
* 淇敼鏃ユ湡      鐗堟湰鍙?       淇敼浜?      淇敼鍐呭
* ------------------------------------------------------------------
* 20150507       V1.0     Zhou Zhaoxiong     鍒涘缓
********************************************************************/
void GetStringContentValue(FILE *fp, UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pszOutput, UINT32 iOutputLen)
{
    UINT8  szSectionName[100]    = {0};
    UINT8  szKeyName[100]        = {0};
    UINT8  szContentLine[256]    = {0};
    UINT8  szContentLineBak[256] = {0};
    UINT32 iContentLineLen       = 0;
    UINT32 iPositionFlag         = 0;

    // 鍏堝杈撳叆鍙傛暟杩涜寮傚父鍒ゆ柇
    if (fp == NULL || pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL)
    {
        printf("GetStringContentValue: input parameter(s) is NULL!\n");
        return;
    }

    sprintf(szSectionName, "[%s]", pszSectionName);
    strcpy(szKeyName, pszKeyName);

    while (feof(fp) == 0)
    {
        memset(szContentLine, 0x00, sizeof(szContentLine));
        fgets(szContentLine, sizeof(szContentLine), fp);      // 鑾峰彇娈靛悕

        // 鍒ゆ柇鏄惁鏄敞閲婅(浠?寮€澶寸殑琛屽氨鏄敞閲婅)鎴栦互鍏朵粬鐗规畩瀛楃寮€澶寸殑琛?
        if (szContentLine[0] == ';' || szContentLine[0] == '\r' || szContentLine[0] == '\n' || szContentLine[0] == '\0')
        {
            continue;
        }

        // 鍖归厤娈靛悕
        if (strncasecmp(szSectionName, szContentLine, strlen(szSectionName)) == 0)     
        {
            while (feof(fp) == 0)
            {
                memset(szContentLine,    0x00, sizeof(szContentLine));
                memset(szContentLineBak, 0x00, sizeof(szContentLineBak));
                fgets(szContentLine, sizeof(szContentLine), fp);     // 鑾峰彇瀛楁鍊?

                // 鍒ゆ柇鏄惁鏄敞閲婅(浠?寮€澶寸殑琛屽氨鏄敞閲婅)
                if (szContentLine[0] == ';')
                {
                    continue;
                }

                memcpy(szContentLineBak, szContentLine, strlen(szContentLine));

                // 鍖归厤閰嶇疆椤瑰悕
                if (strncasecmp(szKeyName, szContentLineBak, strlen(szKeyName)) == 0)     
                {
                    iContentLineLen = strlen(szContentLine);
                    for (iPositionFlag = strlen(szKeyName); iPositionFlag <= iContentLineLen; iPositionFlag ++)
                    {
                        if (szContentLine[iPositionFlag] == ' ')
                        {
                            continue;
                        }
                        if (szContentLine[iPositionFlag] == '=')
                        {
                            break;
                        }

                        iPositionFlag = iContentLineLen + 1;
                        break;
                    }

                    iPositionFlag = iPositionFlag + 1;    // 璺宠繃=鐨勪綅缃?

                    if (iPositionFlag > iContentLineLen)
                    {
                        continue;
                    }

                    memset(szContentLine, 0x00, sizeof(szContentLine));
                    strcpy(szContentLine, szContentLineBak + iPositionFlag);

                    // 鍘绘帀鍐呭涓殑鏃犲叧瀛楃
                    for (iPositionFlag = 0; iPositionFlag < strlen(szContentLine); iPositionFlag ++)
                    {
                        if (szContentLine[iPositionFlag] == '\r' || szContentLine[iPositionFlag] == '\n' || szContentLine[iPositionFlag] == '\0')
                        {
                            szContentLine[iPositionFlag] = '\0';
                            break;
                        }
                    }

                    // 灏嗛厤缃」鍐呭鎷疯礉鍒拌緭鍑虹紦瀛樹腑
                    strncpy(pszOutput, szContentLine, iOutputLen-1);
                    break;
                }
                else if (szContentLine[0] == '[')
                {
                    break;
                }
            }
            break;
        }
    }
}


/**********************************************************************
* 鍔熻兘鎻忚堪锛?浠庨厤缃枃浠朵腑鑾峰彇瀛楃涓?
* 杈撳叆鍙傛暟锛?pszSectionName-娈靛悕, 濡? GENERAL
             pszKeyName-閰嶇疆椤瑰悕, 濡? EmployeeName
             pDefaultVal-榛樿鍊?
             iOutputLen-杈撳嚭缂撳瓨闀垮害
             pszConfigFileName-閰嶇疆鏂囦欢鍚?
* 杈撳嚭鍙傛暟锛?pszOutput-杈撳嚭缂撳瓨
* 杩?鍥?鍊硷細 鏃?
* 鍏跺畠璇存槑锛?鏃?
* 淇敼鏃ユ湡      鐗堟湰鍙?      淇敼浜?       淇敼鍐呭
* ------------------------------------------------------------------
* 20150507       V1.0     Zhou Zhaoxiong     鍒涘缓
********************************************************************/  
void GetConfigFileStringValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pDefaultVal, UINT8 *pszOutput, UINT32 iOutputLen, UINT8 *pszConfigFileName)
{
    FILE  *fp                    = NULL;
    UINT8  szWholePath[256]      = {0};

    // 鍏堝杈撳叆鍙傛暟杩涜寮傚父鍒ゆ柇
    if (pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL || pszConfigFileName == NULL)
    {
        printf("GetConfigFileStringValue: input parameter(s) is NULL!\n");
        return;
    }

    // 鑾峰彇榛樿鍊?
    if (pDefaultVal == NULL)
    {
        strcpy(pszOutput, "");
    }
    else
    {
        strcpy(pszOutput, pDefaultVal);
    }

    // 鎵撳紑閰嶇疆鏂囦欢
	memcpy(szWholePath, pszConfigFileName, strlen(pszConfigFileName));
    fp = fopen(szWholePath, "r");
    if (fp == NULL)
    {
        printf("GetConfigFileStringValue: open %s failed!\n", szWholePath);
        return;
    }

    // 璋冪敤鍑芥暟鐢ㄤ簬鑾峰彇鍏蜂綋閰嶇疆椤圭殑鍊?
    GetStringContentValue(fp, pszSectionName, pszKeyName, pszOutput, iOutputLen);

    // 鍏抽棴鏂囦欢
    fclose(fp);
    fp = NULL;
}


/**********************************************************************
* 鍔熻兘鎻忚堪锛?浠庨厤缃枃浠朵腑鑾峰彇鏁村瀷鍙橀噺
* 杈撳叆鍙傛暟锛?pszSectionName-娈靛悕, 濡? GENERAL
             pszKeyName-閰嶇疆椤瑰悕, 濡? EmployeeName
             iDefaultVal-榛樿鍊?
             pszConfigFileName-閰嶇疆鏂囦欢鍚?
* 杈撳嚭鍙傛暟锛?鏃?
* 杩?鍥?鍊硷細 iGetValue-鑾峰彇鍒扮殑鏁存暟鍊?  -1-鑾峰彇澶辫触
* 鍏跺畠璇存槑锛?鏃?
* 淇敼鏃ユ湡      鐗堟湰鍙?      淇敼浜?       淇敼鍐呭
* ------------------------------------------------------------------
* 20150507       V1.0     Zhou Zhaoxiong     鍒涘缓
********************************************************************/  
INT32 GetConfigFileIntValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT32 iDefaultVal, UINT8 *pszConfigFileName)
{
    UINT8  szGetValue[512] = {0};
    INT32  iGetValue       = 0;

    // 鍏堝杈撳叆鍙傛暟杩涜寮傚父鍒ゆ柇
    if (pszSectionName == NULL || pszKeyName == NULL || pszConfigFileName == NULL)
    {
        printf("GetConfigFileIntValue: input parameter(s) is NULL!\n");
        return -1;
    }

    GetConfigFileStringValue(pszSectionName, pszKeyName, NULL, szGetValue, 512-1, pszConfigFileName);    // 鍏堝皢鑾峰彇鐨勫€煎瓨鏀惧湪瀛楃鍨嬬紦瀛樹腑

    if (szGetValue[0] == '\0' || szGetValue[0] == ';')    // 濡傛灉鏄粨鏉熺鎴栧垎鍙? 鍒欎娇鐢ㄩ粯璁ゅ€?
    {
        iGetValue = iDefaultVal;
    }
    else
    {
        iGetValue = atoi(szGetValue);
    }

    return iGetValue;
}
