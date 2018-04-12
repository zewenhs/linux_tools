//
//  main.cpp
//  usbpc
//
//  Created by apps on 12/3/15.
//  Copyright © 2015 apps. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "usbprn.h"
#include "cmdfunc.h"
#include <libusb-1.0/libusb.h>
#include "telink_usb.h"

#if 1

#define CMDS_MAX_ARGC 		64
#define CMDS_MAX_ARG_LENGTH 	36
#define CMDS_MAX_CMD_LENGTH 	36

int CMDS_Exec (char * cmdline);
int CMDS_Exec_Single (char * cmdline);

int bPCReadFile;
char cmdbuf[16384];
//LPBYTE lpBIBuff;
int bi_wptr;
int bi_rptr;
int bi_enable;
int bi_wptr_update;
extern libusb_device_handle *m_hDev;
extern libusb_context *ctx;

#define BIBUFFSIZE    1024*1024*2
//#define BIBUFFSIZE    1024

int uart_mode;

#if 1
int MainTask (char * str)
{
    bi_wptr = 0;
    bi_rptr = 0;
    bi_enable = 0;

    Cmd_Func_Init ();
    uart_mode = 0;

    strcpy (cmdbuf, str);


    if (strlen (cmdbuf) > 0) {
        if (! CMDS_Exec(cmdbuf) ) return 0;
    }
    else
        CMDS_Exec_Single ("nocmd");
    Cmd_Func_Close ();

    return 1;
}

int CMDS_Exec (char * cmdline)
{

    char* pstr = cmdline;

    int ret = 1;
    int stop = 0;
    char cmdbuf[1024] = "nocmd ";

    do {
        while ((*pstr==' ' || *pstr=='\t' || *pstr==';' ||
                *pstr=='\r' || *pstr=='\n') && * pstr != '\0') pstr++;
        char * ps = pstr;
        while (*pstr!=';' && *pstr != '\0') pstr++;

        if (*pstr == '\0') stop = 1;

        if (*ps!='\0') {
            *pstr='\0';
            if (*ps == '-') {
                strcpy (cmdbuf+6, ps);
                ret = CMDS_Exec_Single (cmdbuf);
            }
            else
                ret = CMDS_Exec_Single (ps);
            if (stop)	return ret;
            else		pstr++;
        }
        else {
            return 1;
        }
    } while (ret);

    return 0;
}

int CMDS_Exec_Single (char * cmdline)
{

    int i;
    int ret;
    int argc = 0;
    char* argv[CMDS_MAX_ARGC];
    char* pstr = cmdline;
    char * pStrCmd;

    for (i=0; i<CMDS_MAX_ARGC; i++) argv[i] = cmdline + strlen (cmdline);

    // skip leading blanks
    while (*pstr == ' ' && * pstr != '\0')	pstr++;
    pStrCmd = pstr;
    int loop = 1;

    while (loop)	{
        if (argc>0 && *pstr == '"') {
            argv[argc-1]++;
            pstr++;
            while (*pstr != '"' && *pstr != '\0')	pstr++;
        }
        else {
            while (*pstr != ' ' && *pstr != '\0')	pstr++;
        }
        if (*pstr) {
            *pstr++ = '\0';
            while (*pstr == ' ') pstr++;
            argv[argc++] = pstr;
        }
        else
            loop = 0;
    }

    if ((argc > 0) && !(*argv[argc-1]))
        argc--;

    if (strcmp(pStrCmd, "") != 0)  {
       {
            ret = Cmd_Process (pStrCmd, argc, argv);
        }
    }

    return ret;
}
#endif

#endif

int TL_Dut_cmd_Process(libusb_device_handle *hDev, TL_DutcmdTypdef cmd, TL_ModeTypdef Mode, TL_ChipTypdef Type, unsigned long int p1,unsigned long int p2)
{
    unsigned char dut_cmd_buff[10]={0};
    unsigned char clear_buff[4]={0};
    long int t;
	struct timeval ts, te;
    unsigned int timeout_ms = 2000;
    memset(dut_cmd_buff,0,10);
    memset(clear_buff,0,4);

    dut_cmd_buff[0]= cmd;
    dut_cmd_buff[1]= p1&0xff;
    dut_cmd_buff[2]= (p1>>8)&0xff;
    dut_cmd_buff[3]= (p1>>16)&0xff;
    dut_cmd_buff[4]= (p1>>24)&0xff;
    dut_cmd_buff[5]= p2&0xff;
    dut_cmd_buff[6]= (p2>>8)&0xff;
    dut_cmd_buff[7]= (p2>>16)&0xff;
    dut_cmd_buff[8]= (p2>>24)&0xff;


	if(WriteMem(hDev,0x8004,clear_buff,3,USB)!=3)
	{
		printff(" Fail to Clear buff! \t\n");
		return 0;
	}
	if(WriteMem(hDev,0x8007,dut_cmd_buff,9,USB)!=9)
	{
		printff(" Fail to write buff! \t\n");
		return 0;
	}
	dut_cmd_buff[0]= cmd|0x80;
	if(WriteMem(hDev,0x8007,dut_cmd_buff,1,USB)!=1)
	{
		printff(" Fail to execute cmd! \t\n");
		return 0;
	}
	gettimeofday(&ts, NULL);
	while(dut_cmd_buff[0]&0x80)
	{
		ReadMem(hDev,0x8007,dut_cmd_buff,1,USB);
		sleep(1);
		gettimeofday(&te, NULL);
		t = (te.tv_sec - ts.tv_sec) * 1000 + (te.tv_usec - ts.tv_usec) /1000;
		if(t > timeout_ms)
		{
			printff(" Wait Flash ACK timeout: %d ms \t\n",timeout_ms);
			return 0;
		}
	}
	if(ReadMem(hDev,0x8004,dut_cmd_buff,4,USB)!=4)
	{
		return 0;
	}

	if((dut_cmd_buff[2]&0xff)!=cmd)
	{
		printff(" Wait Flash ACK Failure! \t\n");
		return 0;
	}
            
           
    if(cmd==TL_DUTCMD_FLASH_ASK)
    {
        int bin_buffer_adr = dut_cmd_buff[0] + dut_cmd_buff[1]*256;
        if((Type==CHIP_8255)||(Type==CHIP_8255_A2))
        {
            return (bin_buffer_adr+0x40000);
        }
        else
        {
            return bin_buffer_adr;
        }

    }
    return 1;
}


int MCU_Init(libusb_device_handle *hDev,int Type)
{
	unsigned char buffer[2]={0};
	unsigned char ram_buffer[1024*8]={0};
	signed long int size=0;
	FILE *fp = fopen("dut_5326_flash_v0002.bin","rb");
	if(NULL==fp){printf(" Fail to open file! \t\n");return 0;}
	fseek(fp,0L,SEEK_END);
    size=ftell(fp);
    fseek(fp,0,SEEK_SET);
    fread(ram_buffer,1,size,fp);
    fclose(fp);
    
    //*********    reset mcu    *********************
	buffer[0]=0x05;
	unsigned char i=0;
	while((buffer[1]&0x05)!=0x05)
	{	
		WriteMem(hDev, 0x602, buffer, 1,USB);
		ReadMem(hDev, 0x602, buffer+1, 1,USB);
		//printf("buffer:%d\n", buffer[1]); 
	   	i++;
	   	if(i>3){printf(" TC32 USB : USB Err! \t\n");return 1;}
	}
	printf(" TC32 USB : USB OK \t\n");
 //********************   Disable watch dog, Disable interrupt,download file to ram and start MCU   *********************
	buffer[0]=0x00;
	
	WriteMem(hDev, 0x622, buffer, 1,USB);
	WriteMem(hDev, 0x643, buffer, 1,USB);

	buffer[0]=0x40;
	WriteMem(hDev, 0x60c, buffer, 1,USB);
	WriteMem(hDev, 0x60d, buffer, 1,USB);

	buffer[0]=0xff;
   	WriteMem(hDev, 0x104, buffer, 1,USB);

   if(WriteMem(hDev,0x8000,ram_buffer,size,USB)!=size)
   {
	   printff("Write file to ram fail via USB! \t\n");
	   return 0;
   }
   #if 0
   if((Type==CHIP_8366)||(Type==CHIP_8368))
   {
	   buffer[0]=0x01;
	   WriteMem (hDev,0x8ff0,buffer,1,USB);
   }
   #endif
   buffer[0] = 0x88;
   int t1 = WriteMem(hDev,0x602,buffer,1,USB);
   if(t1 != 1)
   {
	   printff(" Fail to start MCU via USB! \t\n");
	   return 0;
   }
 return 1;
}

#define BIN_BUF_SIZE (1024*1024)
int telink_usb_download(libusb_device_handle *hDev, unsigned int adr, const char *file_path)
{
	int Type = 100;//for IC type, 预留
	//unsigned int Adr = 0x20000;
	struct timeval start, end;
	
	signed long int size=0;
    unsigned char bin_buffer[BIN_BUF_SIZE]={0};
    
    if(!MCU_Init(hDev,100)){return -1;}
    FILE *fp=NULL;
    fp = fopen(file_path,"rb");
    if(NULL==fp){printff( "Fail to open bin file! \t\n");fclose(fp);return -1;}
    fseek(fp,0L,SEEK_END);
    size=ftell(fp);
    fseek(fp,0,SEEK_SET);
    fread(bin_buffer,1,size,fp);
    fclose(fp);
    #if 1 
	{
		unsigned long int EraseSector_Num = (size%0x1000)? (1+(size/0x1000)):(size/0x1000);
		unsigned long int PageWrite_Num   = (size%0x100)? (1+(size/0x100)):(size/0x100);
		unsigned char Last_Bytes_Num  = size%0x100;
		unsigned int j;
		gettimeofday(&start, NULL);
		#if 1
		for(unsigned int i=0;i<(EraseSector_Num-1);i++)
		{
			if(TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_ERASE,USB,Type,adr+i*0x1000,4)!=0)
			{
				printff(" Flash Sector (4K) Erase at address %x \t\n",adr+i*0x1000);

				int ram_adr = TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_ASK,USB,Type,0,0);

				for(j=0;j<16;j++)
				{
					
					if((Type==CHIP_8255)||(Type==CHIP_8255_A2))
					{
						#if 0
						if(WriteMem2(hDev,ram_adr,bin_buffer+j*0x100+i*0x1000,256,USB)!=256)
					   {
							 printff("\n USB Download fail! \t\n");
							 return;
						}
						#endif
					}
					else
					{
						if(WriteMem(hDev,ram_adr,bin_buffer+j*0x100+i*0x1000,256,USB)!=256)
						{
							 printff("\n USB Download fail! \t\n");
							 return -1;
						}
					}

					if(TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_WRITE,USB,Type,adr+j*0x100+i*0x1000,256)!=0)
					{
						if(j%4==0) printff(" Flash Page Program at address %x \t\n",adr+j*0x100+i*0x1000);
					}
					else
					{
						printff(" Flash Page Program Error at address %x \t\n",adr+j*0x100+i*0x1000);
					}
				}
			}
			else
			{
				printff(" Flash Sector (4K) Erase Error at address %x \t\n",adr+i*0x1000);
			}
		}

		PageWrite_Num = PageWrite_Num - ((EraseSector_Num-1)*16);

		if(TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_ERASE,USB,Type,adr+(EraseSector_Num-1)*0x1000,4)!=0)
		{
			printff(" Flash Sector (4K) Erase at address %lx \t\n",adr+(EraseSector_Num-1)*0x1000);

			int ram_adr = TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_ASK,USB,Type,0,0);
			for(unsigned int i=0;i<PageWrite_Num;i++)
			{
				int Page_Bytes = ((i+1)==PageWrite_Num)? ((Last_Bytes_Num==0)? 256:Last_Bytes_Num):256;
				if((Type==CHIP_8255)||(Type==CHIP_8255_A2))
				{
					#if 0
					if(WriteMem2(hDev,ram_adr,bin_buffer+256*i+(EraseSector_Num-1)*0x1000,Page_Bytes,USB)!=Page_Bytes)
					{
						 printff("\n USB Download fail! \t\n");
						 return;
					}
					#endif
				}
				else
				{
					if(WriteMem(hDev,ram_adr,bin_buffer+256*i+(EraseSector_Num-1)*0x1000,Page_Bytes,USB)!=Page_Bytes)
					{
						 printff("\n USB Download fail! \t\n");
						 return -1;
					}
				}

				//ToDO
				if(TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_WRITE,USB,Type,adr+256*i+(EraseSector_Num-1)*0x1000,Page_Bytes)!=0)
				{
					if(i%4==0)printff(" Flash Page Program at address %x \t\n",(unsigned int)(i*256)+(unsigned int)(EraseSector_Num-1)*0x1000+adr);
				}
				else
				{
					printff(" Flash Page Program Error at address %x \t\n",(unsigned int)(i*256)+(unsigned int)(EraseSector_Num-1)*0x1000+adr);
					return -1;
				}
			}
		}
		else
		{
			printff(" Flash Sector (4K) Erase Error at address %lx \t\n",adr+(EraseSector_Num-1)*0x1000);
			return -1;
		}
		#else
		if(TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_ERASE,USB,Type,Adr,EraseSector_Num*4)!=0)
		{
			int ram_adr = TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_ASK,USB,Type,NULL,NULL);
			for(unsigned int i=0;i<PageWrite_Num;i++)
			{
				int Page_Bytes = ((i+1)==PageWrite_Num)? ((Last_Bytes_Num==0)? 256:Last_Bytes_Num):256;
				if((Type==CHIP_8255)||(Type==CHIP_8255_A2))
				{
					#if 0
					if(WriteMem2(hDev,ram_adr,bin_buffer+256*i,Page_Bytes,USB)!=Page_Bytes)
					{
						 printff("\n USB Download fail! \t\n");
						 return;
					}
					#endif
				}
				else
				{
					if(WriteMem(hDev,ram_adr,bin_buffer+256*i,Page_Bytes,USB)!=Page_Bytes)
					{
						 printff("\n USB Download fail! \t\n");
						 return;
					}
				}
				if(TL_Dut_cmd_Process(hDev,TL_DUTCMD_FLASH_WRITE,USB,Type,Adr+256*i,Page_Bytes)!=0)
				{
					if((i%16)==0)
					{
						printff(" Flash Sector (4K) Erase & Program at address %x \t\n",Adr+256*i);
					}
					else if((i%4)==0)
					{
						printff(" Flash Sector (4K) Program at address %x \t\n",Adr+256*i);
					}
				}
				else
				{
					if((i%16)==0)
					{
						printff(" Flash Sector (4K) Erase at address %x \t\n",Adr+256*i);
					}
					else if((i%4)==0)
					{
						printff(" Flash Sector Check Error at Addr %x \t\n",Adr+256*i);
					}
				}
			}
		}
		else
		{
			printff(" Erase Flash fail! \t\n");
			return;
		}
		#endif
		gettimeofday(&end, NULL);
		printff(" File Download to 0x%.6x: %ld bytes \t\n",adr,size);
		float time_use = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
		time_use /= 1000000;
		printff(" Total Time: %.2f s \t\n", time_use);
		
	}
#endif
	return 0;
}

#define EP_IN	0x88
#define EP_OUT	5
#define TIME_OUT	1000
#define TIME_OUT_2	2000

int telink_usb_action(libusb_device_handle *hDev, TL_CMDType cmd, unsigned char *data)
{
	unsigned char buf[10] = {0};
	int size = -10;
	int ret = -1;
	switch(cmd)
	{
		case SCAN_ON:
			buf[0] = 0xfa; buf[1]= 0x01; buf[2] = 0x01;
			ret = libusb_bulk_transfer(hDev, EP_OUT, buf, 3, &size, TIME_OUT);
			break;

		case SCAN_OFF:
			buf[0] = 0xfa; buf[1]= 0x01; buf[2] = 0x00;
			ret = libusb_bulk_transfer(hDev, EP_OUT, buf, 3, &size, TIME_OUT);
			break;

		case CONNECT:
			if(data == NULL)
			{
				printf("fatal error!!! NULL pointer!!!\n");
				return -1;
			}
			buf[0] = 0xfa; buf[1] = 0x02;
			memcpy(buf + 2, data, 6);
			ret = libusb_bulk_transfer(hDev, EP_OUT, buf, 8, &size, TIME_OUT);
			break;

		case DISCONNECT:
			buf[0] = 0xfa; buf[1] = 0x03; buf[2] = 0x00;
			ret = libusb_bulk_transfer(hDev, EP_OUT, buf, 3, &size, TIME_OUT);
			break;

		case OTA:
			buf[0] = 0xfa; buf[1] = 0x04; buf[2] = 0x00;
			ret =libusb_bulk_transfer(hDev, EP_OUT, buf, 3, &size, TIME_OUT);
			break;

		case BAT_STATUS:
			buf[0] = 0xfa; buf[1] = 0x05; buf[2] = 0x00;
			ret = libusb_bulk_transfer(hDev, EP_OUT, buf, 3, &size, TIME_OUT);
			break;
	}

	return ret;
}

int telink_usb_get_data(libusb_device_handle *hDev, unsigned char *buf, int len, int *size)
{
	int ret = -1;
	ret = libusb_bulk_transfer(hDev, EP_IN, buf, len, size, TIME_OUT_2);
	
	return ret;
}

void telink_usb_close(libusb_device_handle *m_hDev)
{
	libusb_close(m_hDev); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the
}




