/*
** Mnemonic Table Scan for string
**
**  22 Nov 96  Pz	replacement for:
**			OS/32 SVC 2,17 Scan Mnemonic Table 
**  26 Mar 97  Cb       added parentheses to xor expr - (c ^ ' ')
**
** structure of a mnemonic table:
**  char table[] = {
**       0x80+'Z', 'e', 'r', 'o', 't', 'h', '\0',
**       0x80+'F', 0x80+'i', 'r', 's', 't', '\0',
**       0x80+'S', 0x80+'e', 's', 'o', 'n', 'd', '\0',
**       0x80+'L', 'a', 's', 't', '\0',
**       '\0'
**  };
**  where: table char entry is case insensitive,
**         table char MSBit => required character,
**         table entry ends with '\0',
**         table ends with '\0' entry
**
**  return: -1 => not found
**          0-index table entry
*/

#include <ctype.h>

#define EoS	(char)0
#define EoT	(char)0

int scan_(	/* Mnemonic Table Scan for string */
    char *table,	/* Mnemonic Table */
    char *string	/* Mnemonic String */
    )
{
    char c		/* Mnemonic Table  Character */
       , *s		/* Mnemonic String pointer */
       , *t;		/* Mnemonic Table  pointer */
    int  e;		/* Mnemonic Table  Entry */

    /*
    ** For each entry in the Mnemonic Table
    **     For each character in the Mnemonic Entry
    **      that (case insensitive) matches
    **         Skip the character
    **     If not required and not within word
    **         Return found
    **     Skip to next entry
    ** Return not found
    */

    /* For each entry in the Mnemonic Table */
    for (t = table, e = 0; *t != EoT; e++) {
	/* For each character in the Mnemonic Entry */
	for (s = string; (c = *t & 0x7F) != EoS; t++, s++)
	    /*  !( match  ||  not null && other case match) */
	    if (!((c == *s || isalpha(c)) && (c ^ ' ') == *s))    /* clb */
		break;
	/*  !(required|| within word) => found entry */
	if (!(c != *t || isalpha(*s))) return e;
	while (*t++ != EoS) ;	/* skip to next entry */
    }
    return -1;
}
