/* POCS.c: create a header file for a POC structure
Input file format:
STRUCTURENAME 
FIELD_TYPE_1 FIELD_NAME_1 
FIELD_TYPE_2 FIELD_NAME_2
...
FIELD_TYPE_3 FIELD_NAME_3 
input file example:
===BeginOfFile===
Student
int age
char* name
Data* birth_date
===EndOfFile===
NOTE for compiling an input file: 
- struct name must have an Uppercase first letter
- fields names must be all lowercased
- data type for fields:
  int long float char char* 
  or pointers to other structures having an header created 
  with this program
  */

#define     MAX_FIELDS   40    /* max number of fields */
#define     MAX_STR     4096  /* max length of a field name */
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "./toolkit.h"

/* every functions write a component of the out file */
void  includes();       /* define and headers */
void  declare ();       /* struct declaration */  
void  prototypes();     /* prototypes */
void  create  ();       /* constructor */
void  clone   ();       /* constructor by copy */
void  destroy ();       /* destructor */
void  getty   (int);    /* accessors (get) 
                           the parameter is the field position within the class */
void  setty   (int);    /* accessors (set)
                           the parameter is the field position within the class */
void  load    ();       /* serializer- costructor from CSV file */
void  save    ();       /* serializer - save the object state on a CSV file */
void  show    ();       /* su stdout */
void  footer  ();    

char    *primitives_types[] = {"int","long","float","char"};
typedef enum  {PRIMITIVE=0, STRING, POINTER }   Type;
char    *field_name [MAX_FIELDS];
char    *field_type [MAX_FIELDS];
Type    type[MAX_FIELDS];
int     field_number;
char    *struct_name;
char    *struct_name_upper; /* for #define in header file */

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
  
  if (struct_name = read_field_from_csv(in, '\n'));
  
  sprintf(out_file, "%s.h", struct_name);
  printf("nome del file di output:\"%s\"\n", out_file);
  printf("typedef struct {\n");
  for(      field_number = 0;
            field_type[field_number] = read_field_from_csv(in, ' ');
            field_number++) {
    /* set the type */
    clean_string(field_type[field_number], 3, ' ', '\t');
    type[field_number] = POINTER;
    for (i = 0; i < 4; i++)
      if ( strcmp(field_type[field_number], primitives_types[i]) == 0 ) 
        type[field_number] = PRIMITIVE;
    if ( strcmp(field_type[field_number], "char*") == 0 ) 
        type[field_number] = STRING;
      
    /* set the name */
    field_name[field_number] = read_field_from_csv(in, ' ');
    
    printf("\t%s\t", field_type[field_number]);
    printf("%s;\n", field_name[field_number]);
    
    if (type[field_number] == POINTER) 
            field_type[field_number] = clean_string (field_type[field_number], 1, '*');
  }
  printf(" } %s;\n", struct_name);
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
  for (i = 0; i < field_number; i++)   getty(i);
  for (i = 0; i < field_number; i++)   setty(i);
  load();
  save();
  show();
  footer();  
  
  fclose(out);
  
  return 0;
}
    
void    includes    ()  {
  int   len, i; /* on type */
  
  struct_name_upper = uppercase(strdup(struct_name));
  
  fprintf(out, "#ifndef\t%s_H\n",struct_name_upper);
  fprintf(out, "#define\t%s_H\n\n",struct_name_upper);
  fprintf(out, "#include\t<stdio.h>\n");
  fprintf(out, "#include\t<stdlib.h>\n");
  fprintf(out, "#include\t<string.h>\n");
  fprintf(out, "#include\t\"./toolkit.h\"\n");
  
  for(i = 0; i < field_number; i++)
    if (type[i] == POINTER)
      fprintf(out, "#include\t\"./%s.h\"\n", field_type[i]);
  fprintf(out, "\n");  
}

void    declare    ()  {
  int   i;
  
  fprintf(out,"typedef\tstruct\t{");
  for (i=0;i<field_number;i++) {
    fprintf(out,"\n");
    switch  (type[i]) {
        case POINTER: {
        	fprintf(out, "\t%s\t*%s;", field_type[i], field_name[i]);
        	break; }
        case PRIMITIVE: {
	        fprintf(out, "\t%s\t%s;", field_type[i], field_name[i]);
        	break; }
        case STRING:   {
        	fprintf(out,"\tchar\t*%s;", field_name[i]);
        	break; }
    }
  }
  fprintf(out, "\n}\t%s;\n\n", struct_name);
}

void    prototypes () {
  int   i;

  fprintf(out, "/* accessors */\n");
  for (i=0; i<field_number;i++) {
    switch  (type[i]) {
    case POINTER: {
        fprintf(out, "void\tset_%s\t(%s*,%s*);\n",
                        field_name[i],
                        struct_name,
                        field_type[i]);
        fprintf(out, "%s*\tget_%s\t(%s);\n",
                        field_type[i],
                        field_name[i],
                        struct_name);
        break; }
    case PRIMITIVE: {
        fprintf(out, "void\tset_%s\t(%s*,%s);\n",
                        field_name[i],
                        struct_name,
                        field_type[i]);
        fprintf(out, "%s\tget_%s\t(%s);\n",
                        field_type[i],
                        field_name[i],
                        struct_name);
        break; }
    case STRING: {
        fprintf(out, "void\tset_%s\t(%s* a,char*);\n",
                        field_name[i],
                        struct_name);
        fprintf(out, "char*\tget_%s\t(%s);\n",
                        field_name[i],
                        struct_name);
        break; }
    }
  }
  fprintf(out, "/* destructor */\n");
  fprintf(out, "void\tdestroy_%s\t(%s*);\n",
            struct_name,
            struct_name);
  fprintf(out, "/* constructors */\n");
  fprintf(out, "%s*\tcreate_%s\t(",
    struct_name,
    struct_name);
  for (i = 0; i < field_number; i++) {
    switch  (type[i]) {
        case POINTER: {
          fprintf(out, "%s*",
                field_type[i]);
          break; }
        case PRIMITIVE: {
          fprintf(out, "%s", 
            field_type[i]);
          break; }
        case STRING: {
          fprintf(out, "char*");
          break; }
    }
    if (i == field_number - 1) fprintf(out, ");\n");
    else fprintf(out, ", ");
  }
  fprintf(out, "%s*\tclone_%s\t(%s);\n",
        struct_name,
        struct_name,
        struct_name);
  fprintf(out, "%s*\tload_%s\t(FILE*,char);\n",
        struct_name,
        struct_name);
  fprintf(out, "/* serializer */\n");
  fprintf(out, "void\tsave_%s\t(%s, FILE*, char, char);\n",
        struct_name,
        struct_name);
  fprintf(out,"/* others */\n");
  fprintf(out, "void\tshow_%s\t(%s);\n\n\n",
        struct_name,
        struct_name);
}

void    setty   (int i) {
  switch  (type[i]) {
    case POINTER: {
        fprintf(out, "void\tset_%s\t(%s *a,%s *_%s) {\n",
                        field_name[i],
                        struct_name,
                        field_type[i],
                        field_name[i]);
        fprintf(out, "\tdestroy_%s(a->%s);\n",
                        field_type[i],
                        field_name[i]);
        fprintf(out, "\ta->%s = clone_%s(*_%s); }\n\n",
                        field_name[i],
                        field_type[i],
                        field_name[i]);
        break; }
    case PRIMITIVE: {
        fprintf(out, "void\tset_%s\t(%s *a,%s _%s) {\n",
                        field_name[i],
                        struct_name,
                        field_type[i],
                        field_name[i]);
        fprintf(out, "\ta->%s = _%s; }\n\n",
                        field_name[i],
                        field_name[i]);
        break; }
    case STRING: {
        fprintf(out, "void\tset_%s\t(%s* a,char* _%s) {\n",
                        field_name[i],
                        struct_name,
                        field_name[i]);
        fprintf(out, "\tfree(a->%s);\n",
                        field_name[i]);
        fprintf(out, "\ta->%s=strdup(_%s); }\n\n",
                        field_name[i],
                        field_name[i]);
        break; }
  }
}

void    getty   (int i) {

    switch  (type[i])   {
        case POINTER: {
            fprintf(out, "%s*\tget_%s\t(%s a) {\n",
                        field_type[i],
                        field_name[i],
                        struct_name);
            fprintf(out, "\treturn clone_%s(*a.%s); }\n\n",
                        field_type[i],
                        field_name[i]);
            break; }
        case PRIMITIVE: {
            fprintf(out, "%s\tget_%s\t(%s a) {\n",
                        field_type[i],
                        field_name[i],
                        struct_name);
            fprintf(out, "\treturn (a.%s); }\n\n",
                        field_name[i]);
            break; }
        case STRING: {
            fprintf(out, "char*\tget_%s\t(%s a) {\n",
                        field_name[i],
                        struct_name);
            fprintf(out, "\treturn strdup(a.%s); }\n\n",
                        field_name[i]);
            break; }
    }
}

void    destroy ()  {
  int i;
  
  fprintf(out, "void\tdestroy_%s\t(%s *a) {\n",
            struct_name,
            struct_name);
  for (i = 0; i < field_number; i++) {
    switch  (type[i]) {
    case    POINTER: {
      fprintf(out, "\tdestroy_%s(a->%s);\n",
            field_type[i],
            field_name[i]);
      break; }
    case    PRIMITIVE: {
      break; }
    case    STRING: {
      fprintf(out, "\tfree(a->%s);\n",
            field_name[i]);
      break; }
    }
  }
  fprintf(out, "\tfree(a); }\n\n");
}

void    create  ()  {
  int i;
  
  fprintf(out, "%s*\tcreate_%s\t(",
    struct_name,
    struct_name);

  for (i = 0; i < field_number; i++) {
    switch  (type[i]) {
        case POINTER: {
	        fprintf(out, "%s* _%s",
                field_type[i],
                field_name[i]);
	        break; }
        case PRIMITIVE: {
	    fprintf(out, "%s _%s", 
            field_type[i],
            field_name[i]);
	    break; }
        case STRING: {
	    fprintf(out, "char* _%s",
            field_name[i]);
	    break; }
    }
    if (i == field_number - 1) fprintf(out, ") {\n");
    else fprintf(out, ", ");
  }
  
  fprintf(out, "\t%s* ret;\n\n", struct_name);
  fprintf(out, "\tret=(%s*)malloc(sizeof(%s));\n",
        struct_name,
        struct_name);
  
  for (i = 0; i < field_number; i++) {
    switch  (type[i]) {
        case POINTER: {
	        fprintf(out, "\tret->%s=clone_%s(*_%s);\n",
	              field_name[i],
	              field_type[i],
	              field_name[i]);
	        break; }
        case PRIMITIVE: {
	        fprintf(out, "\tret->%s=_%s;\n", 
                field_name[i],
                field_name[i]);
        	break; }
        case STRING: {
        	fprintf(out, "\tret->%s=strdup(_%s);\n",
                field_name[i],
                field_name[i]);
        	break; }
    }
  }
  fprintf(out, "\treturn (ret); }\n\n");
}

void    clone() {
  int i;
  
  fprintf(out, "%s*\tclone_%s\t(%s a) {\n",
        struct_name,
        struct_name,
        struct_name);
  fprintf(out, "\treturn(create_%s(", struct_name);
  
  for (i = 0; i < field_number; i++)   {
    fprintf(out, "a.%s", field_name[i]);
    if ( i == field_number - 1) fprintf(out, ")); }\n\n");
    else fprintf(out, ", ");
  }
}

void  load  ()  {
  int   flag_PRIMITIVE;
  char  format;
  int   i;
    
  fprintf(out, "%s*\tload_%s\t(FILE* fp,char separator) {\n",
        struct_name,
        struct_name);
  flag_PRIMITIVE=0;
  for (i = 0; i < field_number; i++) {
    switch  (type[i]) {
    case POINTER: {
	     fprintf(out, "\t%s* _%s;\n",
            field_type[i],
            field_name[i]);
	     break; }
    case PRIMITIVE: {
	     flag_PRIMITIVE++;
	     fprintf(out, "\t%s _%s;\n",
            field_type[i],
            field_name[i]);
	     break; }
    case STRING: {
	     fprintf(out, "\tchar* _%s;\n", 
            field_name[i]);
	     break; }
    }
  }
  if (flag_PRIMITIVE)
      fprintf(out, "\tchar* tmp;\n");
  
  fprintf(out, "\t%s* ret;\n\n", struct_name);
    
  for (i = 0; i < field_number; i++) {
    switch  (type[i]) {
    case POINTER: {
        fprintf(out, "\t_%s=load_%s(fp,separator);\n",
            field_name[i],
            field_type[i]);
        if (i == 0) fprintf(out, "\tif(_%s==NULL) return NULL;\n", field_name[i]);
        break; }
    case PRIMITIVE: {
        fprintf(out, "\ttmp=read_field_from_csv(fp,separator);\n");
        if (i==0) fprintf(out,"\tif (tmp == NULL) return NULL;\n");
        
        format = 'd';
        if (strcmp("float", field_type[i]) == 0)  format = 'f';
        if (strcmp("char", field_type[i]) == 0)   format = 'c';

        fprintf(out, "\tsscanf(tmp,\"%%%c\",&_%s);\n", format, field_name[i]);
        fprintf(out, "\tfree(tmp);\n");
    	break; }
    case STRING: {
      fprintf(out, "\t_%s=read_field_from_csv(fp,separator);\n", field_name[i]);
      if (i == 0) fprintf(out, "\tif(_%s==NULL) return NULL;\n", field_name[i]);
      break; }
    }
  }
    
  fprintf(out, "\tret=create_%s(", struct_name);
  
  for ( i = 0; i < field_number; i++) {
    fprintf(out, "_%s", field_name[i]);
    if ( i == field_number - 1 ) fprintf(out, ");\n");
    else fprintf(out, ", ");
  }
  
  for ( i = 0; i < field_number; i++) {
    switch  (type[i]) {
    case POINTER: {
      fprintf(out, "\tdestroy_%s(_%s);\n", 
            field_type[i],
            field_name[i]);
      break; }
    case PRIMITIVE: {
      break; }
    case STRING: {
      fprintf(out, "\tfree(_%s);\n", field_name[i]);
      break; }
    }  
  }
  fprintf(out, "\n\treturn (ret); }\n\n");
}

void  save  ()  {
  int i;
  char format; 
  
  fprintf(out, "void\tsave_%s\t(%s a,FILE* fp,char separator,char terminator) {",
    struct_name,
    struct_name);
  for ( i = 0; i < field_number; i++) {
    fprintf(out,"\n");
    switch  (type[i]) {
    case POINTER: {
      fprintf(out, "\tsave_%s(*a.%s,fp,separator,%s);",
        field_type[i],
        field_name[i],
        (i<field_number-1)? "separator" : "terminator");
      break; }
    case PRIMITIVE: {
      format = 'd';
      if (strcmp("float", field_type[i]) == 0) format = 'f';
      else if (strcmp("char", field_type[i]) == 0) format = 'c';

      fprintf(out,"\tfprintf(fp,\"%%%c%%c\",a.%s,%s);",
        format, 
        field_name[i],
        (i<field_number-1)? "separator" : "terminator");
      break; }
    case STRING: {
		  fprintf(out, "\tfprintf(fp,\"%%s%%c\",a.%s);", 
        field_name[i],
        (i<field_number-1)? "separator" : "terminator");
		  break; }
    }
  }
  fprintf(out, " }\n\n");  
}

void show() {
  int i;
  char format; 
  
  fprintf(out, "void\tshow_%s\t(%s a) {\n",
    struct_name,
    struct_name);
  for ( i = 0; i < field_number; i++) {
    fprintf(out, "\tprintf(\"%s:", field_name[i]);
    switch  (type[i]) {
    case POINTER: {
      fprintf(out, "\");\n");
      fprintf(out, "\tshow_%s(*a.%s);\n", 
        field_type[i],
        field_name[i]);
      break; }
    case PRIMITIVE: {
      format = 'd';
      if (strcmp("float", field_type[i]) == 0) format = 'f';
      else if (strcmp("char", field_type[i]) == 0) format = 'c';
      fprintf(out,"%%%c\\n\",a.%s);\n", 
        format, 
        field_name[i]);
      break; }
    case STRING: {
      fprintf(out, "%%s\\n\",a.%s);\n", field_name[i]);
      break; }
    }
  }
  fprintf(out, "\tprintf(\"\\n\"); }\n\n");  
}

void footer() {
  int   len, i;
  
  len = strlen(struct_name_upper);
  
  fprintf(out, "#endif /* %s_H */", struct_name_upper);
  free(struct_name_upper);
}