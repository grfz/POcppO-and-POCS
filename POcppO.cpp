/* POcppO.cpp: create an header POcppO to manage a class
Il file di input deve rispondere al seguente formato:
CLASSNAME  
PROPERTY_TYPE_1 PROPERTY_NAME_1 
PROPERTY_TYPE_2 PROPERTY_NAME_2
...
PROPERTY_TYPE_3 PROPERTY_NAME_3 
input file example:
===BeginOfFile===
Student
int age
char* name
Data* birth_date
===EndOfFile===
NOTE for compiling an input file: 
- class name must have an Uppercase first letter
- property names must be all lowercased
- data type for properties:
  int long float char char* 
  or handlers to other classes having an header created 
  with this program
  */

#define     MAX_PROPERTIES   40    /* max number of properties */
#define     MAX_STR     4096  /* max length of a property name */
#define     STRING_C "char*"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "./toolkit.h"

/* every functions write a component of the out file */
void  includes();       /* define, headers */
void  declare ();       /* class declaration */  
void  prototypes();     /* prototypes */
void  create  ();       /* constructor */
void  clone   ();       /* constructor by copy */
void  destroy ();       /* destructor */
void  getty   (int);    /* accessors (get) 
                           the parameter is the property position inside the class */
void  setty   (int);    /* accessors (set)
                           the parameter is the property position inside the class */
void  load    ();       /* serialization- costructor from CSV file */
void  save    ();       /* serialization - save the object state on a CSV file */
void  show    ();       /* su stdout */
void  footer  ();      

char    *primitives_types[] = {"int","long","float","char"};
typedef enum  {PRIMITIVE=0,STRING,HANDLER }   Tipo;
char    *property_name [MAX_PROPERTIES];
char    *property_type [MAX_PROPERTIES];
Tipo    type[MAX_PROPERTIES];
int     property_number;
char    *class_name;
char    *class_name_upper; /* for #define in header file */

FILE    *out; /* the file out: spreadly used in every function */

int main  (int argc, char** argv) {
  int   i;
  char  out_file[30];
  char  *tmp;
  FILE  *in;
  
  if ((in = fopen(argv[1], "r")) == NULL) {
     printf("USAGE:%s INPUTFILE\n", argv[0]);
     return (-1);
  }
  
  if (class_name = read_field_from_csv(in, '\n'));
  
  sprintf(out_file, "%s.hpp", class_name);
  printf("output file:\"%s\"\n", out_file);
  printf("class %s {\n\tprivate:", class_name);
  for(      property_number = 0;
            property_type[property_number] = read_field_from_csv(in, ' ');
            property_number++) {
    /* set the type */
    clean_string(property_type[property_number], 2, ' ', '\t');
    type[property_number] = HANDLER;
    for (i = 0; i < 4; i++)
      if ( strcmp(property_type[property_number], primitives_types[i]) == 0 ) 
        type[property_number] = PRIMITIVE;
    if ( strcmp(property_type[property_number], STRING_C) == 0 ) 
        type[property_number] = STRING;
      
    /* set the name */
    property_name[property_number] = read_field_from_csv(in, ' ');
    
    printf("\t%s\t", property_type[property_number]);
    printf("%s;\n", property_name[property_number]);
    
    if (type[property_number] == HANDLER) 
            property_type[property_number] = clean_string (property_type[property_number], 1, '*');
  }
  printf(" };\n");
  fclose(in);
  
  /* opening output file */
  if ((out = fopen(out_file, "w")) == NULL) {
     printf("cannot open output file\n");
     return (-2);
  }
  
  includes();
  declare();
  prototypes();
  create();
  clone();
  destroy();
  for (i = 0; i < property_number; i++)   getty(i);
  for (i = 0; i < property_number; i++)   setty(i);
  load();
  save();
  show();
  footer();  
  
  fclose(out);
  
  return 0;
}

void    includes    ()  {
  int   len, i; /* i:on type[] */
  
  class_name_upper = uppercase(strdup(class_name));
  
  fprintf(out, "#ifndef\t%s_HPP\n",class_name_upper);
  fprintf(out, "#define\t%s_HPP\n\n",class_name_upper);
  fprintf(out, "#include\t<stdio.h>\n");
  fprintf(out, "#include\t<stdlib.h>\n");
  fprintf(out, "#include\t<string.h>\n");
  fprintf(out, "#include\t\"./toolkit.h\"\n");
  
  for(i = 0; i < property_number; i++)
    if (type[i] == HANDLER)
      fprintf(out, "#include\t\"./%s.hpp\"\n", property_type[i]);
  fprintf(out, "\n");  
}

void    declare    ()  {
  int   i;
  
  fprintf(out,"class\t%s\t{\n\tprivate:", class_name);
  for (i=0;i<property_number;i++) {
    fprintf(out,"\n");
    switch  (type[i]) {
        case HANDLER: {
        	fprintf(out, "\t\t%s\t*%s;", property_type[i], property_name[i]);
        	break; }
        case PRIMITIVE: {
	        fprintf(out, "\t\t%s\t%s;", property_type[i], property_name[i]);
        	break; }
        case STRING:   {
        	fprintf(out,"\t\tchar\t*%s;", property_name[i]);
        	break; }
    }
  }
  fprintf(out, "\n\tpublic:");
}

void    prototypes () {
  int   i;

  fprintf(out, "\n\t\t/* accessors */\n");
  for (i=0; i<property_number;i++) {
    switch  (type[i]) {
    case HANDLER: {
        fprintf(out, "\t\tvoid\tset_%s\t(%s*);\n",
                        property_name[i],
                        property_type[i]);
        fprintf(out, "\t\t%s*\tget_%s\t();\n",
                        property_type[i],
                        property_name[i]);
        break; }
    case PRIMITIVE: {
        fprintf(out, "\t\tvoid\tset_%s\t(%s);\n",
                        property_name[i],
                        property_type[i]);
        fprintf(out, "\t\t%s\tget_%s\t();\n",
                        property_type[i],
                        property_name[i]);
        break; }
    case STRING: {
        fprintf(out, "\t\tvoid\tset_%s\t(char*);\n",
                        property_name[i]);
        fprintf(out, "\t\tchar*\tget_%s\t();\n",
                        property_name[i]);
        break; }
    }
  }
  fprintf(out, "\t\t/* distruttore */\n");
  fprintf(out, "\t\t~%s\t();\n",
            class_name);
  fprintf(out, "\t\t/* costruttori */\n");
  fprintf(out, "\t\t%s\t(",
    class_name);
  for (i = 0; i < property_number; i++) {
    switch  (type[i]) {
        case HANDLER: {
          fprintf(out, "%s*",
                property_type[i]);
          break; }
        case PRIMITIVE: {
          fprintf(out, "%s", 
            property_type[i]);
          break; }
        case STRING: {
          fprintf(out, STRING_C);
          break; }
    }
    if (i == property_number - 1) fprintf(out, ");\n");
    else fprintf(out, ", ");
  }
  fprintf(out, "\t\t%s\t(const %s&);\n",
        class_name,
        class_name);
  fprintf(out, "\t\t%s\t(FILE*);\n",
        class_name);
  fprintf(out, "\t\t/* serializzatore */\n");
  fprintf(out, "\t\tvoid\tsave\t( FILE*, char, char);\n");
  fprintf(out,"\t\t/* altro */\n");
  fprintf(out, "\t\tvoid\tshow\t();\n};\n\n");
}

void    setty   (int i) {
  switch  (type[i]) {
    case HANDLER: {
        fprintf(out, "void\t%s :: set_%s\t(%s *_%s) {\n",
                        class_name,
                        property_name[i],
                        property_type[i],
                        property_name[i]);
        fprintf(out, "\tdelete(%s);\n",
                        property_name[i]);
        fprintf(out, "\t%s = new %s(*_%s); }\n\n",
                        property_name[i],
                        property_type[i],
                        property_name[i]);
        break; }
    case PRIMITIVE: {
        fprintf(out, "void\t%s :: set_%s\t(%s _%s) {\n",
                        class_name,
                        property_name[i],
                        property_type[i],
                        property_name[i]);
        fprintf(out, "\t%s = _%s; }\n\n",
                        property_name[i],
                        property_name[i]);
        break; }
    case STRING: {
        fprintf(out, "void\t%s :: set_%s\t(char* _%s) {\n",
                        class_name,
                        property_name[i],
                        property_name[i]);
        fprintf(out, "\tfree(%s);\n",
                        property_name[i]);
        fprintf(out, "\t%s = strdup(_%s); }\n\n",
                        property_name[i],
                        property_name[i]);
        break; }
  }
}

void    getty   (int i) {

    switch  (type[i])   {
        case HANDLER: {
            fprintf(out, "%s*\t%s :: get_%s\t() { ",
                        property_type[i],
                        class_name,
                        property_name[i]);
            fprintf(out, "return new %s(*%s); }\n\n",
                        property_type[i],
                        property_name[i]);
            break; }
        case PRIMITIVE: {
            fprintf(out, "%s\t%s :: get_%s\t() { ",
                        property_type[i],
                        class_name,
                        property_name[i]);
            fprintf(out, "return (%s); }\n\n",
                        property_name[i]);
            break; }
        case STRING: {
            fprintf(out, "char*\t%s :: get_%s\t() { ",
                        class_name,
                        property_name[i]);
            fprintf(out, "return strdup(%s); }\n\n",
                        property_name[i]);
            break; }
    }
}

void    destroy ()  {
  int i;
  
  fprintf(out, "%s :: ~%s\t() {\n",
            class_name,
            class_name);
  for (i = 0; i < property_number; i++) {
    switch  (type[i]) {
    case    HANDLER: {
      fprintf(out, "\tdelete(%s);\n",
            property_name[i]);
      break; }
    case    PRIMITIVE: {
      break; }
    case    STRING: {
      fprintf(out, "\tfree(%s);\n",
            property_name[i]);
      break; }
    }
  }
  fprintf(out, "}\n\n");
}

void    create  ()  {
  int i;
  
  fprintf(out, "%s :: %s\t(",
    class_name,
    class_name);

  for (i = 0; i < property_number; i++) {
    switch  (type[i]) {
        case HANDLER: {
	        fprintf(out, "%s* _%s",
                property_type[i],
                property_name[i]);
	        break; }
        case PRIMITIVE: {
	    fprintf(out, "%s _%s", 
            property_type[i],
            property_name[i]);
	    break; }
        case STRING: {
	    fprintf(out, "char* _%s",
            property_name[i]);
	    break; }
    }
    if (i == property_number - 1) fprintf(out, ") {\n");
    else fprintf(out, ", ");
  }
  
  for (i = 0; i < property_number; i++) {
    switch  (type[i]) {
        case HANDLER: {
	        fprintf(out, "\t%s = new %s(*_%s);\n",
	              property_name[i],
	              property_type[i],
	              property_name[i]);
	        break; }
        case PRIMITIVE: {
	        fprintf(out, "\t%s=_%s;\n", 
                property_name[i],
                property_name[i]);
        	break; }
        case STRING: {
        	fprintf(out, "\t%s=strdup(_%s);\n",
                property_name[i],
                property_name[i]);
        	break; }
    }
  }
  fprintf(out, "}\n\n");
}

void    clone() {
  int i;
  
  fprintf(out, "%s :: %s\t(const %s& a) {\n",
        class_name,
        class_name,
        class_name);
  
  for (i = 0; i < property_number; i++)   {
    switch  (type[i]) {
        case HANDLER: {
          fprintf(out, "\t%s = new %s(*a.%s);\n",
                property_name[i],
                property_type[i],
                property_name[i]);
          break; }
        case PRIMITIVE: {
          fprintf(out, "\t%s = a.%s;\n", 
                property_name[i],
                property_name[i]);
          break; }
        case STRING: {
          fprintf(out, "\t%s = strdup(a.%s);\n",
                property_name[i],
                property_name[i]);
          break; }
    }  
  }
  fprintf(out, "}\n\n");
}

void  load  ()  {
  int   flag_primitive;
  char  format;
  int   i;
    
  fprintf(out, "%s :: %s\t(FILE* fp,char separator) {\n",
        class_name,
        class_name);
  flag_primitive=0;
  for (i = 0; i < property_number; i++) 
    if  (type[i] == PRIMITIVE) 
	     flag_primitive++;
  if (flag_primitive)
      fprintf(out, "\tchar* tmp;\n");
    
  for (i = 0; i < property_number; i++) {
    switch  (type[i]) {
    case HANDLER: {
        fprintf(out, "\t%s = new %s(fp,separator);\n",
            property_name[i],
            property_type[i]);
        break; }
    case PRIMITIVE: {
        fprintf(out, "\ttmp = read_field_from_csv(fp,separator);\n");       
        format = 'd';
        if (strcmp("float", property_type[i]) == 0)  format = 'f';
        if (strcmp("char", property_type[i]) == 0)   format = 'c';

        fprintf(out, "\tsscanf(tmp,\"%%%c\",&%s);\n", format, property_name[i]);
        fprintf(out, "\tfree(tmp);\n");
    	break; }
    case STRING: {
      fprintf(out, "\t%s = read_field_from_csv(fp,separator);\n", property_name[i]);
      if (i == 0) fprintf(out, "\tif(%s==NULL) return;\n", property_name[i]);
      break; }
    }
  }
  fprintf(out, "}\n\n");
}

void  save  ()  {
  int i;
  char format; 
  
  fprintf(out, "void\t%s :: save\t(FILE* fp, char separator, char terminator) {",
    class_name);
  for ( i = 0; i < property_number; i++) {
    fprintf(out,"\n");
    switch  (type[i]) {
    case HANDLER: {
      fprintf(out, "\t%s->save(fp,separator,%s);",
        property_name[i],
        (i < property_number-1)? "separator" : "terminator");
      break; }
    case PRIMITIVE: {
      format = 'd';
      if (strcmp("float", property_type[i]) == 0) format = 'f';
      else if (strcmp("char", property_type[i]) == 0) format = 'c';

      fprintf(out,"\tfprintf(fp,\"%%%c%%c\",%s,%s);",
        format, 
        property_name[i],
        (i<property_number-1)? "separator" : "terminator");
      break; }
    case STRING: {
		  fprintf(out, "\tfprintf(fp,\"%%s%%c\",%s,%s);", 
        property_name[i],
        (i < property_number-1)? "separator" : "terminator" );
		  break; }
    }
  }
  fprintf(out, " }\n\n");  
}

void show() {
  int i;
  char format; 
  
  fprintf(out, "void\t%s :: show\t() {\n",
    class_name);
  for ( i = 0; i < property_number; i++) {
    fprintf(out, "\tprintf(\"%s:", property_name[i]);
    switch  (type[i]) {
    case HANDLER: {
      fprintf(out, "\");\n");
      fprintf(out, "\t%s->show();\n",
        property_name[i]);
      break; }
    case PRIMITIVE: {
      format = 'd';
      if (strcmp("float", property_type[i]) == 0) format = 'f';
      else if (strcmp("char", property_type[i]) == 0) format = 'c';
      fprintf(out,"%%%c\\n\",%s);\n", 
        format, 
        property_name[i]);
      break; }
    case STRING: {
      fprintf(out, "%%s\\n\",%s);\n", property_name[i]);
      break; }
    }
  }
  fprintf(out, "\tprintf(\"\\n\"); }\n\n");  
}

void footer() {
  int   len, i;
  
  len = strlen(class_name_upper);
  
  fprintf(out, "#endif /* %s_HPP */", class_name_upper);
  free(class_name_upper);
}