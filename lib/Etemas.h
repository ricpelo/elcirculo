! INFSP Version
!
! ETEMAS   Otro enfoque para el parsing de Temas de conversacion
! Versi�n 2.0 biplataforma
!
!
!  Para uso con Inform/INFSP
!  (c) 1999-2000 Zak McKraken  (6/Mayo/1999, 17/Octubre/2000)
!
! USO:
!
!  1) Incluir este fichero en cualquier punto despu�s del "parser.h"
!  2) Definir, (despu�s de esta inclusi�n) tantos objetos "Temas-raiz"
!  como se quiera, por ejemplo:
!   Object TemasVoxPopuli;
!   Object TemasdeManolo;
!   Object Secretos;
!
!  3) Definir objetos dentro de los objetos anteriores, cada uno de
!  ellos con su propiedad 'name' que son las palabras que el jugador
!  puede usar para referirse a ese tema (no es necesario especificar
!  art�culos o 'de').
!
!  Cada Tema puede tener activado el atributo "parsing_estricto" que
!  indica que a la hora de "encajar" ese tema, *todas* las palabras
!  escritas por el jugador deben encajar con palabras de la propiedad
!  'name', y en el mismo orden. Tambi�n puede tener el atributo
!  "parsing_normal", seg�n el cual su nombre se "parsea" como el de
!  cualquier otro objeto. Si no tiene activado ninguno de estos
!  atributos, el encaje se har� de forma m�s flexible: Cuantas m�s
!  palabras de las escritas por el jugador encajen con el nombre, mejor;
!  pero las que no encajen se ignoran sin m�s.
!
!  Por ejemplo:
!    Object tiempo_atmosferico "el tiempo" TemasVoxPopuli
!    with name 'buen' 'tiempo' 'llover' 'sol' 'calor' 'frio';
!
!    Object euro "el euro" TemasVoxPopuli
!    with name 'euro';
!
!  4) La propiedad life de los PNJ, en el apartado de Answer,
!  Ask o Talk deben llamar a la funci�n AveriguarTema(),
!  pas�ndole como par�metro la colecci�n de temas en la que buscar. El
!  valor retornado es el tema de conversaci�n m�s probable a partir de
!  las palabras escritas por el jugador. Por ejemplo:
!
!     tema=AveriguarTema(TemasVoxPopuli);
!     switch(tema) {
!       tiempo_atmosferico: "Creo que va a llover";
!       euro: "Es la nueva moneda, creo.";
!       default: "No s� nada de eso";
!      }
!
!  As�, el juego admitir�:
!  > PREGUNTA A MANOLO SOBRE EL TIEMPO
!  > MANOLO, PARECE QUE HACE SOL
!  > PREGUNTA A MANOLO POR EL CAMBIO DEL EURO
!
! Estas preguntas s�lo llevar�n al tema "tiempo" y "euro" con el
! parsing flexible usado por defecto. Con el parsing normal, la
! primera ser�a comprendida (pues 'tiempo' est� en la lista de
! palabras de un tema), pero las otras dos no (porque 'parece' y
! 'cambio' no est�n en la lista de palabras de ningun
! tema). Finalmente, con parsing_estricto no ser�a comprendida ninguna
! de las frases, pues aunque 'tiempo' est� en la lista de palabras de
! un tema, no es su primera palabra. Con parsing estricto s�lo ser�a
! comprendido:
!
!  > PREGUNTA A MANOLO SOBRE EL BUEN TIEMPO 
!  > PREGUNTA A MANOLO POR EL EURO
!

#ifndef WORDSIZE;      ! Para compatibilidad con Inform 6.15
Constant WORDSIZE= 2;
Constant TARGET_ZCODE;
#endif;

Attribute parsing_estricto alias talkable;
Attribute parsing_normal alias absent;
! Uso alias de otros atributos existentes, para no definir nuevos ya
! que el n�mero de atributos m�ximo de la m�quina Z es limitado, y
! despu�s de todo un Tema no necesita los atributos talkable ni absent.


! Simplemente cuenta cu�ntas palabras de las del topic encajan en el
! obj dado (las que no encajan se ignoran sin m�s, a diferencia de
! InterpretarNombre, que deja de contar tan pronto como llega a una
! palabra no reconocida)
[ FlexibleInterpretarNombre obj i w n;
    if (parser_trace>=5) print "[ FlexibleInterpretarNombre: llamado
	para objeto ~",(name)obj, "~^  Tratando de encajar ",
	consult_words, " palabras, empezando en la ",wn,": ";
    for (i=0:i<consult_words:i++)
    {
    	w=NextWordStopped();
	if (WordInProperty(w, obj, name)) n++;
    }
    if (parser_trace>=5) print n, " palabras encajadas]^";
!    if (n<consultar_num_palabras) return 0;
    return n;
];

[ MoreStrictInterpretarNombre obj i w listanombres n_nombres;
    listanombres=obj.&name;
    n_nombres=(obj.#name)/WORDSIZE;
    if (n_nombres<consult_words) return 0;
    for (i=0:i<consult_words:i++)
    {
	w=NextWordStopped();
	if (w~=listanombres-->i) return i;
    }
    return i;
];

       
	

[ AveriguarTema T i n k max tmax;
    max=0;  ! La mejor puntuaci�n de momento
    tmax=0; ! El objeto correspondiente a esta puntuaci�n
    k=wn;
    objectloop (i in T) { !Buscar entre los temas dados
      	wn=consult_from;  !Empezando por la primera palabra
	if (i provides parse_name) ! Si el tema tiene su propio
	                           ! parse_name, lo llamamos
	    n=ValueOrRun(parse_name);
	else
	    if (i has parsing_estricto) ! Si se solicita parsing estricto
	    n=MoreStrictInterpretarNombre(i);
	else
	    if (i hasnt parsing_normal) ! Si no se solicta el parser
		                        ! normal, se hace flexible
		n=FlexibleInterpretarNombre(i);
	else n=ParseNoun(i);
	! De lo contrario, el InterpretarNombre normal
	if (n>max) 
	{
	    max=n;                 ! Actualizar m�ximos
	    tmax=i;
	}
    }
    if (parser_trace>=4) print "[ AveriguarTema ha detectado ~", (name)
	tmax, "~ (", tmax, ") como posible tema, con puntuaci�n ",
	    max, " ]^";
    wn=k;
    return(tmax);     ! Y retornar el mejor hallado
                      ! Observar que no hay intento de aclarar
                      ! ambig�edades si dos o m�s coincidieran en
                      ! puntuaci�n 
];
