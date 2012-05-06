!----------------------------------------------------------------------------
! Betatest.h   Version 1.02                                 A Betatesting Aid
! Marnie Parker  aka Doe    doeadeer3@aol.com                         2/29/00
!----------------------------------------------------------------------------
! A very simple aid, but useful. This grew out of discussion with Luc French
! when he suggested, as a beta tester, simpler ways to beta test. A big nod
! to Jonadab as he seems to have been the originator of this idea in a game
! of his that Luc had beta tested.
!
! This makes a game more easily beta testable (especially a large game).
!
! Entering "beta test on" turns beta testing mode on. It does not need to
! be turned off once on. It will be automatically turned off upon quitting.
!
! What it does:
! 1. Turns on scripting. So a beta tester's moves can be recorded to a file.
! 2. Also turns on the commenting function. Allows a beta tester to make
!    comments (such as when finding a bug) which appear on the screen and
!    will also be saved in the script file, but not be parsed by the game.
!    Comments must be preceded by a !. Example: ! comment. As with any
!    single letter Inform command, such as x lamp, the ! must be followed
!    by a space before the rest of the line/comment is entered.
!
! This means beta testers don't need to go through the script file later
! to make comments. They can make comments as they play/beta test. (They
! could anyway, but it is annoying when the game says, "I didn't understand
! that sentence.", or tries to do an inventory or something. :-) )
!
! To start a game with beta testing automatically on, just put
! <<BestTestOn>>; at the end of the Initialise routine.
!
! Include after parser.h.

! Comments:  I assume this would not be released in a finished game, only
! during beta testing. But who knows? Anyone with a little bit of Inform
! knowledge could have written this, but I figured I would share it anyway.

! Version 1.01 - Added a few comments.
! Version 1.02 - Added a few more comments.

!---------------------------------------------------------------------------
! Routines
!---------------------------------------------------------------------------

Replace ScriptOnSub;
Replace ScriptOffSub;
Replace QuitSub;

Global betamode = false;

[ ScriptOnSub;
  if (~~(betamode)) {
    print "Esto también activará automáticamente el modo betatesting.
           ¿Continuar? (S o N) >";
    if (YesOrNo() == 0) "^El modo betatesting continuará desactivado.";
    betamode = true;
  }
  #ifdef TARGET_ZCODE;
  transcript_mode = ((HDR_GAMEFLAGS-->0) & 1);
  if (transcript_mode) return L__M(##ScriptOn, 1);
  @output_stream 2;
  if (((HDR_GAMEFLAGS-->0) & 1) == 0) return L__M(##ScriptOn, 3);
  L__M(##ScriptOn, 2); VersionSub();
  transcript_mode = true;
  #ifnot; ! TARGET_GLULX
  if (gg_scriptstr ~= 0) return L__M(##ScriptOn, 1);
  if (gg_scriptfref == 0) {
    ! fileref_create_by_prompt
    gg_scriptfref = glk($0062, $102, $05, GG_SCRIPTFREF_ROCK);
    if (gg_scriptfref == 0) jump S1Failed;
  }
  ! stream_open_file
  gg_scriptstr = glk($0042, gg_scriptfref, $05, GG_SCRIPTSTR_ROCK);
  if (gg_scriptstr == 0) jump S1Failed;
  glk($002D, gg_mainwin, gg_scriptstr); ! window_set_echo_stream
  L__M(##ScriptOn, 2);
  VersionSub();
  return;
.S1Failed;
  L__M(##ScriptOn, 3);
  #endif;
  "^Se ha activado el modo betatesting. Podrá introducir comentarios comenzando
   la línea con un '!' y no serán interpretados por el juego.";
];
	
[ ScriptOffSub;
  if (betamode) {
    print "Esto también desactivará automáticamente el modo betatesting.
           ¿Continuar? (S o N) >";
    if (YesOrNo() == 0) "^El modo betatesting continuará activado.";
    betamode = false;
  }
  #ifdef TARGET_ZCODE;
  transcript_mode = ((HDR_GAMEFLAGS-->0) & 1);
  if (transcript_mode == false) return L__M(##ScriptOff, 1);
  L__M(##ScriptOff, 2);
  @output_stream -2;
  if ((HDR_GAMEFLAGS-->0) & 1) return L__M(##ScriptOff, 3);
  transcript_mode = false;
  #ifnot; ! TARGET_GLULX
  if (gg_scriptstr == 0) return L__M(##ScriptOff,1);
  L__M(##ScriptOff, 2);
  glk($0044, gg_scriptstr, 0); ! stream_close
  gg_scriptstr = 0;
  #endif;
  "^Se ha desactivado el modo betatesting.";
];

[ QuitSub;
  L__M(##Quit, 2);
  if (YesOrNo() ~= 0) {
    if (betamode) {
      betamode = false;
      <ScriptOff>;
    }
    quit;
  }
];

[ BetaTestOnSub;
  if (betamode) "El modo betatesting ya está activado.";
  betamode = true;
  print "Activando scripting...^";
  <<ScriptOn>>;
];

[ BetaTestOffSub;
  if (~~(betamode)) "El modo betatesting ya está desactivado.";
  betamode = false;
  <<ScriptOff>>;
];

[ BetaCommentSub;
  if (betamode) rtrue;
  "Sólo se pueden usar comentarios en modo betatesting.";
];

Verb meta 'beta'
  *              -> BetaTestOn
  * 'test'       -> BetaTestOn
  * 'test' 'off' -> BetaTestOff
  * 'off'        -> BetaTestOff
  * 'test' 'on'  -> BetaTestOn
  * 'on'         -> BetaTestOn;

Verb meta '!'
  * topic        -> BetaComment;

