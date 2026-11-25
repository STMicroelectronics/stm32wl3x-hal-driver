/**
  ******************************************************************************
  * @file    stm32wl3x_hal_mrsubg_timer.h
  * @author  GPM WBL Application Team
  * @brief   MRSUBG Timer HAL module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024-2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WL3x_HAL_MRSUBG_TIMER_H
#define STM32WL3x_HAL_MRSUBG_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32wl3x_hal_def.h"
#include "stm32wl3x_ll_mrsubg_timer.h"
#include "stm32wl3x_hal_pwr.h"

/** @addtogroup STM32WL3x_HAL_Driver
  * @{
  */

/** @addtogroup MRSUBG_TIMER
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup MRSUBG_TIMER_Exported_Types MRSUBG Timer Exported Types
  * @{
  */

/**
  * @brief MRSUBG TIMER Init Structure definition
  */
typedef struct
{
  uint32_t HSE_XTAL_freq;                /*!< HSE XTAL frequency */
  uint16_t XTAL_StartupTime;             /*!< XTAL startup in us unit */
  BOOL enableInitialCalibration;         /*!< Enable initial estimation of the frequency of the
            * Low Speed Oscillator, otherwise it will be assumed
            * fixed at 32.768 kHz. */
  uint32_t periodicCalibrationInterval;  /*!< Periodic calibration interval in ms, to disable set to 0 */
} MRSUBG_TIMER_InitTypeDef;

/**
  * @brief  Virtual timer callback function type.
  * @param  handle Pointer to the virtual timer handle that expired.
  */
typedef void (*VTIMER_CallbackType)(void *);

/**
  * @brief VIRTUAL TIMER handle Structure definition
  */
typedef struct VTIMER_HandleTypeS
{
  uint64_t expiryTime; /*!< Absolute expiration time in system time units.
                            This field is updated when the timer is started. */
  VTIMER_CallbackType callback; /*!< Function called when the timer expires. */
  BOOL active; /*!< Internal active state of the handle. */
  struct VTIMER_HandleTypeS *next; /*!< Internal link used to chain handles in the expiration queue. */
  void *userData; /*!< User-defined context pointer left untouched by the timer service. */
} VTIMER_HandleType_t;

/**
  * @brief  MRSUBG timer queue state.
  */
typedef enum
{
  MRSUBG_TIMER_OFF = 0,
  MRSUBG_TIMER_PENDING = 1,
} MRSUBG_TIMER_Status;


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup MRSUBG_TIMER_Exported_Constants MRSUBG Timer Exported Constants
  * @{
  */


/**
  * @brief MRSUBG TIMER Status
  */
#define WAKEUP_MRSUBG_TIMER_BUSY   (0x01U)

#define TIME_MAX_VALUE 255

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup MRSUBG_Timer_Exported_Functions MRSUBG Timer Exported Functions
  * @{
  */

/**
  * @brief  Notify user code that the CPU wakeup timer interrupt occurred.
  * @note   Override this weak function in user code to execute application-specific
  *         processing when the CPU wakeup interrupt is received.
  * @retval None
  */
void HAL_MRSUBG_TIMER_CPU_WKUP_Callback(void);

/**
  * @brief  Handle the CPU wakeup timer interrupt.
  * @note   The default handler forwards the event to HAL_MRSUBG_TIMER_CPU_WKUP_Callback().
  * @retval None
  */
void HAL_MRSUBG_TIMER_CPU_WKUP_IRQHandler(void);

/** @defgroup MRSUBG_Timer_EF_Config MRSUBG Timer Init and Configuration functions
  * @{
  */

/**
  * @brief  Initialize the MRSUBG timer service.
  * @param  MRSUBG_TIMER_InitStruct Pointer to the timer initialization parameters.
  * @note   Call this function once before starting any virtual timer.
  *         The routine initializes the virtual-time base, configures the wakeup timer support,
  *         and schedules the internal calibration or anti-wrap maintenance timer.
  * @retval None
  */
void HAL_MRSUBG_TIMER_Init(MRSUBG_TIMER_InitTypeDef *MRSUBG_TIMER_InitStruct);

/**
  * @brief  Service the virtual timer state machine.
  * @note   Call this function regularly from the main loop context.
  *         The routine serves expired virtual timers, executes their callbacks,
  *         refreshes the next hardware wakeup deadline, and finalizes slow-clock calibration
  *         when a periodic calibration cycle completes.
  * @retval None
  */
void HAL_MRSUBG_TIMER_Tick(void);

/**
  * @}
  */

/** @defgroup MRSUBG_Timer_EF_Management MRSUBG Timer Management functions
  * @{
  */

/**
  * @brief  Report the current radio wakeup timer status.
  * @param  time Pointer to the variable that receives the programmed RFIP wakeup time.
  *         The function stores 0 in this variable when no radio wakeup timer is active.
  * @retval 0 No RFIP wakeup timer is currently programmed.
  * @retval 1 An RFIP wakeup timer is currently programmed.
  */
uint8_t HAL_MRSUBG_TIMER_GetRadioTimerValue(uint32_t *time);

/**
  * @brief  Evaluate the deepest low-power mode currently allowed by the timer service.
  * @note   The decision takes into account pending radio wakeup activity, queued virtual timers,
  *         and the configured high-speed clock startup margin.
  * @return Power-save level that can be entered without missing the next scheduled timer event.
  */
PowerSaveLevels HAL_MRSUBG_TIMER_PowerSaveLevelCheck(void);

/**
  * @}
  */

/** @defgroup MRSUBG_Timer_EF_Management_TimeUnit MRSUBG Timer Management Time Unit
  * @{
  */

/**
  * @brief  Convert a duration from microseconds to machine time units.
  * @param  time Duration expressed in microseconds.
  * @return Equivalent duration expressed in machine time units, based on the current
  *         interpolated timer frequency.
  */
uint64_t HAL_MRSUBG_TIMER_UsToMachinetime(uint64_t time);

/**
  * @brief  Convert a duration from machine time units to microseconds.
  * @param  time Duration expressed in machine time units.
  * @return Equivalent duration expressed in microseconds, based on the current
  *         interpolated timer frequency.
  */
uint64_t HAL_MRSUBG_TIMER_MachinetimeToUs(uint64_t time);

/**
  * @brief  This function returns the current reference time expressed in system time units.
  *         The returned value can be used as absolute time parameter where needed in the other
  *         HAL_MRSUBG_TIMER* APIs
  * @return absolute current time expressed in system time units.
  */
uint64_t HAL_MRSUBG_TIMER_GetCurrentSysTime(void);

/**
  * @}
  */

/** @defgroup MRSUBG_Timer_EF_Virtual_Timer MRSUBG Timer Virtual Timer functions
  * @{
  */

/**
  * @brief Starts a one-shot virtual timer for the given relative timeout value expressed in us
  * @param timerHandle: The virtual timer
  * @param usRelTimeout: The relative time, from current time, expressed in us
  * @retval 0 if the timerHandle is valid.
  * @retval 1 if the timerHandle is not valid. It is already started.
  */
uint32_t HAL_MRSUBG_TIMER_StartVirtualTimer(VTIMER_HandleType_t *timerHandle, uint64_t usRelTimeout);

/**
  * @brief Starts a one-shot virtual timer for the given relative timeout value expressed in ms
  * @param timerHandle: The virtual timer
  * @param msRelTimeout: The relative time, from current time, expressed in ms
  * @retval 0 if the timerHandle is valid.
  * @retval 1 if the timerHandle is not valid. It is already started.
  */
uint32_t HAL_MRSUBG_TIMER_StartVirtualTimerMs(VTIMER_HandleType_t *timerHandle, uint32_t msRelTimeout);

/**
  * @brief  Stop a virtual timer.
  * @param  timerHandle Pointer to the virtual timer handle to stop.
  * @note   If the handle is present in the queue, the routine removes it and refreshes the
  *         next hardware wakeup deadline when required.
  * @retval None
  */
void HAL_MRSUBG_TIMER_StopVirtualTimer(VTIMER_HandleType_t *timerHandle);

/**
  * @brief Returns the absolute expiry time of a running virtual timer expressed in internal system time units.
  * @param timerHandle: The virtual timer
  * @retval sysTime: Absolute time expressed in internal system time units.
  */
uint64_t HAL_MRSUBG_TIMER_ExpiryTime(VTIMER_HandleType_t *timerHandle);

/**
  * @brief  Virtual timer Timeout Callback. It signals that a host timeout occurred.
  * @retval None
  */
void HAL_MRSUBG_TIMER_TimeoutCallback(void);

/**
  * @brief  Count the virtual timers currently linked in the queue.
  * @return Number of timer handles present in the internal queue.
  */
uint32_t HAL_MRSUBG_TIMER_GetPendingTimers(void);

/**
  * @brief  Initialize the low-level resources required by the MRSUBG timer service.
  * @note   Override this weak function in user code to configure clocks, interrupts,
  *         or board-specific resources needed by the wakeup timer path.
  * @retval None
  */
void HAL_MRSUBG_TIMER_MspInit(void);

/**
  * @brief  De-initialize the low-level resources used by the MRSUBG timer service.
  * @note   Override this weak function in user code to release board-specific resources
  *         configured by HAL_MRSUBG_TIMER_MspInit().
  * @retval None
  */
void HAL_MRSUBG_TIMER_MspDeInit(void);

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*STM32WL3x_HAL_MRSUBG_TIMER_H */
