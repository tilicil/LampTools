#ifndef __ANIME_TASK_H
#define __ANIME_TASK_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef enum
{
    ANIME_MODE_DRL_ON = 0,
    ANIME_MODE_PL_ON,
    ANIME_MODE_WEL,
    ANIME_MODE_FRW,
    ANIME_MODE_MAX,
}Anime_Mode_t;                          /*动画模式枚举*/

#define ANIME_TASK_TIMERCNT         (10U)      /*任务轮询时间*/
#define ANIME_TASK_TIMEREXPIERED    (ANIME_TASK_TIMERCNT * 4)   /*任务超时执行时间*/
#define ANIME_TASK_DIMMING_RATE     (4U)       /*调光率 * 执行时间 = 动画帧间隔时间*/
#define ANIME_TASK_ARR_LEN          (16U)       /*动画单帧数组长度*/

/*****************************************************************************/
typedef void (*Anime_ArrExecute_Func)(const uint8_t *inputarr);

typedef struct
{
    uint8_t  u8target_mode;
    uint16_t u16target_delay;
    uint8_t  u8target_steps;
    uint8_t  u8target_reload_times;
    const uint8_t  (*pu8target_arr)[ANIME_TASK_ARR_LEN];
}Anime_Target_Info_t;

typedef struct
{
    uint16_t u16run_timercnt;
    uint8_t  u8run_last_mode;
    uint8_t  u8run_curr_mode;
    uint16_t u16run_delaycnt;
    uint16_t u16run_step_index;
}Anime_Running_Info_t;

typedef struct
{
    Anime_Mode_t tinput_mode;
    Anime_ArrExecute_Func tinput_func;
    bool  bfeedback_runtoend;
}Anime_Input_Info_t;

/*********************************************************************************/
extern Anime_Input_Info_t tInput_Info;
extern const Anime_Target_Info_t tAnime_Target_Info[ANIME_MODE_MAX];
extern Anime_Running_Info_t tAnime_Running_Info;

/**
 * @breif: 
 * @note: 
 * @return [*]
 */
extern void Anmie_Arr_Execute(uint8_t const  *input_arr);

/**
 * @breif: 
 * @note: 
 * @param [Anime_Input_Info_t] *ptinput_info
 * @param [uint8_t] mode
 * @return [*]
 */
extern uint8_t Anime_InputInfo_Init(Anime_Input_Info_t *ptinput_info, uint8_t mode, void (*func)(const uint8_t*));
/**
 * @breif: 
 * @note: 
 * @return [*]
 */
/**/
extern void Anime_Task_Run(Anime_Input_Info_t *ptinput_info,Anime_Running_Info_t *ptrun_info, Anime_Target_Info_t const *ptarget_info);

#endif
