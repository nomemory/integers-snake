static void show_memory() {
    printw("\nmap=0x%04x\n", map);
    printw("\nvars=0x%02x\n", vars);
    printw("\ts_hpos=0x%01x (%d)\n", s_hpos, s_hpos);
    printw("\ts_tpos=0x%01x (%d)\n", s_tpos, s_tpos);
    printw("\ts_apos=0x%01x (%d)\n", s_apos, s_apos);
    printw("\ts_len=0x%01x (%d)\n", s_len, s_len);
    printw("\nshape=0x%08x\n", shape);
    i=32;
    while(i-->0) {
        printw("\t[%02d]=%x", i, (shape>>(i*2))&0x3);
        if (!s_mod_p2(i,8)) addch('\n');
    }
    printw("\ts_hdir=%2x\n", s_hdir);
    printw("\ts_tdir=%2x\n", s_tdir);    
}