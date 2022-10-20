/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/custom_stm.c
  * Description        : Custom Example Service.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "custom_stm.h"
#include "common_blesvc.h"

/* USER CODE BEGIN Includes */
#include "stm32wbxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct {
    uint16_t CustomCntr_SrvHdle;  /**< ControlService handle */
    uint16_t CustomOn_ChrHdle;    /**< OnCharachteristic handle */
    uint16_t CustomOf_ChrHdle;    /**< OfCharacteristic handle */
    uint16_t CustomDat_SrvHdle;   /**< DataService handle */
    uint16_t CustomVlt_ChrHdle;   /**< ViolationCountCharacteristic handle */
    uint16_t CustomDbg_A_ChrHdle; /**< DebugAxisService handle */
} CustomContext_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED 1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH UUID_TYPE_128
#else
#define BM_UUID_LENGTH UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint8_t SizeOn_Chr = 1;
static const uint8_t SizeOf_Chr = 1;
static const uint8_t SizeVlt_Chr = 3;
static const uint8_t SizeDbg_A_Chr = 247;
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT")
static CustomContext_t CustomContext;

/**
 * END of Section BLE_DRIVER_CONTEXT
 */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t Custom_STM_Event_Handler(void *pckt);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
    do {                                                                                                                                                                 \
        uuid_struct[0] = uuid_0;                                                                                                                                         \
        uuid_struct[1] = uuid_1;                                                                                                                                         \
        uuid_struct[2] = uuid_2;                                                                                                                                         \
        uuid_struct[3] = uuid_3;                                                                                                                                         \
        uuid_struct[4] = uuid_4;                                                                                                                                         \
        uuid_struct[5] = uuid_5;                                                                                                                                         \
        uuid_struct[6] = uuid_6;                                                                                                                                         \
        uuid_struct[7] = uuid_7;                                                                                                                                         \
        uuid_struct[8] = uuid_8;                                                                                                                                         \
        uuid_struct[9] = uuid_9;                                                                                                                                         \
        uuid_struct[10] = uuid_10;                                                                                                                                       \
        uuid_struct[11] = uuid_11;                                                                                                                                       \
        uuid_struct[12] = uuid_12;                                                                                                                                       \
        uuid_struct[13] = uuid_13;                                                                                                                                       \
        uuid_struct[14] = uuid_14;                                                                                                                                       \
        uuid_struct[15] = uuid_15;                                                                                                                                       \
    } while (0)

/* Hardware Characteristics Service */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 D973F2E0-B19E-11E2-9E96-0800200C9A66: Service 128bits UUID
 D973F2E1-B19E-11E2-9E96-0800200C9A66: Characteristic_1 128bits UUID
 D973F2E2-B19E-11E2-9E96-0800200C9A66: Characteristic_2 128bits UUID
 */
#define COPY_CONTROLSERVICE_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x00, 0x00, 0x00, 0x01, 0xcc, 0x7a, 0x48, 0x2a, 0x98, 0x4a, 0x7f, 0x2e, 0xd5, 0xb3, 0xe5, 0x8f)
#define COPY_ONCHARACHTERISTIC_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x00, 0x00, 0x00, 0x10, 0x8e, 0x22, 0x45, 0x41, 0x9d, 0x4c, 0x21, 0xed, 0xae, 0x82, 0xed, 0x19)
#define COPY_OFCHARACTERISTIC_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x00, 0x00, 0x00, 0x11, 0x8e, 0x22, 0x45, 0x41, 0x9d, 0x4c, 0x21, 0xed, 0xae, 0x82, 0xed, 0x19)
#define COPY_DATASERVICE_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x00, 0x00, 0x00, 0x02, 0xcc, 0x7a, 0x48, 0x2a, 0x98, 0x4a, 0x7f, 0x2e, 0xd5, 0xb3, 0xe5, 0x8f)
#define COPY_VIOLATIONCOUNTCHARACTERISTIC_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x00, 0x00, 0x00, 0x20, 0x8e, 0x22, 0x45, 0x41, 0x9d, 0x4c, 0x21, 0xed, 0xae, 0x82, 0xed, 0x19)
#define COPY_DEBUGAXISSERVICE_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x00, 0x00, 0x00, 0x21, 0x8e, 0x22, 0x45, 0x41, 0x9d, 0x4c, 0x21, 0xed, 0xae, 0x82, 0xed, 0x19)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t Custom_STM_Event_Handler(void *Event) {
    hci_event_pckt                        *  event_pckt;
    evt_blecore_aci                       *  blecore_evt;
    aci_gatt_attribute_modified_event_rp0 *  attribute_modified;
    aci_gatt_read_permit_req_event_rp0    *  read_request;

    SVCCTL_EvtAckStatus_t                   return_value;
    Custom_STM_App_Notification_evt_t       notification;
    /* USER CODE BEGIN Custom_STM_Event_Handler_1 */

    /* USER CODE END Custom_STM_Event_Handler_1 */

    return_value = SVCCTL_EvtNotAck;
    event_pckt = (hci_event_pckt *) (((hci_uart_pckt *) Event)->data);

    switch (event_pckt->evt) {
        case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
            blecore_evt = (evt_blecore_aci *) event_pckt->data;
            switch (blecore_evt->ecode) {

                case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
                    /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
                    attribute_modified = (aci_gatt_attribute_modified_event_rp0 *) blecore_evt->data;

                    // TODO: is offset there?
                    if (attribute_modified->Attr_Handle == CustomContext.CustomOn_ChrHdle + 1) {  // process ON_CHR write
                        return_value = SVCCTL_EvtAckFlowEnable;
                        notification.Custom_Evt_Opcode = CUSTOM_STM_ON_CHR_WRITE_EVT;
                        notification.DataTransfered.Length = attribute_modified->Attr_Data_Length;
                        notification.DataTransfered.pPayload = attribute_modified->Attr_Data;
                        Custom_STM_App_Notification(&notification);
                    } else if (attribute_modified->Attr_Handle == CustomContext.CustomOf_ChrHdle + 1) {  // process OF_CHR write
                        return_value = SVCCTL_EvtAckFlowEnable;
                        notification.Custom_Evt_Opcode = CUSTOM_STM_OF_CHR_WRITE_EVT;
                        notification.DataTransfered.Length = attribute_modified->Attr_Data_Length;
                        notification.DataTransfered.pPayload = attribute_modified->Attr_Data;
                        Custom_STM_App_Notification(&notification);
                    } else if (attribute_modified->Attr_Handle == CustomContext.CustomDbg_A_ChrHdle + 2) {  // process A_CHR notification
                        return_value = SVCCTL_EvtAckFlowEnable;

                        switch (attribute_modified->Attr_Data[0]) {
                            case COMSVC_Notification: {
                                notification.Custom_Evt_Opcode = CUSTOM_STM_DBG_A_CHR_NOTIFY_ENABLED_EVT;
                                Custom_STM_App_Notification(&notification);
                                break ;
                            }

                            case !COMSVC_Notification: {
                                notification.Custom_Evt_Opcode = CUSTOM_STM_DBG_A_CHR_NOTIFY_DISABLED_EVT;
                                Custom_STM_App_Notification(&notification);
                                break ;
                            }
                        }
                    /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
                    break;
                }
                case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE:
                    /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ */
                    read_request = (aci_gatt_read_permit_req_event_rp0 *) blecore_evt->data;

                    if (read_request->Attribute_Handle == CustomContext.CustomVlt_ChrHdle + 1) {  // VLT_CHR read request
                        notification.Custom_Evt_Opcode = CUSTOM_STM_VLT_CHR_READ_EVT;
                        aci_gatt_allow_read(read_request->Connection_Handle);
                        Custom_STM_App_Notification(&notification);
                    }
                    /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ */
                    break;
                case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
                    /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ */
                    /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ */
                    break;

                default:
                    /* USER CODE BEGIN EVT_DEFAULT */

                    /* USER CODE END EVT_DEFAULT */
                    break;
            }
            /* USER CODE BEGIN EVT_VENDOR*/

            /* USER CODE END EVT_VENDOR*/
            break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

            /* USER CODE BEGIN EVENT_PCKT_CASES*/

            /* USER CODE END EVENT_PCKT_CASES*/

        default:
            break;
    }

    /* USER CODE BEGIN Custom_STM_Event_Handler_2 */

    /* USER CODE END Custom_STM_Event_Handler_2 */

    return (return_value);
} /* end Custom_STM_Event_Handler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void SVCCTL_InitCustomSvc(void) {

    Char_UUID_t uuid;
    /* USER CODE BEGIN SVCCTL_InitCustomSvc_1 */

    /* USER CODE END SVCCTL_InitCustomSvc_1 */

    /**
   *	Register the event handler to the BLE controller
   */
    SVCCTL_RegisterSvcHandler(Custom_STM_Event_Handler);

    /*
     *          ControlService
     *
     * Max_Attribute_Records = 1 + 2*2 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
     * service_max_attribute_record = 1 for ControlService +
     *                                2 for OnCharachteristic +
     *                                2 for OfCharacteristic +
     *                              = 5
     */

    COPY_CONTROLSERVICE_UUID(uuid.Char_UUID_128);
    aci_gatt_add_service(UUID_TYPE_128,
                         (Service_UUID_t *) &uuid,
                         PRIMARY_SERVICE,
                         5,
                         &(CustomContext.CustomCntr_SrvHdle));

    /**
     *  OnCharachteristic
     */
    COPY_ONCHARACHTERISTIC_UUID(uuid.Char_UUID_128);
    aci_gatt_add_char(CustomContext.CustomCntr_SrvHdle,
                      UUID_TYPE_128, &uuid,
                      SizeOn_Chr,
                      CHAR_PROP_WRITE,
                      ATTR_PERMISSION_NONE,
                      GATT_NOTIFY_ATTRIBUTE_WRITE,
                      0x10,
                      CHAR_VALUE_LEN_CONSTANT,
                      &(CustomContext.CustomOn_ChrHdle));
    /**
     *  OfCharacteristic
     */
    COPY_OFCHARACTERISTIC_UUID(uuid.Char_UUID_128);
    aci_gatt_add_char(CustomContext.CustomCntr_SrvHdle,
                      UUID_TYPE_128, &uuid,
                      SizeOf_Chr,
                      CHAR_PROP_WRITE,
                      ATTR_PERMISSION_NONE,
                      GATT_NOTIFY_ATTRIBUTE_WRITE,
                      0x10,
                      CHAR_VALUE_LEN_CONSTANT,
                      &(CustomContext.CustomOf_ChrHdle));

    /*
     *          DataService
     *
     * Max_Attribute_Records = 1 + 2*2 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
     * service_max_attribute_record = 1 for DataService +
     *                                2 for ViolationCountCharacteristic +
     *                                2 for DebugAxisService +
     *                                1 for DebugAxisService configuration descriptor +
     *                              = 6
     */

    COPY_DATASERVICE_UUID(uuid.Char_UUID_128);
    aci_gatt_add_service(UUID_TYPE_128,
                         (Service_UUID_t *) &uuid,
                         PRIMARY_SERVICE,
                         6,
                         &(CustomContext.CustomDat_SrvHdle));

    /**
     *  ViolationCountCharacteristic
     */
    COPY_VIOLATIONCOUNTCHARACTERISTIC_UUID(uuid.Char_UUID_128);
    aci_gatt_add_char(CustomContext.CustomDat_SrvHdle,
                      UUID_TYPE_128, &uuid,
                      SizeVlt_Chr,
                      CHAR_PROP_READ,
                      ATTR_PERMISSION_NONE,
                      GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                      0x10,
                      CHAR_VALUE_LEN_CONSTANT,
                      &(CustomContext.CustomVlt_ChrHdle));
    /**
     *  DebugAxisService
     */
    COPY_DEBUGAXISSERVICE_UUID(uuid.Char_UUID_128);
    aci_gatt_add_char(CustomContext.CustomDat_SrvHdle,
                      UUID_TYPE_128, &uuid,
                      SizeDbg_A_Chr,
                      CHAR_PROP_NOTIFY,
                      ATTR_PERMISSION_NONE,
                      GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                      0x10,
                      CHAR_VALUE_LEN_CONSTANT,
                      &(CustomContext.CustomDbg_A_ChrHdle));

    /* USER CODE BEGIN SVCCTL_InitCustomSvc_2 */

    /* USER CODE END SVCCTL_InitCustomSvc_2 */

    return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus Custom_STM_App_Update_Char(Custom_STM_Char_Opcode_t CharOpcode, uint8_t *pPayload) {
    tBleStatus result = BLE_STATUS_INVALID_PARAMS;
    /* USER CODE BEGIN Custom_STM_App_Update_Char_1 */

    /* USER CODE END Custom_STM_App_Update_Char_1 */

    switch (CharOpcode) {

        case CUSTOM_STM_ON_CHR:
            result = aci_gatt_update_char_value(CustomContext.CustomCntr_SrvHdle,
                                                CustomContext.CustomOn_ChrHdle,
                                                0,          /* charValOffset */
                                                SizeOn_Chr, /* charValueLen */
                                                (uint8_t *) pPayload);
            /* USER CODE BEGIN CUSTOM_STM_ON_CHR*/
            /* USER CODE END CUSTOM_STM_ON_CHR*/
            break;

        case CUSTOM_STM_OF_CHR:
            result = aci_gatt_update_char_value(CustomContext.CustomCntr_SrvHdle,
                                                CustomContext.CustomOf_ChrHdle,
                                                0,          /* charValOffset */
                                                SizeOf_Chr, /* charValueLen */
                                                (uint8_t *) pPayload);
            /* USER CODE BEGIN CUSTOM_STM_OF_CHR*/

            /* USER CODE END CUSTOM_STM_OF_CHR*/
            break;

        case CUSTOM_STM_VLT_CHR:
            result = aci_gatt_update_char_value(CustomContext.CustomDat_SrvHdle,
                                                CustomContext.CustomVlt_ChrHdle,
                                                0,           /* charValOffset */
                                                SizeVlt_Chr, /* charValueLen */
                                                (uint8_t *) pPayload);
            /* USER CODE BEGIN CUSTOM_STM_VLT_CHR*/

            /* USER CODE END CUSTOM_STM_VLT_CHR*/
            break;

        case CUSTOM_STM_DBG_A_CHR:
            result = aci_gatt_update_char_value(CustomContext.CustomDat_SrvHdle,
                                                CustomContext.CustomDbg_A_ChrHdle,
                                                0,             /* charValOffset */
                                                SizeDbg_A_Chr, /* charValueLen */
                                                (uint8_t *) pPayload);
            /* USER CODE BEGIN CUSTOM_STM_DBG_A_CHR*/

            /* USER CODE END CUSTOM_STM_DBG_A_CHR*/
            break;

        default:
            break;
    }

    /* USER CODE BEGIN Custom_STM_App_Update_Char_2 */

    /* USER CODE END Custom_STM_App_Update_Char_2 */

    return result;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
