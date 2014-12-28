#ifndef	TOOLKIT_H
#define	TOOLKIT_H

#include	<stdarg.h>	/* for the Ellipsis  */
#include	<string.h>	/* for strdup() */
 
#define		MAX_STR	4096
/* clean a string from characters */
char	*clean_string	(char	*s, int n_param, ...)	{
	char	*temp, *saved;
	int 	found, index;
	va_list parameter_list;

	va_start(parameter_list, n_param);

	for ( saved = temp = s; *temp; temp++) {
		va_start(parameter_list, n_param);
		for ( found = 0, index = n_param; index; index--)
			if (va_arg(parameter_list, int) == *temp)	found = 1;
		if (found == 0)	{
			*s = *temp;
			s++;
		} 
	}	
	*s = '\0';
	return (saved);
}

/*	Reads chars from afile until EOF, CR or a separator.
	If the separator is '\n' it reads a complete line.
	if it doesnt find anything, returns NULL.
	WARNING: returns a dynamically allocated string 
				(watch out for memory leaks)	*/
char	*read_field_from_csv	(FILE 	*fp, char sep)	{
	char	buffer	[MAX_STR + 1];
	int 	i;	/* on buffer[] */

	for (	i = 0; 
			(buffer[i] = fgetc(fp)) != EOF && buffer[i] != '\n' && buffer[i]!= sep; 
			i++	);
	if (i == 0)	return NULL;
	buffer[i]='\0';
	return (strdup(buffer)); 
}

/* overwrite string to uppercase */
char	*uppercase   (char *s)   {
    int i; /* on s */
    
    for (i=0; *(s+i); i++)
        if (*(s+i)>='a' && *(s+i)<='z')		*(s+i)+='A'-'a';
    return  (s);
}

#endif	/* TOOLKIT_H */