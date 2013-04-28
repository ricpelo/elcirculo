!==============================================================================
! diary.h by Gunther Schmidl <gschmidl@gmx.at>                 Release 1.000226
!==============================================================================
!
! Modificaciones (c) 2012 Ricardo Pérez (Sothoth) - Alpha Aventuras
!
! This class defines a diary item that can be filled in by the game author to
! provide background information or similar. It is derived from L. Ross
! Raszewski's manual.h, and requires his Utility.h and IString.h libraries.
!
!------------------------------------------------------------------------------
! CUSTOMIZATION
!------------------------------------------------------------------------------
!
! You can customize the strings this library will print by either overriding 
! the default values or setting the appropriate self.D_....__TX from within the 
! game. 
!
! D_AWAY__TX is used when the player types 'Q' to exit the diary
! D_UPDT__TX is printed when the diary is updated
! D_INFO__TX is printed when the diary is updated for the first time.
!
! You can use special formatting in D_UPDT__TX and D_INFO__TX: see the 
! FormatText routine for information. If you don't need this fancy stuff, 
! define NoFormat somewhere before inclusion of the library. 
!
! This library uses the colour patch by Adam Cadre that'll correctly restore
! colors upon UNDO and RESTORE; see http://www.adamcadre.ac/inform.html
! Basically, if you want to use colour, you'll have to define USES_COLOUR
! somewhere in the game and provide two Globals: 'fg' and 'bg'.
! If this isn't what you use, you'll have to change the 'Colour' routine
! below.
!
! The only things you'll have to change directly are the "Go to what page" box
! (since Inform can't handle 'box self.D_PAGE__TX') and the key handling, 
! should you change the default keys from P, N, Q and G.
!
!------------------------------------------------------------------------------
! HOW TO USE IT
!------------------------------------------------------------------------------
!
! Use "Diary objectname" to define the class. Whatever you give for a 
! description will be printed on the title page. The object's short name will
! be displayed on the first line of the screen. The game will automatically 
! wait for any key, then display the next page.
!
! Example:   
!
! Diary mydiary "Adventurer's Diary"
! with  
!  description
!  [;
!   @erase_window NULL;
!   box "The Adventurer's Diary"
!       "----------------------"
!       " THIS IS MINE SO KEEP "
!       "    YOUR HANDS OFF    ";
!  ];   
!
!------------------------------------------------------------------------------
!
! A diary page is a simple object. Its short name will be displayed on the 
! second line of the screen, and the description in the window itself. Be
! careful not to put too much text on one page.
! If you give the page the attribute 'on', it will be listed in the table of
! contents (see below)
!
! Example:
!
! Object p1 "24 Frobuary"
! with
!  description "^blah blah blah",
! has on;
!
!------------------------------------------------------------------------------
!
! If you want, you can use Class ContentsPage to provide a table of contents.
! The short name will be displayed on the second line of the screen, and you
! can provide the PageHeader property to display a short text before the 
! table begins. If you need several content pages, just define them; they will 
! be left empty if unneeded.
!
! Example:
!
! ContentsPage "Contents";
! 
!------------------------------------------------------------------------------
!
! DO NOT MOVE ANY PAGES INTO THE DIARY YOURSELF!    
!
! You must use diary.update(pagename, flag) for this reason, or the diary
! will not open on the correct page.
!  
! Use diary.update(pagename, 2) to insert content pages or if you want to 
! add a page without notifying the user.
!
! Use diary.update(pagename, 1) to insert the first of a number of pages (or
! one page only.) This will notify the player that the diary has been updated.
!
! Use diary.update(pagename) to insert the rest of the pages. The game will
! silently add them but not update the page counter, so the diary will open
! on the last page inserted with flag 1.       
!
!------------------------------------------------------------------------------
! WHAT ELSE YOU NEED
!------------------------------------------------------------------------------
!
! A verb 'diary' plus a subroutine 'DiarySub' (or whatever you want to call 
! them) that look similar to these:
!
! [ DiarySub;
!  mydiary.select(mydiary.last_page);
! ];
!
! Verb 'diary' * -> Diary;
!
!==============================================================================


Include "Utility";
Include "IString";


Class Diary
  with    
    D_PKEY__TX "<- = Página anterior",
    D_NKEY__TX "-> = Página siguiente",
    D_QKEY__TX " Q = Volver al juego",
    D_GKEY__TX " G = Ir a una página ",
    D_PAGE__TX "Página ",
    D_AWAY__TX "Cierras el diario.^",
    D_UPDT__TX "Tu diario se ha actualizado.",         
    D_INFO__TX "Teclea @<<%BDIARIO%P@>> para leer la última entrada.",
    update [ flag;   
      !Pmove(page, self);
!      give page on;
      self.notify(flag);      
    ],
    notify [ flag
      o;

!      if (flag == 1 or 2) {
        self.last_page = -1;
        objectloop (o in self && o has on) {
          self.last_page++;
          o.page = self.last_page;
        }
!      }

      if (flag == 0) {
        print "^[";
        FormatText(self.D_UPDT__TX);
        if (self.message == 0) {
          self.message = 1;    
          print " ";
          FormatText(self.D_INFO__TX); 
          print "]^";
        } else "]";
      } 
      
!      if (flag == 2) self.last_page = children(self);
    ],
    last_page 0,
    message 0,
    emblazon [
      i j;
!      if (pretty_flag == 0) return self.Lowkey_emblazon();
      glk_window_clear(gg_quotewin);
      if (hayGraficos) {
        if (gg_statuswin) glk_window_close(gg_statuswin, 0);
        gg_statuswin = glk_window_open(gg_mainwin,
                                       winmethod_Above + winmethod_Fixed,
                                       3, wintype_TextGrid, GG_STATUSWIN_ROCK);
        glk_request_hyperlink_event(gg_statuswin);
      } else {
        StatusLineHeight(3);
      }
      glk_window_get_size(gg_statuswin, gg_arguments, gg_arguments + WORDSIZE);
      i = gg_arguments-->0;
      if (gg_conversawin) glk_window_close(gg_conversawin, 0);
      gg_conversawin = glk_window_open(gg_mainwin,
                                       winmethod_Above +
                                       winmethod_Proportional +
                                       winmethod_NoBorder,
                                       100, wintype_TextBuffer,
                                       GG_CONVERSAWIN_ROCK);
      glk_request_hyperlink_event(gg_conversawin);
      glk_set_window(gg_statuswin);
      glk_set_style(style_SubHeader);
      MoveCursor(1, 1); spaces(i);
      MoveCursor(2, 1); spaces(i);
      MoveCursor(3, 1); spaces(i);
      MoveCursor(1, 2);
      print (string) self.D_PAGE__TX, self.pagen, "/", self.last_page;
      glk_set_hyperlink('0');
      CenterU(self.doname(), 1);
      glk_set_hyperlink(0);
      CenterU(self.dopagename(), 3);
      MoveCursor(2, 2);
      glk_set_hyperlink('P');
      print (string) self.D_PKEY__TX;
      glk_set_hyperlink(0);
      j = i - 20;
      MoveCursor(1, j);
      glk_set_hyperlink('Q');
      print (string) self.D_QKEY__TX;
      glk_set_hyperlink(0);
      MoveCursor(2, j);
      glk_set_hyperlink('N');
      print (string) self.D_NKEY__TX;
      glk_set_hyperlink(0);
!      MoveCursor(3, j);
!      glk_set_hyperlink('G');
!      print (string) self.D_GKEY__TX;
!      glk_set_hyperlink(0);
      glk_set_window(gg_conversawin);
    ],
!    Lowkey_emblazon [;
!      print "^^---"; self.doname(); print "---^";
!      print (string) self.D_PAGE__TX, self.pagen, " ";
!      self.dopagename(); print "^^";
!      print (string) self.D_PKEY__TX; spaces(4);
!      print (string) self.D_NKEY__TX; new_line;
!      print (string) self.D_QKEY__TX; spaces(4);
!      print (string) self.D_GKEY__TX;
!      new_line; new_line;
!    ],
    Diary_HandleGlkEvent [ ev context buffer;
      if (self has general) {     ! Si estamos mostrando el diario... 
        context = context;
        buffer = buffer;
        switch (ev-->0) {
          evtype_Redraw, evtype_Arrange:
            self.emblazon();
        }
      }
    ],
    select [ startpage;
      self.pagen = startpage;
      give self general;          ! Estamos mostrando el diario (ver HandleGlk)
      while (self.dopage() ~= 2)
        ;
      give self ~general;         ! Ya no lo estamos mostrando
!      if (pretty_flag ~= 0) glk_window_clear(gg_mainwin);
      glk_set_window(gg_mainwin);
      print (string) self.D_AWAY__TX;
      rtrue;
    ],
    page 0, 
    pagen 0,
    doname [; 
      !print (name) self;
      return self.short_name;
    ],
    dopagename [; 
      !print (name) self.page;
      return self.page;
    ],
    dopage [
      j o i;
      objectloop (o in self && o has on) {
        if (i == self.pagen) break;
        i++;
      }
      self.page = o;
      self.emblazon();
      if (self.page ~= nothing) self.page.description();
      j = self.keyloop();
      switch (j) {
        0: self.pagen--;
        1: self.pagen++;
        2: return 2;
        3: self.pagen = 0;
        default: self.pagen = j.page;
      }
      if (self.pagen < 0) self.pagen = 0;
      if (self.pagen > self.last_page) self.pagen = self.last_page;
      return 1;
    ],
    keyloop [
      keypress done;
      while (true) {
        glk_request_char_event(gg_mainwin);
        done = false;
        while (~~done) {
          glk_select(gg_event);
          switch (gg_event-->0) {
            evtype_CharInput:
              keypress = gg_event-->2;
              done = true;

            evtype_Hyperlink:
              glk_cancel_char_event(gg_mainwin);
              keypress = gg_event-->2;
              done = true;
          }
        }
        if (keypress ofclass Object) return keypress;
        switch (keypress) {
          '0':
            return 3;

          129, 'P', 'p', -2:
            return 0;

          32, 10, 13:
            if (self.pagen == 0) return 1;

!          'G', 'g':
!            if (keypress == 'G' or 'g') {
!              glk_window_clear(gg_conversawin);
!              if (pretty_flag) box "¿A qué página quieres ir?";
!              else             print "¿A qué página quieres ir? >";
!              KeyboardPrimitive(buffer, parse);
!              if (parse-->(WORDSIZE - 1) == 0) return 4;
!              k = TryNumber(1);
!              self.pagen = k;
!              return 3;
!            }

          130, 'N', 'n', -3:
            return 1;

          27, 'Q', 'q':
            return 2;
        }
      }
    ];


Class DiaryEntry
  with
    update [ flag
      d;
      d = parent(self); if (d == nothing) rfalse;
      if (self hasnt on) {
        give self on;
        d.update(flag);
      }
    ],
    page 0,
    description [
      o sp;
      objectloop (o in self && o has on) {
        if (sp) print " ";
        else sp = true;
        if (o hasnt general) glk_set_style(style_Emphasized);
        PrintOrRun(o, description, 1);
        if (o hasnt general) {
          glk_set_style(style_Normal);
          give o general;
        }
      }
    ];

    
Class ContentsPage
  class DiaryEntry
  with
    update [ flag
      d;
      d = parent(self); if (d == nothing) rfalse;
      if (self hasnt on) {
        give self on;
        d.update(flag);
      }
    ],
    description [
      o i;
      glk_set_style(style_Preformatted);
      new_line;
      self.PageHeader();
      objectloop (o in parent(self)) {
        i++;
        if (o has on && i < ValueOrRun(self, ucutoff) &&
            i > ValueOrRun(self, lcutoff)) {
          self.listItem = o;
          glk_set_hyperlink(o);
          self.ListEntry(i - 1);
          glk_set_hyperlink(0);
          new_line;
        }
      }
      glk_set_style(style_Normal);
    ],
    ucutoff [
      i;
      glk_window_get_size(gg_statuswin, gg_arguments, gg_arguments + WORDSIZE);
      i = gg_arguments-->0;
      return (self.page + 1) * (i - 5);
    ],
    lcutoff [
      i;
      glk_window_get_size(gg_statuswin, gg_arguments, gg_arguments + WORDSIZE);
      i = gg_arguments-->0;
      return (self.page) * (i - 5) + 1;
    ],
    page 0,
    PageHeader [; 
      rtrue;
    ],
    ListItem 0,
    PrintItem [;
      return self.ListItem.short_name; 
!      print (name) self.ListItem;
    ],
    ListEntry [ num
      i j;
      glk_window_get_size(gg_conversawin, gg_arguments,
                          gg_arguments + WORDSIZE);
      j = gg_arguments-->0;
      i = j - 13;
!      if (i <= 0) i = j - 3;
      spaces(5);
!      print (string) Self.PrintItem();
      LJustify(Self.PrintItem(), i, LEFT, '.');
      print num;
    ];

!----------------------------------------------------------- format text nicely

#ifndef NoFormat;

Array printed_text -> 104;

[ Colour f b;
  SetColour(f, b);
];             

[ FormatText format j k;
  j = PrintAnyToArray(printed_text + WORDSIZE, 100, format);
  for (k = WORDSIZE : k < j + WORDSIZE : k++) {
    if (printed_text->k == '%') {
      switch (printed_text->(++k)) {
        '%': print "%";
        ' ': spaces 1;
        'B': style bold;
        'P': style roman;
        'R': style reverse;
        'U': style underline;
        'n': font on;
        'N': font off;
        '1': Colour(1, 0);
        '2': Colour(2, 0);
        '3': Colour(3, 0);
        '4': Colour(4, 0);
        '5': Colour(5, 0);
        '6': Colour(6, 0);
        '7': Colour(7, 0);
        '8': Colour(8, 0);
        '9': Colour(9, 0);
        'a': Colour(0, 1);
        'b': Colour(0, 2);
        'c': Colour(0, 3);
        'd': Colour(0, 4);
        'e': Colour(0, 5);
        'f': Colour(0, 6);
        'g': Colour(0, 7);
        'h': Colour(0, 8);
        'i': Colour(0, 9);
      }
    } else print (char) printed_text->k;
  }
];   

#endif;

