; Copyright (c) Future Technology Devices International 2014



; THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
; OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
; FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
; BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGE.



Objective: 
==========
This ReadMe file contains the details of the Single JPEG Viewer application release package. 

This package contains the FT800 Single JPEG Viewer application targeting the Windows PC platform.  

This application demonstrates how to check whether or not a JPEG file is compatible with the FT800 series, and,
if compatible, how to load and display a JPEG file using the FT800 series function CMD_LOADIMAGE.

Release package contents:
=========================
The folder structure is shown as below.


+---Bin
|   \---Msvc_win32
|          \--libMPSSE.a - MPSSE library file.
|               
+---Hdr
|      \---Msvc_win32
|   		\---\ftd2xx.h - ftd2xx header file
|   		\---\libMPSSE_spi - MPSSE header file
|		\---\FT_DataTypes.h - Includes the FT800 	
|					datatypes according to the platform.
|   		\---\FT_Platform.h - Includes Platform specific 
|							macros.
|   
|   \---\FT_CoPro_Cmds.h  - Includes the Coprocessor commands.	
|  \---\FT_Gpu.h - Includes the GPU commands.
|   \---\FT_Gpu_Hal.h - Includes the GPU HAL commands.
|   \---\FT_Hal_Utils.h - Includes the HAL utilities.
|
|
+---Project
|      \---Msvc_win32
|       \---FT_single_jpg_viewer – project folder of Single JPEG Viewer demo application based on MSVC/PC platform
|		\---FT_single_jpg_viewer.sln – Solution file of Single JPEG Viewer Demo application.
|		\---FT_single_jpg_viewer.sln – Solution file of Single JPEG Viewer Demo application.
|		\---Ft_single_jpg_viewer.vcxproj– project file of Single JPEG Viewer Demo application.
|		\---FT_single_jpg_viewer.vcxproj.filters
|		\---FT_single_jpg_viewer.vcxproj.user
|
+---Src
|   	\---FT_CoPro_Cmds.c - Coprocessor commands source file.
|   	\---FT_Gpu_Hal.c - Gpu hal source commands file.
|   	\---Ft_single_jpg_viewer.c - Main file of Single JPEG Viewer demo.
|
+--Test – folder containing input test files, only files with .jp* extension will be read



Configuration Instructions:
===========================
This section contains details regarding various configurations supported by this software.

Two display profiles are supported by this software: (1) WQVGA (480x272) (2) QVGA (320x240)

By default WQVGA display profile is enabled. To enable QVGA, uncomment “#define SAMAPP_DISPLAY_QVGA” macro and comment “#define SAMAPP_DISPLAY_WQVGA” in respective FT_Platform.h file.


Installation Instruction:
=========================

Unzip the package onto a respective project folder and open the solution file in the project folder and execute it. For MSVC/PC platform please execute .\FT_App_Imageviewer\Project\Msvc_win32\FT_App_Imageviewer\FT_App_Imageviewer.sln solution. 

The MSVC project file is compatible with Microsoft visual C++ 2013 Express.

Reference Information:
======================
Please refer to AN_339 'Using JPEGS with the FT800 Series' for more information on application design, setup etc.
Please refer to FT800_Series_Programmer_Guide for more information on programming.

Known issues:
=============
1. The SPI host(Windows PC) are assuming the data layout in memory as Little Endian format. 
 


Extra Information:
==================
N.A


Release Notes (Version Significance):
=====================================

Version 0.1 - intial draft of the release notes
