! Decir.h --- Decir ambiguo
! Versi�n 2.0 Biplataforma
!
! 6/Mayo/1999     primera versi�n
! 17/Octubre/2000 versi�n biplataforma 
! (c) Zak McKraken
!
! NOTA: La versi�n biplataforma es 100% id�ntica a la original.
! No se requieren modificaciones. No obstante, la publico como
! una "versi�n 2 biplataforma" para evitar dudas.
!
! Esta mini librer�a implementa la acci�n Decir (nueva acci�n) que se generar� cuando
! el player  ponga "DI LO QUE SEA", sin referirse a ninguna criatura
! en particular.
!
! Debe incluirse detr�s de "SpanishG.h"
!
! el comando "DI ALGO" sin especificar a qui�n, generar� la acci�n
! DECIR y dejar� preparadas las variables consult_from y
! consult_words (lo cual significa que es compatible con la
! librer�a ETemas). No ser� llamada la rutina before de ning�n objeto
! para esta acci�n, pero el programador puede capturarla en la rutina
! react_before.  
!
! si el programador No modifica su juego, (es decir, si No captura la
! acci�n Decir) simplemente por el hecho de incluir esta libreria 
! ya obtendr� el siguiente comportamiento:
!
! 1) si el player pone "DI HOLA" en una habitaci�n en la que s�lo hay
! a PNJ (o a objeto talkable), se generar� la acci�n Answer sobre
! ese objeto, lo mismo que si hubiera usado "DI HOLA A MANOLO".
!
! 2) si hay m�s de a PNJ u objeto talkable, aparece el mensaje
! "Hablas, pero nadie te hace caso. Quiz�s debas dirigirte a alguien
! en particular".
!
! 3) si No hay nadie con qui�n hablar, aparece el mensaje "Aqui no hay
! nadie que pueda escucharte".
!
! Por otro lado, el programador puede capturar la acci�n Decir en
! la rutina before de cualquier PNJ u objeto talkable, para hacer que
! "escuche" cuando el player habla cerca. (tambi�n puede escuchar lo
! que habla con otro PNJ si captura las acciones Answer, Ask
! y Tell)
!
! Ejemplo (que usar�a la librer�a ETemas):
!
! Object Manolo "Manolo"
! with name 'manolo'
!      react_before [ tema;
!         Decir: tema=AveriguarTema(Temas);
!             switch(tema){
!               hola: "Manolo apenas levanta la vista ante tu
!                      saludo.";
!               tema_manolo: "Manolo te mira perplejo por un
!                      momento.";
!               adios: "Manolo te mira para comprobar que te vas.";
!               }
!         ],
!  has animate;
!
! Reaccionando antes a la acci�n Decir se capturan las frases
! "lanzadas al aire" por el player. Recordar que sin embargo, las
! frases dirigidas a alquien en concreto producen la acci�n Answer.



Global HayAlguien;    ! Contador de cu�ntos talkables hay en las cercan�as
Global QuienHay;      ! Uno de ellos (el �ltimo encontrado)


! La siguiente rutina recibe un objeto y debe contarlo como talkable
! si es un personaje o un objeto talkable distinto del jugador.
[ EresAlguien obj;
    if (obj==player) return;
    if (obj has animate || obj has talkable) {
    QuienHay=obj;
    HayAlguien++;
    }
];


! Rutina por defecto para la acci�n Decir
! Se ejecuta si el programador no ha capturado esta acci�n en
! react_before (o si ha retornado false tras capturarla).
!
! Averigua cu�ntos PNJ hay en la estancia y act�a de forma acorde.
!
[ DecirSub ;
    HayAlguien=0;
    LoopOverScope(EresAlguien);
    if (HayAlguien==1)
    {
    print "[", (al) QuienHay, "]^";
    <<Answer 0 QuienHay>>;
    }
    else if (HayAlguien>1)    
        "Hablas pero nadie te hace caso. Quiz�s debas dirigirte a alguien
    en particular.";
    else "Aqui no hay nadie que pueda escucharte.";
];


! Esta rutina es usada por el parser para determinar d�nde termina la
! frase que el jugador pone tras DI. La frase termina, bien con la
! �ltima palabra, o bien con la �ltima aparici�n de la preposici�n
! 'a', para distinguir DI HOLA de DI HOLA A MANOLO (esto implica
! tambi�n que la frase usada por el jugador no puede usar esta
! preposici�n, como p. ej: DI VETE A LA PORRA). Para este caso debe
! especificar con qui�n habla: DI VETE A LA PORRA A MANOLO
[ frase w ultima;
    consult_from = wn;
    ultima=0;
    do {
    w=NextWordStopped();
        if (parser_trace > 6) {
           print "[frase: ";
           if (w~=0 or -1) print (address) w;
           else print "NULL";
           print "]^";
        } 
    if (w=='a//') ultima=wn;
    } until (w==-1);
    if (parser_trace > 6) print "[frase: ultima palabra de la frase=", ultima -1,"]^";
    if (ultima==0)
        wn--;
    else 
    wn=ultima-1;
    consult_words=wn-consult_from;
    if (parser_trace > 6) 
       print "[frase: contiene ", consult_words," palabras]^";
    if (consult_words==0) return -1;
    return 1;
];


! Nueva gram�tica del verbo "DI" (y sus sin�nimos "GRITA" y "RESPONDE"
!
Extend 'di' replace
    * frase                -> Decir
    * 'a//' creature topic        -> Answer reverse
    * frase 'a//' creature        -> Answer;

