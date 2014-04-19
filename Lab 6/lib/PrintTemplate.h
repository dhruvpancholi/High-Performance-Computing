#include <iostream>
#include <typeinfo>
#include <cstdlib>

#include <stdio.h> 
#include <string.h>
#include <cmath>

using namespace std;

// Function Definition(s)
//template <class any_data_type2> void print(any_data_type2 *A, int Row, int Width, string str = "ans");

int terminal_command_numout(char s[]) //SRC: http://www.lainoox.com/how-to-execute-shell-commands-c/
{
	int DATA_SIZE = 512;
			
	FILE *pf = popen(s,"r"); ;
    char data[512];

	// Error-check  
    if(!pf){
      fprintf(stderr, "Could not open pipe for output.\n");
      return -1;
    }
 
    // Grab data from process execution
    fgets(data, DATA_SIZE , pf);
     
	// Error-check  
    if (pclose(pf) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");
        
    return atoi(data)-5; // -5 because when it was seen that at when the last character is at the rightmost position, the print isn't "right".
    					 // Thus, few spaces are lessend.
}	

// function template declaration and definition
template <class any_data_type1>
int char_space(any_data_type1 *A1, int NumEle)
{
	any_data_type1 arr_min =A1[0];
	any_data_type1 arr_max =A1[0];
	for(int i=0;i<NumEle;i++)
	{
		if(arr_min>A1[i])
			arr_min=A1[i];
		if(arr_max<A1[i])
			arr_max=A1[i];
	}
	
	int min_chars = (int) ceil(log10(abs(arr_min)));
	int max_chars = (int) ceil(log10(abs(arr_max)));
	
	if(arr_min<0)
		 min_chars += 1;
	if(arr_max<0)
		 max_chars += 1;

	return max(min_chars,max_chars);
}

template <class any_data_type2>
void print(any_data_type2 *A, int Row, int Width, string str = "ans")
{	
	//NOTE : Though there won't be any syntatical errors for decimal places within the above specified range, but even in that range if the number has big number 		//       before the decimal point and the number after the decimal point is a smaller one, then the printed number would differ from the initialized value.
	
	char term_command_tcols[] ="tput cols";
	int term_rowsize = terminal_command_numout(term_command_tcols);
	int char_space_each_element;
	int nb,na,numcols,extval;
	
	const char *c1 = typeid(any_data_type2).name();
	switch(c1[0])
	{
		case 'i':
			extval=abs(A[0]);	
			for(int i=0;i<Row*Width;i++)
			{
				if(extval<abs(A[i]))
					extval=abs(A[i]);
			}
			if(ceil(log10(extval))>9)
			{
				fprintf(stderr, "Printing Error : Max. or Min. of this int array to be printed exceeds character space limit.\n");
				return;
			}				
			
			na=0;
			nb = char_space(A,Row*Width);
			char_space_each_element = na + nb + 2;
			numcols = term_rowsize/char_space_each_element;	
			
			break;
		case 'f': case 'd':
			na=4;
			nb = char_space(A,Row*Width);
			char_space_each_element = na + nb + 1 + 2;
			numcols = term_rowsize/char_space_each_element;	
			
			if(char_space_each_element>16)
			{
				fprintf(stderr, "Printing Error : Max. or Min. of the array needs to be at most 9 digits.\n");
				return;
			}		
					
			break;
		default:
			cout << "Not Int Datatype" << endl;
	}
	
//	// DEBUG OUPUTS
//	cout  << "term_rowsize = " << term_rowsize << endl;
//	cout << "na = " << na << endl;
//	cout << "Char Space = " << nb << endl;
//	printf("Char space reqd for single number = %d\n", char_space_each_element);
//	cout << "numcols = " << numcols << endl;	

	cout << str << " =";
	//printf("%s =",str);
	
	char fstr[nb+na+2]; // [f]ormatted [s]tring
	
	for(int blockID = 0;blockID<Width/numcols;blockID++)
	{
		printf("\n Columns %d through %d\n\n",blockID*numcols,blockID*numcols+(numcols-1));
		for(int j=0;j<Row;j++)
		{
			cout << "  ";
			for(int i=0;i<numcols;i++)
			{
				formatstr1(A[j*Width+blockID*numcols + i],fstr,nb,na);
				cout << fstr << "  ";
			}
			cout << endl;
		}
	}	
	
	if(Width%numcols!=0)
	{
		int cols_covered_sofar = (Width/numcols)*numcols; 	
		if(Width%numcols==1)	
			printf("\n Column %d\n\n",cols_covered_sofar);
		else
			printf("\n Columns %d through %d\n\n",cols_covered_sofar,Width-1);
		for(int j=0;j<Row;j++)
		{
			cout << "  ";
			for(int i=cols_covered_sofar;i<Width;i++)
			{
				formatstr1(A[j*Width+i],fstr,nb,na);
				cout << fstr << "  ";
			}
			cout << endl;
		}
	}
	cout << endl;
}

void formatstr1(double num, char formatted_string[],int num_before_decimal_places,int num_after_decimal_places)
{
	bool sgn_num=true;
	if(num<0)
		sgn_num=false;

	num=abs(num);
		
	// NOTE ***************************************
	// num_before_decimal_places => Not more than 8
	// num_after_decimal_places => Not more than 4	
	
	// PART1: Get the decimal point and after places

	char decimal_andafter_places_string[10];
	int sign_after_deci_num = (int) pow(10,num_after_decimal_places)*(num - ((int) num));	// Get the after decimal point places as an int
	char zeros_supplier_str[9]="00000000";
	
	if(sign_after_deci_num!=0)
	{
		int num_zeros = (num_after_decimal_places-1) - floor(log10(sign_after_deci_num));	

		char significant_after_decimal_places_str[20];
		sprintf(significant_after_decimal_places_str, "%d", sign_after_deci_num);

		char zeros_str[9];
		strcpy(zeros_str,zeros_supplier_str);
		zeros_str[num_zeros] = '\0';

		strcpy (decimal_andafter_places_string,".");
		strcat (decimal_andafter_places_string,zeros_str);
		strcat (decimal_andafter_places_string,significant_after_decimal_places_str);
	}
	else
	{
		strcpy (decimal_andafter_places_string,".");
		strcat(decimal_andafter_places_string,zeros_supplier_str);
		decimal_andafter_places_string[num_after_decimal_places+1] = '\0'; 
	}
	
	if(strcmp(decimal_andafter_places_string,".")==0)	
		strcpy(decimal_andafter_places_string,"");
		
	// PART2: Get the Before Decimal places

	char before_decimal_places_string[10];
	int sign_before_deci_num = floor(num); 	// Get the before decimal point places as an int	
	char spaces_supplier_str[9] = "        ";

	if(sign_before_deci_num!=0)
	{ 
		int num_spaces = (num_before_decimal_places-1) -floor(log10(sign_before_deci_num));	

		char significant_before_decimal_places_str[10];
		
		if(!sgn_num)
		{
			sign_before_deci_num = -sign_before_deci_num;
			num_spaces -= 1;
		}
			
		sprintf(significant_before_decimal_places_str, "%d",sign_before_deci_num);
		
		spaces_supplier_str[num_spaces] = '\0';
	
		strcpy (before_decimal_places_string,spaces_supplier_str);
		strcat (before_decimal_places_string,significant_before_decimal_places_str);
		
	}
	else
	{
		strcpy (before_decimal_places_string,spaces_supplier_str);
		
		if(!sgn_num)
			before_decimal_places_string[num_before_decimal_places-2]='-';
			
		before_decimal_places_string[num_before_decimal_places-1]='0';
		before_decimal_places_string[num_before_decimal_places]='\0';
	}
		
	strcat (before_decimal_places_string,decimal_andafter_places_string);
	before_decimal_places_string[num_before_decimal_places+ num_after_decimal_places+1] = '\0';
	
	strcpy (formatted_string,before_decimal_places_string);

	return;
}

