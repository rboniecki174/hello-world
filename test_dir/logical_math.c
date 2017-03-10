/**********************************************************************/
/*                                                                    */
/* Logical Arithmatic Manipulation Functions                R00-00.00 */
/*                                                                    */
/**********************************************************************/

unsigned char  logical_set_(short      * value)
{
 unsigned char    result;

 result         = (unsigned char) (*value);
 return(result);

}

unsigned char  logical_add_(unsigned char * byte, short      * value)
{
  unsigned char   result;

  result         = (*byte) + (unsigned char) (*value);
  return(result);
}

unsigned char  logical_sub_(unsigned char * byte, short      * value)
{
  unsigned char   result;

  result         = (*byte) - (unsigned char) (*value);
  return(result);
}

short           logical_get_(unsigned char * byte)
{
  short            value;

  value        =  (*byte);
  return(value);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

