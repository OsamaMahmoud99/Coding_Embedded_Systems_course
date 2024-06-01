#include "main.h"
#include "lcd.h"

static void protimer_event_dispatcher(protimer_t *const mobj , event_t *const e);
static uint8_t process_button_pad_value(uint8_t btn_pad_value);
static void display_init(void);

static protimer_t protimer;
void setup() {
  Serial.begin(115200);
  display_init();

  pinMode(PIN_BUTTON1 , INPUT);
  pinMode(PIN_BUTTON2 , INPUT);
  pinMode(PIN_BUTTON3 , INPUT);

  protimer_init(&protimer);
}

void loop() {
    uint8_t b1,b2,b3 , btn_pad_value;
    protimer_user_event_t ue;
    static uint32_t current_time = millis();
    static protimer_tick_event_t te;

    b1 = digitalRead(PIN_BUTTON1);
    b2 = digitalRead(PIN_BUTTON2);
    b3 = digitalRead(PIN_BUTTON3);

    btn_pad_value = (b1<<2)|(b2<<1)|b3;

    btn_pad_value = process_button_pad_value(btn_pad_value);

    if(btn_pad_value){
      if(btn_pad_value == BTN_PAD_VALUE_INC_TIME){
          ue.super.Sig = INC_TIME;
      }
      else if(btn_pad_value == BTN_PAD_VALUE_DEC_TIME){
        ue.super.Sig = DEC_TIME;
      }
      else if(btn_pad_value == BTN_PAD_VALUE_SP){
        ue.super.Sig = START_PAUSE;
      }
      else if(btn_pad_value == BTN_PAD_VALUE_ABRT){
        ue.super.Sig = ABRT;
      }

      protimer_event_dispatcher(&protimer , &ue.super);
    }
    
    // dispatch the time tick event for every 100ms
    if(millis() - current_time >= 100){
      //100ms has passed
      current_time = millis();
      te.super.Sig = TIME_TICK;
      ++te.ss;
      if(te.ss > 10) {
        te.ss = 1;
        protimer_event_dispatcher(&protimer,&te.super);
      }
    }
    
}

static void protimer_event_dispatcher(protimer_t *const mobj , event_t *const e)
{
  event_status_t status;
  protimer_state_t source , target;

  source = mobj->active_state;
  status = (*mobj->active_state)(mobj,e);
  if(status == EVENT_TRANSATION)
  {
    event_t ee;
    target = mobj->active_state;
    //1.run exit action for source state
    ee.Sig = EXIT;
    mobj->active_state = source;
    (*mobj->active_state)(mobj,&ee);
    //2.run entry action for target state 
    ee.Sig = ENTRY;
    mobj->active_state = target;
    (*mobj->active_state)(mobj,&ee);
  }
}

static uint8_t process_button_pad_value(uint8_t btn_pad_value)
{
  static button_state_t btn_sm_state = NOT_PRESSED;
  static uint32_t curr_time = millis();

  switch(btn_sm_state)
  {
    case NOT_PRESSED:{
      if(btn_pad_value){
        btn_sm_state = BOUNCE;
        curr_time = millis();
      }
      break;
    }
    case BOUNCE:{
      if(millis() - curr_time >= 50){

        if(btn_pad_value){
          btn_sm_state = PRESSED;
          return btn_pad_value;
        }
        else{
          btn_sm_state = NOT_PRESSED;
        }
      }
      break;
    }
    case PRESSED:{
      if(!btn_pad_value){
        btn_sm_state = BOUNCE;
        curr_time = millis();
      }
      break;
    }
  }

  return 0;
}

static void display_init(void)
{
  lcd_begin(16 , 2);
  lcd_clear();
  lcd_move_cursor_L_to_R();
  lcd_set_cursor(0,0);
  lcd_no_auto_scroll();
  lcd_cursor_off();
}