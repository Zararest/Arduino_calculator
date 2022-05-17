#include <LiquidCrystal.h>

#define MAXLEN 20
#define ALPH_SIZE 20

#define TO_LEFT -1
#define TO_RIGHT 1
#define UP 1
#define DOWN -1

#define RS 8
#define ENBL 9
#define D1 4
#define D2 5
#define D3 6
#define D4 7

#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 10

#define ALPHABET "!0123456789+-*/^()"
#define LINE_SIZE 16

enum{

    initial_cond = 0,
    read = 1,
    ans = 2,
    num_of_cond = 3
};

class Display{

    LiquidCrystal lcd;
    int cursor_pos = 0;             //позиция по реальной линии
    int line_pos = 0;               //откуда начинается отображение линии
    int condition = initial_cond;

    int first_line[MAXLEN] = {0};    //линии будут заполнены \0
    char alphabet[ALPH_SIZE] = {ALPHABET};

    void move_cursor(int direction);
    void move_line(int direction);
    void change_symbol(int direction);
    void change_condition();

    void print_first_line();

    void clear_lines();
    void calc_answer();

    static int detect_button();
public:

    Display();
    void check_button();
    void setup_display();
    void wait_after_button();
};

Display::Display(): lcd(RS, ENBL, D1, D2, D3, D4){

    alphabet[0] = '\0';
}

void Display::move_cursor(int direction){

    int new_pos = cursor_pos + direction;
    if (new_pos < 0 || new_pos > MAXLEN) return;

    cursor_pos = new_pos;

    if (new_pos < line_pos){

        move_line(TO_LEFT);
    }

    if (new_pos >= line_pos + LINE_SIZE){

        move_line(TO_RIGHT);
    }

    lcd.setCursor(cursor_pos - line_pos, 0);
}

void Display::move_line(int direction){
    
    int new_pos = line_pos + direction;
    if (new_pos < 0 || new_pos > MAXLEN - LINE_SIZE) return;

    line_pos = new_pos;
    print_first_line();
}

void Display::print_first_line(){

    lcd.setCursor(0, 0);
    
    char buf[LINE_SIZE + 1];
    buf[LINE_SIZE] = '\0';

    for (int i = 0; i < LINE_SIZE; i++){

        int cur_symbol_num = first_line[line_pos + i] % ALPH_SIZE; //в массиве хранятся номера в алфавите
        buf[i] = alphabet[cur_symbol_num];
    }
    
    lcd.print(buf);
    lcd.setCursor(cursor_pos - line_pos, 0);
}

void Display::change_symbol(int direction){

    int cur_symbol = first_line[cursor_pos];

    if (cur_symbol + direction < 0){

        cur_symbol = ALPH_SIZE - 1;
    } else{

        cur_symbol = (cur_symbol + direction) % ALPH_SIZE;
    }

    first_line[cursor_pos] = cur_symbol;

    /*
    char buf[2];
    buf[1] = '\0';
    buf[0] = alphabet[cur_symbol];
    */

    lcd.print(alphabet[cur_symbol]);
    lcd.setCursor(cursor_pos - line_pos, 0);
}

void Display::change_condition(){
    
    condition = (condition + 1) % num_of_cond;

    switch (condition){
    
        case read:
            calc_answer();
            break;

        case ans:
            clear_lines();
            break;  
    }
}

void Display::calc_answer(){
    char buf[MAXLEN + 1];
    buf[MAXLEN] = '\0';

    for (int i = 0; i < MAXLEN; i++){

        int cur_symbol_num = first_line[line_pos + i] % ALPH_SIZE;
        buf[i] = alphabet[cur_symbol_num];
    }

    Calc calc(buf);
    double ans = calc.get_ans();
    
    lcd.setCursor(0, 1);                                        

    if (calc.check_error()){
        
        lcd.print(ans);
    } else{

        lcd.print("syntax error");
    }  

    lcd.setCursor(cursor_pos - line_pos, 0);
}

void Display::clear_lines(){

    lcd.setCursor(0, 0);  
    lcd.print("                ");
    lcd.setCursor(0, 1);  
    lcd.print("                ");
    lcd.setCursor(0, 0);

    for (int i = 0; i < MAXLEN; i++){

        first_line[i] = 0;
    }

    line_pos = 0;
    cursor_pos = 0;
}

void Display::wait_after_button(){

  delay(200);
}

void Display::check_button(){

  int button = detect_button();

  switch (button) {
    case BTN_UP:
      change_symbol(UP);
      wait_after_button();
      break;
    case BTN_DOWN:
      change_symbol(DOWN);
      wait_after_button();
      break;
    case BTN_LEFT:
      move_cursor(TO_LEFT);
      wait_after_button();
      break;
    case BTN_RIGHT:
      move_cursor(TO_RIGHT);
      wait_after_button();
      break;
    case BTN_SELECT:
      change_condition();
      wait_after_button();
      delay(1000);
      break;
  }
}

void Display::setup_display(){

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.blink();
}

int Display::detect_button(){

  int keyAnalog =  analogRead(A0);
  Serial.println (keyAnalog);
  if (keyAnalog < 60) {

    return BTN_RIGHT;
  } else if (keyAnalog < 200) {

    return BTN_UP;
  } else if (keyAnalog < 400) {

    return BTN_DOWN;
  } else if (keyAnalog < 600) {

    return BTN_LEFT;
  } else if (keyAnalog < 800) {

    return BTN_SELECT;
  } else {

    return BTN_NONE;
  }
}

Display displ;

void setup() {
  Serial.begin(9600);
  displ.setup_display();
}

void loop() {

  displ.check_button();
}
