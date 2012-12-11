!--------------------------------------------------------------------------
!
! SONIDOS: Una librería de manejo 'realista' de las fuentes de sonido
!
! Para usar con InformaTE! de Zak.
!
! Esta librería intenta 'ocultar' todo el tratamiento realista
! de fuentes de sonido, permitiendo al jugador 'escuchar' sonidos
! lejanos, amortiguados o incluso deformados por los obstáculos
! que se encuentren en su camino.
!
! Desarrollado por Mel Hython en el Febrero del 2000
!
!---------------------------------------------------------------------------

!System_file;

Message "Estás incluyendo el módulo de Sonidos de Mel Hython
    ¡Atente a las consecuencias!";

#ifndef WORDSIZE;
Constant WORDSIZE = 2;
#endif;

#ifndef Lugar;
Class Lugar with number;
#endif;

#ifndef en_ruta;
Attribute en_ruta;
#endif;

#Stub RutinaPreLocal 0;

Default SONIDOS_MAX_NIVEL_ANIDAMIENTO = 10;
Default SONIDOS_MAX_NUM_OYENTES       = 100;
Default SONIDOS_MAX_NUM_LUGARES       = 100;
Default SONIDOS_MAX_NUM_RUIDOS        = 100;

!
! VARIABLES GLOBALES
!  

! Reducciones de intensidad sonora:
Global costePorObstaculo         = 4;  ! Por pared o puerta cerrada interpuesta
Global costePorHabitacion        = 2;  ! Por puerta abierta
Global costePorContenedor        = 1;  ! Por contenedor abierto
Global costePorContenedorCerrado = 2;  ! Por contenedor cerrado

Global nivelAudible  = 2;
Global nivelSusurro  = 4;
Global nivelMurmullo = 6;
Global nivelNormal   = 8;
Global nivelAlto     = 10;
Global nivelMuyAlto  = 12;

Array tablaOyentes table SONIDOS_MAX_NUM_OYENTES;
Global indiceOyentes = 0;
Global oyentesApuntados = false;

Array tablaRuidos table SONIDOS_MAX_NUM_RUIDOS;
Global indiceRuidos = 0;
Global ruidosApuntados = false;

Array tablaLugares table SONIDOS_MAX_NUM_LUGARES;
Global indiceLugares = 0;
Global lugaresApuntados = false;

! Función que apunta todos los lugares:
[ RegistraLugares
  i;

  if (lugaresApuntados) return;

  objectloop (i ofclass Lugar) {
    if (indiceLugares < SONIDOS_MAX_NUM_LUGARES) {
      indiceLugares++;
      tablaLugares-->indiceLugares = i;
    } else {
      print "ERROR: superado el límite de lugares; ", (name) i, 
            " no podrá ser usada como lugar de sonidos.^";
    }
  }

  lugaresApuntados = true;
];

! Funcion de registro general
[ RegistraTodo;
  if (~~lugaresApuntados) RegistraLugares();
  if (~~ruidosApuntados)  RegistraRuidos();
  if (~~oyentesApuntados) RegistraOyentes();
];

!
! PROPIEDADES Y ATRIBUTOS NUEVOS
!
Property cercanoA 0;

!
! CLASES NUCLEO
!  

! Clase para pasar contenidos al método 'escuchado':
Class ContenidoSonoro
  with
    clase 0,           ! --> TipoDeSonido usado para emitirlo
    origen 0,          ! --> fuente del sonido
    modo 0,            ! --> modo de emisión del sonido
    intensidad 0,      ! --> potencia percibida
    sePuedeVer false,  ! --> el jugador ve la fuente
    seEscuchaEn 0,     ! --> de donde parece provenir
    texto "";          ! --> lo que dice el sonido si es inteligible  

! Fifo de llamadas a oyentes. Como mucho 10 anidamientos
Global indiceAnidamiento = 0;

ContenidoSonoro cO1;
ContenidoSonoro cO2;
ContenidoSonoro cO3;
ContenidoSonoro cO4;
ContenidoSonoro cO5;
ContenidoSonoro cO6;
ContenidoSonoro cO7;
ContenidoSonoro cO8;
ContenidoSonoro cO9;
ContenidoSonoro cO10;

Array contOyente table cO1 cO2 cO3 cO4 cO5 cO6 cO7 cO8 cO9 cO10;

! Usado como base
ContenidoSonoro contGeneral
  with
    clase tipoPlano;

[ ClonaContenido _origen;
  ! print "[",indiceAnidamiento+1,"] ", (name)_origen.clase, " desde ", (name)_origen.origen, "^";
  if (indiceAnidamiento < SONIDOS_MAX_NIVEL_ANIDAMIENTO) {
    indiceAnidamiento++;
    (contOyente-->indiceAnidamiento).clase       = _origen.clase;
    (contOyente-->indiceAnidamiento).origen      = _origen.origen;
    (contOyente-->indiceAnidamiento).modo        = _origen.modo;
    (contOyente-->indiceAnidamiento).intensidad  = _origen.intensidad;
    (contOyente-->indiceAnidamiento).sePuedeVer  = _origen.sePuedeVer;
    (contOyente-->indiceAnidamiento).seEscuchaEn = _origen.seEscuchaEn;
    (contOyente-->indiceAnidamiento).texto       = _origen.texto;
  }

  return (contOyente-->indiceAnidamiento);
];

[ DestruyeContenido;
  if (indiceAnidamiento > 0) indiceAnidamiento--;
  ! print "[", indiceAnidamiento, "]^";
];

!
! Clase general para todos los sonido
!
Class TipoDeSonido
  with 
    !
    ! La descripcion mínima del sonido irá en 'short_name'
    ! o en las comillas de definición del objeto
    !
    reconocible  false,  ! El jugador reconoce el origen incluso a distancia
    verboOir "Oyes",     ! Descripcion del medio de recepcion
    verboDecir "dice",   ! Verbo base usado para decir cosas
    verboEmitir "emite", ! Verbo base usado para emitir sonidos
    adjetivoDeVoz 0,     ! Adjetivo referente solo a voz
    usarDistancia false, ! Por defecto no se habla de la distancia
    usarPotencia true,   ! por defecto se describe la potencia
    usarDireccion true,  ! por defecto se dice de donde proviene si se sabe
    !
    ! Textos para dar flexibilidad
    !
    adjetivoLejana "lejana",         ! Distancia lejana-femenina
    adjetivoMuyLejana "muy lejana",  ! Distancia muy lejana-femenina
    adjetivoLejano "lejano",         ! Distancia lejano-masculino
    adjetivoMuyLejano "muy lejano",  ! Distancia muy lejano-masculino
    noVozSusurro "que apenas se oye",
    noVozMurmullo "que suena muy bajito",
    noVozNormal 0,
    noVozAlto "que se oye muy claramente",
    noVozMuyAlto "de tal intensidad que casi te deja sordo",
    vozSusurro "pero apenas se entiende",
    vozMurmullo "pero suena como un susurro",
    vozNormal 0,
    vozAlto "con un torrente de voz",
    vozMuyAlto "con tal fuerza que casi te deja sordo",
    !
    ! Rutinas
    !
    ! Rutina que devuelve un texto indicando nada, 'lejana' o 'muy lejana' según
    ! si la fuente se ve y su intensidad sonora
    diDistancia [ intensidad sePuedeVer femenino;
      if (sePuedeVer) return 0;

      if (femenino) {
        if (intensidad <= nivelSusurro)       return self.adjetivoMuyLejana;
        else if (intensidad <= nivelMurmullo) return self.adjetivoLejana;
        else                                  return 0;
      } else {
        if (intensidad <= nivelSusurro)       return self.adjetivoMuyLejano;
        else if (intensidad <= nivelMurmullo) return self.adjetivoLejano;
        else                                  return 0;
      }
    ],
    ! Rutina que devuelve una descripcion de potencia auditiva escuchada
    diPotencia [ intensidad modo esVoz;
      ! intensidad --> potencia auditiva
      ! modo       --> forma en la que fue emitido
      ! esVoz      --> contiene true si el contenido tenía texto
      modo = modo;
      if (~~esVoz) {
        if (intensidad <= nivelSusurro)       return self.noVozSusurro;
        else if (intensidad <= nivelMurmullo) return self.noVozMurmullo;
        else if (intensidad <= nivelNormal)   return self.noVozNormal;
        else if (intensidad <= nivelAlto)     return self.noVozAlto;
        else                                  return self.noVozMuyAlto;
      } else {
        if (intensidad <= nivelSusurro)       return self.VozSusurro;
        else if (intensidad <= nivelMurmullo) return self.VozMurmullo;
        else if (intensidad <= nivelNormal)   return self.VozNormal;
        else if (intensidad <= nivelAlto)     return self.VozAlto;
        else                                  return self.VozMuyAlto;
      }
    ],
    ! Rutina que imprime el mensaje final de jugador. Esta es la rutina que
    ! puede redefinir el programador para crear sus propios sonidos.
    escuchado [ contenido
      textoPotencia textoDistancia _esVoz;
      ! El objeto contenido tiene los atributos:
      !
      ! origen       --> fuente del sonido
      ! modo         --> forma en la que fue emitido el sonido 
      ! intensidad   --> potencia percibida del sonido de 0 a 10
      ! sePuedeVer   --> true si el jugador lo ve directamente
      ! seEscuchaEn  --> origen aparente (contenedor, dirección...)
      ! texto        --> lo que dice la voz

      ! No se ha pasado un texto es un sonido ininteligible, una voz 
      if (contenido.texto == 0) {
        _esVoz = false;      ! En principio no es voz

        if (VR(self.usarDistancia)) {
          textoDistancia = self.diDistancia(contenido.intensidad,
                                            contenido.sePuedeVer,
                                            (self has female));
        } else {
          textoDistancia = 0;
        }

        ! En esta clase base se ignora el modo
        contenido.modo = contenido.modo;

        ! En esta clase base si el umbral de audicion está en 3
        if (contenido.intensidad > nivelAudible) {
          ! Es escuchado por el jugador
          if ((contenido.sePuedeVer) && (VR(self.usarDireccion))) {
            ! Se ve, indicar origen
            if (VR(self.verboEmitir) == 0) {
              print (The) contenido.origen, " ", (string) VR(self.verboDecir);
              ! Justo para este caso, al usar el verbo de decir, se cambia a la
              ! coletilla que se usa con texto
              _esVoz = true;
            } else {
              print (The) contenido.origen, " ", (string) VR(self.verboEmitir),
                    " ", (name) self;
            }
          } else {
            ! No se ve directamente
            if ((VR(self.usarDireccion)) && (contenido.seEscuchaEn ~= 0)) {
              ! pero se sabe de donde
              print (string) VR(self.verboOir), " ", (name) self;
              if (textoDistancia ~= 0) print " ", (string) VR(textoDistancia);
              print " proveniente ", (del) contenido.seEscuchaEn;
            } else {
              ! y no se sabe de donde
              print (string) VR(self.verboOir), " ", (name) self;
              if (textoDistancia ~= 0) print " ", (string) VR(textoDistancia);
              print " proveniente de no se sabe dónde";
            }
          }

          ! Marcar intensidad vista
          if (VR(self.usarPotencia)) {
            textoPotencia = self.diPotencia(contenido.intensidad,
                                            contenido.modo, _esVoz);
            if (textoPotencia ~= 0) print ", ", (string)VR(textoPotencia), ".^";
            else                    print ".^";
          } else {
            print ".^";
          }
        }
      } else {
        if (self.usarDistancia) {
          textoDistancia = self.diDistancia(contenido.intensidad,
                                            contenido.sePuedeVer, _esVoz);
        } else {
          textoDistancia = 0;
        }

        ! Tiene texto es una voz
        if (contenido.intensidad > nivelAudible) {
          ! Es escuchado por el jugador
          if ((contenido.sePuedeVer) && (VR(self.usarDireccion))) {
            ! Se ve, indicar origen
            if (contenido.intensidad > nivelSusurro) {
              ! Se entiende lo que dice
              print (The) contenido.origen, " ", (string) VR(self.verboDecir);

              if (self.adjetivoDeVoz ~= 0) {
                print " con voz ", (string) VR(self.adjetivoDeVoz);
              }
              if (textoDistancia ~= 0) print " ", (string) VR(textoDistancia);
              
              print ": ~", (string) contenido.texto,"~";
            } else {
              ! No se entiende lo que dice
              print (The) contenido.origen, " ", (string) VR(self.verboDecir),
                    " algo";

              if (self.adjetivoDeVoz ~= 0) {
                print " con voz ", (string) VR(self.adjetivoDeVoz);
              }
              if (textoDistancia ~= 0) print " ", (string) VR(textoDistancia);
            }
          } else {
            ! No se ve directamente
            if (VR(self.reconocible)) {
              if ((VR(self.usarDireccion)) && (contenido.seEscuchaEn ~= 0)) {
                ! Se sabe de donde viene
                print (string) VR(self.verboOir), " la voz ";

                if (self.adjetivoDeVoz ~= 0) {
                  print (string) VR(self.adjetivoDeVoz), " ";
                }
                if (textoDistancia ~= 0) {
                  print (string) VR(textoDistancia), " ";
                }

                print (del) contenido.origen, "proveniente ",
                      (del) contenido.seEscuchaEn, " que ",
                      (string) VR(self.verboDecir);
              } else {
                ! No se sabe de donde viene
                print (string) VR(self.verboOir), " la voz ";

                if (self.adjetivoDeVoz ~= 0) {
                  print (string) VR(self.adjetivoDeVoz), " ";
                }
                if (textoDistancia ~= 0) {
                  print (string) VR(textoDistancia), " ";
                }

                print (del) contenido.origen,
                      "proveniente de no se sabe dónde que ",
                      (string) VR(self.verboDecir);
              }
            } else {
              if ((VR(self.usarDireccion)) && (contenido.seEscuchaEn ~= 0)) {
                ! Se sabe de donde viene
                print (string) VR(self.verboOir), " una voz ";

                if (self.adjetivoDeVoz ~= 0) {
                  print (string) VR(self.adjetivoDeVoz), " ";
                }
                if (textoDistancia ~= 0) {
                  print (string) VR(textoDistancia), " ";
                }

                print "proveniente ", (del) contenido.seEscuchaEn, " que ",
                      (string) VR(self.verboDecir);
              } else {
                ! No se sabe de donde viene
                print (string) self.verboOir, " una voz ";

                if (self.adjetivoDeVoz ~= 0) {
                  print (string) VR(self.adjetivoDeVoz), " ";
                }
                if (textoDistancia ~= 0) {
                  print (string) VR(textoDistancia), " ";
                }

                print "proveniente de no se sabe dónde que ",
                      (string) VR(self.verboDecir);
              }
            }

            if (contenido.intensidad > nivelSusurro) {
              print ": ~", (string) contenido.texto, "~";
            } else {
              print " algo";
            }
          }

          ! Marcar intensidad vista
          if (VR(self.usarPotencia)) {
            textoPotencia = self.diPotencia(contenido.intensidad, 
                                            contenido.modo, true);
            if (textoPotencia ~= 0) print ", ", (string)VR(textoPotencia), ".^";
            else                    print ".^";
          } else {
            print".^";
          }
        }
      }
    ];

!
! Clase de objetos que hablan usando estos sonidos
!
Class ObjetoHablante
  with
    ! Simplemente contiene un método para llamar a la función de provocar 
    ! un sonido de clase Voz usando su propia voz (que sera de tipo plano si
    ! no se cambia):
    voz tipoPlano,
    habla [ _texto _intensidad _modo;
      ! print "~",(name)self, " usando ", (name)self.voz, "~^";
      ! Rellenamos el contenido de forma apropiada
      contGeneral.origen      = self;     ! El que sea
      contGeneral.modo        = _modo;    ! Nada el que sea
      contGeneral.sePuedeVer  = true;     ! Fuera de la vista
      contGeneral.seEscuchaEn = 0;        ! Da igual
      contGeneral.texto       = _texto;   ! Texto pedido
      contGeneral.clase       = VR(self.voz);
      ! Intensidad proporcionada o intensidad media
      if (_intensidad ~= 0) contGeneral.intensidad = _intensidad;
      else                  contGeneral.intensidad = nivelNormal;
      ! 'Tocar' el sonido
      TocaSonido(contGeneral);
    ];

!
! Clase de objetos que 'escuchan' los sonidos que se producen
!
Class ObjetoOyente
  with
    ! Simplemente reciben en el método 'oido' dos parámetros correspondientes al
    ! tipo de voz usada y al contenido acústico
    intPend 0,
    seEscPend 0,  
    sePuePend 0,
    apunta [ _sonido;
      if (self hasnt en_ruta) {
        self.intPend   = _sonido.intensidad;
        self.seEscPend = _sonido.seEscuchaEn;
        self.sePuePend = _sonido.sePuedeVer;
        give self en_ruta;
      }
    ],
    limpia [;
      give self ~en_ruta;
      self.intPend = 0;
    ],
    oido [ _sonido;
      ! El objeto base hace una tontería; tiene que ser cada objeto el que 
      ! haga algo particular para reaccionar ante los sonidos que oiga
      if ((TestScope(self)) && (_sonido.intensidad > nivelAudible) &&
          (_sonido.origen ~= self)) {
        print (The) self, " parece haber oído algo.^";
        ! print (name)_sonido.seEscuchaEn, _sonido.seEscuchaEn;
      }
    ];

! Función necesaria para registrar oyentes
[ RegistraOyente _oyente;
  if (_oyente ofclass ObjetoOyente) {
    if (indiceOyentes < SONIDOS_MAX_NUM_OYENTES) {
      indiceOyentes++;
      tablaOyentes-->indiceOyentes = _oyente;
      return true;
    }
!    return false;
  }
  return false;
];

[ RegistraOyentes
  i;
  if (oyentesApuntados) return;
  objectloop (i ofclass ObjetoOyente) RegistraOyente(i);
  oyentesApuntados = true;
];

!
! Objeto que produce sonido ambiente
!
Class Ruido
  with
    voz tipoPlano,    ! Clase de Voz a usar
    frase 0,          ! Texto que se dice permanentemente
    intensidad 0,     ! Nivel con el que se habla
    origen 0,         ! Objeto que lo produce
    sonando true,     ! De base esta sonando
    found_in [;
      ! Se oyen sonidos si estan presentes y no estan ocultos
      if (self hasnt concealed) {
        return TocaDesde(VR(self.voz), VR(self.origen), VR(self.frase),
                         VR(self.intensidad));
      } else {
        return self.jugadorOye();
      }
    ],
    jugadorOye [
      enc_orig inicial se_oye i _intensidad indLug;
      ! Se ve si el sonido alcanza 'location'
      if ((~~self.origen) || (~~self.sonando)) return false;
      if (self.intensidad ~= 0) _intensidad = VR(self.intensidad);
      else                      _intensidad = nivelNormal;

      inicial = parent(VR(self.origen));

      ! Intentamos 'sacar' el sonido a un 'Lugar'
      enc_orig = false;
      while (inicial) {
        if (inicial ofclass Lugar) {
          ! Lo hemos logrado, propagar sonido desde ahí
          PropagaSonido(inicial, _intensidad);
          enc_orig = true;
          break;
        } else {
          ! Cada 'escalada' implica perder potencia
          if (inicial ~= selfobj) {
            if (inicial has open) {
              _intensidad = _intensidad - costePorContenedor;
            } else {
              _intensidad = _intensidad - costePorContenedorCerrado;
            }
          }

          ! Seguimos escalando en los padres
          inicial = parent(inicial);
          
          ! Caso especial: nos saltamos al jugador como origen válido de sonidos
          if (inicial == selfobj) inicial = location;
        }
      }

      if (enc_orig == false) return false;

      se_oye = ((location has en_ruta) && (location.number > nivelAudible));

      ! Limpiar la propagación
      for (indLug = 1 : indLug <= indiceLugares : indLug++) {
        i = tablaLugares-->indLug;
        give i ~en_ruta;
        i.number = 0;
      }

      return se_oye;
    ],
    describe [;
      return true;
    ],
    iniciar [;
      if (~~self.sonando) {
        self.sonando = true;

        if ((self hasnt concealed) && (self.jugadorOye())) {
          print "Empiezas a oir ", (name) VR(self.voz), ".^";
          move self to location;
        }
      }
    ],
    parar [;
      if (self.sonando) {
        if ((self hasnt concealed) && (self.jugadorOye())) {
          print "Dejas de oir ", (name) VR(self.voz), ".^";
        }

        self.sonando = false;
        move self to contGeneral;
      }
    ],
    before [
      jo;
      Listen: 
        ! Escuchar un sonido es como examinar una cosa 
        ! muestra su descripción si es que existe
        jo = self.jugadorOye();
        if ((self provides description) && jo) {
          PrintOrRun(self, description);
          rtrue;
        } else if (jo) {
          rfalse;
        } else {
          "No puedes oir eso por aquí cerca.";
        }

      default:
        "No puedes hacer eso con un sonido.";
    ],
  has static;

! Función necesaria para registrar Ruidos
[ RegistraRuido _ruido;
  if (_ruido ofclass Ruido) {
    if (indiceRuidos < SONIDOS_MAX_NUM_RUIDOS) {
      indiceRuidos++;
      tablaRuidos-->indiceRuidos = _ruido;
      return true;
    }
!    return false;
  }

  return false;
];

[ RegistraRuidos
  i;
  if (ruidosApuntados) return;
  objectloop (i ofclass Ruido) RegistraRuido(i);
  ruidosApuntados = true;
];

!
! OBJETOS DE LA LIBRERIA
!

!
! Objetos que se usan en muchas funciones de la librería
!
TipoDeSonido tipoPlano;

!
! FUNCIONES
!

! Transmisión recursiva de un sonido desde un objeto EN una habitación:
[ TocaSonido contenido
  i inicial temp_i k enc_orig origen_aparente j hab_esc tipo _oido indOye indLug
  retor;

  ! La primera vez crear el array
  RegistraTodo();

  retor = false;

  ! por comodidad de manejo
  tipo = contenido.clase;

  ! contenido.origen es el objeto que produce el sonido
  origen_aparente = contenido.origen;
  if (contenido.origen ofclass Lugar) inicial = contenido.origen;
  else                                inicial = parent(contenido.origen);

  ! Intentamos 'sacar' el sonido a un 'Lugar'
  enc_orig = false;
  while (inicial) {
    if (inicial ofclass Lugar) {
      ! Lo hemos logrado, propagar sonido desde ahí
      PropagaSonido(inicial, contenido.intensidad );
      contenido.seEscuchaEn = origen_aparente;
      enc_orig = true;
      break;
    } else {
      ! Sólo cambiamos el origen aparente si el padre no es un soporte, no es
      ! transparente, ni un contenedor abierto
      if ((inicial hasnt supporter) && (inicial hasnt transparent) &&
          (inicial hasnt open)) {
        origen_aparente = inicial;
      }

      ! Seguimos escalando en los padres
      inicial = parent(inicial);

      ! Cada 'escalada' implica perder potencia
      if (origen_aparente has open) {
        contenido.intensidad = contenido.intensidad - costePorContenedor;
      } else {
        contenido.intensidad = contenido.intensidad - costePorContenedorCerrado;
      }
      
      ! Caso especial, nos saltamos al jugador como origen válido de los sonidos
      if (inicial == selfobj) inicial = location;
    }
  }

  if (enc_orig == false) {
    ! print_ret (The) contenido.origen, " no está en ningún sitio.^";
    return false;
  }

  ! Vemos si el jugador oye el sonido
  if ((location ofclass Lugar) && (location has en_ruta) &&
      (location.number > nivelAudible)) { 
    ! Buscamos un origen 'aparente', simplemente un Lugar cercano con 
    ! intensidad mayor
    enc_orig = false;
    objectloop (i in Compass) {
      k = ConduceSonidoA(i, location);
      ! print (name)i, "-", (name)k, "-", k.cantidad, "-", location.cantidad, " ";
      if (k && (k has en_ruta) && (k.number > location.number)) {
        ! Esta misma vale
        enc_orig = true;
        break;
      }
    }
    ! print "^";

    temp_i = contenido.intensidad;
    contenido.intensidad = location.number;
    ! print "De: ", (name)contenido.origen, "^";
    contenido.sePuedeVer = TestScope(contenido.origen);
    ! Si proviene de un 'Lugar' diferente ponemos el aparente
    ! si no, dejamos la fuente original aparente
    ! print "¿?: ", (name)inicial, " ", (name)i, "^";
    if (location ~= inicial) contenido.seEscuchaEn = i;
    tipo.escuchado(contenido);
    retor = true;
    contenido.intensidad = temp_i;
  }

  ! Vemos si algún oyente oye el sonido
  for (indOye = 1 : indOye <= indiceOyentes : indOye++) {
    i = tablaOyentes-->indOye;
    ! Si el objeto no está en un Lugar, escuchar desde 'fuera' del contenedor
    hab_esc = parent(i);
    
    while (hab_esc) {
      if (hab_esc ofclass Lugar) {
        break;
      } else {
        ! Cada 'escalada' implica perder potencia
        if (hab_esc has open) {
          contenido.intensidad = contenido.intensidad - costePorContenedor;
        }
        else {
          contenido.intensidad = contenido.intensidad -
                                 costePorContenedorCerrado;
        }

        ! Seguimos escalando en los padres
        hab_esc = parent(hab_esc);
      }
    }

    if (hab_esc has en_ruta) {
      ! Sí que lo oye. Buscamos un origen 'aparente': simplemente un Lugar
      ! cercano con intensidad mayor
      if (hab_esc == parent(i)) {
        enc_orig = false;
        objectloop (j in Compass) {
          k = ConduceSonidoA(j, parent(i));
          if (k && (k has en_ruta) && (k.number > parent(i).number)) {
            ! Esta misma vale
            enc_orig = true;
            break;
          }
        }
      } else {
        ! Lamentablemente, si el oyente está en algo que no sea un Lugar,
        ! el origen será desconocido... simplemente 'fuera'
        enc_orig = false;
      }

      if (~~enc_orig) contenido.seEscuchaEn = 0;
    
      temp_i = contenido.intensidad;
      contenido.intensidad = hab_esc.number;
      contenido.sePuedeVer = TestScope(contenido.origen, i);
      ! Si proviene de un 'Lugar' diferente ponemos el aparente
      ! si no, dejamos la fuente original aparente
      if (parent(i) ~= inicial) contenido.seEscuchaEn = j;
      ! print (name)i, " -> ", contenido.intensidad, "^";
      i.apunta(contenido);
      contenido.intensidad = temp_i;
    }
  }

  ! Limpiar la propagación
  for (indLug = 1 : indLug <= indiceLugares : indLug++) {
    i = tablaLugares-->indLug;
    if (i has en_ruta) {
      give i ~en_ruta;
      i.number = 0;
    }
  }

  ! Ahora hacer que oigan los oyentes
  for (indOye = 1 : indOye <= indiceOyentes : indOye++) {
    i = tablaOyentes-->indOye;
    if (i has en_ruta) {
      _oido = ClonaContenido(contenido);
      _oido.intensidad = i.intPend;
      _oido.sePuedeVer = i.sePuePend;
      _oido.seEscuchaEn = i.seEscPend;
      !  print (name)i, " --> ", _oido.intensidad, " ", i.intPend, "^";
      i.limpia();
      i.oido(_oido);
      DestruyeContenido();
    }
  }

  return retor;
];

Global sonidoTrasUnObstaculo;

[ PropagaSonido habitacion _intensidad
  i k _nueva;
  ! print (name)habitacion, " con intensidad ", _intensidad, "^";

  if (habitacion ~= 0) {
    give habitacion en_ruta;
    habitacion.number = _intensidad;
    
    ! Propagamos a todos los Lugares
    objectloop (i in Compass) {
      k = ConduceSonidoA(i, habitacion);
      if (k) {
        if (sonidoTrasUnObstaculo) _nueva = _intensidad - costePorObstaculo;
        else                       _nueva = _intensidad - costePorHabitacion;
        if ((k.number < _nueva) && (k ofclass Lugar)) PropagaSonido(k, _nueva);
      }
    }
  }
];

! Realiza el efecto de una voz lejana que se escucha siempre lejos
! reconocible o no
[ VozLejana _texto _intensidad _tipo _origen _modo
  i indOye;
  ! Si el origen se deja vacío es que es desconocido

  ! Rellenamos el contenido de forma apropiada
  contGeneral.origen      = _origen;       ! El que sea
  contGeneral.modo        = _modo;         ! El que sea
  contGeneral.sePuedeVer  = false;         ! Fuera de la vista
  contGeneral.seEscuchaEn = 0;             ! En un lugar desconocido
  contGeneral.texto       = _texto;        ! Texto pedido
  if (_intensidad == 0) {                  ! Si no han dicho nada
    contGeneral.intensidad = nivelNormal;  !   --> Intensidad media
  } else {                                 ! sino
    contGeneral.intensidad = _intensidad;  !   --> la que hayan dicho
  }
  if (_tipo == 0) contGeneral.clase = tipoPlano;
  else            contGeneral.clase = _tipo;

  ! Se toca con el sonido de clase base
  if (_tipo == 0) tipoPlano.escuchado(contGeneral);
  else            _tipo.escuchado(contGeneral);

  ! Avisar del sonido a todos los oyentes
  for (indOye = 1 : indOye <= indiceOyentes : indOye++) {
    i = tablaOyentes-->indOye;
    i.oido(ClonaContenido(contGeneral));
    DestruyeContenido();
  }
];

[ ConduceSonidoA direccion estelugar
  k tmp tmp2;
  sonidoTrasUnObstaculo = false;

  ! print "-ConduceSonidoA: ", (name) direccion, ":";
  if (estelugar provides direccion.door_dir) { ! Conexion normal
    k = estelugar.(direccion.door_dir);
  } else {                                     ! Pared que deja pasar el sonido
    if (estelugar provides cercanoA) k = dameLugarCercano(estelugar, direccion);
    else                             k = 0;

    if (~~k) return 0;
    sonidoTrasUnObstaculo = true;
  }

  if (ZRegion(k) == 2) { 
    ! print "Rutina que devuelve ";
    k = k();
  }
  if (ZRegion(k) ~= 1) { 
    ! print " no se puede pasar^";
    return 0;
  }

  if (k has door) {
    ! print " una puerta ";
    sonidoTrasUnObstaculo = (k hasnt open);

    tmp = parent(k);
    move k to estelugar;
    tmp2 = k.door_to();
    move k to tmp;
    k = tmp2;
    ! print " que lleva a ";
  }
    
  ! print (name) k;
  if (~~(k ofclass Lugar)) {
    ! print " que no es un lugar válido.^";
    return 0;
  }

  ! print ".^";
  return k;
];

! Proporciona la localidad en esa dirección a través de paredes que dejan pasar
! sonido:
[ dameLugarCercano estelugar direccion lon
  i;
  if (~~(esteLugar provides cercanoA)) {
    ! No hay lugares cercanos
    return 0;
  }

  ! ver número de entradas
  lon = (esteLugar.#cercanoA) / WORDSIZE;
  ! print lon, "^";

  ! Recorrerlas en pares
  i = 0;
  while (lon > 0) {
    if (VR(esteLugar.&cercanoA-->i) == direccion) {
      if (lon > 1) return (VR(esteLugar.&cercanoA-->(i + 1)));
      else         return 0;  
    }

    lon = lon - 2;
    i = i + 2;
  }  

  return 0;
];

! Tocador de sonidos desde objetos 'no-hablantes'
[ TocaDesde _tipo _origen _texto _intensidad _modo;
  contGeneral.origen     = _origen;        ! El que sea
  contGeneral.modo       = _modo;          ! El que sea
  contGeneral.sePuedeVer = TestScope(_origen);        
  contGeneral.texto      = _texto;         ! Texto pedido
  contGeneral.clase      = _tipo;          ! Clase dada
  if (_intensidad == 0) {                  ! Si no han dicho nada
    contGeneral.intensidad = nivelNormal;  !   --> Intensidad media
  } else {                                 ! sino
    contGeneral.intensidad = _intensidad;  !   --> la que hayan dicho
  }

  return TocaSonido(contGeneral);
];

! Nueva función de Escuchar que 'oye' todos los ruidos
[ ListenEspecial
  i contador indRud;
  contador = 0;
  if (noun == 0) {
    for (indRud = 1 : indRud <= indiceRuidos : indRud++) {
      ! print "<", indRud, ",", indiceRuidos, ">^";
      i = tablaRuidos-->indRud;
      if (TocaDesde(i.voz, i.origen, i.frase, i.intensidad)) {
        ! Puede ser escuchado por el jugador
        contador++;
      }
    }
  } else {
    for (indRud = 1 : indRud <= indiceRuidos : indRud++) {
      i = tablaRuidos-->indRud;
      if ((i.sonando) && (i.origen == noun)) {
        "Parece que ", (del) noun, " procede ", (name) i.voz, ".";
      }
    }

    if (noun in Compass) {
      "Escuchas un poco por si ", (del) noun, " proviene algún sonido.";
    } else if (noun == selfobj) {
      "Te escuchas un rato, pero no oyes nada raro.";
    } else if (noun has animate) {
      "Escuchas un poco por si ", (el) noun, " dice algo.";
    } else {
      "Escuchas un poco por si ", (el) noun, " emite algún sonido.";
    }
  }

  ! print "[Escuchar con ", (name) noun, " y total ", contador, "]^";

  if (contador == 0) {
    ! Si no se escucha nada mensaje estandar de la librería
    L__M(##Listen, 1, noun);
  }
];

[ ListenNadaSub;
  ! print "[EscucharKK con ", (name) noun, "]^";
  ! L__M(##Listen, 1, noun);
  "No veo eso por aquí.";
];

! Rutina que ejecuta rutinas si las encuentra
#ifndef VR;
[ VR valor;
  ! print "[VR: ", valor, " --> ", ZRegion(valor), " ]^";
  if (ZRegion(valor) == 2) return valor();
  else                     return valor;
];
#endif;

! Función para determinar si está al alcance del oído
[ Oible
  i indRud;
  ! print "[ Oible con ", scope_stage, "]^";
  switch (scope_stage) {
    1: rfalse;
    2: for (indRud = 1 : indRud <= indiceRuidos : indRud++) {
         i = tablaRuidos-->indRud;
         PlaceInScope(i);
       }
  }
];

! Nuevo Locale simplemente para enganchar un nuevo hook general de usuario justo
! antes de los mensajes:
[ LocaleEspecial descin text_without_ALSO text_with_ALSO
  o p num_objs must_print_ALSO;

  ! Nuevo hook
  TrataRuidos();

  objectloop (o in descin) give o ~workflag;
  num_objs = 0;
  objectloop (o in descin)
    if (o hasnt concealed && NotSupportingThePlayer(o)) {
      #Ifndef MANUAL_PRONOUNS;
      PronounNotice(o);
      #Endif;
      if (o has scenery) {
        if (o has supporter && child(o)) SayWhatsOn(o);
      }
      else {
        give o workflag; num_objs++;
        p = initial;
        if ((o has door or container) && o has open && o provides when_open) {
          p = when_open; jump Prop_Chosen;
        }
        if ((o has door or container) && o hasnt open && o provides when_closed) {
          p = when_closed; jump Prop_Chosen;
        }
        if (o has switchable && o has on && o provides when_on) {
          p = when_on; jump Prop_Chosen;
        }
        if (o has switchable && o hasnt on && o provides when_off) {
          p = when_off;
        }

      .Prop_Chosen;

        if (o.&describe && RunRoutines(o, describe)) {
          must_print_ALSO = true;
          give o ~workflag; num_objs--;
          continue;
        }
        if (o.p && (o hasnt moved || p ~= initial)) {
          new_line;
          PrintOrRun(o, p);
          must_print_ALSO = true;
          give o ~workflag; num_objs--;
          if (o has supporter && child(o)) SayWhatsOn(o);
        }
      }
    }

  if (num_objs == 0) return 0;

  if (actor ~= player) give actor concealed;
  if (text_without_ALSO) {
    new_line;
    if (must_print_ALSO) print (string) text_with_ALSO, " ";
    else                 print (string) text_without_ALSO, " ";
    WriteListFrom(child(descin),
      ENGLISH_BIT+RECURSE_BIT+PARTINV_BIT+TERSE_BIT+CONCEAL_BIT+WORKFLAG_BIT);
  }
  else {
    if (must_print_ALSO) L__M(##Look, 5, descin);
    else                 L__M(##Look, 6, descin);
  }
  if (actor ~= player) give actor ~concealed;
  return num_objs;
];

!
! Tratamiento de ruidos colgando de Local
!
[ TrataRuidos;
  ! Nuevo hook
  RutinaPreLocal();

  ! Tratamiento de ruidos
  ! QQQ: de momento vacío no se me ocurre realmente nada que
  ! hacer con ella y que cumpla todos los requisitos
];

[ ListenSub;
  ListenEspecial();
];

[ Locale obj;
  LocaleEspecial(obj);
];

!
! Cambiar el Scope de 'Escuchar'
!
Extend 'escucha' replace
  *                   -> Listen
  * 'a//' noun        -> Listen
  * noun              -> Listen
  * 'a//' scope=Oible -> Listen
  * scope=Oible       -> Listen
  * topic             -> ListenNada;

