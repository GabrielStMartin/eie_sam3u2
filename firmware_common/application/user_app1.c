/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */

static const LedNameType UserApp1_aRightLeds[U8_BINARY_COUNTER_LED_ARR_SZ] = {RED, ORANGE, YELLOW, GREEN};
static const LedNameType UserApp1_aLeftLeds[U8_BINARY_COUNTER_LED_ARR_SZ] = {WHITE, PURPLE, BLUE, CYAN};

static u16 UserApp1_u16BlinkCounter;
static u8 UserApp1_u8BinaryCounter;
static u8 UserApp1_u8BacklightColorIndex;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  UserApp1_u16BlinkCounter = 0;
  UserApp1_u8BinaryCounter = 0;
  UserApp1_u8BacklightColorIndex = 0;

  UserApp1SetAllLightsOff();

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }
} /* end UserApp1Initialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();
} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
static void UserApp1SetAllLightsOff(void)
{
  for(u8 i = 0; i < U8_TOTAL_LEDS; ++i)
  {
    LedOff(i);
  }
}

static void UserApp1SetBinaryCounterLeds(u8 aValue)
{
  for(u8 i = 0; i < U8_BINARY_COUNTER_LED_ARR_SZ; ++i)
  {
    if(aValue & (1 << i))
    {
      LedOn(UserApp1_aRightLeds[i]);
      LedOn(UserApp1_aLeftLeds[i]);
    }
    else
    {
      LedOff(UserApp1_aRightLeds[i]);
      LedOff(UserApp1_aLeftLeds[i]);
    }
  }
}

static void UserApp1SetLcdBakclight(u8 aValue)
{
  switch(aValue)
  {
    /* White */
    case 0:
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
      break;

    /* Purple */
    case 1:
      LedOn(LCD_RED);
      LedOff(LCD_GREEN);
      LedOn(LCD_BLUE);
      break;

    /* Blue */
    case 2:
      LedOff(LCD_RED);
      LedOff(LCD_GREEN);
      LedOn(LCD_BLUE);
      break;

    /* Cyan */
    case 3:
      LedOff(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
      break;

    /* Green */
    case 4:
      LedOff(LCD_RED);
      LedOn(LCD_GREEN);
      LedOff(LCD_BLUE);
      break;

    /* Yellow */
    case 5:
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOff(LCD_BLUE);
      break;

    /* Red */
    case 6:
      LedOn(LCD_RED);
      LedOff(LCD_GREEN);
      LedOff(LCD_BLUE);
      break;

    /* Off */
    default:
      LedOff(LCD_RED);
      LedOff(LCD_GREEN);
      LedOff(LCD_BLUE);
      break;
  }
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
  if(++UserApp1_u16BlinkCounter == U16_BLINK_PERIOD_MS)
  {
    UserApp1_u16BlinkCounter = 0;

    if(++UserApp1_u8BinaryCounter == U8_BINARY_COUNTER_MAX)
    {
      UserApp1_u8BinaryCounter = 0;
    }

    if(++UserApp1_u8BacklightColorIndex == U8_LCD_BACKLIGHT_COLOR_MAX)
    {
      UserApp1_u8BacklightColorIndex = 0;
    }
  }

  UserApp1SetBinaryCounterLeds(UserApp1_u8BinaryCounter);
  UserApp1SetLcdBakclight(UserApp1_u8BacklightColorIndex);
} /* end UserApp1SM_Idle() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)
{
} /* end UserApp1SM_Error() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
