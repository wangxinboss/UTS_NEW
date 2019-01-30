#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>



int pdaf_init(char *filename, int& cap_num, int& output_size_max);
int pdaf_spc(unsigned short*raw, int& output_size1, unsigned char *output1);
int pdaf_dcc(char **raw_seq, int *lensPos, int& output_size2, unsigned char *output2);
int pdaf_dcc(char **raw_seq, int *lensPos, char* spctable,int& output_size2, unsigned char *output2);
int pdaf_generate_eeprom(int output_size1, unsigned char *output1, int output_size2, unsigned char *output2, int& eeprom_size, unsigned char *eeprom);

int pdaf_verify_eeprom(unsigned char *eeprom);
int pdaf_verify_spc(unsigned char *output1);
int pdaf_verify_dcc(unsigned char *output2);
int pdaf_get_DAC_dcc(unsigned char *output2, int& DAC);
int pdaf_get_DAC_dcc(unsigned short*raw,unsigned char *output1,unsigned char *output2,int currentdac ,int& DAC);

int pdaf_get_LinearitySize(unsigned char *output2, int& GridX,int& GridY);
int pdaf_get_Linearity(unsigned char *output2,double *LinearityBuf);

