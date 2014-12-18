#ifndef	STUDENT_H
#define	STUDENT_H

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"./toolkit.h"
#include	"./Data.h"

typedef	struct	{
	char	*name;
	int	age;
	Data	*birth_date;
}	Student;

/* accessors */
void	set_name	(Student* a,char*);
char*	get_name	(Student);
void	set_age	(Student*,int);
int	get_age	(Student);
void	set_birth_date	(Student*,Data*);
Data*	get_birth_date	(Student);
/* destructor */
void	destroy_Student	(Student*);
/* constructors */
Student*	create_Student	(char*, int, Data*);
Student*	clone_Student	(Student);
Student*	load_Student	(FILE*,char);
/* serializator */
void	save_Student	(Student, FILE*, char, char);
/* others */
void	show_Student	(Student);


Student*	create_Student	(char* _name, int _age, Data* _birth_date) {
	Student* ret;

	ret=(Student*)malloc(sizeof(Student));
	ret->name=strdup(_name);
	ret->age=_age;
	ret->birth_date=clone_Data(*_birth_date);
	return (ret); }

Student*	clone_Student	(Student a) {
	return(create_Student(a.name, a.age, a.birth_date)); }

void	destroy_Student	(Student *a) {
	free(a->name);
	destroy_Data(a->birth_date);
	free(a); }

char*	get_name	(Student a) {
	return strdup(a.name); }

int	get_age	(Student a) {
	return (a.age); }

Data*	get_birth_date	(Student a) {
	return clone_Data(*a.birth_date); }

void	set_name	(Student* a,char* _name) {
	free(a->name);
	a->name=strdup(_name); }

void	set_age	(Student *a,int _age) {
	a->age = _age; }

void	set_birth_date	(Student *a,Data *_birth_date) {
	destroy_Data(a->birth_date);
	a->birth_date = clone_Data(*_birth_date); }

Student*	load_Student	(FILE* fp,char separator) {
	char* _name;
	int _age;
	Data* _birth_date;
	char* tmp;
	Student* ret;

	_name=read_field_from_csv(fp,separator);
	if(_name==NULL) return NULL;
	tmp=read_field_from_csv(fp,separator);
	sscanf(tmp,"%d",&_age);
	free(tmp);
	_birth_date=load_Data(fp,separator);
	ret=create_Student(_name, _age, _birth_date);
	free(_name);
	destroy_Data(_birth_date);

	return (ret); }

void	save_Student	(Student a,FILE* fp,char separator,char terminator) {
	fprintf(fp,"%s%c",a.name);
	fprintf(fp,"%d%c",a.age,separator);
	save_Data(*a.birth_date,fp,separator,terminator); }

void	show_Student	(Student a) {
	printf("name:%s\n",a.name);
	printf("age:%d\n",a.age);
	printf("birth_date:");
	show_Data(*a.birth_date);
	printf("\n"); }

#endif /* STUDENT_H */