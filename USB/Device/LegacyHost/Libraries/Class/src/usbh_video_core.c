// Search for "GRXFSIZ" to change RX FIFO size
// See mor info at "usbh_video_stram_parsing.c" file

/* Includes ------------------------------------------------------------------*/
#include "usbh_video_core.h"
#include "usbh_video_desc_parsing.h"
#include "usbh_video_stream_parsing.h"

/** @defgroup CDC_CORE_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4   
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN VIDEO_HandleTypeDef   UVC_Machine __ALIGN_END  = {0};
/**
  * @}
  */ 


static USBH_Status USBH_VIDEO_InterfaceInit  (USB_OTG_CORE_HANDLE *pdev , 
                                       void *phost);

void USBH_VIDEO_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pdev , 
                                  void *phost);

static USBH_Status USBH_VIDEO_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                    void *phost);

static USBH_Status USBH_VIDEO_Process(USB_OTG_CORE_HANDLE *pdev , 
                              void *phost);
static USBH_Status USBH_VIDEO_CSRequest(USB_OTG_CORE_HANDLE *pdev , 
                                    void *phost, uint8_t feature, uint8_t channel);
static USBH_Status USBH_VIDEO_HandleCSRequest(USB_OTG_CORE_HANDLE *pdev ,void *phost);
static USBH_Status USBH_VIDEO_InputStream (USB_OTG_CORE_HANDLE *pdev ,void *phost);

static USBH_Status UVC_ProcessTransmission(USB_OTG_CORE_HANDLE *pdev ,void *phost);

USBH_Status USBH_VS_GetErr(USB_OTG_CORE_HANDLE *pdev ,void *phost);

USBH_Class_cb_TypeDef  UVC_cb = 
{
  USBH_VIDEO_InterfaceInit,
  USBH_VIDEO_InterfaceDeInit,
  USBH_VIDEO_ClassRequest,
  USBH_VIDEO_Process
};

// This struct is used for PROBE control request ( Setup Packet )
VIDEO_ProbeTypedef ProbeParams_Cur;
VIDEO_ProbeTypedef ProbeParams_Max;
VIDEO_ProbeTypedef ProbeParams_Min;
uint8_t errcode = 0;

//Buffer to store received UVC data packet
volatile uint32_t tmp_packet_framebuffer[(UVC_RX_FIFO_SIZE_LIMIT + 3) >> 2];

/** @defgroup Private_Functions
  * @{
  */ 
/**
  * @brief  USBH_FindInterface 
  *         Find the interface index for a specific class.
  * @param  phost: Host Handle
  * @param  Class: Class code
  * @param  SubClass: SubClass code
  * @param  Protocol: Protocol code
  * @retval interface index in the configuration structure
  * @note : (1)interface index 0xFF means interface index not found
  */
uint8_t  USBH_FindInterface(USB_OTG_CORE_HANDLE *pdev , void *phost, uint8_t Class, uint8_t SubClass, uint8_t Protocol)
{
  USBH_HOST *pphost = phost;
  USBH_InterfaceDesc_TypeDef    *pif ;
  int8_t                        if_ix = 0;
  
  pif = (USBH_InterfaceDesc_TypeDef *)0;
  
  while (if_ix < USBH_MAX_NUM_INTERFACES)
  {
    pif = &(pphost->device_prop.Itf_Desc[if_ix]);
    if(((pif->bInterfaceClass == Class) || (Class == 0xFF))&&
       ((pif->bInterfaceSubClass == SubClass) || (SubClass == 0xFF))&&
         ((pif->bInterfaceProtocol == Protocol) || (Protocol == 0xFF)))
    {
      return  if_ix;
    }
    if_ix++;
  }
  return 0xFF;
}

/**
  * @brief  USBH_VIDEO_InterfaceInit 
  *         The function init the Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
//static USBH_StatusTypeDef USBH_VIDEO_InterfaceInit (USBH_HandleTypeDef *phost)
static USBH_Status USBH_VIDEO_InterfaceInit  (USB_OTG_CORE_HANDLE *pdev , void *phost)
{
  USBH_HOST *pphost = phost;
  USBH_Status status = USBH_FAIL ;
  USBH_Status out_status ;    
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  uint8_t  interface, index;
  
  uint16_t ep_size_in = 0;  
  
  interface = USBH_FindInterface(pdev, pphost, CC_VIDEO, USB_SUBCLASS_VIDEOCONTROL, 0x00);
  if(interface == 0xFF)interface = USBH_FindInterface(pdev, pphost, CC_VIDEO, USB_SUBCLASS_VIDEOCONTROL, 0x01);

  if(interface == 0xFF) /* Not Valid Interface */
  {
    USB_ErrLog("Cannot Find the interface for %d class.\n", CC_VIDEO);
    status = USBH_FAIL;      
  }
  else
  {
    /* 1st Step:  Find IN Video Interfaces */
    out_status = USBH_VIDEO_FindStreamingIN(pdev,phost);
    
    if(out_status == USBH_FAIL)
    {
      USB_ErrLog(" class configuration not supported.\n");
      status = USBH_FAIL;
      return USBH_FAIL;
    }
    
    /* 2nd Step:  Select Video Streaming Interfaces with best endpoint size*/ 
    for (index = 0; index < VIDEO_MAX_VIDEO_STD_INTERFACE; index++)
    {      
      if( VIDEO_Handle->stream_in[index].valid == 1)
      {
        uint16_t ep_size = VIDEO_Handle->stream_in[index].EpSize;
        USB_Log("index = %d,EpSize = %d\n",index,ep_size);
        if ((ep_size > ep_size_in) && (ep_size <= UVC_RX_FIFO_SIZE_LIMIT))
        {
          ep_size_in = ep_size;
          VIDEO_Handle->camera.interface = VIDEO_Handle->stream_in[index].interface;
          VIDEO_Handle->camera.AltSettings = VIDEO_Handle->stream_in[index].AltSettings;                
          VIDEO_Handle->camera.Ep = VIDEO_Handle->stream_in[index].Ep;
          VIDEO_Handle->camera.EpSize = VIDEO_Handle->stream_in[index].EpSize;
          VIDEO_Handle->camera.Poll = VIDEO_Handle->stream_in[index].Poll;         
          VIDEO_Handle->camera.supported = 1;    
        }
      }
    }
    USB_Log("Selected EP size: %d bytes\n", ep_size_in);
    
    
    /* 3rd Step:  Find and Parse Video interfaces */ 
    USBH_VIDEO_ParseCSDescriptors(pdev,phost);
    
    /* 4rd Step:  Find desrcroptors for target settings */
    USBH_VIDEO_AnalyseFormatDescriptors(&VIDEO_Handle->class_desc);
    if (USBH_VIDEO_Best_bFormatIndex == -1)
    {
      status = USBH_FAIL;
      return USBH_FAIL;
    }
    
    USBH_VIDEO_AnalyseFrameDescriptors(&VIDEO_Handle->class_desc);
    if (USBH_VIDEO_Best_bFrameIndex == -1)
    {
      status = USBH_FAIL;
      return USBH_FAIL;
    }
    
    if(VIDEO_Handle->camera.supported == 1)
    {       
      VIDEO_Handle->camera.Pipe = USBH_Alloc_Channel(pdev, VIDEO_Handle->camera.Ep);
        
      USBH_Open_Channel  (pdev,
                    VIDEO_Handle->camera.Pipe,
                    pphost->device_prop.address,
                    pphost->device_prop.speed,
                    EP_TYPE_ISOC,
                    VIDEO_Handle->camera.EpSize);
    }
    
    VIDEO_Handle->req_state     = VIDEO_REQ_INIT;
    VIDEO_Handle->control_state = VIDEO_CONTROL_INIT;
    
    status = USBH_OK;
  }
  return status;
}



/**
  * @brief  USBH_VIDEO_InterfaceDeInit 
  *         The function DeInit the Pipes used for the Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
void USBH_VIDEO_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pdev , 
                                  void *phost)
{
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
    
  if(VIDEO_Handle->camera.Pipe != 0x00)
  {
    USB_OTG_HC_Halt(pdev, VIDEO_Handle->camera.Pipe);
    USBH_Free_Channel  (pdev,VIDEO_Handle->camera.Pipe);
    VIDEO_Handle->camera.Pipe = 0;     /* Reset the pipe as Free */  
    memset(&UVC_Machine,0,sizeof(UVC_Machine));
  }
}

/**
  * @brief  USBH_VIDEO_ClassRequest 
  *         The function is responsible for handling Standard requests
  *         for Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_Status USBH_VIDEO_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                    void *phost)
{   
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  USBH_Status status = USBH_BUSY;
  USBH_Status req_status = USBH_BUSY;
  
  /* Switch VIDEO REQ state machine */
  switch (VIDEO_Handle->req_state)
  {
  case VIDEO_REQ_INIT:
  case VIDEO_REQ_GET_MAX:
    req_status = USBH_VS_GetCur(pdev, phost, VS_PROBE_CONTROL << 8, UVC_GET_MAX, &ProbeParams_Max);
    if(req_status == USBH_OK || req_status == USBH_NOT_SUPPORTED)
    {
        VIDEO_Handle->req_state = VIDEO_REQ_GET_MIN;
    }
      break;
  case VIDEO_REQ_GET_MIN:
    req_status = USBH_VS_GetCur(pdev, phost, VS_PROBE_CONTROL << 8, UVC_GET_MIN,&ProbeParams_Min);
    if(req_status == USBH_OK || req_status == USBH_NOT_SUPPORTED)
    {
        VIDEO_Handle->req_state = VIDEO_REQ_GET_CUR1;
    }
      break;
  case VIDEO_REQ_GET_CUR1:
    req_status = USBH_VS_GetCur(pdev, phost, VS_PROBE_CONTROL << 8, UVC_GET_CUR, &ProbeParams_Cur);
    if(req_status == USBH_OK)
    {
        VIDEO_Handle->req_state = VIDEO_REQ_SET_CUR;
    }
    break;
  case VIDEO_REQ_SET_CUR:
    memset(&ProbeParams_Cur, 0, sizeof(ProbeParams_Cur));
    // Set needed params, at commit stage this parameters must be receied during "GET_CUR"
    ProbeParams_Cur.bmHint = 1;
    ProbeParams_Cur.bFormatIndex = USBH_VIDEO_Best_bFormatIndex;             
    ProbeParams_Cur.bFrameIndex = USBH_VIDEO_Best_bFrameIndex;
    
    //Maximum framerate can be selected here
    #if 1
    if(ProbeParams_Max.dwFrameInterval < 400000)
    {
        ProbeParams_Cur.dwFrameInterval = 400000;//25 FPS
    }
    else
    {
        ProbeParams_Cur.dwFrameInterval = ProbeParams_Max.dwFrameInterval;
    }
    #else
    //ProbeParams_Cur.dwFrameInterval = 333333;//30 FPS
    ProbeParams_Cur.dwFrameInterval = 400000;//25 FPS
    //ProbeParams_Cur.dwFrameInterval = 666666;//15 FPS
    //ProbeParams_Cur.dwFrameInterval = 2000000;//5 FPS
    #endif
    ProbeParams_Cur.dwMaxVideoFrameSize = USBH_VIDEO_Max_bFrameSize;
    
    req_status = USBH_VS_SetCur(pdev, phost, VS_PROBE_CONTROL << 8, &ProbeParams_Cur);
    if(req_status == USBH_OK)
    {

       VIDEO_Handle->req_state = VIDEO_REQ_GET_CUR2;
    }
    break;
  case VIDEO_REQ_GET_CUR2:
    req_status = USBH_VS_GetCur(pdev, phost, VS_PROBE_CONTROL << 8, UVC_GET_CUR, &ProbeParams_Cur);
    if(req_status == USBH_OK)
    {
        VIDEO_Handle->req_state = VIDEO_REQ_SET_CUR_COM;
    }
    break;
  case VIDEO_REQ_SET_CUR_COM:
      req_status = USBH_VS_SetCur(pdev, phost, VS_COMMIT_CONTROL << 8,&ProbeParams_Cur);
      if(req_status == USBH_OK)
      {
         VIDEO_Handle->req_state = VIDEO_REQ_SET_DEFAULT_IN_INTERFACE;
         VIDEO_Handle->control_state = VIDEO_CONTROL_IDLE;
      }
      break;
  case VIDEO_REQ_SET_DEFAULT_IN_INTERFACE:
    if(VIDEO_Handle->camera.supported == 1)
    {
        req_status = USBH_SetInterface(pdev,
                                        phost, 
                                        VIDEO_Handle->camera.interface, 
                                        0);
        if(req_status == USBH_OK)
        {
            VIDEO_Handle->req_state = VIDEO_REQ_SET_IN_INTERFACE;
        }
    }
    else
    {
      VIDEO_Handle->req_state = VIDEO_REQ_SET_DEFAULT_IN_INTERFACE;
    }
    break;
    
  case VIDEO_REQ_CS_REQUESTS:
    if(USBH_VIDEO_HandleCSRequest(pdev,phost) == USBH_OK)
    {
      VIDEO_Handle->req_state = VIDEO_REQ_SET_IN_INTERFACE;
    }
    break;
   
  case VIDEO_REQ_SET_IN_INTERFACE:
    if(VIDEO_Handle->camera.supported == 1)
    {
      req_status = USBH_SetInterface(pdev,
                                     phost, 
                                     VIDEO_Handle->camera.interface, 
                                     VIDEO_Handle->camera.AltSettings);
      
      if(req_status == USBH_OK)
      {
        VIDEO_Handle->req_state = VIDEO_REQ_IDLE;
        VIDEO_Handle->steam_in_state = VIDEO_STATE_START_IN;
      }
    }
    else
    {
      VIDEO_Handle->req_state = VIDEO_REQ_SET_IN_INTERFACE;   
    }
    break;
  case VIDEO_REQ_IDLE:
    status  = USBH_OK;    
  default:
    break;
  }
  return status; 
}

/**
  * @brief  USBH_VIDEO_CSRequest 
  *         The function is responsible for handling AC Specific requests for a specific feature and channel
  *         for Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_Status USBH_VIDEO_CSRequest(USB_OTG_CORE_HANDLE *pdev , 
                                    void *phost, uint8_t feature, uint8_t channel)
{   
  USBH_Status status = USBH_BUSY;

  return status; 
}

/**
  * @brief  USBH_VIDEO_HandleCSRequest 
  *         The function is responsible for handling VC Specific requests for a all features 
  *         and associated channels for Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_Status USBH_VIDEO_HandleCSRequest(USB_OTG_CORE_HANDLE *pdev ,void *phost)
{ 

  USBH_Status status = USBH_BUSY;
  USBH_Status cs_status = USBH_BUSY;
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
        
  cs_status = USBH_VIDEO_CSRequest(pdev,
                                   phost, 
                                   VIDEO_Handle->temp_feature, 
                                   0);
  
  if(cs_status != USBH_BUSY)
  {        
  }
  
  return status;
}

static USBH_Status USBH_VIDEO_Process(USB_OTG_CORE_HANDLE *pdev ,void *phost)
{
    USBH_HOST *pphost = phost;
    
    /* Call Application process */
    ((USBH_HOST *)phost)->usr_cb->UserApplication();
    
    /*Handle the transmission */
    UVC_ProcessTransmission(pdev,phost);
    
    return USBH_OK ;
}

/**
  * @brief  UVC_ProcessTransmission 
  *         The function is for managing state machine for Video data transfers 
            MUST be called frequently!
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_Status UVC_ProcessTransmission(USB_OTG_CORE_HANDLE *pdev ,void *phost)
{   
  USBH_Status status = USBH_BUSY;
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  
  if(VIDEO_Handle->camera.supported == 1)
  {
    USBH_VIDEO_InputStream(pdev,phost);
  }
 
  return status;
}

/**
  * @brief  Handle Input stream process
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_Status USBH_VIDEO_InputStream (USB_OTG_CORE_HANDLE *pdev ,void *phost)
{
  // Called from USB_HOST_fast_class_call
  USBH_Status status = USBH_BUSY;
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  URB_STATE result;
  static uint32_t rxlen;
  
  switch(VIDEO_Handle->steam_in_state)
  {
    case VIDEO_STATE_START_IN:
      USBH_IsocReceiveData(pdev,
                            (uint8_t*)tmp_packet_framebuffer,
                            VIDEO_Handle->camera.EpSize,
                            VIDEO_Handle->camera.Pipe);
      VIDEO_Handle->steam_in_state = VIDEO_STATE_DATA_IN;
      VIDEO_Handle->camera.timer = HCD_GetCurrentFrame(pdev);
    break;
    case VIDEO_STATE_DATA_IN:
      result = HCD_GetURB_State(pdev, VIDEO_Handle->camera.Pipe);
      if((result == URB_DONE) && ((HCD_GetCurrentFrame(pdev) - VIDEO_Handle->camera.timer) >= VIDEO_Handle->camera.Poll))
      {
        VIDEO_Handle->camera.timer = HCD_GetCurrentFrame(pdev);
        volatile uint32_t rxlen = HCD_GetXferCnt(pdev, VIDEO_Handle->camera.Pipe);//Return the last transfered packet size.
        uvc_stream_data_process((uint16_t)rxlen);
          
        USBH_IsocReceiveData(pdev,
                            (uint8_t*)tmp_packet_framebuffer,
                            VIDEO_Handle->camera.EpSize,
                            VIDEO_Handle->camera.Pipe);
      }
    break;
    default:
      break;
  }
  
  return status;  
}

//*****************************************************************************
//*****************************************************************************

USBH_Status USBH_VS_SetCur(USB_OTG_CORE_HANDLE *pdev ,void *phost, uint16_t request_type,VIDEO_ProbeTypedef* ProbeParams)
{
  uint16_t wLength = 26;
  USBH_HOST *pphost = phost;
  
  //H2D - host to device
  pphost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
  USB_REQ_TYPE_CLASS;
  
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  
  pphost->Control.setup.b.bRequest = UVC_SET_CUR;
  pphost->Control.setup.b.wValue.w = request_type;
  pphost->Control.setup.b.wIndex.w = VIDEO_Handle->camera.interface; //Video Streaming interface number
  pphost->Control.setup.b.wLength.w = wLength;

  return USBH_CtlReq(pdev, pphost, (uint8_t *)ProbeParams, wLength);
}

USBH_Status USBH_VS_GetCur(USB_OTG_CORE_HANDLE *pdev ,void *phost, uint16_t request_type, uint8_t request, VIDEO_ProbeTypedef* ProbeParams)
{
  USBH_HOST *pphost = phost;
  uint16_t wLength = 26;

  //H2D - host to device
  pphost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
  USB_REQ_TYPE_CLASS;
  
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  
  pphost->Control.setup.b.bRequest = request;
  pphost->Control.setup.b.wValue.w = request_type;
  pphost->Control.setup.b.wIndex.w = VIDEO_Handle->camera.interface; //Video Streaming interface number
  pphost->Control.setup.b.wLength.w = wLength;

  USBH_Status status = USBH_CtlReq(pdev,phost, (uint8_t *)ProbeParams, wLength);
  if (status == USBH_OK)
  {
    if (ProbeParams->dwMaxVideoFrameSize > 0)
    {
      return USBH_OK;
    }
    else
      return USBH_FAIL;
  }
  
  return status;
}

USBH_Status USBH_VS_GetErr(USB_OTG_CORE_HANDLE *pdev ,void *phost)
{
  USBH_HOST *pphost = phost;
  uint16_t wLength = 1;

  //H2D - host to device
  pphost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
  USB_REQ_TYPE_CLASS;
  
  VIDEO_HandleTypeDef *VIDEO_Handle = &UVC_Machine;
  
  pphost->Control.setup.b.bRequest = UVC_GET_CUR;
  pphost->Control.setup.b.wValue.w = VS_STREAM_ERROR_CODE_CONTROL << 8;
  pphost->Control.setup.b.wIndex.w = VIDEO_Handle->camera.interface; //Video Streaming interface number
  pphost->Control.setup.b.wLength.w = wLength;

  USBH_Status status = USBH_CtlReq(pdev,phost, (uint8_t *)&errcode, wLength);
    
  return status;
}
