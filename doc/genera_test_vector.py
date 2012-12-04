# -*- coding: iso-8859-15 -*-
import sys
import getopt
import shutil

def usage():
		print "Syntaxis: genera_test_vector -i inputfile -o outputfile -c commentcharacter -n 200";

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
    maxcommands=200; # Por defecto...
			
    try:
        opts, args = getopt.getopt(argv,"hi:o:c:n:",["help","input","output","comment","num"]);
    except getopt.GetoptError, err:        
        usage();
        sys.exit(2);        

    for o, a in opts: 		
        if o in ("-h","--help"):
            usage();
            sys.exit();
        elif o=="-i":
            inputfile = a;
        elif o=="-o":
            outputfile=a;
        elif o=="-n":
            maxcommands=int(a);            
        elif o=="-c":
            print commentstyle;
            commentstyle=commentstyle.replace("!",a);
            print commentstyle;
        else:
            assert False, "unhandled option";
			
    # Comprueba que existe el fichero de entrada...
    if (fileExists(inputfile)==0):
            print "Input file cannot be found: ",inputfile;
            usage();
            sys.exit(-1);

		   
    if(outputfile==""):
            print"An output file is missing.";
            usage();
            sys.exit(-1);
		   
    file1 = inputfile;
    file2 = outputfile

    print "Processing: ",inputfile;	
    f1 = open(file1,'rb');
    f2 = open(file2,'wb');	

    # Bucle principal..
    # Quiero extraer línea por línea...
    f2.write ("[test_function;\r\n");
    f2.write ("test_machine.clear();\r\n");
    numcommands=0;
    warning_given=0;
    for line in f1:
        index=-1;
        end=-1;
        if (len(line)>0):
            if ((line.rfind(">"))>-1):
                index=line.rfind(">");
                index=index+1;
            if (line.find(commentstyle)>-1):
                end=line.find(commentstyle);
            if (index>-1): # Ha encontrado algo
                if (end>0):
                    cadena = line[index:end];
                else:
                    cadena=line[index:];
                cadena=cadena.rstrip();
                if (len(cadena)>0):
                    if (numcommands>maxcommands):
                        if (warning_given==0):
                            print ("(WARNING) The maximum number of commands have been exceed.\r\n");
                            warning_given=1;
                        cadena = "!test_machine.inserta(\""+cadena+"\");\r\n";
                    else:
                        cadena = "test_machine.inserta(\""+cadena+"\");\r\n";
                    f2.write(cadena);
                    numcommands=numcommands+1;
    f2.write ("test_machine.run();\r\n");
    f2.write ("];\r\n");
    f1.close();
    f2.close();	
    print "Finished ",numcommands," commmands processed (",maxcommands," commands are allowed).";	
	
if __name__ == "__main__":
    main(sys.argv[1:])	
