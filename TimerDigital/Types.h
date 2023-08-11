typedef enum 
{
  STARTUP,
  LOOP,
  CONFIG
} TimerState_t;

typedef enum 
{
  CURRENT_HOUR,
  CURRENT_MINUTE,
  ON_HOUR,
  ON_MINUTE,
  OFF_HOUR,
  OFF_MINUTE,
} ConfigState_t;