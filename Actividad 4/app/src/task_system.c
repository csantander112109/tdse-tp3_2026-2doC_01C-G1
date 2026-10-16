/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"

#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"

/********************** macros and definitions *******************************/
#define DEL_SYS_MIN			0ul
#define DEL_SYS_MED			250ul
#define DEL_SYS_MAX			500ul

/* Modes to excite Task System */
typedef enum task_system_mode {NORMAL, SETUP, MODE_QTY} task_system_mode_t;

#define SYSTEM_DTA_QTY	MODE_QTY

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];

/********************** internal functions declaration ***********************/
void task_system_normal_statechart(void);
void task_system_setup_statechart(void);

void task_system_set_mode(task_system_mode_t);

/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking Code";
const char *p_task_system__ 	= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/
task_system_mode_t g_task_system_mode;

// datos por defecto de los motores segun imagen de la consigna
static motor_config_t motors[2] = {
    {true, 8, S_LEFT},
    {true, 8, S_LEFT}
};

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	uint32_t index;
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_system_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system__);

	init_event_task_system();

	task_system_set_mode(NORMAL);

	for (index = 0; SYSTEM_DTA_QTY > index; index++)
	{
		/* Update Task System Data Pointer */
		p_task_system_dta = &task_system_dta_list[index];

		/* Init & Print out: Task execution FSM */
		state = ST_SYS_IDLE;
		p_task_system_dta->state = state;

		event = EV_SYS_IDLE;
		p_task_system_dta->event = event;

		b_event = false;
		p_task_system_dta->flag = b_event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event,
					GET_NAME(b_event), (b_event ? "true" : "false"));
	}

	put_event_task_display(0, 0, "task_system_mode");
	put_event_task_display(0, 1, " NORMAL         ");

	task_system_set_mode(NORMAL);
}

void task_system_update(void *parameters)
{
	/* Run Task Statechart */
	switch (g_task_system_mode)
	{
		case NORMAL:

			task_system_normal_statechart();

			break;

		case SETUP:

			task_system_setup_statechart();

			break;

		default:

			task_system_set_mode(NORMAL);

			break;
		}
}

//void task_system_normal_statechart(void)
//{
//	task_system_dta_t *p_task_system_dta;
//
//	/* Update Task System Data Pointer */
//	p_task_system_dta = &task_system_dta_list[NORMAL];
//
//	if (true == any_event_task_system())
//	{
//		p_task_system_dta->flag = true;
//		p_task_system_dta->event = get_event_task_system();
//	}
//
//	switch (p_task_system_dta->state)
//	{
//		case ST_SYS_IDLE:
//
//			if ((true == p_task_system_dta->flag) && (EV_SYS_BTN_A == p_task_system_dta->event))
//			{
//				p_task_system_dta->flag = false;
//				put_event_task_actuator(EV_LED_ACTIVE, ID_LED_A);
//				p_task_system_dta->state = ST_SYS_ACTIVE;
//
//				put_event_task_display(0, 0, "task_system_mode");
//				put_event_task_display(0, 1, " SETUP          ");
//
//				task_system_set_mode(SETUP);
//			}
//
//			break;
//
//		case ST_SYS_ACTIVE:
//
//			if ((true == p_task_system_dta->flag) && (EV_SYS_IDLE == p_task_system_dta->event))
//			{
//				p_task_system_dta->flag = false;
//				put_event_task_actuator(EV_LED_IDLE, ID_LED_A);
//				p_task_system_dta->state = ST_SYS_IDLE;
//			}
//
//			break;
//
//		default:
//
//			p_task_system_dta->tick  = DEL_SYS_MIN;
//			p_task_system_dta->state = ST_SYS_IDLE;
//			p_task_system_dta->event = EV_SYS_IDLE;
//			p_task_system_dta->flag = false;
//
//			break;
//	}
//}
//
//void task_system_setup_statechart(void)
//{
//	task_system_dta_t *p_task_system_dta;
//
//	/* Update Task System Data Pointer */
//	p_task_system_dta = &task_system_dta_list[SETUP];
//
//	if (true == any_event_task_system())
//	{
//		p_task_system_dta->flag = true;
//		p_task_system_dta->event = get_event_task_system();
//	}
//
//	switch (p_task_system_dta->state)
//	{
//		case ST_SYS_IDLE:
//
//			if ((true == p_task_system_dta->flag) && (EV_SYS_BTN_A == p_task_system_dta->event))
//			{
//				p_task_system_dta->flag = false;
//				put_event_task_actuator(EV_LED_ACTIVE, ID_LED_A);
//				p_task_system_dta->state = ST_SYS_ACTIVE;
//
//				put_event_task_display(0, 0, "task_system_mode");
//				put_event_task_display(0, 1, " NORMAL         ");
//
//				task_system_set_mode(NORMAL);
//			}
//
//			break;
//
//		case ST_SYS_ACTIVE:
//
//			if ((true == p_task_system_dta->flag) && (EV_SYS_IDLE == p_task_system_dta->event))
//			{
//				p_task_system_dta->flag = false;
//				put_event_task_actuator(EV_LED_IDLE, ID_LED_A);
//				p_task_system_dta->state = ST_SYS_IDLE;
//			}
//
//			break;
//
//		default:
//
//			p_task_system_dta->tick  = DEL_SYS_MIN;
//			p_task_system_dta->state = ST_SYS_IDLE;
//			p_task_system_dta->event = EV_SYS_IDLE;
//			p_task_system_dta->flag = false;
//
//			break;
//	}
//}

void task_system_normal_statechart(void)
{
    task_system_dta_t *p_dta = &task_system_dta_list[NORMAL];
    char buf_row0[17];
    char buf_row1[17];

    if (true == any_event_task_system()) {
        p_dta->flag = true;
        p_dta->event = get_event_task_system();
    }

    switch (p_dta->state)
    {
        case ST_SYS_MAIN:
            if ((true == p_dta->flag) && (EV_SYS_ENTER == p_dta->event))
            {
                p_dta->flag = false;

                task_system_dta_list[SETUP].state = ST_SYS_MENU1;
                task_system_dta_list[SETUP].current_motor_idx = M_MOTOR_1;

                task_system_set_mode(SETUP);

                put_event_task_display(0, 0, "Select Motor:   ");
                put_event_task_display(0, 1, "> Motor 1       ");
            }
            else if (true == p_dta->flag)
            {
                p_dta->flag = false;
            }
            break;

        default:
            p_dta->state = ST_SYS_MAIN;
            p_dta->flag = false;

            snprintf(buf_row0, 17, "M1:%s,%d,%c    ", motors[0].power?"ON ":"OFF", motors[0].speed, motors[0].spin==S_LEFT?'L':'R');
            snprintf(buf_row1, 17, "M2:%s,%d,%c    ", motors[1].power?"ON ":"OFF", motors[1].speed, motors[1].spin==S_LEFT?'L':'R');
            put_event_task_display(0, 0, buf_row0);
            put_event_task_display(0, 1, buf_row1);
            break;
    }
}

void task_system_setup_statechart(void)
{
    task_system_dta_t *p_dta = &task_system_dta_list[SETUP];
    char buf_row0[17];
    char buf_row1[17];
    bool update_lcd = false;

    if (true == any_event_task_system()) {
        p_dta->flag = true;
        p_dta->event = get_event_task_system();
    }

    if (!p_dta->flag) return;

    p_dta->flag = false;
    update_lcd = true;

    switch (p_dta->state)
    {
        case ST_SYS_MENU1:
            if (EV_SYS_ESCAPE == p_dta->event) {
                task_system_dta_list[NORMAL].state = ST_SYS_MAIN;
                task_system_set_mode(NORMAL);

                snprintf(buf_row0, 17, "M1:%s,%d,%c    ", motors[0].power?"ON ":"OFF", motors[0].speed, motors[0].spin==S_LEFT?'L':'R');
                snprintf(buf_row1, 17, "M2:%s,%d,%c    ", motors[1].power?"ON ":"OFF", motors[1].speed, motors[1].spin==S_LEFT?'L':'R');
                put_event_task_display(0, 0, buf_row0);
                put_event_task_display(0, 1, buf_row1);
                return;
            }
            else if (EV_SYS_ENTER == p_dta->event) {
                p_dta->current_param_idx = P_POWER;
                p_dta->state = ST_SYS_MENU2;
            }
            else if (EV_SYS_NEXT == p_dta->event) {
                p_dta->current_motor_idx = (p_dta->current_motor_idx == M_MOTOR_1) ? M_MOTOR_2 : M_MOTOR_1;
            }
            break;

        case ST_SYS_MENU2:
            if (EV_SYS_ESCAPE == p_dta->event) {
                p_dta->state = ST_SYS_MENU1;
            }
            else if (EV_SYS_ENTER == p_dta->event) {
                p_dta->state = ST_SYS_MENU3;
            }
            else if (EV_SYS_NEXT == p_dta->event) {
                if (p_dta->current_param_idx == P_POWER) p_dta->current_param_idx = P_SPEED;
                else if (p_dta->current_param_idx == P_SPEED) p_dta->current_param_idx = P_SPIN;
                else p_dta->current_param_idx = P_POWER;
            }
            break;

        case ST_SYS_MENU3:
            if (EV_SYS_ESCAPE == p_dta->event || EV_SYS_ENTER == p_dta->event) {
                p_dta->state = ST_SYS_MENU2;
            }
            else if (EV_SYS_NEXT == p_dta->event) {
                motor_config_t *motor_activo = &motors[p_dta->current_motor_idx];

                if (p_dta->current_param_idx == P_POWER) {
                    motor_activo->power = !motor_activo->power;
                }
                else if (p_dta->current_param_idx == P_SPEED) {
                    motor_activo->speed++;
                    if (motor_activo->speed > 9) motor_activo->speed = 0;
                }
                else if (p_dta->current_param_idx == P_SPIN) {
                    motor_activo->spin = (motor_activo->spin == S_LEFT) ? S_RIGHT : S_LEFT;
                }
            }
            break;

        default:
            p_dta->state = ST_SYS_MENU1;
            break;
    }

    if (update_lcd && g_task_system_mode == SETUP)
    {
        if (p_dta->state == ST_SYS_MENU1) {
            snprintf(buf_row0, 17, "Select Motor:   ");
            snprintf(buf_row1, 17, "> Motor %d    ", (p_dta->current_motor_idx == M_MOTOR_1) ? 1 : 2);
        }
        else if (p_dta->state == ST_SYS_MENU2) {
            snprintf(buf_row0, 17, "Motor %d Select: ", (p_dta->current_motor_idx == M_MOTOR_1) ? 1 : 2);
            if (p_dta->current_param_idx == P_POWER) snprintf(buf_row1, 17, "> Power      ");
            else if (p_dta->current_param_idx == P_SPEED) snprintf(buf_row1, 17, "> Speed      ");
            else if (p_dta->current_param_idx == P_SPIN) snprintf(buf_row1, 17, "> Spin       ");
        }
        else if (p_dta->state == ST_SYS_MENU3) {
            snprintf(buf_row0, 17, "Edit Value:     ");
            if (p_dta->current_param_idx == P_POWER) {
                snprintf(buf_row1, 17, "> %s         ", motors[p_dta->current_motor_idx].power ? "ON " : "OFF");
            } else if (p_dta->current_param_idx == P_SPEED) {
                snprintf(buf_row1, 17, "> %d          ", motors[p_dta->current_motor_idx].speed);
            } else if (p_dta->current_param_idx == P_SPIN) {
                snprintf(buf_row1, 17, "> %s       ", (motors[p_dta->current_motor_idx].spin == S_LEFT) ? "LEFT " : "RIGHT");
            }
        }

        put_event_task_display(0, 0, buf_row0);
        put_event_task_display(0, 1, buf_row1);
    }
}

void task_system_set_mode(task_system_mode_t task_system_mode)
{
	g_task_system_mode = task_system_mode;
}

/********************** end of file ******************************************/
