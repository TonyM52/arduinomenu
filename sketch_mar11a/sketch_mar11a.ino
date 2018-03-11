/* This is Nick's adaptation of my Menus1 sketch - he's demonstrating other 
 and better ways to approach the menu system, introducing techniques I hadn't seen.  
*/


// Call Libraries:
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h> // Arduino pin i/o class header

/*  Declare Structures:  (New! These are "Forward Declarations" - tell the compiler 
that these strucxtures (will) exist, although they will be detailed later. */

struct Menu;
struct MenuItem;


//Define LCD pinout:

const int LCD_COLS=16, LCD_ROWS=2;
const int rs=8, en=9, db4=4, db5=5, db6=6, db7=7;
hd44780_pinIO lcd(rs, en, db4, db5, db6, db7);


// Defining the content of the structures (I think!):
struct Menu {
  struct MenuItem **items; 
  int pos;
  int num_items;
};
// so, the Menu structure contains Menuitems
  // first "*" tells it that it's a pointer, 2nd tells it that its an 
  // array of pointers


struct MenuItem {
  char *text;
  boolean is_submenu;
  union {
    void (*func)();
    struct Menu *submenu;
  };
};
// so, the MenuItem structure contains text characters and submenu items
  // the "union" tells it that the MenuItem entry can be either a func
  // or a sub-menu pointer. 
// the boolean "is_submenu" tells the script whether it contains a func or a sub-menu.

/*  Foo and B Functions are the Top Level menu items, 
 while Bar and Baz are sub-menu items under B Functions.
 do_foo is the function that "does" the Foo action, similarly 
 deo_bar and do_baz "do" the bar and baz actions.  */
 
void do_foo() {
  lcd.setCursor(0,1);
  lcd.print("Called do_foo()");
  delay(1000);
}

void do_bar() {
  lcd.setCursor(0,1);
  lcd.print("Called do_bar()");
  delay(1000);
}

void do_baz() {
  lcd.setCursor(0,1);
  lcd.print("Called do_baz()");
  delay(1000);
}
//**************************************************************************
void initMenuItem(struct MenuItem* i, char* text, void (*func)()) {
  i->text = text;
  i->is_submenu=false;
  i->func = func;
}

void initSubMenu(struct MenuItem* i, char* text, struct Menu *submenu) {
  i->text = text;
  i->is_submenu=true;
  i->submenu = submenu;
}

void initMenu(struct Menu* m, struct MenuItem *items[], int n_items) {
  m->items = items;
  m->num_items = n_items;
  m->pos = 0;
}

struct MenuItem foo, bar, baz, b_functions;
struct Menu top_menu, b_submenu;
struct Menu* p_menu;

struct MenuItem* top_menu_items[] = { &foo, &b_functions };
struct MenuItem* b_submenu_items[] = { &bar, &baz };

enum { btn_none, btn_up, btn_down, btn_right, btn_left, btn_select };

int getButtonPress() {
  int i = analogRead(0); // Note: with no button pressed, the analogue read returns a value of 1023.
  
  if      (i == 0)  return btn_right;
  else if (i < 150) return btn_up;
  else if (i < 300) return btn_down;
  else if (i < 450) return btn_left;
  else if (i < 650) return btn_select;
  else return btn_none;
}

void menuHandler(struct Menu* p_menu) {
  int button = 0;
  do {
    Serial.print(p_menu->pos);
    Serial.print("/");
    Serial.println(p_menu->num_items);
    MenuItem* i = p_menu->items[p_menu->pos];
    
    lcd.clear();
    lcd.print(i->text);
    Serial.println(i->text);
    
    while(getButtonPress() != btn_none) { delay(100); };
    do {
      button = getButtonPress();
      delay(100);
    } while( button==btn_none );

    Serial.println(button);
    
    if(button == btn_up) {
      if(--(p_menu->pos) < 0) p_menu->pos = p_menu->num_items - 1;
    }
    
    if(button == btn_down) {
      if(++(p_menu->pos) >= p_menu->num_items) p_menu->pos = 0;
    }
    
    if(button == btn_select) {
      if(i->is_submenu) menuHandler(i->submenu);  // Call the menu handler and pass it the submenu structure
      else (i->func)();                           // Call the function pointed to by the menu item
    }
  } while(button != btn_left);
}

void setup() {
    Serial.begin(9600);
Serial.println ("Completed set-up loop");
 lcd.begin(LCD_COLS, LCD_ROWS);
  
  
  initMenuItem(&foo, "Foo", do_foo);
  initMenuItem(&bar, "Bar", do_bar);
  initMenuItem(&baz, "Baz", do_baz);
  initSubMenu(&b_functions, "B Functions", &b_submenu);
  
  initMenu(&top_menu, top_menu_items, sizeof(top_menu_items) / sizeof(MenuItem*));
  initMenu(&b_submenu, b_submenu_items, sizeof(b_submenu_items) / sizeof(MenuItem*));
}

void loop() {
  Serial.print("Begin top menu\n");
  menuHandler(&top_menu);
  Serial.print("Exited top menu\n");
}

