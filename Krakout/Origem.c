#include"pch.h"
#include<stdio.h>
#include<conio.h>


void main(void)
{
	int x, y, z;

	for (x = 0, y = 0, z = 0; z < 5000; x++, y++, z++)
	{
		gotoxy(x, y);
		printf("O");
		delay(100);
		clrscr();
	}
}