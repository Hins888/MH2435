#include "mh2435_it.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

#include <stdio.h>


void hard_fault_handler_s(void);

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  hard_fault_handler_s();
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

void hard_fault_handler_c(unsigned int *hardfault_args) {

  unsigned int stacked_r0;
  unsigned int stacked_r1;
  unsigned int stacked_r2;
  unsigned int stacked_r3;
  unsigned int stacked_r12;
  unsigned int stacked_lr;
  unsigned int stacked_pc;
  unsigned int stacked_psr;

  stacked_r0 = ((unsigned long)hardfault_args[0]);
  stacked_r1 = ((unsigned long)hardfault_args[1]);
  stacked_r2 = ((unsigned long)hardfault_args[2]);
  stacked_r3 = ((unsigned long)hardfault_args[3]);

  stacked_r12 = ((unsigned long)hardfault_args[4]);
  stacked_lr = ((unsigned long)hardfault_args[5]);
  stacked_pc = ((unsigned long)hardfault_args[6]);
  stacked_psr = ((unsigned long)hardfault_args[7]);

  printf("\n\n[Hard faulthandler - all numbers in hex]\n");
  printf("R0 = 0x%08x\n", stacked_r0);
  printf("R1 = 0x%08x\n", stacked_r1);
  printf("R2 = 0x%08x\n", stacked_r2);
  printf("R3 = 0x%08x\n", stacked_r3);
  printf("R12 = 0x%08x\n", stacked_r12);
  printf("LR [R14] = 0x%08x  subroutine call return address\n", stacked_lr);
  printf("PC [R15] = 0x%08x  program counter  SP = 0x%08x \n", stacked_pc,
         hardfault_args);
  printf("PSR = %x\n", stacked_psr);
}

#if 1
#if defined(__CC_ARM)
__ASM void hard_fault_handler_s(void) {
	import hard_fault_handler_c 
    tst LR, #4;
	ITE EQ;
	MRSEQ r0, MSP;
	MRSNE r0, PSP;
	B hard_fault_handler_c;
}

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__STATIC_FORCEINLINE void hard_fault_handler_s(void) {
  __ASM volatile( //
      "tst LR, #4"; "ITE EQ"; "MRSEQ R0, MSP"; "MRSEQ R0, PSP";
      "B hard_fault_handler_c";);
}
#elif defined(__GNUC__)
void hard_fault_handler_s(void) {
  asm("tst LR, #4");
  asm("ITE EQ");
  asm("MRSEQ R0, MSP");
  asm("MRSEQ R0, PSP");
  asm("B hard_fault_handler_c");
}
#elif defined(__ICCARM__)
void hard_fault_handler_s(void) {
  __ASM("tst LR, #4\n"
        "ITE EQ\n"
        "MRSEQ R0, MSP\n"
        "MRSEQ R0, PSP\n"
        "B hard_fault_handler_c\n");
}
#endif
#endif



