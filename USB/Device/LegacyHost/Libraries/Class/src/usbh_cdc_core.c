/* Includes ------------------------------------------------------------------*/
#include "usbh_cdc_core.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup CDC_CLASS
* @{
*/

/** @defgroup CDC_CORE 
  * @brief    This file includes CDC Layer Handlers for USB Host CDC class.
* @{
*/ 

/** @defgroup CDC_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Defines
* @{
*/ 
#define CDC_BUFFER_SIZE                 2048
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4   
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN CDC_Machine_TypeDef   CDC_Machine __ALIGN_END ;

CDC_Requests                        CDC_ReqState;
CDC_Xfer_TypeDef                    CDC_TxParam;
CDC_Xfer_TypeDef                    CDC_RxParam;

// Import USB-ESL-CircularBuffer to make the SendBuffer and ReadBuffer of CDC to 4-byte aligned
static uint32_t SendByteBuffer[CONFIG_USBCDC_SERVICE_SEND_BYTE_BUFFER_SIZE >> 2];
static uint32_t ReadByteBuffer[CONFIG_USBCDC_SERVICE_READ_BYTE_BUFFER_SIZE >> 2];

static CircularBufferStruct SendCircularBuffer;
static CircularBufferStruct ReadCircularBuffer;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4   
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t               TxBuf [CDC_BUFFER_SIZE] __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4   
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t               RxBuf [CDC_BUFFER_SIZE] __ALIGN_END ;

CDC_Usercb_TypeDef                  UserCb;
uint8_t                             RX_Enabled = 0;
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_FunctionPrototypes
* @{
*/ 
static void CDC_InitTxRxParam(void);

static void CDC_ReceiveData(CDC_Xfer_TypeDef *cdc_Data);

static void CDC_ProcessTransmission(USB_OTG_CORE_HANDLE *pdev, USBH_HOST  *phost);

static void CDC_ProcessReception(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost);

static USBH_Status CDC_InterfaceInit  (USB_OTG_CORE_HANDLE *pdev , 
                                       void *phost);

void CDC_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pdev , 
                                  void *phost);

static USBH_Status CDC_Handle(USB_OTG_CORE_HANDLE *pdev , 
                              void *phost);

static USBH_Status CDC_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                    void *phost);


USBH_Class_cb_TypeDef  CDC_cb = 
{
  CDC_InterfaceInit,
  CDC_InterfaceDeInit,
  CDC_ClassRequest,
  CDC_Handle
};
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Functions
* @{
*/ 

/**
  * @brief  CDC_InterfaceInit 
  *         The function init the CDC class.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval  USBH_Status :Response for USB CDC driver intialization
  */
static USBH_Status CDC_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev, 
                                      void *phost)
{
  USBH_HOST *pphost = phost;
  USBH_Status status = USBH_OK ;
  
  /* Communication Interface */
  if((pphost->device_prop.Itf_Desc[0].bInterfaceClass  == COMMUNICATION_DEVICE_CLASS_CODE)&& \
    (pphost->device_prop.Itf_Desc[0].bInterfaceSubClass  == ABSTRACT_CONTROL_MODEL) && \
      (pphost->device_prop.Itf_Desc[0].bInterfaceProtocol == COMMON_AT_COMMAND))
  {
    /*Collect the notification endpoint address and length*/
    CDC_Machine.CDC_CommItf.ep_addr = pphost->device_prop.Ep_Desc[0][0].bEndpointAddress;
    CDC_Machine.CDC_CommItf.length  = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
    
    if(pphost->device_prop.Ep_Desc[0][0].bEndpointAddress & 0x80)
    {
      CDC_Machine.CDC_CommItf.notificationEp =\
        (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
    }
    /*Allocate the length for host channel number in*/
    CDC_Machine.CDC_CommItf.hc_num_in = USBH_Alloc_Channel(pdev, 
                                                           CDC_Machine.CDC_CommItf.notificationEp );
    
    /* Open channel for IN endpoint */
    USBH_Open_Channel  (pdev,
                        CDC_Machine.CDC_CommItf.hc_num_in,
                        pphost->device_prop.address,
                        pphost->device_prop.speed,
                        EP_TYPE_INTR,
                        CDC_Machine.CDC_CommItf.length); 
  }
  else
  {
    pphost->usr_cb->DeviceNotSupported();   
  }
  
  
  /* Data Interface */
  if((pphost->device_prop.Itf_Desc[1].bInterfaceClass  == DATA_INTERFACE_CLASS_CODE)&& \
    (pphost->device_prop.Itf_Desc[1].bInterfaceSubClass  == RESERVED) && \
      (pphost->device_prop.Itf_Desc[1].bInterfaceProtocol == NO_CLASS_SPECIFIC_PROTOCOL_CODE))
  {
    /*Collect the class specific endpoint address and length*/
    CDC_Machine.CDC_DataItf.ep_addr = pphost->device_prop.Ep_Desc[1][0].bEndpointAddress;
    CDC_Machine.CDC_DataItf.length  = pphost->device_prop.Ep_Desc[1][0].wMaxPacketSize;
    
    if(pphost->device_prop.Ep_Desc[1][0].bEndpointAddress & 0x80)
    {      
      CDC_Machine.CDC_DataItf.cdcInEp = (pphost->device_prop.Ep_Desc[1][0].bEndpointAddress);
    }
    else
    {
      CDC_Machine.CDC_DataItf.cdcOutEp = (pphost->device_prop.Ep_Desc[1][0].bEndpointAddress);
    }
    
    if(pphost->device_prop.Ep_Desc[1][1].bEndpointAddress & 0x80)
    {
      CDC_Machine.CDC_DataItf.cdcInEp = (pphost->device_prop.Ep_Desc[1][1].bEndpointAddress);
    }
    else
    {
      CDC_Machine.CDC_DataItf.cdcOutEp = (pphost->device_prop.Ep_Desc[1][1].bEndpointAddress);
    }
    
    /*Allocate the length for host channel number out*/
    CDC_Machine.CDC_DataItf.hc_num_out = USBH_Alloc_Channel(pdev, 
                                                            CDC_Machine.CDC_DataItf.cdcOutEp);
    /*Allocate the length for host channel number in*/
    CDC_Machine.CDC_DataItf.hc_num_in = USBH_Alloc_Channel(pdev, 
                                                           CDC_Machine.CDC_DataItf.cdcInEp);  
    
    /* Open channel for OUT endpoint */
    USBH_Open_Channel  (pdev,
                        CDC_Machine.CDC_DataItf.hc_num_out,
                        pphost->device_prop.address,
                        pphost->device_prop.speed,
                        EP_TYPE_BULK,
                        CDC_Machine.CDC_DataItf.length);  
    /* Open channel for IN endpoint */
    USBH_Open_Channel  (pdev,
                        CDC_Machine.CDC_DataItf.hc_num_in,
                        pphost->device_prop.address,
                        pphost->device_prop.speed,
                        EP_TYPE_BULK,
                        CDC_Machine.CDC_DataItf.length);
    
    /*Initilise the Tx/Rx Params*/
    CDC_InitTxRxParam();
    
    
    /*Initialize the class specific request with "GET_LINE_CODING"*/
    CDC_ReqState = CDC_GET_LINE_CODING_RQUEST ;
  }
  else
  {
    pphost->usr_cb->DeviceNotSupported();   
  }  
  return status;
  
}



/**
* @brief  CDC_InterfaceDeInit 
*         The function DeInit the Host Channels used for the CDC class.
* @param  pdev: Selected device
* @param  hdev: Selected device property
* @retval None
*/
void CDC_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pdev,
                          void *phost)
{
  if ( CDC_Machine.CDC_CommItf.hc_num_in)
  {
    USB_OTG_HC_Halt(pdev, CDC_Machine.CDC_CommItf.hc_num_in);
    USBH_Free_Channel  (pdev,CDC_Machine.CDC_CommItf.hc_num_in);
    CDC_Machine.CDC_CommItf.hc_num_in = 0;     /* Reset the Channel as Free */
  }
  
  if ( CDC_Machine.CDC_DataItf.hc_num_out)
  {
    USB_OTG_HC_Halt(pdev, CDC_Machine.CDC_DataItf.hc_num_out);
    USBH_Free_Channel  (pdev,CDC_Machine.CDC_DataItf.hc_num_out);
    CDC_Machine.CDC_DataItf.hc_num_out = 0;     /* Reset the Channel as Free */
  }
  
  if ( CDC_Machine.CDC_DataItf.hc_num_in)
  {
    USB_OTG_HC_Halt(pdev, CDC_Machine.CDC_DataItf.hc_num_in);
    USBH_Free_Channel  (pdev,CDC_Machine.CDC_DataItf.hc_num_in);
    CDC_Machine.CDC_DataItf.hc_num_in = 0;     /* Reset the Channel as Free */
  } 
//  return USBH_OK;
}

/**
  * @brief  CDC_ClassRequest 
  *         The function is responsible for handling CDC Class requests
  *         for CDC class.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval  USBH_Status :Response for USB Set Protocol request
  */
static USBH_Status CDC_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                    void *phost)
{   
  USBH_HOST *pphost = phost;
  
  USBH_Status status         = USBH_BUSY;
  USBH_Status ClassReqStatus = USBH_BUSY;
  
  switch(CDC_ReqState) 
  {
    
  case CDC_GET_LINE_CODING_RQUEST: 
    /*Issue the get line coding request*/
    ClassReqStatus = CDC_GETLineCoding(pdev, phost);
    if( ClassReqStatus == USBH_OK )
    {          /*Change the state */
      CDC_ReqState = CDC_SET_CONTROL_LINE_STATE_REQUEST;
    }
    break;
    
  case CDC_SET_LINE_CODING_RQUEST: 
    
    /*Issue the set line coding request*/
    ClassReqStatus = CDC_SETLineCoding(pdev, phost);
    if( ClassReqStatus == USBH_OK )
    {
      /*Change the state */
      CDC_ReqState = CDC_GET_LINE_CODING_RQUEST ;
    }
    if(ClassReqStatus == USBH_NOT_SUPPORTED )
    {
      /* a Clear Feature should be issued here */
      CDC_ReqState = CDC_ERROR_STATE;
    }
    break;
    
  case CDC_SET_CONTROL_LINE_STATE_REQUEST:
    /*Issue the set control line coding */
    ClassReqStatus = CDC_SETControlLineState(pdev, phost);
    if( ClassReqStatus == USBH_OK )
    {
      /*Change the state */
      CDC_ReqState = CDC_SET_CONTROL_LINE_STATE_REQUEST;
      /*Also set the state of receive CDCRxParam to IDLE*/
      //CDC_RxParam.CDCState = CDC_IDLE; 
      RX_Enabled = 1;

      status = USBH_OK; /*This return from class specific routinues request*/
    }
    break;
    
  case CDC_ERROR_STATE:
    
    ClassReqStatus = USBH_ClrFeature(pdev,
                                     phost,
                                     0x00,
                                     pphost->Control.hc_num_out);
    
    if(ClassReqStatus == USBH_OK )
    {        
      /*Change the state to waiting*/
      CDC_ReqState = CDC_GET_LINE_CODING_RQUEST ;
    }
    break;      
  }
  
  return status; 
}


/**
  * @brief  CDC_Handle 
  *         The function is for managing state machine for CDC data transfers 
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status
  */
static USBH_Status CDC_Handle(USB_OTG_CORE_HANDLE *pdev , 
                              void   *phost)
{
  USBH_Status status = USBH_OK;
  USBH_HOST *pphost = phost;
  
  /* Call Application process */
  pphost->usr_cb->UserApplication();  
  
  /*Handle the transmission */
  CDC_ProcessTransmission(pdev, pphost);
  
  /*Always send in packet to device*/    
  CDC_ProcessReception(pdev, pphost);
  
  return status;
}

/**
  * @brief  The function is responsible for sending data to the device
  * @param  pdev: Selected device
  * @retval None
  */
void CDC_ProcessTransmission(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  static uint32_t len ;
  static uint8_t data0_flag = false;
  URB_STATE URB_StatusTx = URB_IDLE;
  
  URB_StatusTx =   HCD_GetURB_State(pdev , CDC_Machine.CDC_DataItf.hc_num_out);

  CircularBufferStruct* circular = &SendCircularBuffer;

  switch(CDC_TxParam.CDCState)
  {
  case CDC_IDLE:
    break;
    
  case CDC_SEND_DATA:
    
    if ((URB_StatusTx == URB_DONE ) || (URB_StatusTx == URB_IDLE))
    {

        
      /*
      // Check the data length is more then the CDC_Machine.CDC_DataItf.CDC_DataItf.length
      if(CDC_TxParam.DataLength > CDC_Machine.CDC_DataItf.length)
      {
        
        len = CDC_Machine.CDC_DataItf.length ;
        // Send the data 
        USBH_BulkSendData (pdev,
                           CDC_TxParam.pRxTxBuff, 
                           len , 
                           CDC_Machine.CDC_DataItf.hc_num_out);    
      }
      else
      {
        len = CDC_TxParam.DataLength ;
        // Send the remaining data
        USBH_BulkSendData (pdev,
                           CDC_TxParam.pRxTxBuff, 
                           len, 
                           CDC_Machine.CDC_DataItf.hc_num_out);
        
      }
      */

      if (!circular->StartPop(circular, circular->Length, true, true))
          break;

      USBH_BulkSendData(pdev, circular->Buffer + circular->PopOffset, circular->PopSize, CDC_Machine.CDC_DataItf.hc_num_out);
      if((circular->PopSize > 0) && (circular->PopSize % pdev->host.hc[CDC_Machine.CDC_DataItf.hc_num_out].max_packet == 0))data0_flag = true;
      else data0_flag = false;
      CDC_TxParam.CDCState = CDC_DATA_SENT;
    }
    
    break;
    
  case CDC_DATA_SENT:
    /*Check the status done for transmssion*/
    if(URB_StatusTx == URB_DONE )
    {
      /*
      // Point to next chunc of data
      CDC_TxParam.pRxTxBuff += len ;
      
      // decrease the data length
      CDC_TxParam.DataLength -= len;    
      
      if(CDC_TxParam.DataLength == 0)
      {
        CDC_TxParam.CDCState = CDC_IDLE;
      }
      else
      {
        CDC_TxParam.CDCState = CDC_SEND_DATA; 
      }
      */
      
      if(data0_flag)
      {
          USBH_BulkSendData(pdev, NULL, 0, CDC_Machine.CDC_DataItf.hc_num_out);
          data0_flag = false;
      }
      else
      {
          circular->EndPop(circular, circular->PopSize);
          CDC_TxParam.CDCState = CircularBufferUsedSize(circular) ? CDC_SEND_DATA : CDC_IDLE;
      }
    }
    else if( URB_StatusTx == URB_NOTREADY)
    {
      /*Send the same data */
      /*
      USBH_BulkSendData (pdev,
                         (CDC_TxParam.pRxTxBuff), 
                         len, 
                         CDC_Machine.CDC_DataItf.hc_num_out);
      */
      USBH_BulkSendData(pdev, circular->Buffer + circular->PopOffset, circular->PopSize, CDC_Machine.CDC_DataItf.hc_num_out);
    }
    
    break;

  case CDC_READ_DATA:
    break;

  case CDC_BUSY:
    break;

  case CDC_GET_DATA:
    break;

  case CDC_POLL:
    break;

  case CDC_CTRL_STATE:
    break;
    
  }
}
/**
  * @brief  This function responsible for reception of data from the device
  * @param  pdev: Selected device
  * @retval None
  */
static void CDC_ProcessReception(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  USB_OTG_HCCHAR_TypeDef   hcchar;
  CircularBufferStruct* circular = &ReadCircularBuffer;
  if(RX_Enabled == 1)
  {
    URB_STATE URB_StatusRx = HCD_GetURB_State(pdev , CDC_Machine.CDC_DataItf.hc_num_in);  
    
    switch(CDC_RxParam.CDCState)
    {
      
    case CDC_IDLE:
      /*
      // check the received length lesser then the remaining space available in the buffer
      if(CDC_RxParam.DataLength < (CDC_RxParam.BufferLen - CDC_Machine.CDC_DataItf.length))
      {
        // Receive the data
        USBH_BulkReceiveData(pdev,
                             CDC_RxParam.pFillBuff,
                             CDC_Machine.CDC_DataItf.length, 
                             CDC_Machine.CDC_DataItf.hc_num_in);
        
        // change the cdc state to USBH_CDC_GET_DATA
        CDC_RxParam.CDCState = CDC_GET_DATA;
      }
      */

        hcchar.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[CDC_Machine.CDC_DataItf.hc_num_in]->HCCHAR);
        if(hcchar.b.chen == 0)
        {
            
          // The minium push size for ReadCircularBuffer is 1/4 buffer length, and it should be the multiple of max packet size of IN endpoint
          if (!circular->StartPush(circular, circular->Length >> 2, true, false)) {
            if (circular->Content == 0)
                circular->PushOffset = circular->PopOffset = 0;
            break;
          }
          // Start receive from IN endpoint
          USBH_BulkReceiveData(pdev, circular->Buffer + circular->PushOffset, circular->PushSize, CDC_Machine.CDC_DataItf.hc_num_in);
          CDC_RxParam.CDCState = CDC_GET_DATA;
        }
      break;
      
    case CDC_GET_DATA:
      /*
      // Check the last state of the device is URB_DONE
      if(URB_StatusRx == URB_DONE)
      {

        // Move the pointer as well as datalength
        CDC_RxParam.DataLength += pdev->host.hc[CDC_Machine.CDC_DataItf.hc_num_in].xfer_count ;
        CDC_RxParam.pFillBuff += pdev->host.hc[CDC_Machine.CDC_DataItf.hc_num_in].xfer_count ;
        
        
        // Process the recived data
        CDC_ReceiveData(&CDC_RxParam);
        
        // change the state od the CDC state
        CDC_RxParam.CDCState = CDC_IDLE;
      }
      */

      // restart receive when error
      if (URB_StatusRx == URB_ERROR)
          USBH_BulkReceiveData(pdev, circular->Buffer + circular->PushOffset, circular->PushSize, CDC_Machine.CDC_DataItf.hc_num_in);

      if (URB_StatusRx != URB_DONE)
        break;

      uint32_t readLength = HCD_GetXferCnt(pdev, CDC_Machine.CDC_DataItf.hc_num_in);

      // fire the Read event
      if (UserCb.ReadCallback && UserCb.ReadCallback((uint32_t*)(circular->Buffer + circular->PushOffset), readLength)) {
        readLength = 0;
      }

      circular->EndPush(circular, readLength);

      // Meano TODO: for the readLength may be less than circular->PushSize, the EndPush will set PushSize to 0 in the furture
      circular->PushSize = 0;
      CDC_RxParam.CDCState = CDC_IDLE;

      break;

    case CDC_READ_DATA:

      break;

    case CDC_BUSY:

      break;

    case CDC_SEND_DATA:

      break;

    case CDC_DATA_SENT:

      break;

    case CDC_POLL:

      break;

    case CDC_CTRL_STATE:

      break;
    }
  }
}

/**
  * @brief  Initialize the transmit and receive buffer and its parameter
  * @param  None
  * @retval None
  */
static void CDC_InitTxRxParam(void)
{
  /*Initialize the Transmit buffer and its parameter*/
  CDC_TxParam.CDCState = CDC_IDLE;
//  CDC_TxParam.DataLength = 0;
//  CDC_TxParam.pRxTxBuff = TxBuf;
  
  /*Initialize the Receive buffer and its parameter*/
  CDC_RxParam.CDCState = CDC_IDLE;
//  CDC_RxParam.DataLength = 0;
//  CDC_RxParam.pFillBuff = RxBuf;  
//  CDC_RxParam.pEmptyBuff = RxBuf;
//  CDC_RxParam.BufferLen = sizeof(RxBuf);

  CircularBufferConstractor(&SendCircularBuffer, 4);
  SendCircularBuffer.Init(&SendCircularBuffer, (uint8_t*)SendByteBuffer, sizeof(SendByteBuffer));

  CircularBufferConstractor(&ReadCircularBuffer, 4);
  ReadCircularBuffer.Init(&ReadCircularBuffer, (uint8_t*)ReadByteBuffer, sizeof(ReadByteBuffer));
}
/**
  * @brief  This is a call back function from cdc core layer to redirect the 
  *         received data on the user out put system
  * @param  cdc_Data: type of USBH_CDCXfer_TypeDef
  * @retval None
  */
/*
static void CDC_ReceiveData(CDC_Xfer_TypeDef *cdc_Data)
{
  uint8_t *ptr; 
  
  if(cdc_Data->pEmptyBuff < cdc_Data->pFillBuff)
  {
    ptr = cdc_Data->pFillBuff;
    *ptr = 0x00;
    
    // redirect the received data on the user out put system
    UserCb.Receive(cdc_Data->pEmptyBuff);
    
    cdc_Data->pFillBuff  = cdc_Data->pEmptyBuff ; 
    cdc_Data->DataLength = 0;    // Reset the data length to zero
  }
}
*/

uint32_t VCPHostSendBytes(uint8_t* buffer, uint32_t size) {
    if(CDC_TxParam.CDCState != CDC_IDLE)
        return 0;

    uint32_t sendLength = SendCircularBuffer.Push(&SendCircularBuffer, buffer, size, false);
    return CDC_TxParam.CDCState = CDC_SEND_DATA;
}

uint32_t VCPHostSendBufferSize(bool isFreeSize) {
    return isFreeSize ? CircularBufferFreeSize(&SendCircularBuffer) : CircularBufferUsedSize(&SendCircularBuffer);
}

uint32_t VCPHostReadBytes(uint8_t* buffer, uint32_t size) {
    return ReadCircularBuffer.Pop(&ReadCircularBuffer, buffer, size, true);
}

uint32_t VCPHostReadBufferSize(bool isFreeSize) {
    return isFreeSize ? CircularBufferFreeSize(&ReadCircularBuffer) : CircularBufferUsedSize(&ReadCircularBuffer);
}

/**
  * @brief  This function send data to the device.
  * @param  fileName : name of the file 
  * @retval the pushed size will be returned 
  * FS_SUCCESS : returned to the parent function when the file length become to zero
  */
void CDC_SendData(uint8_t *data, uint16_t length)
{
  /*
  if(CDC_TxParam.CDCState == CDC_IDLE)
  {

    CDC_TxParam.pRxTxBuff = data; 
    CDC_TxParam.DataLength = length;
    CDC_TxParam.CDCState = CDC_SEND_DATA;
  }
  */
  VCPHostSendBytes(data, length);
}

/**
  * @brief  This function send data to the device.
  * @param  fileName : name of the file 
  * @retval the filestate will be returned 
  * FS_SUCCESS : returned to the parent function when the file length become to zero
  */
void  CDC_StartReception( USB_OTG_CORE_HANDLE *pdev)
{
  RX_Enabled = 1;
}

/**
  * @brief  This function send data to the device.
  * @param  fileName : name of the file 
  * @retval the filestate will be returned 
  * FS_SUCCESS : returned to the parent function when the file length become to zero
  */
void  CDC_StopReception( USB_OTG_CORE_HANDLE *pdev)
{
  RX_Enabled = 0; 
  USB_OTG_HC_Halt(pdev, CDC_Machine.CDC_DataItf.hc_num_in);
  USBH_Free_Channel  (pdev,CDC_Machine.CDC_DataItf.hc_num_in);
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/


/**
* @}
*/


/**
* @}
*/

