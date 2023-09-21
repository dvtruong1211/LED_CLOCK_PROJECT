#ifndef _CAPTURE_H_
#define _CAPTURE_H_


typedef enum
{
	IC_STATE_NONE,
	IC_STATE_START,
	IC_STATE_HOLD,
	IC_STATE_DATA,
	IC_STATE_STOP
} ic_state_t;


void capture_tick();

#endif
