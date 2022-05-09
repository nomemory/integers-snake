#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

// Macro reference:
// // https://gist.github.com/Jeff-Russ/c9b471158fa7427280e6707d9b11d7d2

#define s_napms 120

uint32_t map = 0x700;
uint32_t vars = 0x20090a;
uint64_t shape = 0x2a;
int8_t i = 0;

#define SU 0                       
#define SD 1                       
#define SL 2                       
#define SR 3
#define s_mod_p2(x,p2) (x&(p2-1))       
#define s_is_set(b) ((map&(1<<(b)))!=0)
#define s_tog(b) (map^=(1<<(b)))
#define s_set_0(b) (map&=~(1<<b))
#define s_set_1(b) (map|=(1<<b))
#define s_rnd s_mod_p2(rand(),32)
#define s_ls_bits(len) ((1<<(len))-1)
#define s_mask5(x) ((x)&0x1f)
#define s_mask(start,len) (s_ls_bits(len)<<(start))
#define s_prep(y,start,len) (((y)&s_ls_bits(len))<<(start))
#define s_get(y,start,len) (((y)>>(start))&s_ls_bits(len))
#define s_set(x,bf,start,len) (x=((x)&~s_mask(start,len))|s_prep(bf,start,len))
#define s_hpos s_get(vars,0,5)
#define s_tpos s_get(vars,5,5)
#define s_len s_get(vars,10,5)
#define s_apos s_get(vars,15,5)
#define s_chdir s_get(vars,20,2)
#define s_hpos_set(pos) s_set(vars,pos,0,5)
#define s_tpos_set(pos) s_set(vars,pos,5,5)
#define s_len_set(len) s_set(vars,len,10,5)
#define s_apos_set(app) s_set(vars,app,15,5)
#define s_chdir_set(cdir) s_set(vars,cdir,20,2)
#define s_len_inc s_len_set(s_len+1)
#define s_hdir ((shape>>(s_len*2)&3))
#define s_tdir (shape&3)
#define s_hdir_set(d) s_set(shape,d,s_len*2,2)
#define s_tdir_set(d) s_set(shape,d,0,2)
#define s_shape_rot(nd) do { shape>>=2; s_hdir_set(nd); } while(0);
#define s_shape_add(nd) do { s_len_inc; shape<<=2; s_tdir_set(nd); } while(0);
#define s_init do { srand(time(0)); initscr(); keypad(stdscr, TRUE); cbreak(); noecho(); } while(0);
#define s_exit(e) do { endwin(); exit(e); } while(0);
static void rnd_apple() {
    i = s_rnd;
    while(s_is_set(i)) i = s_rnd;
    s_apos_set(i);
}
static void show_map() {
    clear();
    i=32;
    while(i-->0) {
        if (i==s_apos) { addch('@'); addch(' '); }
        else { addch(s_is_set(i) ? '#':'.'); addch(' '); }
        if (!s_mod_p2(i,8)) { addch('\n'); }
    };
}
#define s_next_l s_mask5(s_hpos+1)
#define s_next_r s_mask5(s_hpos-1)
#define s_next_u s_mask5(s_hpos+8)
#define s_next_d s_mask5(s_hpos-8)
static void check_l() { if ((s_mod_p2(s_next_l,8) < s_mod_p2(s_hpos,8)) || s_is_set(s_next_l)) s_exit(-1); }
static void check_r() { if ((s_mod_p2(s_next_r,8) > s_mod_p2(s_hpos,8)) || s_is_set(s_next_r)) s_exit(-1); }
static void check_u() { if ((s_next_u < s_hpos) || s_is_set(s_next_u)) s_exit(-1); }
static void check_d() { if ((s_next_d > s_hpos) || s_is_set(s_next_d)) s_exit(-1); }
static void move_snake() {
    if (s_hdir==SL) { check_l(); s_hpos_set(s_hpos+1); } 
    else if (s_hdir==SR) { check_r(); s_hpos_set(s_hpos-1); } 
    else if (s_hdir==SU) { check_u(); s_hpos_set(s_hpos+8); }
    else if (s_hdir==SD) { check_d(); s_hpos_set(s_hpos-8); }
    s_set_1(s_hpos);
    if (s_apos==s_hpos) {
        rnd_apple();
        s_shape_add(s_tdir);
        return;
    }
    s_set_0(s_tpos);
    if (s_tdir==SL) { s_tpos_set(s_tpos+1); } 
    else if (s_tdir==SR) { s_tpos_set(s_tpos-1); } 
    else if (s_tdir==SU) { s_tpos_set(s_tpos+8); } 
    else if (s_tdir==SD) { s_tpos_set(s_tpos-8); }
}

#define s_key_press(k1, k2) if (s_hdir==k2) break; s_chdir_set(k1); break;
int main(void) {
    s_init;
    rnd_apple();
    while(1) {
        show_map();
        timeout(80);
        switch (getch()) {
            case KEY_UP : { s_key_press(SU, SD) };
            case KEY_DOWN : { s_key_press(SD, SU) };
            case KEY_LEFT : { s_key_press(SL, SR) };
            case KEY_RIGHT : { s_key_press(SR, SL) };
            case 'q' : exit(0); // Quits the game
        }
        move_snake();
        s_shape_rot(s_chdir);
        napms(200);
    }
    s_exit(0);
}