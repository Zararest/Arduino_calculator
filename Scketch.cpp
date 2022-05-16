#include "./Calculator.h"
#include <stdio.h>

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

#define ALPHABET "!0123456789+-*/^()"
#define LINE_SIZE 16

struct LiquidCrystal{

    LiquidCrystal(int a, int b, int c, int d, int e, int f){}

    void setCursor(int col, int row){

        printf("cursor: %i, %i\n", col, row);
    }

    void print(const char* buf){

        printf("print: %s\n", buf);
    }
};

enum{

    init = 0,
    read = 1,
    ans = 2,
    num_of_cond = 3
};

class Display{

    LiquidCrystal lcd;
    int cursor_pos = 0;             //позиция по реальной линии
    int line_pos = 0;               //откуда начинается отображение линии
    int condition = init;

    int first_line[MAXLEN] = {0};    //линии будут заполнены \0
    char alphabet[ALPH_SIZE] = {ALPHABET};

    void move_cursor(int direction);
    void move_line(int direction);
    void change_symbol(int direction);
    void change_condition();

    void print_first_line();

    void clear_lines();
    void calc_answer();
public:

    Display();
    void check_button(){

        change_symbol(UP);
        change_symbol(UP);
        move_cursor(TO_RIGHT);

        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        change_symbol(UP);
        move_cursor(TO_RIGHT);

        change_symbol(UP);
        change_symbol(UP);
        move_cursor(TO_RIGHT);

        change_condition();
    }
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

void Display::move_line(int direction){ //не сдвигает обратно курсор
    
    int new_pos = line_pos + direction;
    if (new_pos < 0 || new_pos > MAXLEN - LINE_SIZE) return;

    line_pos = new_pos;
    print_first_line();
    //lcd.setCursor(cursor_pos - line_pos, 0);                        //error лишняя строчка
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

    char buf[2];
    buf[1] = '\0';
    buf[0] = alphabet[cur_symbol];

    lcd.print(buf);
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

void Display::calc_answer(){  //разобраться с синтаксическими ошибками

    char buf[MAXLEN + 1];
    buf[MAXLEN] = '\0';

    for (int i = 0; i < MAXLEN; i++){

        int cur_symbol_num = first_line[line_pos + i] % ALPH_SIZE;
        buf[i] = alphabet[cur_symbol_num];
    }

    Calc calc(buf);
    printf("%lf\n", calc.get_ans());                 
    //sprintf(buf, "%lf", calc.get_ans());          //error

    lcd.setCursor(0, 1);                                        

    if (calc.check_error()){

        lcd.print(buf);
    } else{

        lcd.print("syntax error");
    }  

    lcd.setCursor(cursor_pos - line_pos, 0);    //error
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


int main(){

    Display displ;

    displ.check_button();
}