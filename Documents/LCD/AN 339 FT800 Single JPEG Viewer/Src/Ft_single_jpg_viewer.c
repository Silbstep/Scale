/* FT_single_jpg_viewer v1.0 */
/* Sample application to load a single JPEG */
/*checks if the file is a baseline JPEG and if the dimensions will fit the screen*/

#include "FT_Platform.h"
#ifdef MSVC_PLATFORM
#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <math.h>
#include <windows.h>
#include <io.h>
#include <direct.h>
#include "FT_Gpu_Hal.h"
#endif

#define SAMAPP_DELAY_BTW_APIS (1000)
#define SAMAPP_ENABLE_DELAY() Ft_Gpu_Hal_Sleep(SAMAPP_DELAY_BTW_APIS)
#define SAMAPP_ENABLE_DELAY_VALUE(x) Ft_Gpu_Hal_Sleep(x)

/* Global variables for display resolution to support various display panels */
/* Default is WQVGA - 480x272 */
/* To select either WQVGA or QVGA (320x240) edit definition in FT_Platform.h*/
#ifdef SAMAPP_DISPLAY_WQVGA
ft_int16_t FT_DispWidth = 480;
ft_int16_t FT_DispHeight = 272;
ft_int16_t FT_DispHCycle =  548;
ft_int16_t FT_DispHOffset = 43;
ft_int16_t FT_DispHSync0 = 0;
ft_int16_t FT_DispHSync1 = 41;
ft_int16_t FT_DispVCycle = 292;
ft_int16_t FT_DispVOffset = 12;
ft_int16_t FT_DispVSync0 = 0;
ft_int16_t FT_DispVSync1 = 10;
ft_uint8_t FT_DispPCLK = 5;
ft_char8_t FT_DispSwizzle = 0;
ft_char8_t FT_DispPCLKPol = 1;
#endif

#ifdef SAMAPP_DISPLAY_QVGA	
/* Values specific to QVGA LCD display */
FT_DispWidth = 320;
FT_DispHeight = 240;
FT_DispHCycle =  408;
FT_DispHOffset = 70;
FT_DispHSync0 = 0;
FT_DispHSync1 = 10;
FT_DispVCycle = 263;
FT_DispVOffset = 13;
FT_DispVSync0 = 0;
FT_DispVSync1 = 2;
FT_DispPCLK = 8;
FT_DispSwizzle = 2;
FT_DispPCLKPol = 0;
#endif

ft_uint16_t image_height = 0;
ft_uint16_t image_width = 0;

/* Global used for buffer optimization */
Ft_Gpu_Hal_Context_t host,*phost;

ft_uint32_t Ft_CmdBuffer_Index;
ft_uint32_t Ft_DlBuffer_Index;

static ft_uint16_t nooffiles = 0;

#ifdef BUFFER_OPTIMIZATION
ft_uint8_t  Ft_DlBuffer[FT_DL_SIZE];
ft_uint8_t  Ft_CmdBuffer[FT_CMD_FIFO_SIZE];
#endif

//**************************************************************************
ft_void_t Ft_App_WrCoCmd_Buffer(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t cmd)
//**************************************************************************

{
#ifdef  BUFFER_OPTIMIZATION
   /* Copy the command instruction into buffer */
   ft_uint32_t *pBuffcmd;
   pBuffcmd =(ft_uint32_t*)&Ft_CmdBuffer[Ft_CmdBuffer_Index];
  
   *pBuffcmd = cmd;
#endif

   /* Increment the command index */
   Ft_CmdBuffer_Index += FT_CMD_SIZE;  

}

//**************************************************************************
ft_void_t Ft_App_WrCoCmd_Buffer2(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t cmd)
//**************************************************************************
{
#ifdef  BUFFER_OPTIMIZATION
   /* Copy the command instruction into buffer */
   ft_uint32_t *pBuffcmd;
   pBuffcmd =(ft_uint32_t*)&Ft_CmdBuffer[Ft_CmdBuffer_Index];
  
   *pBuffcmd = cmd;
#endif

   /* Increment the command index */
   Ft_CmdBuffer_Index += FT_CMD_SIZE;  
}

//**************************************************************************
ft_void_t Ft_App_WrDlCmd_Buffer(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t cmd)
//**************************************************************************
{
#ifdef BUFFER_OPTIMIZATION  
   /* Copy the command instruction into buffer */
   ft_uint32_t *pBuffcmd;
   pBuffcmd =(ft_uint32_t*)&Ft_DlBuffer[Ft_DlBuffer_Index];
   *pBuffcmd = cmd;
#endif

   /* Increment the command index */
   Ft_DlBuffer_Index += FT_CMD_SIZE;  
}

//******************************************************************************
ft_void_t Ft_App_WrCoStr_Buffer(Ft_Gpu_Hal_Context_t *phost,const ft_char8_t *s)
//******************************************************************************
{
#ifdef  BUFFER_OPTIMIZATION  
  ft_uint16_t length = 0;
  length = strlen(s) + 1;//last for the null termination
  
  strcpy(&Ft_CmdBuffer[Ft_CmdBuffer_Index],s);  


  /* increment the length and align it by 4 bytes */
  Ft_CmdBuffer_Index += ((length + 3) & ~3);  
#endif  
}

//***********************************************************
ft_void_t Ft_App_Flush_DL_Buffer(Ft_Gpu_Hal_Context_t *phost)
//***********************************************************
{
#ifdef  BUFFER_OPTIMIZATION    
   if (Ft_DlBuffer_Index> 0)
     Ft_Gpu_Hal_WrMem(phost,RAM_DL,Ft_DlBuffer,Ft_DlBuffer_Index);
#endif     
   Ft_DlBuffer_Index = 0;
   
}

//***********************************************************
ft_void_t Ft_App_Flush_Co_Buffer(Ft_Gpu_Hal_Context_t *phost)
//***********************************************************
{
#ifdef  BUFFER_OPTIMIZATION
	
   if (Ft_CmdBuffer_Index > 0)
     Ft_Gpu_Hal_WrCmdBuf(phost,Ft_CmdBuffer,Ft_CmdBuffer_Index);
#endif     
   Ft_CmdBuffer_Index = 0;
}

/* API to check the status of previous DLSWAP and perform DLSWAP of new DL */
/* Check for the status of previous DLSWAP and if still not done wait for few ms and check again */
//**************************************************
ft_void_t SAMAPP_GPU_DLSwap(ft_uint8_t DL_Swap_Type)
//**************************************************
{
	ft_uint8_t Swap_Type = DLSWAP_FRAME,Swap_Done = DLSWAP_FRAME;

	if(DL_Swap_Type == DLSWAP_LINE)
	{
		Swap_Type = DLSWAP_LINE;
	}

	/* Perform a new DL swap */
	Ft_Gpu_Hal_Wr8(phost,REG_DLSWAP,Swap_Type);

	/* Wait till the swap is done */
	while(Swap_Done)
	{
		Swap_Done = Ft_Gpu_Hal_Rd8(phost,REG_DLSWAP);

		if(DLSWAP_DONE != Swap_Done)
		{
			Ft_Gpu_Hal_Sleep(10);//wait for 10ms
		}
	}	
}

//*************************
ft_void_t Ft_BootupConfig()
//*************************
{	
	/* Do a power cycle */	
	Ft_Gpu_Hal_Powercycle(phost,FT_TRUE);

	Ft_Gpu_Hal_Rd16(phost,RAM_G);

	/* Set the clk to external clock */
	Ft_Gpu_HostCommand(phost,FT_GPU_EXTERNAL_OSC);  
	
	Ft_Gpu_Hal_Sleep(10);
	  
	/* Switch PLL output to 48MHz */
	Ft_Gpu_HostCommand(phost,FT_GPU_PLL_48M);  
	Ft_Gpu_Hal_Sleep(10);

	/* Do a core reset */
	Ft_Gpu_HostCommand(phost,FT_GPU_CORE_RESET);     

	/* Access address 0 to wake up the FT800 */
	Ft_Gpu_HostCommand(phost,FT_GPU_ACTIVE_M);  
    Ft_Gpu_Hal_Wr8(phost, REG_GPIO_DIR,0x80 | Ft_Gpu_Hal_Rd8(phost,REG_GPIO_DIR));
    Ft_Gpu_Hal_Wr8(phost, REG_GPIO,0x080 | Ft_Gpu_Hal_Rd8(phost,REG_GPIO));
	
	{
		ft_uint8_t chipid;
		//Read Register ID to check if FT800 is ready. 
		chipid = Ft_Gpu_Hal_Rd8(phost, REG_ID);
		while(chipid != 0x7C)
			chipid = Ft_Gpu_Hal_Rd8(phost, REG_ID);
#ifdef MSVC_PLATFORM
		printf("VC1 register ID after wake up %x\n",chipid);
#endif
	}

	//*********write display setup parameters 
    Ft_Gpu_Hal_Wr16(phost, REG_HCYCLE, FT_DispHCycle);
    Ft_Gpu_Hal_Wr16(phost, REG_HOFFSET, FT_DispHOffset);
    Ft_Gpu_Hal_Wr16(phost, REG_HSYNC0, FT_DispHSync0);
    Ft_Gpu_Hal_Wr16(phost, REG_HSYNC1, FT_DispHSync1);
    Ft_Gpu_Hal_Wr16(phost, REG_VCYCLE, FT_DispVCycle);
    Ft_Gpu_Hal_Wr16(phost, REG_VOFFSET, FT_DispVOffset);
    Ft_Gpu_Hal_Wr16(phost, REG_VSYNC0, FT_DispVSync0);
    Ft_Gpu_Hal_Wr16(phost, REG_VSYNC1, FT_DispVSync1);
    Ft_Gpu_Hal_Wr8(phost, REG_SWIZZLE, FT_DispSwizzle);
    Ft_Gpu_Hal_Wr8(phost, REG_PCLK_POL, FT_DispPCLKPol);
    Ft_Gpu_Hal_Wr8(phost, REG_PCLK,FT_DispPCLK);//after this display is visible on the LCD
    Ft_Gpu_Hal_Wr16(phost, REG_HSIZE, FT_DispWidth);
    Ft_Gpu_Hal_Wr16(phost, REG_VSIZE, FT_DispHeight);

}


/* Boot up for FT800 */
/* Initial boot up DL */
const ft_uint8_t FT_DLCODE_BOOTUP[12] = 
{
    0,0,0,2,//GPU instruction CLEAR_COLOR_RGB
	
    7,0,0,38, //GPU instruction CLEAR
    0,0,0,0,  //GPU instruction DISPLAY
};

typedef struct 
{
	  char name[256];  //filename buffer
	  ft_uint32_t file_size;
}Image;
Image Image_prop[128];  

static ft_uint16_t cts=0;

FILE *pfile;
FILE *fp2;
static ft_uint32_t filesize;
static ft_uint32_t filesize2;
static ft_uint8_t home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};

//*******************************
  ft_uint16_t Ft_List_Out_files()
//*******************************
// Find *.jpg file & ensure file size is a multiple of 4 bytes
{ 	
	//ft_uint16_t nooffiles=0;
    char *path = "..\\..\\..\\Test";  //Look for *.jpg files in Test directory
	struct _finddata_t Files;
	long file_spec;	
	if (!_chdir(path))
	{
		if ((file_spec = _findfirst("*.jp*", &Files)) == -1L)
			{
			return nooffiles;
		}
		else
		{
			ft_strcpy_P(Image_prop[nooffiles].name, Files.name);
			//ensure file size is a multiple of 4 bytes
			Image_prop[nooffiles].file_size = (Files.size + 3)&~3; 
			nooffiles++;
		}
	}
	return nooffiles;
}

//*********************
  ft_void_t Load_JPEG()
//*********************

{
	ft_uint8_t imbuff[8192];  
	ft_uint16_t blocklen;
	ft_uint8_t error_code = 0;
	ft_uint16_t xcoord;
	ft_uint16_t ycoord;

	ft_uint8_t markerID[2];   
	ft_uint8_t marker_info[8];
	ft_uint8_t marker_next_byte[8];
	ft_uint16_t size[8] = { 0 };

	//read first 2 bytes to check for JPEG SOI marker 0xFFD8
	    fseek(pfile, 0, SEEK_SET);  // Beginning of file
		fread(&markerID, 1, 2, pfile); //read first 2 bytes
		
		if (((markerID[0] << 8) + markerID[1]) != 0xFFD8) 
		{ 
		error_code = 1; //No SOI marker (0xFFD8) detected			
	    }
		else
			{
			while (fread(&markerID, 1, 1, pfile) > 0)  
			{
				if (markerID[0] == 0xFF)
				{
					fread(&marker_next_byte, 1, 1, pfile);
					if (marker_next_byte[0] >> 4 == 0xE){  //APPn marker found if true
						fread(&marker_info, 1, 2, pfile); //read APPn marker length
						fseek(pfile, ((marker_info[0]*256) + marker_info[1]-2), SEEK_CUR); //skip APPn marker
					    }				
					if (marker_next_byte[0] == 0xC0){  //baseline JPEG marker found
						fread(&marker_info, 1, 8, pfile); //read next 8 bytes & extract height & width
						image_height = marker_info[3]*256 + marker_info[4];
						image_width = marker_info[5]*256 + marker_info[6];
						printf("image_height = %d\n", image_height);
						if (image_height > FT_DispHeight) error_code = 2; //Max is 272 for WQVGA, 240 for QVGA
						printf("image_width = %d\n", image_width);
						if (image_width > FT_DispWidth) error_code = 2;	//Max is 480 for WQVGA, 320 for QVGA					
					    } 		
					if (marker_next_byte[0] == 0xC2){  //check if progressive JPEG
						error_code = 3;
				}									
			}
			} 													
}
		if (error_code != 0) { //error flagged, display message
			fclose(pfile);	/* close the opened JPEG file */
			Ft_CmdBuffer_Index = 0;
			Ft_Gpu_CoCmd_Dlstart(phost);
			Ft_App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
			Ft_App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
			if (error_code == 1) Ft_Gpu_CoCmd_Text(phost, FT_DispWidth / 2, FT_DispHeight / 2, 26, OPT_CENTERX | OPT_CENTERY, "Error: Selected file is not a JPEG!");
			if (error_code == 2) Ft_Gpu_CoCmd_Text(phost, FT_DispWidth / 2, FT_DispHeight / 2, 26, OPT_CENTERX | OPT_CENTERY, "Error: Image size is not compatible!");
			if (error_code == 3) Ft_Gpu_CoCmd_Text(phost, FT_DispWidth / 2, FT_DispHeight / 2, 26, OPT_CENTERX | OPT_CENTERY, "Error: Selected file is a progressive JPEG!");
			Ft_App_WrCoCmd_Buffer(phost, DISPLAY());
			Ft_Gpu_CoCmd_Swap(phost);
			Ft_App_Flush_Co_Buffer(phost);
			Ft_Gpu_Hal_WaitCmdfifo_empty(phost);
			}
		else
		{
			//no error, therefore load image into FT800 via command processor
			Ft_Gpu_Hal_WrCmd32(phost, CMD_LOADIMAGE); //Load a JPEG image via command processor
			Ft_Gpu_Hal_WrCmd32(phost, 0L); //Destination starting address in Graphics RAM
    		Ft_Gpu_Hal_WrCmd32(phost, 0); //Options 0 for RGB565, 1 for L8 monochrome, 
			//2 for RGB565 with no display list commands, 3 for L8 with no display list commands		
			fseek(pfile, 0, SEEK_SET);
			while (filesize > 0)
			{
				blocklen = filesize > 8192 ? 8192 : filesize;
				/*copy the data into imbuff and then transfter it to command buffer*/
				fread(imbuff, 1, blocklen, pfile);	
				filesize -= blocklen; //reduce filesize by blocklen
				Ft_Gpu_Hal_WrCmdBuf(phost, imbuff, blocklen); //write JPEG file to command buffer
			}

			fclose(pfile);	/* close the opened JPEG file */

			//centre image on display			
			xcoord = (FT_DispWidth/2) - (0.5*image_width);
			ycoord = (FT_DispHeight/2) - (0.5*image_height);

			Ft_CmdBuffer_Index = 0; 
			Ft_App_WrCoCmd_Buffer2(phost, CLEAR(1, 1, 1)); //clear screen to predefined values
			//***************************************************************************************************
			/*Display list commands in this section are generated by CMD_LOADIMAGE unless option
			//is set to 2 or 3
			Ft_App_WrCoCmd_Buffer(phost, BITMAP_SOURCE(0L)); //specify the source address of //bitmap in RAM_G
			//specify bit map format, linestride and height for RGB565
			Ft_App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT(RGB565, image_width * 2, image_height));
			//specify bit map format, linestride and height for L8
			Ft_App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT(L8, image_width * 1, image_height));
			// controls drawing of bitmap
			Ft_App_WrCoCmd_Buffer(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, image_width, image_height));*/
			//***************************************************************************************************
			Ft_App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
			Ft_App_WrCoCmd_Buffer(phost, VERTEX2II(xcoord, ycoord, 0, 0));
			Ft_App_WrCoCmd_Buffer(phost, DISPLAY());  //ends the display all commands after this ignored
			Ft_Gpu_CoCmd_Swap(phost);
			Ft_App_Flush_Co_Buffer(phost);
		}
}

//***********************
ft_void_t Loadimage2ram()
//***********************
{
	pfile = fopen(Image_prop[cts].name,"rb");	// read Binary (rb)
	filesize = Image_prop[cts].file_size;
	cts++;
    if(cts>(nooffiles-1)) cts = 0;
	fseek(pfile,0,SEEK_SET);
	
	Load_JPEG();
}

//***************************
//
ft_void_t single_jpg_viewer()
//
//***************************
{
   #ifdef MSVC_PLATFORM
	nooffiles = Ft_List_Out_files();
	if(nooffiles==NULL)
	{
		Ft_CmdBuffer_Index = 0;
		Ft_Gpu_CoCmd_Dlstart(phost);
		Ft_App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
		Ft_App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
		Ft_Gpu_CoCmd_Text(phost, FT_DispWidth / 2, FT_DispHeight / 2, 26, OPT_CENTERX | OPT_CENTERY, "Error: No JPEG files found!");
		Ft_App_WrCoCmd_Buffer(phost, DISPLAY());
		Ft_Gpu_CoCmd_Swap(phost);
		Ft_App_Flush_Co_Buffer(phost);
		Ft_Gpu_Hal_WaitCmdfifo_empty(phost);

		return;
	}	
	#endif
  
 Loadimage2ram(); 

}

#ifdef MSVC_PLATFORM
//**************************************************************************************
//**************************************************************************************
/* Main entry point */
ft_int32_t main(ft_int32_t argc,ft_char8_t *argv[])
//**************************************************************************************
//**************************************************************************************
#endif

{
	//declare halinit
	Ft_Gpu_HalInit_t halinit;
	
	halinit.TotalChannelNum = 1;

	//initialise and return info about MPSSE cable
              
	Ft_Gpu_Hal_Init(&halinit);
	host.hal_config.channel_no = 0;

	//set SPI clock rate

#ifdef MSVC_PLATFORM_SPI
	host.hal_config.spi_clockrate_khz = 12000; //in KHz
#endif

	//configure and open the SPI channel
    Ft_Gpu_Hal_Open(&host);
            
	phost = &host;
	
    Ft_BootupConfig();

    /*It is optional to clear the screen here*/	
    Ft_Gpu_Hal_WrMem(phost, RAM_DL,(ft_uint8_t *)FT_DLCODE_BOOTUP,sizeof(FT_DLCODE_BOOTUP));
    Ft_Gpu_Hal_Wr8(phost, REG_DLSWAP,DLSWAP_FRAME);
    Ft_Gpu_Hal_Sleep(1000);//Show the booting up screen. 
    single_jpg_viewer();

	/* Close all the opened handles */
    Ft_Gpu_Hal_Close(phost);

    Ft_Gpu_Hal_DeInit();

#ifdef MSVC_PLATFORM
	return 0;
#endif
}

/* Nothing beyond this */













