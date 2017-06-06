
#ifndef    _MACRO_H_
#define    _MACRO_H_

//L_SBI = LONG INT SBI

#define         L_SBI(reg, bit)                                 ((reg) |= (uint32)((uint32)1 << bit))
#define         L_CBI(reg, bit)                                 ((reg) &= (uint32)(~((uint32)1 << bit)))
#define         L_XBI(reg, bit)                                 ((reg) ^= (uint32)((uint32)1 << bit))
#define         L_BIT_IS_SET(reg, bit)                          (((reg & ((uint32)((uint32)1<<bit))) == ((uint32)((uint32)1<<bit)))? (1): (0))
#define         L_BIT_IS_CLEAR(reg, bit)                        (((reg & ((uint32)((uint32)1<<bit))) == ((uint32)((uint32)1<<bit)))? (0): (1))

//------------------------------------------------------------------------------
//S_SBI = SHORT INT SBI

#define         S_SBI(reg, bit)                                 ((reg) |= (uint16)((uint16)1 << bit))
#define         S_CBI(reg, bit)                                 ((reg) &= (uint16)(~((uint16)1 << bit)))
#define         S_XBI(reg, bit)                                 ((reg) ^= (uint16)((uint16)1 << bit))
#define         S_BIT_IS_SET(reg, bit)                          (((reg & ((uint16)((uint16)1<<bit))) == ((uint16)((uint16)1<<bit)))? (1): (0))
#define         S_BIT_IS_CLEAR(reg, bit)                        (((reg & ((uint16)((uint16)1<<bit))) == ((uint16)((uint16)1<<bit)))? (0): (1))

//------------------------------------------------------------------------------
//C_SBI = CHAR INT SBI

#define         C_SBI(reg, bit)                                 ((reg) |= (uint8)((uint8)1 << bit))
#define         C_CBI(reg, bit)                                 ((reg) &= (uint8)(~((uint8)1 << bit)))
#define         C_XBI(reg, bit)                                 ((reg) ^= (uint8)((uint8)1 << bit))
#define         C_BIT_IS_SET(reg, bit)                          (((reg & ((uint8)((uint8)1<<bit))) == ((uint8)((uint8)1<<bit)))? (1): (0))
#define         C_BIT_IS_CLEAR(reg, bit)                        (((reg & ((uint8)((uint8)1<<bit))) == ((uint8)((uint8)1<<bit)))? (0): (1))

//------------------------------------------------------------------------------
#define         XBYTE(addr)                                     (*(volatile unsigned char *)addr)
#define         XWORD(addr)                                     (*(volatile short unsigned int *)addr)
#define         XDWORD(addr)                                    (*(volatile long unsigned int *)addr)

//------------------------------------------------------------------------------
#define         MIN_GET(a,b)                                    ((a)<(b)?(a):(b))
#define         MAX_GET(a,b)                                    ((a)<(b)?(b):(a))
#define         CONVERT_CHAR_TO_NUMB(char)                      (char - '0')
#define         CONVERT_NUMB_TO_CHAR(numb)                      (numb + '0')

//------------------------------------------------------------------------------
//��ȡһά�ṹ�����鳤��
#define	        GET_ARRAY_COUNT(addr)					        (sizeof(addr)/sizeof(addr[0]))
//��ȡ��ά�ṹ������ÿһά����
#define	        GET_PLANAR_ARRAY_COUNT(addr)			        (sizeof(addr[0])/sizeof(addr[0][0]))
//ȡ�ṹ���ڳ�Ա����ƫ�Ƶ�ַ
#define         PARAM_OFFSET(Struct, Field) 				    ((unsigned int)(unsigned int*)&(((Struct *)0)->Field))

//------------------------------------------------------------------------------

typedef         void (*const FP_pfvoid)(void *);

//�涨�����ķ���ֵȫ����������ʵ��  ǧ��Ҫ��������ͬʱ���ֻ���������  0Ϊ�� 1Ϊ��
#ifndef    IDLE

    #define     IDLE                                             (1)
#endif

#ifndef    BUSY

    #define    BUSY                                             (0)
#endif

#ifndef    CLOSE

    #define    CLOSE                                            ('0')
#endif

#ifndef    OPEN

    #define    OPEN                                             ('1')
#endif

#ifndef	   NULL
	
	#define    NULL                                             (0)
#endif

#ifndef	   TRUE
	
	#define    TRUE                                             (1)
#endif

#ifndef	   FALSE
	
	#define    FALSE                                            (0)
#endif

#endif
