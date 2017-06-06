
#include "stdint.h"
#include "string.h"
// #include "App_config.h"
#include "library.h"
// #include "macro.h"
// #include "ui_config.h"
#include "CS99xx.h"


/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ��Ϊ�����뺯�� ����ѭ�����жϷ����������ô˺���ʱ ���������                                                               
 *             ������Դ���ƻ�
 *             The maximal possible delay is 768 us / F_CPU in MHz                                                               
 *                                                                            
******************************************************************************/

void LIB_DelayUs(uint16_t delayUs)
{
    uint8_t			i;

	for (i = 0; i < delayUs; i++)
	{
		__nop();
	}
	//���ô˺�������׼ȷ��ʱ
	//_delay_us(delayUs);


}

void LIB_SoftDelayUs(uint16_t delayUs)
{
    LIB_DelayUs(delayUs);
}
/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : �����ʱ ��ʱֵ��׼ȷ ���ݾ���Ҫ�����                                                               
 *             The maximal possible delay is 262.14 ms / F_CPU in MHz                                                               
 *                                                                            
******************************************************************************/

void LIB_DelayMs(uint16_t delayMs)
{
    uint32_t      interger                                = delayMs/20;
//	uint32_t      remain                                  = delayMs%20;
	uint32_t      i                                       = 0;
	
	for (; i < interger; i++)
	{
		//_delay_ms(20);
	}
	//_delay_ms(remain);
}

void LIB_SoftDelayMs(uint16_t delayMs)
{
	uint8_t delayi;
	while(--delayMs)
	{
		for(delayi=0;delayi<124;delayi++);
	}
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

uint32_t LIB_MonitorUs(uint32_t monitorUs)
{
    //���ô˺������м��
	//return (__HAL_MonitorTmrStart(monitorUs));
	return 1;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��ȡ10��n����                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : û�в��ñ�־��������Ϊ��׼�������ø������� ������                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

uint32_t LIB_Get10nData(uint32_t n)
{
    uint32_t rt 					                                = 1;
	
    for (; n > 0; n--)
    {
        rt 					                                   *= 10;    
    }
    return rt;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���ݲ��                                                              
 *                                                                           
 *  ��ڲ��� : rscData:Ҫ��ֵ����� splitLen��������ݵĳ��� pdestSaveAddr����ֺ�����ݴ�ŵ�ַ                                                               
 *                                                                             
 *  ���ڲ��� : ��                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ������LIB_DataSplit(4096, 4, saveData);                                                            
 *                   4096 ��ֺ�����saveData������  saveData[0] = 4 saveData[1] = 0
 *                                                    saveData[2] = 9 saveData[3] = 6          
 *                                                                            
******************************************************************************/

void LIB_DataSplit(uint32_t rscData, uint32_t splitLen, uint8_t *pdestSaveAddr)
{
    uint32_t TmpFor10n     = 0;
    splitLen--;

    for (; splitLen > 0; splitLen--)
    {
        TmpFor10n        = LIB_Get10nData(splitLen);						//ȡ10��n����
        *pdestSaveAddr   = rscData/TmpFor10n;
        rscData         %= TmpFor10n; 
        pdestSaveAddr++;   
    }

    *pdestSaveAddr = rscData;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���ݼ���                                                              
 *                                                                           
 *  ��ڲ��� : ����Ϊ�ַ���                                                              
 *                                                                             
 *  ���ڲ��� : ��                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

uint32_t LIB_DataCalculate(const uint8_t *rscStr, uint32_t calculateLen)
{
    uint32_t i           = 0;
	uint32_t returnValue = 0;

	for (; i < calculateLen; i++)
	{
		returnValue += ((rscStr[calculateLen - i - 1] - 0x30)*LIB_Get10nData(i));	
	}
	
	return returnValue;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �����ַ�����ĳ���ַ���λ��                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� : ���ҵ� �򷵻�λ�������� ���Ҳ��� �򷵻�-1                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                            
 *                                                                        
 *                                                                            
******************************************************************************/

int8_t LIB_Strpos(const uint8_t *rscStr, uint8_t c)
{
    uint32_t i = 0;
	
    for(; i < strlen((const char *)rscStr); i++)
    {
        if (rscStr[i] == c)
        {
            return i;
        }
        else
        {
            continue;
        }
    } 

    return -1;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���ַ�����ĳһλ�ô�����һ�ַ���                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                                                                        
 *                                                                            
******************************************************************************/

uint32_t LIB_StrInsert(uint8_t *rscStr, uint8_t insertPosIndex, uint8_t insertChar)
{
    uint32_t   i             = 0;
    uint32_t   j             = strlen((const char *)rscStr) - 1;              //ת��Ϊ������

    if (insertPosIndex > j)
	{
		return 0;
	}
	
	for (i = j; i >= insertPosIndex; i--)
    {
        rscStr[i + 1]             = rscStr[i];
    }
    rscStr[insertPosIndex]        = insertChar;
	
	return 1;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��������ַ�����ĳ���ַ���λ��                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� : ���ҵ� �򷵻�λ�������� ���Ҳ��� �򷵻�-1                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                                                                        
 *                                                                            
******************************************************************************/

int8_t LIB_StrBkpos(const uint8_t *rscStr, uint32_t c)
{
    uint32_t   i = 0;
    uint32_t   j = strlen((const char *)rscStr);

    for(; i < j; i++)
    {
        if (rscStr[j-i-1] == c)
        {
            return j-i-1;
        }
        else
        {
            continue;
        }
    } 

    return -1;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �����ַ�����ǰn�ַ���ĳ���ַ���λ��                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� : ���ҵ� �򷵻�λ�������� ���Ҳ��� �򷵻�-1                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                                                                        
 *                                                                            
******************************************************************************/
void LIB_Set_Zero(uint16_t *rscStr,uint32_t len)
{
	uint32_t 	i 			= 0;
	
    for(; i < len; i++)
    {
        if (rscStr[i] != 0)
        {
            rscStr[i] = 0;
        }
	}
}


int8_t LIB_Strnpos(const uint8_t *rscStr, uint32_t c, uint32_t len)
{
    uint32_t 	i 			= 0;
	
    for(; i < len; i++)
    {
        if (rscStr[i] == c)
        {
            return i;
        }
        else
        {
            continue;
        }
    } 

    return -1;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ������ת��Ϊ�ַ�                                                              
 *                                                                           
 *  ��ڲ��� : prscSaveAddr:���ִ�ŵ�ַ Ҳ��ת�����ַ���ŵ�ַ convertLen��ת������                                                               
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ������uint8_t numb[4] = {4, 0, 9, 6}; LIB_ConvertNmubToChar(numb, 4);                                                               
 *                   ת����numb[0] = '4' numb[1] = '0' numb[2] = '9' numb[3] = '6'                                                      
 *                                                                            
******************************************************************************/

void LIB_ConvertNmubToChar(uint8_t *prscSaveAddr, uint32_t convertLen)
{
    uint32_t 	i 				= 0;
	
    for (; i < convertLen; i++)
    {
        *(prscSaveAddr + i) 	   += '0';    
    }        
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��ʱδ������                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

int8_t LIB_SearchNoneNumbIndex(uint8_t *rscStr, uint32_t strLen)
{
    uint32_t   i  		= 0;
	
    for (; i < strLen; i++)
    {
        if (rscStr[i] == '0')
        {
            continue;
        }
        else if (rscStr[i] == '.')
        {
            //��һ���ַ�������Ϊ.s
            return (i - 1);
        }
        else
        {
            return (i);
        }
    }
    
    return -1;
}

void LIB_HideZeroForString(uint8_t *rscStr, uint32_t strLen)
{
    uint32_t i   		= 0;
    int8_t 		  len  		= LIB_SearchNoneNumbIndex(rscStr, strLen);
    
    if (-1 == len)
    {
        len = strLen - 1;
    }
    //����ǰ�����
    //���һλ����������
    for (; i < len; i++)
    {            
        if ('0' == rscStr[i])
        {
            rscStr[i] = ' ';
        }
        else
        {
            break;
        }
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ������ת��Ϊ�ַ��� ����ȥǰ�����                                                                
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ��ڲ��� rscDataֻ����������                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

void LIB_ConvertNmubToCharWithSpace(uint32_t rscData, uint32_t splitLen, uint8_t *pdestSaveAddr)
{
	LIB_ConvertNmubToString(rscData, splitLen, pdestSaveAddr);
    LIB_HideZeroForString(pdestSaveAddr, splitLen);
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ������ת��Ϊ�ַ���                                                                 
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ��ڲ��� rscDataֻ����������                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

void LIB_ConvertNmubToString(uint32_t rscData, uint32_t splitLen, uint8_t *pdestSaveAddr)
{
    LIB_DataSplit(rscData, splitLen, pdestSaveAddr);
    LIB_ConvertNmubToChar(pdestSaveAddr, splitLen);
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��ȡһ�ֽں�У��ֵ  ��������ļ�ϵͳ���ֻ�ú�У��ֵ                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

uint8_t LIB_GetChkSum(uint8_t *rscAddr, uint32_t len)
{
	/*
    typedef struct
    {
        uint8_t           m_chkLowSum;
        uint8_t           m_chkHighSum;

    }tChkSum;
    typedef union
    {
        uint16_t          u_chkSum;
        tChkSum         u_tchkSum;

    }uChkSum;
    
    uint32_t   i            = 0;
    uChkSum 		u_chkSum     = {0}; 

	for (; i < len; i++)
	{
        u_chkSum.u_tchkSum.m_chkHighSum ^= *(rscAddr + i);
        u_chkSum.u_tchkSum.m_chkLowSum  += *(rscAddr + i);	
	}

	return u_chkSum.u_chkSum;
    */
    uint8_t           chkSum                                      = 0;
    uint32_t          i                                           = 0;  
    for (; i < len; i++)
		{
        chkSum                                                 += *(rscAddr + i);
    }
    return chkSum|0x80;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��֤�����ַ����Ϸ�                                                              
 *                                                                           
 *  ��ڲ��� : ����Ϊ�����ַ���                                                              
 *                                                                             
 *  ���ڲ��� : ��                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

#ifdef			LIB__CHK_DATA_STR_VALID

uint32_t LIB_ChkDateStrValid(uint8_t *rscDateStr)
{
	uint8_t       year                                            = LIB_DataCalculate((void *)&rscDateStr[0], 4);
    uint8_t       month                                           = LIB_DataCalculate((void *)&rscDateStr[5], 2);
    uint8_t       day                                             = LIB_DataCalculate((void *)&rscDateStr[8], 2);
    uint8_t       maxValue                                        = 31;

	//�ж���
    if (year > 4095)
    {
    	return FALSE;
    }
    //�ж���
    if ((month > 12) || (month < 1))
    {
        return FALSE;
    }
            
    //�ж���  
    if ((month == 4) || (month == 6) || (month == 9) || (month == 11))
	{
		maxValue                                                = 30;
	} 
    //����·�Ϊ2�·� �� ������
	else if (month == 2)
	{
	    if ((year % 4) == 0)
		{
			maxValue                                            = 29;
		}
		else
		{
		    maxValue                                            = 28;
		}
	}
	//�ж���
	if ((day > maxValue) || (day < 1))
	{
		return FALSE;
	}
	
	return TRUE;
}
#endif

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��֤ʱ���ַ����Ϸ�                                                              
 *                                                                           
 *  ��ڲ��� : ����Ϊʱ���ַ���                                                              
 *                                                                             
 *  ���ڲ��� : ��                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

#ifdef			LIB__CHK_DATA_STR_VALID

uint32_t LIB_ChkTimeStrValid(uint8_t *rscTimeStr)
{
	uint8_t       hour        		                            = LIB_DataCalculate((void *)&rscTimeStr[0], 2);;
    uint8_t       minute      		                            = LIB_DataCalculate((void *)&rscTimeStr[3], 2);;
    uint8_t       second      		                            = LIB_DataCalculate((void *)&rscTimeStr[6], 2);;
	
	if ((hour > 23) || (minute > 59) || (second > 59))
	{
		return FALSE;
	}

	return TRUE;
}
#endif

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �ַ���ѭ������                                                              
 *                                                                           
 *  ��ڲ��� :                                                              
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

void LIB_StringRor(uint8_t *rscAddr, uint32_t len, uint32_t rorCnt)
{
    uint32_t       i           = 0;
    uint32_t       j           = 0;
    uint32_t       tmp         = 0;

    for (; i < rorCnt; i++)
    {
        tmp                         = rscAddr[len - 1];
        for (j = 1; j < len; j++)
        {
            rscAddr[len - j]        = rscAddr[len - j - 1];
        }
        rscAddr[0]                  = tmp;
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �ַ���ѭ������                                                              
 *                                                                           
 *  ��ڲ��� :                                                              
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

void LIB_StringRol(uint8_t *rscAddr, uint32_t len, uint32_t rolCnt)
{
    uint32_t       i           = 0;
    uint32_t       j           = 0;
    uint32_t       tmp         = 0;

    for (; i < rolCnt; i++)
    {
        tmp                         = rscAddr[0];
        for (j = 0; j < len - 1; j++)
        {
            rscAddr[j]              = rscAddr[j + 1];
        }
        rscAddr[j]                  = tmp;
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �ַ����߼�����                                                              
 *                                                                           
 *  ��ڲ��� :                                                              
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

void LIB_StringLsr(uint8_t *rscAddr, uint32_t len, uint32_t lsrCnt)
{
    uint32_t       i           = 0;
    uint32_t       j           = 0;

    for (; i < lsrCnt; i++)
    {
        for (j = 1; j < len; j++)
        {
            rscAddr[len - j]        = rscAddr[len - j - 1];
        }
        rscAddr[0]                  = '0';
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �ַ����߼�����                                                              
 *                                                                           
 *  ��ڲ��� :                                                              
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

void LIB_StringLsl(uint8_t *rscAddr, uint32_t len, uint32_t lsrCnt)
{
    uint32_t       i           = 0;
    uint32_t       j           = 0;

    for (; i < lsrCnt; i++)
    {
        for (j = 0; j < len; j++)
        {
            rscAddr[j]              = rscAddr[j + 1];
        }
        rscAddr[len]                = 0;
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �ַ������ַ����                                                              
 *                                                                           
 *  ��ڲ��� :                                                              
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

void LIB_StringReplace(uint8_t *rscAddr, uint32_t len, uint32_t rscChar, uint32_t destChar)
{
    uint32_t       i           = 0;

    for (; i < len; i++)
    {
        if (rscAddr[i] == rscChar)
        {
            rscAddr[i]              = destChar;
        }
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �ַ����ȽϺ��� �Ƚ����޶��ĳ���                                                             
 *                                                                           
 *  ��ڲ��� :                                                              
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
******************************************************************************/

uint32_t LIB_Strncmp(const char *prscStr, const char *pcmpStr, uint32_t count)
{
    register signed char result = 0;
    
    while (count)
	{
		if ((result = *prscStr - *pcmpStr++) != 0 || !*prscStr++)
			break;
		count--;
	}

	return result;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��ȡ����λ����                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                               
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                                                                        
 *                                                                            
******************************************************************************/

uint8_t LIB_DataBitLenGet(uint32_t rscData)
{
    uint8_t                 dataBitLen                            = 1;

    //��ȡ��������Ӧ��λ��
    while (rscData/10 >= 1)
    {                                                           
        rscData                                                 = rscData/10;
        dataBitLen++;
    }
    return dataBitLen;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���ݲ�ֺ���                                                              
 *                                                                           
 *  ��ڲ��� : rscData--Ҫ��ֵ����� length--������ݵĳ��� pdestAddr--��ֺ�����ݴ�ŵ�ַ                                                               
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                   
 *                                                                                                                                          
******************************************************************************/
void Util_DataSplit(uint32_t rscData, uint8_t length, int8_t *pdestAddr)
{
    uint32_t    temp    = 0;
    

    length--;

	for (; length > 0; length--)
    {
        temp          = LIB_Get10nData(length);
       *pdestAddr     = rscData/temp;
        rscData      %= temp; 
        pdestAddr++;   
    }

    *pdestAddr = rscData;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ������� ת��Ϊ�ַ� ���ȿ���ָ�� ���С���� ���ز���Ҫ��ʾ����                                                              
 *                                                                           
 *  ��ڲ��� : rscData--Ҫ��ֵ����� length--������ݵĳ��� pdestAddr--��ֺ�����ݴ�ŵ�ַ                                                               
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                   
 *                                                                                                                                          
******************************************************************************/

void Util_MultifunctionalDataSplit(uint32_t rscData, uint8_t length, int8_t *pdestAddr, uint8_t dotPos)
{
    int8_t    temp[10] = {0};


    if (dotPos >= length)
    {
        return;
    }

    Util_DataSplit(rscData, length, pdestAddr);
    LIB_ConvertNmubToChar((uint8_t *)pdestAddr, length);

    if (0 != dotPos)
    {
        strcpy((void *)temp, (void *)(&pdestAddr[dotPos]));
        strcpy((void *)(&pdestAddr[dotPos]), ".");
        strcpy((void *)(&pdestAddr[dotPos + 1]), (void *)temp);
    }
    //else
    //{
    //    strcat((void *)pdestAddr, " ");
    //}
}
/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ������������                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :2012.5.29                                                                
 *                                                                                                                                       
 *  ��    ע :                                                              
 *                                                                        
 *                                                                            
******************************************************************************/
extern char lock_flag;
void LIB_LockKeyFuction(void)
{
	
	rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_STATUS_EVENT | (1));
	system_parameter_t.Com_lock = 1;
	
}

void LIB_OpenKeyFuction(void)
{

	rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_STATUS_EVENT | (0));
	system_parameter_t.Com_lock = 0;
	
}


/*
 *�������ܣ�Int���͵�Str��ת��
 *����������ַ�ָ��, Int_num:����λ��, Dec_num:С��λ��, value:��ֵ
 *�����������
 *��    ע����
 */
void IntToStr(char *pc, u8 Int_num, u8 Dec_num, u32 value)
{
	char temp[10];
	u8  i=0;
	u32 base = 1;
	for(i=0; i<(Int_num+Dec_num); i++)
	{
		temp[i] = (value / base)%10+'0';
		base *= 10;
	}
	for(i=(Int_num+Dec_num);i>0;i--)
	{
		*pc++ = temp[i-1];
		if(Dec_num>0 && i==(Dec_num+1))*pc++ = '.';
	}		
	*pc = '\0';
}

void stritem(char *str)
{
	u8 i;
	char strt[100];
	char *pstr=strt;
	strcpy(strt,str);
	for(i=0;i<strlen(strt);i++)
	{
		if(*pstr != ' ')
		*str++=*pstr;
		pstr++;
	}
	*str = 0;
}
