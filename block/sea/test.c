int main()
{
#ifdef PRINT
	uart1_init();
	u16 j;
#endif
	u16 i;
	u16 state[2*NB];
	u16 mkey[2*NB];
	u16 rkey[NBROUND][2*NB];
	
	
	for(i=0;i<2*NB;i++) state[i]=i;
	for(i=0;i<2*NB;i++) mkey[i]=2*i;

#ifdef PRINT
	printf("----------Depart----------\n\n");
	for(j=1;j<=2*NB;j++) printf("%4X ",state[2*NB - j]);printf("\n\n");
	printf("----------Fin Depart----------\n\n");
	printf("----------Encryption----------\n\n");
#endif

	START_ENCRYPT();
	KeySchedul(mkey,rkey);
	Encrypt(state,rkey);
	
#ifdef PRINT
	for(j=1;j<=2*NB;j++) printf("%4X ",state[2*NB - j]);printf("\n\n");
	printf("----------Fin Encryption----------\n\n");
	printf("----------Decryption----------\n\n");
#endif
	START_DECRYPT();
	KeySchedul(mkey,rkey);
	Decrypt(state,rkey);
#ifdef PRINT
	for(j=1;j<=2*NB;j++) printf("%4X ",state[2*NB - j]);printf("\n\n");
	printf("----------Fin Decryption----------\n\n");
#endif
	END_EXPE();
	return 0;
}
