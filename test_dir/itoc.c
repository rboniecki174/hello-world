/************************************************************************************/
/*                                                                                  */
/* itoc.c       Convert Integer to Character                              R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   No arguments                                                                   */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        16/Apr/1999  Header added for revisions                  */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

void	itoc_(retval,retl,item,count)
int	*count;
int	retl;
int	*item;
char    *retval;
{
	char	tbuff[14];

	int	nb_len,
		sp_len;

	sprintf(tbuff,"%d",*item);

	nb_len = strlen(tbuff);
	sp_len = 14 - nb_len - 1;

	strncat(tbuff,"              ",sp_len);

	strncpy(retval,tbuff,retl);

	*count = nb_len;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
