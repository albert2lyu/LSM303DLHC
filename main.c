#include "stm32f4xx.h"
#include "I2C.h"
#include "LSM303.h"
#include <string.h>
#include <stdio.h>
#include  <math.h>
#include "usart.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */



void uart_init(u32 bound);
void RCC_Configuration(void);
void NVIC_Configuration(void);

void WWDG_Configuration(void);
void Delay(u32 nTime);
void Delayms(vu32 m);  

#define   uchar unsigned char
#define   uint unsigned int	

unsigned char TX_DATA[4];  
unsigned char BUF_A[6],BUF_M[6];                         //接收数据缓存区
char  test=0; 
short X_A,Y_A,Z_A,  X_M,Y_M,Z_M;
float buf_A[3],buf_M[3];
float north;


void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


void usart_send(uchar *s)
{
	  uchar i;
    printf("x=%c", *s );
		for(i=0;i<3;i++)
	  printf("%c", *++s );
	
		Delay (0xfffff);
}
int main(void)
{
	I2C_Configuration();
	uart_init(115200);
  LSM303A_Init();
  LSM303M_Init();
  while (1)
  {
		
		LAM303M_Raed(buf_M);
		LAM303A_Raed(buf_A);
		north=Data_conversion(buf_A,buf_M);//
		printf("\r\n********         LSM303DLHC       *********************\r\n");
		
		printf("X_A=%f m/s^2\r\n",buf_A[0]);
		printf("Y_A=%f m/s^2\r\n",buf_A[1]);
		printf("Z_A=%f m/s^2\r\n",buf_A[2]);

		printf("X_M=%f T\r\n",buf_M[0] );
		printf("Y_M=%f T\r\n",buf_M[1]);
		printf("Z_M=%f T\r\n",buf_M[2]);
		printf("north=%f degree \r\n",north);
		printf("\r\n");

		Delay(8000000);
    
		}
}

