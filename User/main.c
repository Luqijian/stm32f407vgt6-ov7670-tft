//#include "stm32f4xx.h"
#include "main.h"
#include "lcd.h"
#include "hardware_i2c.h"
u8 x=0;
extern RCC_ClocksTypeDef RCC_Clocks;
extern OV7670_IDTypeDef OV7670_Camera_ID;
uint8_t buffer[40];
//extern Camera_TypeDef Camera;
extern ImageFormat_TypeDef ImageFormat;
//extern __IO uint8_t ValueMax;
//extern const uint8_t *ImageForematArray[];
extern uint8_t ov_frame;
//static uint16_t addr_offset;
//�������ݻ�����
uint16_t video_buffer[BUFFER_SIZE];

uint8_t  i;
uint32_t j;
extern __IO uint8_t frame_done;

void LED_Configuration(void);

void usart_param_init(void);
void usart_gpio_init(void);
void usart_rcc_init(void);
void usart_send_char(int32_t ch);
uint16_t usart_receive_char(void);

void TIM3_Int_Init(u16 arr,u16 psc);
void Cam_Start(void);
//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	u16 i;
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		GPIO_ToggleBits(GPIOC, GPIO_Pin_7);
		usart_send_char(ov_frame);
/*		i=SCCB_ReadByte(0x32);
		usart_send_char(i);
		i=SCCB_ReadByte(0x17);
		usart_send_char(i);
		i=SCCB_ReadByte(0x18);
		usart_send_char(i);
		i=SCCB_ReadByte(0x19);
		usart_send_char(i);
		i=SCCB_ReadByte(0x1a);
		usart_send_char(i);
		i=SCCB_ReadByte(0x03);
		usart_send_char(i);*/
		
//		printf("frame:%d\r\n",ov_frame);//��ӡ֡��
//		printf("jpeg_data_len:%d\r\n",jpeg_data_len);//��ӡ֡��
		ov_frame=0;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}


/*******************************************************************************
* Function Name  : LED_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LED_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;  //��һ�� ��ʼ��GPIO��ʱ���õ� �ṹ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE); //ʹ��PE�ڵ�ʱ�ӣ�����˿��ǲ�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //�Խṹ���GPIO_Pin����ֵ������Ҫ��������1,2,3,4�˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�Խṹ���GPIO_Mode����ֵ������IO�ڵ�ģʽ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�Խṹ���GPIO_OType����ֵ������IO�ڵĽṹ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�Խṹ���GPIO_Speed����ֵ�������ٶ���100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //�Խṹ���GPIO_PuPd����ֵ�������ڲ�����
	GPIO_Init(GPIOC, &GPIO_InitStructure);//���ṹ������ʼ�������У�ִ�г�ʼ��������֮ǰ���ö�����Ч��
}

/*
    -------------
    | UART3_TX | PB10  |
    -------------
    | UART3_RX | PB11 |
    -------------
*/
void usart_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    /* ���ð������ڵ� TX ��Ϊ����ʽ���ù��ܣ��ٶ�Ϊ 50MHz */
    gpio_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init_struct);

    /* ���ð������ڵ� RX ��Ϊ�������룬�ٶ�Ϊ 50MHz */
    gpio_init_struct.GPIO_Pin = GPIO_Pin_11;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
	  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init_struct);
}
void usart_rcc_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//����RCC_AHB1PeriphClockCmd����
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	  //��ΪM4û�и���ʱ�ӹ��ܣ��ʸ���ʱ�ӹ��ܴ�����
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //����ʱ����ͬʱ���ö��IO
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
}
void usart_param_init(void)
{ 
    /* 
        USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;//ʹ�ܷ��͹���

//  STM_EVAL_COMInit(COM1, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);
 
}
void usart_send_char(int32_t ch)
{
    /* �ȴ�������� */
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
    {
    }
    USART_SendData(USART3, (uint8_t) ch);
}
uint16_t usart_receive_char(void)
{
    /* �ȴ������� */
    while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
    {
    }
    return USART_ReceiveData(USART3);
}

/*******************************************************************************
* Function Name  : ������
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DelayLed(unsigned long Dly)
{
   while(Dly--);
}
void Cam_Start(void)
{
    LCD_SetCursor2(0x00,0x0000,0x013f,0x00ef);	//���ù�����ʼλ�� 
 // 	DMA_Cmd(DMA2_Stream1, ENABLE); 
//  	DCMI_Cmd(ENABLE); 
//  	DCMI_CaptureCmd(ENABLE); 
	  LCD_WriteRAM_Prepare();
}
int main(void)
{
	
	LED_Configuration();	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PA��
	XCLK_init_ON();
 
  hardware_i2c_init();
	ov7670_configuration();
	
	LCD_Init();           //��ʼ��LCD FSMC�ӿ�
	POINT_COLOR=RED;      //������ɫ����ɫ

  //���ڳ�ʼ��
	usart_rcc_init();
  usart_gpio_init();
  usart_param_init();
	USART_Cmd(USART3, ENABLE);
	
	//��ʱ����ʼ����ͳ��֡��,�ú����ڴ��ڳ�ʼ��֮��
	TIM3_Int_Init(10000-1,8400-1);
	
//	Cam_Start();
//	DCMI_Cmd(ENABLE);
	
	//��ʾͼƬ
/*	LCD_SetCursor2(0x00,0x0000,0x013f,0x00ef);	//���ù�����ʼλ�� 
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM
	for(j=0;j<38400;j++)
	{
		LCD->LCD_RAM=0x001F;	
	}
	for(j=0;j<38400;j++)
	{
		LCD->LCD_RAM=0xF800;	
	}*/


//	LCD_ShowString(0,0,200,12,12,"JIYI_TEST");
//	LCD_Set_Window(0,0,200,300);
//	LCD_DrawRectangle(10,10,50,50);
//  JPEG_Display(&Image_1[2400]);
	while (1)
	{ 
		
	/*	switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break; 
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}	    */    					 
	//	LCD_ShowString(0,0,200,12,12,"JIYI_TEST");	      					 
	//  x++;
	//	if(x==12)x=0;	 
//		LCD_Set_Window(0,0,200,300);
//		sccb_delay(1000000);
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}






#endif
