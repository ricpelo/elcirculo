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
  if (~~(betamode))
  {  print "This will also automatically turn beta testing mode on. Continue? (Y or N) >";
     if (YesOrNo()==0) "^Beta testing mode remains off.";
     betamode = true;
  }
  transcript_mode = ((0-->8) & 1);
  if (transcript_mode) return L__M(##ScriptOn,1);
  @output_stream 2;
  if (((0-->8) & 1) == 0) return L__M(##ScriptOn,3);
  L__M(##ScriptOn,2); VersionSub();
  transcript_mode = true;
  "^Beta testing mode is now on. Comments may be entered by preceding
    the line with a !. They will not be parsed by the game.";
];
	
[ ScriptOffSub;
  if (betamode)
  {  print "This will also automatically turn beta testing mode off. Continue? (Y or N) >";
     if (YesOrNo()==0) "^Beta testing mode remains on.";
     betamode = false;
  }
  transcript_mode = ((0-->8) & 1);
  if (transcript_mode == false) return L__M(##ScriptOff,1);
  L__M(##ScriptOff,2);
  @output_stream -2;
  if ((0-->8) & 1) return L__M(##ScriptOff,3);
  transcript_mode = false;
  "^Beta testing mode is now off.";
];

[ QuitSub; L__M(##Quit,2);
  if (YesOrNo()~=0){ if (betamode){ betamode = false; <ScriptOff>; } quit; }
];

[ BetaTestOnSub;
  if (betamode) "Beta testing mode is already on.";
  betamode = true;
  print "Turning scripting on...^";
  <<ScriptOn>>;
];

[ BetaTestOffSub;
  if (~~(betamode)) "Beta testing mode is already off.";
  betamode = false;
  <<ScriptOff>>;
];

[ BetaCommentSub;
  if (betamode) rtrue;
  "Comments can only be used in beta testing mode.";
];

Verb meta 'beta'
         *              ->BetaTestOn
         * 'test'       ->BetaTestOn
         * 'test' 'off' ->BetaTestOff
         * 'off'        ->BetaTestOff
         * 'test' 'on'  ->BetaTestOn
         * 'on'         ->BetaTestOn;

Verb meta '!'
         * topic        ->BetaComment;


