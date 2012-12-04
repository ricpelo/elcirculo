# -*- coding: iso-8859-15 -*-
import sys
import getopt
import shutil

def usage():
        print"Syntaxis: textool -x atd -r atd -i inputfile -o outputfile -m numwords -l numwords -c commentstyle";
        print " -x a Extract all strings from input file and write them in the output file. ";
        print "    t Extract all text strings from input file and write them in the output file. Default value.";
        print "    d Extract all dictionary strings and write them in the output file.";
        print " -r a Replace all strings in the outputfile with strings found in the input file. ";
        print "    t Replace all text strings in the outputfile with strings found in the input file. Default value.";
        print "    d Replace all dictionary strings in the outputfile with strings found in the input file. ";
        print " -i Path to the input file. ";
        print " -o Path to the desired output file.";
        print " -m Maximum number of words a string should contain to be written in the output file. Default value is no limit.";
        print " -l Minimum number of words a string should contain to be written in the output file. Default value is 2.";
        print " -c Comment Style. Default character is !";
        print"Examples:";
        print"To extract text strings from an INFORM source code: ";
        print"  textool -x t -i aventura.inf -o aventura.txt ";
        print"To replace text strings in an INFORM source code: ";
        print"  textool -r t -i aventura.txt -o aventura.inf ";
        print"To extract dictionary strings from an INFORM source code: ";
        print"  textool -x d -i aventura.inf -o aventura.txt -l 1";
        print"To replace dictionary strings in an INFORM source code: ";
        print"  textool -r d -i aventura.txt -o aventura.inf -l 1 ";
        print"To extract all text and dictionary strings from an INFORM source code: ";
        print"  textool -x a -i aventura.inf -o aventura.txt -l 1 ";
        print"To replace all text and dictionary strings in a INFORM source code: ";
        print"  textool -r a -i aventura.txt -o aventura.inf -l 1 ";
        print"To extract all write-strings from a SUPERGLUS source code: ";
        print"  textool -x t -i aventura.txp -o aventura.txt -l 1 -c ;";
        print"To replace all write-strings in a SUPERGLUS source code: ";
        print"  textool -r t -i aventura.txt -o aventura.txp -l 1 -c ;";
        
def fileExists(f):
    try:
         file = open(f)
    except IOError:
         exists = 0
    else:
        exists = 1
    return exists

        
def main(argv):
    
    extract = False;
    replace = False;
    inputfile = "";
    outputfile= "";
    minwords = 3;
    maxwords = 0;
    odelimiter = "\"";
    cdelimiter = "\"";
    commentstyle = "!";
    
    try:
        opts, args = getopt.getopt(argv,"hx:r:i:o:l:m:c:",["help","extract","replace","input","output","min","max"]);
    except getopt.GetoptError, err:        
        usage();
        sys.exit(2);    

    for o, a in opts:    
   
        if o == "-x":
            extract = True;
            if a=="d":
               odelimiter ="'";
               cdelimiter ="'";
               commentstyle = commentstyle+"\""; # De este modo ignoramos ap�strofes en textos
            elif a=="a":
               odelimiter ="'\"";
               cdelimiter ="'\"";
           
        elif o=="-r":
            replace= True;
            if a=="d":
               odelimiter ="'";
               cdelimiter ="'";
               commentstyle = commentstyle+"\""; # De este modo ignoramos ap�strofes en textos
            elif a=="a":
               odelimiter ="'\"";
               cdelimiter ="'\"";
        elif o in ("-h", "--help"):
            usage();
            sys.exit();
        elif o=="-o":
            outputfile = a
        elif o=="-i":
            inputfile = a;
        elif o=="-l":
            minwords= int(a);  
        elif o=="-m":
            maxwords= int(a);
        elif o=="-c":
            print commentstyle;
            commentstyle=commentstyle.replace("!",a);
            print commentstyle;
        else:
            assert False, "unhandled option"

    
    if (extract==False and replace==False):
        print "Syntax error: Extract or replace mode must be selected.";
        usage();
        sys.exit();

   # Comprueba que existe el fichero de entrada...
    if (fileExists(inputfile)==0):
           print "Input file cannot be found.",inputfile;
           usage();
           sys.exit(2);
    if(outputfile==""):
           print"An output file is missing.";
           usage();
           sys.exit(2);
           
    file1 = inputfile;
    file2 = outputfile;

    inicio=0;
    final =0;
    caracter = "1";
    cadena = "";
    salir = 0;
    numchars=0;
    numstrings=0;
    actual_delimiter = "";
    ignore = 0;
    
    print "Processing: ",inputfile;
    # line = line[:-1] #chop off the newline
    if (extract==True):
            print "Extracting mode";
            f1 = open(file1,'rb');
            f2 = open(file2,'wb');
            # Vuelca todas las cadenas contenidas entre el caracter "
            while (caracter!=""):
                caracter = f1.read(1);                
                                      
                if(inicio==0 and ignore==0):
                        if (commentstyle.find(caracter)>-1): # Nos hemos topado con un comentario...
                                ignore=1;                           
                        elif (odelimiter.find(caracter)>-1):
                            inicio=1;
                            numstrings=numstrings+1;
                            cadena = cadena+caracter;
                            actual_delimiter = cdelimiter[odelimiter.find(caracter)];
                elif(inicio==1 and ignore==0): # Vuelca la cadena...
                        if (actual_delimiter.find(caracter)>-1):
                            cadena = cadena + actual_delimiter+"\r\n";
                            
                            if (cadena.lstrip().count(" ")>=(minwords-1)):
                                    if (maxwords==0):
                                            f2.write(cadena);
                                    elif (cadena.lstrip().count(" ")<=(maxwords-1)):
                                            f2.write(cadena);
                                    else:
                                        numstrings=numstrings-1;
                            else:
                                numstrings=numstrings-1;
                            cadena = "";
                            inicio=0;
                        else:
                            cadena = cadena + caracter;

                        numchars=numchars+1;
                elif ((caracter=='\r' or caracter=='\n')): # Los comentarios dejan de ser efectivos al finalizar una l�nea.
                       ignore=0;    
                        
            f1.close();
            f2.close();
            
    if (replace==True):
        print "Replace mode";        
        f1 = open(file1,'rb'); # Input
        f2 = open(file2,'rb'); # Output donde va contenido el c�digo fuente...
        f3 = open("temp.txt","wb"); #temporal
        numstrings=0;
        vector=[];
        caracter=" ";
        inicio=0;
        ignore=0;
        actual_delimiter = "";
        # Extrae todas las cadenas de texto del fichero de entrada.
        while (caracter!=""):
                caracter = f1.read(1);
                
                if(inicio==0 and ignore==0):
                        if (commentstyle.find(caracter)>-1): # Nos hemos topado con un comentario...
                                ignore=1;                           
                        elif (odelimiter.find(caracter)>-1):
                            inicio=1;
                            numstrings=numstrings+1;
                            cadena = cadena+caracter;
                            actual_delimiter = cdelimiter[odelimiter.find(caracter)];
                elif(inicio==1 and ignore==0): # Vuelca la cadena...
                        if (actual_delimiter.find(caracter)>-1):
                            cadena = cadena + actual_delimiter;
                            
                            if (cadena.lstrip().count(" ")>=(minwords-1)):
                                    if (maxwords==0):
                                            vector.append(cadena);
                                    elif (cadena.lstrip().count(" ")<=(maxwords-1)):
                                            vector.append(cadena);
                                    else:
                                        numstrings=numstrings-1;
                            else:
                                numstrings=numstrings-1;
                            cadena = "";
                            inicio=0;
                        else:
                            cadena = cadena + caracter;

                        numchars=numchars+1;
                elif ((caracter=='\r' or caracter=='\n')): # Los comentarios dejan de ser efectivos al finalizar una l�nea.
                       ignore=0; 
                       
        caracter = " ";
        numstrings=0;
        inicio=0;
        actual_delimiter = "";
        # Reemplaza todas las cadenas
        while (caracter!=""):
                caracter = f2.read(1);
                
                if(inicio==0 and ignore==0):
                        if (commentstyle.find(caracter)>-1): # Nos hemos topado con un comentario...
                                ignore=1;  
                                f3.write(caracter);                                
                        elif (odelimiter.find(caracter)>-1):
                            inicio=1;
                            numstrings=numstrings+1;
                            cadena = cadena+caracter;
                            actual_delimiter = cdelimiter[odelimiter.find(caracter)];
                        else:
                            f3.write(caracter);
                elif(inicio==1 and ignore==0): # Vuelca la cadena...
                        if (actual_delimiter.find(caracter)>-1):
                            cadena = cadena + actual_delimiter;
                            
                            if (cadena.lstrip().count(" ")>=(minwords-1)):
                                    if (maxwords==0):
                                            f3.write(vector[numstrings-1]);
                                    elif (cadena.lstrip().count(" ")<=(maxwords-1)):
                                            f3.write(vector[numstrings-1]);
                                    else:
                                        numstrings=numstrings-1;
                                        f3.write(cadena);
                            else:
                                numstrings=numstrings-1;
                                f3.write(cadena);
                                
                            cadena = "";
                            inicio=0;
                        else:
                            cadena = cadena + caracter;

                        numchars=numchars+1;
                elif ((caracter=='\r' or caracter=='\n')): # Los comentarios dejan de ser efectivos al finalizar una l�nea.
                       ignore=0; 
                       f3.write(caracter);
                else:
                    f3.write(caracter); # Texto dentro de los comentarios...
                
                numchars=numchars+1;
        f1.close();
        f2.close();
        f3.close();
        
        shutil.copyfile(file2,"fuente.backup");
        shutil.copyfile("temp.txt",file2);
        
    print "Finished (",numchars," characters processed,",numstrings," strings found)";

if __name__ == "__main__":
    main(sys.argv[1:])

