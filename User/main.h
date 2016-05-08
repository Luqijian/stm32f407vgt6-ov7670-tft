/**
  ******************************************************************************
  * @file    DCMI/Camera/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

  //����dcmi��lcdӲ��dma��ַ
#define DCMI_DR_ADDRESS       0x50050028
//#define FSMC_LCD_ADDRESS      ((u32)(0x60020000 | 0x0001FFFE))
#define FSMC_LCD_ADDRESS      ((u32)(0x6003FFFf))
#define FSMC_SRAM_ADDRESS     0x64000000
#define USART3_DR_ADDRESS     0x40004804 
//�����ʽ��RGB565��QVGA��ʽ
#define pixel_x    320//VGA=640*480,QVGA=320*240 ������320��������240  QVGA��Ҫ����320*240*2��
#define pixel_y    240//RGB565:��ɫ5λ����ɫ6λ����ɫ5λ��ÿ�����ص���16λ����Ҫ2������ʱ�����ڴ���
#define PICTURE_SIZE   (pixel_x*pixel_y)
#define BUFFER_SIZE    ((pixel_x*pixel_y)/2)

#define USE_LCD
//#define USE_UART

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_it.h"
#include "camera_api.h"
#include "OV7670.h"
#include "stm324xg_eval.h"
//#include "stm324xg_eval_lcd.h"
//#include "key_board.h"
//#include "camera_api.h"
#include "sccb.h"
//#include "stm324xg_eval_fsmc_sram.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void usart_param_init(void);
void UART_DMA_Init(uint32_t addr_offset);
//void LCD_PICTURE_TEST(void);
void DCMI_DMA_CONFIG(uint32_t addr_offset);
#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
