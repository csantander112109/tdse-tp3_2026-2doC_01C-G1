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

#ifndef TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Events to excite Task System */
typedef enum task_system_ev {EV_SYS_IDLE,
							 EV_SYS_BTN_A,
							 EV_SYS_ENTER,
							 EV_SYS_NEXT,
							 EV_SYS_ESCAPE} task_system_ev_t;

/* State of Task System */
//typedef enum task_system_st {ST_SYS_IDLE,
//							 ST_SYS_ACTIVE} task_system_st_t;

// defs. botones
typedef enum task_system_st {ST_SYS_IDLE,
							 ST_SYS_ACTIVE,
							 ST_SYS_MAIN,
							 ST_SYS_MENU1,
							 ST_SYS_MENU2,
							 ST_SYS_MENU3} task_system_st_t;

// defs. lógica de los choices
typedef enum { M_MOTOR_1 = 0, M_MOTOR_2 = 1 } motor_id_t;
typedef enum { P_POWER = 0, P_SPEED = 1, P_SPIN = 2 } param_id_t;
typedef enum { S_LEFT = 0, S_RIGHT = 1 } spin_id_t;

// TDA motor
typedef struct {
    bool        power; // true=ON, false=OFF
    uint8_t     speed; // 0 a 9
    spin_id_t   spin;  // S_LEFT o S_RIGHT
} motor_config_t;

//typedef struct
//{
//	uint32_t			tick;
//	task_system_st_t	state;
//	task_system_ev_t	event;
//	bool				flag;
//} task_system_dta_t;

// task_system_dta_t modificado a partir de máquina de estados
typedef struct {
    uint32_t            tick;
    task_system_st_t    state;
    task_system_ev_t    event;
    bool                flag;

    motor_id_t          current_motor_idx; // motor actual
    param_id_t          current_param_idx; // 	power - speed - spin

    motor_config_t      motors[2];         // [0] = Motor 1, [1] = Motor 2

} task_system_dta_t;

/********************** external data declaration ****************************/
extern task_system_dta_t task_system_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
