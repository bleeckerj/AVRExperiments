 unsigned char del0; 
 unsigned char del1;
 unsigned char del2;  // for delay routines and other crap

  
void Delay (unsigned char del0)  //input in 1.4uS
{
  while(del0>0) del0--;
}

void DelaymS (unsigned char del2)  // input in mS at 8MHz
{
  while (del2>0)
  {
    del2--;
	del1=9;  // adjust here
	while(del1>0)
	{
	  del1--;
	  Delay(76); // and here
	}
  }
}

