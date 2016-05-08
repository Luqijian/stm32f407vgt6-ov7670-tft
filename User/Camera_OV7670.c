/**
  ******************************************************************************
  * @file    Camera_OV7670.c 
  * @author  Jacky.Cheng
  * @version V1.0.0
  * @date    1-June-2012
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ����ͷ���ú�DCMIģʽ����
  ******************************************************************************
  */
#include "main.h"
#include "Sensor_config.h"
#include "sccb.h"
#include "lcd.h"

static uint16_t line_start=0;
static uint8_t  frame_cnt=0;
static uint8_t  frame_start=0;
__IO uint8_t  frame_done;
uint8_t ov_frame=0;//ͳ��֡��

void DCMI_Interface_Init(void)
{
  DCMI_InitTypeDef DCMI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable DCMI clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
  /* DCMI configuration */ 
  //DCMI��׽ģʽ�����ã�Ĭ��������ģʽ,�ῴ���������ƶ�ͼ��
  //�û���ʹ�õ�ʱ��ɵ���ΪSnapShot(ץ��)��Ҳ���ǵ�֡
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;//DCMI_CaptureMode_SnapShot;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_High;
  //ץ֡Ƶ�ʸ�Ϊ4֡��ץһ֡
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;//DCMI_CaptureRate_1of4_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;//����4������ʱ�ӣ�����32λ�֣�����1��DMA����

  /* Configures the DMA2 to transfer Data from DCMI */
  /* Enable DMA2 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  DCMI_DMA_CONFIG(0);
  
  //DCMI�ж����ã���ʹ��֡�жϻ��ߴ�ֱͬ���жϵ�ʱ���
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //ʹ��DCMI�ж�
	//ȫ���ж�IT_DCMI
  DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);//ʹ�ܴ�ֱ�ж�
  //DCMI_ITConfig(DCMI_IT_LINE, ENABLE);//ʹ��ˮƽ�ж�
  //DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);//ʹ��֡�ж�
  
  /* DCMI configuration */
  DCMI_Init(&DCMI_InitStructure);
          
   
//	DCMI_CaptureCmd(ENABLE);
//	DMA_Cmd(DMA2_Stream1, ENABLE);
	DCMI_Cmd(ENABLE);

}

/*DCMI�жϴ�����������ʹ�õ��Ǵ�ֱ��ˮƽͬ���жϣ��û��ɸ�����Ҫ�Լ������жϵķ�ʽ*/

void DCMI_IRQHandler(void)
{
	
	u8 zzz=0;
	zzz++;
	zzz++;
	
  
  //��������������������жϣ�����ȡ��Ҫ��֡����
  if( DCMI_GetITStatus(DCMI_IT_VSYNC)!= RESET)
  {
    DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
      
    if(frame_start)//�ȴ�֡��ʼ�ź�
    {  
       DCMI_CaptureCmd(ENABLE);//��captureλ��1ʱ������DMA�ӿڡ�DCMI_DR�Ĵ���ÿ�յ�һ��32λ���ݿ�ʱ������һ��DMA����
       DMA_Cmd(DMA2_Stream1, ENABLE); //2��DMA��������ÿ��DMA��������8����������ÿ����������8��ͨ�� 
			 LCD_SetCursor2(0x00,0x0000,0x013f,0x00ef);
			 LCD_WriteRAM_Prepare();
       while ((DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1) == RESET))//TCIF1�Ǵ�������жϱ�־��Ӳ����1�������0
       {
       }      
			 //��������ִ�б�ʾ�������������
       DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1);  //HTIF1�ǰ봫������жϱ�־��Ӳ����1�������0                                      
 //      DMA_Cmd(DMA2_Stream1, DISABLE);   
       //�ж�DMA�����Ƿ����   
 //      DCMI_ITConfig(DCMI_IT_VSYNC, DISABLE);//��ʹ�ܴ�ֱ�ж�
 //      DCMI_CaptureCmd(DISABLE);
       frame_done = 1;
			 ov_frame++;
//			 GPIO_ToggleBits(GPIOC, GPIO_Pin_7);
    }
    
    if(frame_cnt<15)//��15֡��ʼ�ɼ����ȴ�����ͷ�ȶ�
    {
      if(frame_cnt==14)
      { 
        //DCMI_CaptureCmd(DISABLE);
        //DMA_Cmd(DMA2_Stream1, ENABLE);
     //   STM_EVAL_LEDToggle(LED2);
        frame_start = 1;//ʹ��֡�ɼ�
        //DCMI_CaptureCmd(ENABLE);
        //DMA_Cmd(DMA2_Stream1, ENABLE);
      }
      frame_cnt = frame_cnt + 1;//֡��ʼ��־λ  
    }  
    else
      frame_cnt = 0;  
  }
  
 /**********************DCMI���жϴ���**************************/ 
  if( DCMI_GetITStatus(DCMI_IT_LINE)!= RESET)//��¼ÿ�е�����
  {
    DCMI_ClearITPendingBit(DCMI_IT_LINE);
    if(frame_start)//�ȴ�֡��ʼ�ź�
    {  
       DMA_Cmd(DMA2_Stream1, ENABLE);
       if(line_start<240)
       {
         //�ϰ��ͼ��
         if(line_start==239)
         {
           DMA_Cmd(DMA2_Stream1, ENABLE); 
            //�ж�DMA�����Ƿ����
           while ((DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1) == RESET))
           {
           }
           
           DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1);                                        
           /* Disable the DMA Stream */
           DMA_Cmd(DMA2_Stream1, DISABLE);  
           DCMI_ITConfig(DCMI_IT_FRAME, DISABLE);//��ʹ�ܴ�ֱ�ж�
           DCMI_ITConfig(DCMI_IT_LINE, DISABLE);//��ʹ��ˮƽ�ж�
           DCMI_CaptureCmd(DISABLE);  
           frame_done = 1;
         }
         line_start = line_start + 1;
       }                           
       else
        line_start = 0;//�����м���,������dmaֹͣ      
    }     
  }  
  
/***********************DCMI֡�жϴ�����*****************************/  
  if( DCMI_GetITStatus(DCMI_IT_FRAME)!= RESET)
  { 
    if(frame_start)//�ȴ�֡��ʼ�ź�
    {  
        DCMI_CaptureCmd(ENABLE); 
        DMA_Cmd(DMA2_Stream1, ENABLE); 
        //�ж�DMA�����Ƿ����
        while ((DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1) == RESET))
        {
        }
           
        DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1);        
        DMA_Cmd(DMA2_Stream1, DISABLE);          
        DCMI_ITConfig(DCMI_IT_FRAME, DISABLE);//��ʹ�ܴ�ֱ�ж�       
        DCMI_CaptureCmd(DISABLE);
     
        frame_done = 1;
    }
    
    if(frame_cnt<15)//��15֡��ʼ�ɼ����ȴ�����ͷ�ȶ�
    {
      if(frame_cnt==14)
      { 
   //     STM_EVAL_LEDToggle(LED2);       
        frame_start = 1;//ʹ��֡�ɼ�          
      }    
      frame_cnt = frame_cnt + 1;//֡��ʼ��־λ
    } 
    else
      frame_cnt = 0;

    DCMI_ClearITPendingBit(DCMI_IT_FRAME);
  } 
}

void DCMI_DMA_CONFIG(uint32_t addr_offset)
{
    DMA_InitTypeDef  DMA_InitStructure;      
    DMA_DeInit(DMA2_Stream1);

    DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;//DCMI���ݼĴ�����ַ����buffer����ʼ��ַ
    //����dma�����ַָ��sram
    DMA_InitStructure.DMA_Memory0BaseAddr = (FSMC_LCD_ADDRESS+addr_offset);//(uint32_t)picture_buffer;//FSMC_SRAM_ADDRESS;//FSMC_LCD_ADDRESS;
    //���䷽������赽�ڴ�
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    //Buffer��С���ó�������һ��ͼ���С
    DMA_InitStructure.DMA_BufferSize = 38400;//PICTURE_SIZE;//ֵ���֡�ʼ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA���������ģʽ����DMA��ͨ�����Ӷ������ʱ����Ҫ����DMA_PeripheralInc_Enable
    //�����ַ��Ҫ����,ʹ��sram��ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//�����ڴ����ģʽ����DMA��Ҫ���ʶ���ڴ����ʱ����Ҫ����DMA_MemoryInc_Enable
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//ÿ�β��������ݳ���
    //Memory���ݿ�ȸ�Ϊ16bit����Ӧsramλ��
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA����ģʽ������ѭ��ģʽ/ͨ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//����DMA�����ȼ���
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    
    /* DMA2 IRQ channel Configuration */
    DMA_Init(DMA2_Stream1, &DMA_InitStructure);
}

//����ͷ���ú���������gpio��dcmi��ʼ��
void Camera_Init(void)
{	 
    u16 i,j;
    u8  temp;
	  u32 cnt;
            
    OV7670_HW_Init();//��ʼ��DCMI GPIO
    //����sccb ����gpio   
    SCCB_GPIO_Config();//��ʼ��SDA,SCL
        
    SCCB_WriteByte(0x12, 0x80);  //�����λ����ͷ
        
    //�����λ�����Ҫ����ʱ 500ms
    for(j=0;j<500;j++)
        
       sccb_delay(1000);      
    
    SCCB_WriteByte(0x0B, 0x00);
        
    temp = SCCB_ReadByte(0x0B);  //������ͷID
	
	  cnt = sizeof(OV7670_RGB_reg)/sizeof(OV7670_RGB_reg[0]);	
	
    if(temp==0x73)//OV7670
    {  
	      for(i=0;i<OV7670_RGB_REG_NUM;i++)
            SCCB_WriteByte(OV7670_RGB_reg[i][0],OV7670_RGB_reg[i][1]);   
//			OV7670_Window_Set(1,174,240,320);
        //for(i=0;i<OV7670_YUV_REG_NUM;i++)
            //SCCB_WriteByte(OV7670_YUV_reg[i][0],OV7670_YUV_reg[i][1]);  
    }
		
    
    sccb_delay(1000000);//�������Ҫ��ʱ���ȴ��ȶ�
   
    //������ͷ������ɺ��ٴ�dcmi�ɼ�����
    DCMI_Interface_Init();
    
    sccb_delay(1000000);
    
} 

void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
 u16 endx;
 u16 endy;
 u8 temp; 
 endx=sx+width*2; //V*2
  endy=sy+height*2;
 if(endy>784)endy-=784;
 temp=SCCB_ReadByte(0X03);    //??Vref????
 temp&=0XF0;
 temp|=((endx&0X03)<<2)|(sx&0X03);
 SCCB_WriteByte(0X03,temp);    //??Vref?start?end???2?
 SCCB_WriteByte(0X19,sx>>2);   //??Vref?start?8?
 SCCB_WriteByte(0X1A,endx>>2);   //??Vref?end??8?

 temp=SCCB_ReadByte(0X32);    //??Href????
 temp&=0XC0;
 temp|=((endy&0X07)<<3)|(sy&0X07);
 SCCB_WriteByte(0X17,sy>>3);   //??Href?start?8?
 SCCB_WriteByte(0X18,endy>>3);   //??Href?end??8?
}


///////////////////

