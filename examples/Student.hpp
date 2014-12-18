#ifndef	STUDENT_HPP
#define	STUDENT_HPP

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"./toolkit.h"
#include	"./Data.hpp"

class	Student	{
	private:
		char	*name;
		int	age;
		Data	*birth_date;
	public:
		/* accessors */
		void	set_name	(char*);
		char*	get_name	();
		void	set_age	(int);
		int	get_age	();
		void	set_birth_date	(Data*);
		Data*	get_birth_date	();
		/* destructor */
		~Student	();
		/* constructors */
		Student	(char*, int, Data*);
		Student	(const Student&);
		Student	(FILE*);
		/* serializator */
		void	save	( FILE*, char, char);
		/* others */
		void	show	();
};

Student :: Student	(char* _name, int _age, Data* _birth_date) {
	name=strdup(_name);
	age=_age;
	birth_date = new Data(*_birth_date);
}

Student :: Student	(const Student& a) {
	name = strdup(a.name);
	age = a.age;
	birth_date = new Data(*a.birth_date);
}

Student :: ~Student	() {
	free(name);
	delete(birth_date);
}

char*	Student :: get_name	() { return strdup(name); }

int	Student :: get_age	() { return (age); }

Data*	Student :: get_birth_date	() { return new Data(*birth_date); }

void	Student :: set_name	(char* _name) {
	free(name);
	name = strdup(_name); }

void	Student :: set_age	(int _age) {
	age = _age; }

void	Student :: set_birth_date	(Data *_birth_date) {
	delete(birth_date);
	birth_date = new Data(*_birth_date); }

Student :: Student	(FILE* fp,char separator) {
	char* tmp;
	name = read_field_from_csv(fp,separator);
	if(name==NULL) return;
	tmp = read_field_from_csv(fp,separator);
	sscanf(tmp,"%d",&age);
	free(tmp);
	birth_date = new Data(fp,separator);
}

void	Student :: save	(FILE* fp, char separator, char terminator) {
	fprintf(fp,"%s%c",name,separator);
	fprintf(fp,"%d%c",age,separator);
	birth_date->save(fp,separator,terminator); }

void	Student :: show	() {
	printf("name:%s\n",name);
	printf("age:%d\n",age);
	printf("birth_date:");
	birth_date->show();
	printf("\n"); }

#endif /* STUDENT_HPP */