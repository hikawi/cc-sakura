// engine/evt.h
//
// Represents Event Scripting module. This is for easier scripting on scenes, to
// avoid having to manage threads themselves to wait and dispatch.

typedef enum
{
    EVT_API_STATUS_BLOCK,
    EVT_API_STATUS_CONTINUE,
    EVT_API_STATUS_FINISH,
} EvtApiStatus;
