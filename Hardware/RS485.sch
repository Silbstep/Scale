EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 7
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Interface:SP485EEN-L U10
U 1 1 638B5129
P 5850 4300
F 0 "U10" H 5900 4150 50  0000 C CNN
F 1 "SP485EEN-L" H 6100 4050 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5850 4300 50  0001 L BNN
F 3 "d:\\Users\\Designs\\Datasheets\\sp481e_sp485e.pdf" H 5850 4300 50  0001 L BNN
F 4 "MaxLinear, Inc." H 5850 4300 50  0001 C CNN "Manufacturer"
F 5 "SP485EEN-L" H 5850 4300 50  0001 L BNN "MPN"
	1    5850 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:TISP4P015 D7
U 1 1 638B5F3B
P 6450 4550
F 0 "D7" V 6404 4680 50  0000 L CNN
F 1 "TISP4P015" V 6495 4680 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 6450 4550 50  0001 C CNN
F 3 "d:\\Users\\Designs\\Datasheets\\tisp4p0xxl1n.pdf" H 6450 4550 50  0001 C CNN
F 4 "Bourns Inc." H 6450 4550 50  0001 C CNN "Manufacturer"
F 5 "TISP4P015L1NR-S" H 6450 4550 50  0001 C CNN "MPN"
	1    6450 4550
	0    1    1    0   
$EndComp
$Comp
L Device:TISP4P015 D8
U 1 1 638B7B46
P 7150 4550
F 0 "D8" V 7104 4680 50  0000 L CNN
F 1 "TISP4P015" V 7195 4680 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 7150 4550 50  0001 C CNN
F 3 "d:\\Users\\Designs\\Datasheets\\tisp4p0xxl1n.pdf" H 7150 4550 50  0001 C CNN
F 4 "Bourns Inc." H 7150 4550 50  0001 C CNN "Manufacturer"
F 5 "TISP4P015L1NR-S" H 7150 4550 50  0001 C CNN "MPN"
	1    7150 4550
	0    1    1    0   
$EndComp
Wire Wire Line
	6250 4300 6450 4300
Wire Wire Line
	6450 4300 6450 4400
Wire Wire Line
	6250 4000 7150 4000
Wire Wire Line
	7150 4000 7150 4400
$Comp
L Device:Polyfuse F2
U 1 1 638B932E
P 7400 4000
F 0 "F2" V 7175 4000 50  0000 C CNN
F 1 "Polyfuse" V 7266 4000 50  0000 C CNN
F 2 "Fuse:Fuse_1206_3216Metric" H 7450 3800 50  0001 L CNN
F 3 "d:\\Users\\Designs\\Datasheets\\eaton-pts1206-6-60-volt-dc-surface-mount-resettable-ptc-data-sheet.pdf" H 7400 4000 50  0001 C CNN
F 4 "Eaton" H 7400 4000 50  0001 C CNN "Manufacturer"
F 5 "PTS120660V010" H 7400 4000 50  0001 C CNN "MPN"
	1    7400 4000
	0    1    1    0   
$EndComp
Wire Wire Line
	7250 4000 7150 4000
Connection ~ 7150 4000
$Comp
L Device:Polyfuse F3
U 1 1 638BA3A6
P 7400 4300
F 0 "F3" V 7175 4300 50  0000 C CNN
F 1 "Polyfuse" V 7266 4300 50  0000 C CNN
F 2 "Fuse:Fuse_1206_3216Metric" H 7450 4100 50  0001 L CNN
F 3 "d:\\Users\\Designs\\Datasheets\\eaton-pts1206-6-60-volt-dc-surface-mount-resettable-ptc-data-sheet.pdf" H 7400 4300 50  0001 C CNN
F 4 "Eaton" H 7400 4300 50  0001 C CNN "Manufacturer"
F 5 "PTS120660V010" H 7400 4300 50  0001 C CNN "MPN"
	1    7400 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	7250 4300 6450 4300
Connection ~ 6450 4300
Text Label 7900 4000 2    50   ~ 0
RS485A
Text Label 7900 4300 2    50   ~ 0
RS485B
Text HLabel 8000 4000 2    50   BiDi ~ 0
RS485A
Text HLabel 8000 4300 2    50   BiDi ~ 0
RS485B
Wire Wire Line
	7550 4000 8000 4000
Wire Wire Line
	7550 4300 8000 4300
$Comp
L power:GND2 #PWR063
U 1 1 638BB4A3
P 6450 4800
F 0 "#PWR063" H 6450 4550 50  0001 C CNN
F 1 "GND2" H 6600 4800 50  0000 C CNN
F 2 "" H 6450 4800 50  0001 C CNN
F 3 "" H 6450 4800 50  0001 C CNN
	1    6450 4800
	1    0    0    -1  
$EndComp
$Comp
L power:GND2 #PWR064
U 1 1 638BBC7E
P 7150 4800
F 0 "#PWR064" H 7150 4550 50  0001 C CNN
F 1 "GND2" H 7300 4800 50  0000 C CNN
F 2 "" H 7150 4800 50  0001 C CNN
F 3 "" H 7150 4800 50  0001 C CNN
	1    7150 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 4700 7150 4800
Wire Wire Line
	6450 4700 6450 4800
$Comp
L power:GND2 #PWR062
U 1 1 638BC220
P 5850 4700
F 0 "#PWR062" H 5850 4450 50  0001 C CNN
F 1 "GND2" H 6000 4700 50  0000 C CNN
F 2 "" H 5850 4700 50  0001 C CNN
F 3 "" H 5850 4700 50  0001 C CNN
	1    5850 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 4600 5850 4700
$Comp
L power:GND #PWR?
U 1 1 638C0A96
P 3950 4700
AR Path="/62FC98BB/638C0A96" Ref="#PWR?"  Part="1" 
AR Path="/6380B1CC/638C0A96" Ref="#PWR?"  Part="1" 
AR Path="/6388E130/638C0A96" Ref="#PWR060"  Part="1" 
F 0 "#PWR060" H 3950 4450 50  0001 C CNN
F 1 "GND" H 4050 4700 50  0000 C CNN
F 2 "" H 3950 4700 50  0001 C CNN
F 3 "" H 3950 4700 50  0001 C CNN
	1    3950 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 4600 3950 4600
Wire Wire Line
	3950 4600 3950 4700
Wire Wire Line
	4950 4700 4950 4600
Wire Wire Line
	4950 4600 4850 4600
Wire Wire Line
	4950 3450 4950 3800
Wire Wire Line
	4950 3800 4850 3800
Wire Wire Line
	5250 3450 4950 3450
Wire Wire Line
	4850 3900 4950 3900
Wire Wire Line
	4950 3900 4950 3800
Connection ~ 4950 3800
$Comp
L Device:C_Small C?
U 1 1 638C0AB2
P 3650 3650
AR Path="/62FC98BB/638C0AB2" Ref="C?"  Part="1" 
AR Path="/6380B1CC/638C0AB2" Ref="C?"  Part="1" 
AR Path="/6388E130/638C0AB2" Ref="C39"  Part="1" 
F 0 "C39" H 3742 3696 50  0000 L CNN
F 1 "100nF" H 3742 3605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3650 3650 50  0001 C CNN
F 3 "~" H 3650 3650 50  0001 C CNN
	1    3650 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 638C0AB8
P 3650 3850
AR Path="/62FC98BB/638C0AB8" Ref="#PWR?"  Part="1" 
AR Path="/6380B1CC/638C0AB8" Ref="#PWR?"  Part="1" 
AR Path="/6388E130/638C0AB8" Ref="#PWR059"  Part="1" 
F 0 "#PWR059" H 3650 3600 50  0001 C CNN
F 1 "GND" H 3750 3850 50  0000 C CNN
F 2 "" H 3650 3850 50  0001 C CNN
F 3 "" H 3650 3850 50  0001 C CNN
	1    3650 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 638C0ABE
P 3650 3450
AR Path="/62FC98BB/638C0ABE" Ref="#PWR?"  Part="1" 
AR Path="/6380B1CC/638C0ABE" Ref="#PWR?"  Part="1" 
AR Path="/6388E130/638C0ABE" Ref="#PWR056"  Part="1" 
F 0 "#PWR056" H 3650 3300 50  0001 C CNN
F 1 "+3V3" H 3665 3623 50  0000 C CNN
F 2 "" H 3650 3450 50  0001 C CNN
F 3 "" H 3650 3450 50  0001 C CNN
	1    3650 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 3750 3650 3850
Wire Wire Line
	3650 3450 3650 3500
Wire Wire Line
	3650 3500 3950 3500
Wire Wire Line
	3950 3500 3950 3800
Wire Wire Line
	3950 3800 4050 3800
Connection ~ 3650 3500
Wire Wire Line
	3650 3500 3650 3550
Wire Wire Line
	4050 3900 3950 3900
Wire Wire Line
	3950 3900 3950 3800
Connection ~ 3950 3800
Text Label 3650 4400 0    50   ~ 0
SRX
Text Label 3650 4200 0    50   ~ 0
STXEN
Text Label 3650 4300 0    50   ~ 0
STX
Text HLabel 3550 4400 0    50   Input ~ 0
SRX
Text HLabel 3550 4200 0    50   Input ~ 0
STXEN
Text HLabel 3550 4300 0    50   Input ~ 0
STX
Wire Wire Line
	3550 4400 4050 4400
Wire Wire Line
	3550 4200 4050 4200
Wire Wire Line
	3550 4300 4050 4300
$Comp
L power:GND2 #PWR061
U 1 1 638C1C32
P 4950 4700
F 0 "#PWR061" H 4950 4450 50  0001 C CNN
F 1 "GND2" H 5100 4700 50  0000 C CNN
F 2 "" H 4950 4700 50  0001 C CNN
F 3 "" H 4950 4700 50  0001 C CNN
	1    4950 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 638C39D2
P 5250 3600
AR Path="/62FC98BB/638C39D2" Ref="C?"  Part="1" 
AR Path="/6380B1CC/638C39D2" Ref="C?"  Part="1" 
AR Path="/6388E130/638C39D2" Ref="C38"  Part="1" 
F 0 "C38" H 5342 3646 50  0000 L CNN
F 1 "100nF" H 5342 3555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5250 3600 50  0001 C CNN
F 3 "~" H 5250 3600 50  0001 C CNN
	1    5250 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3700 5250 3800
Wire Wire Line
	5250 3450 5250 3500
$Comp
L power:GND2 #PWR058
U 1 1 638C4D09
P 5250 3800
F 0 "#PWR058" H 5250 3550 50  0001 C CNN
F 1 "GND2" H 5400 3800 50  0000 C CNN
F 2 "" H 5250 3800 50  0001 C CNN
F 3 "" H 5250 3800 50  0001 C CNN
	1    5250 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 638C6D60
P 6100 3500
AR Path="/62FC98BB/638C6D60" Ref="C?"  Part="1" 
AR Path="/6380B1CC/638C6D60" Ref="C?"  Part="1" 
AR Path="/6388E130/638C6D60" Ref="C37"  Part="1" 
F 0 "C37" H 6192 3546 50  0000 L CNN
F 1 "100nF" H 6192 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6100 3500 50  0001 C CNN
F 3 "~" H 6100 3500 50  0001 C CNN
	1    6100 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 3600 6100 3700
$Comp
L power:GND2 #PWR057
U 1 1 638C6D67
P 6100 3700
F 0 "#PWR057" H 6100 3450 50  0001 C CNN
F 1 "GND2" H 6250 3700 50  0000 C CNN
F 2 "" H 6100 3700 50  0001 C CNN
F 3 "" H 6100 3700 50  0001 C CNN
	1    6100 3700
	1    0    0    -1  
$EndComp
$Comp
L power:+5C #PWR054
U 1 1 638CACD0
P 6100 3300
F 0 "#PWR054" H 6100 3150 50  0001 C CNN
F 1 "+5C" H 6115 3473 50  0000 C CNN
F 2 "" H 6100 3300 50  0001 C CNN
F 3 "" H 6100 3300 50  0001 C CNN
	1    6100 3300
	1    0    0    -1  
$EndComp
$Comp
L power:+5C #PWR055
U 1 1 638CB541
P 5250 3400
F 0 "#PWR055" H 5250 3250 50  0001 C CNN
F 1 "+5C" H 5265 3573 50  0000 C CNN
F 2 "" H 5250 3400 50  0001 C CNN
F 3 "" H 5250 3400 50  0001 C CNN
	1    5250 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3400 5250 3450
Connection ~ 5250 3450
Wire Wire Line
	6100 3300 6100 3350
Wire Wire Line
	6100 3350 5850 3350
Wire Wire Line
	5850 3350 5850 3700
Connection ~ 6100 3350
Wire Wire Line
	6100 3350 6100 3400
Wire Wire Line
	5450 4000 5350 4000
Wire Wire Line
	5350 4000 5350 4400
Wire Wire Line
	4850 4400 5350 4400
Wire Wire Line
	5450 4100 4850 4100
Wire Wire Line
	5450 4200 4850 4200
Wire Wire Line
	5450 4300 4850 4300
Wire Wire Line
	4050 4100 3950 4100
Wire Wire Line
	3950 4100 3950 4600
Connection ~ 3950 4600
$Comp
L Converter_DCDC:B0505MT-1WR4 U8
U 1 1 638101A8
P 5250 1600
F 0 "U8" H 5250 1967 50  0000 C CNN
F 1 "B0505MT-1WR4" H 5250 1876 50  0000 C CNN
F 2 "Module:B0505MT-1WR4" H 5100 1600 50  0001 L BNN
F 3 "d:\\Users\\Designs\\Datasheets\\B05xxMT-1WR4.pdf" H 5250 1600 50  0001 L BNN
F 4 "Mornsun" H 5250 1600 50  0001 L BNN "Manufacturer"
F 5 "B0505MT-1WR4" H 5250 1875 50  0001 C CNN "MPN"
	1    5250 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 63814308
P 4600 1600
AR Path="/62FC98BB/63814308" Ref="C?"  Part="1" 
AR Path="/6380B1CC/63814308" Ref="C?"  Part="1" 
AR Path="/6388E130/63814308" Ref="C35"  Part="1" 
F 0 "C35" H 4692 1646 50  0000 L CNN
F 1 "4.7uF" H 4692 1555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4600 1600 50  0001 C CNN
F 3 "~" H 4600 1600 50  0001 C CNN
	1    4600 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 63815873
P 5900 1600
AR Path="/62FC98BB/63815873" Ref="C?"  Part="1" 
AR Path="/6380B1CC/63815873" Ref="C?"  Part="1" 
AR Path="/6388E130/63815873" Ref="C36"  Part="1" 
F 0 "C36" H 5992 1646 50  0000 L CNN
F 1 "10uF" H 5992 1555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5900 1600 50  0001 C CNN
F 3 "~" H 5900 1600 50  0001 C CNN
	1    5900 1600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 638190DC
P 4600 1400
AR Path="/630D00E1/638190DC" Ref="#PWR?"  Part="1" 
AR Path="/6388E130/638190DC" Ref="#PWR050"  Part="1" 
F 0 "#PWR050" H 4600 1250 50  0001 C CNN
F 1 "+5V" H 4615 1573 50  0000 C CNN
F 2 "" H 4600 1400 50  0001 C CNN
F 3 "" H 4600 1400 50  0001 C CNN
	1    4600 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6381A106
P 4600 1800
AR Path="/62FC98BB/6381A106" Ref="#PWR?"  Part="1" 
AR Path="/6380B1CC/6381A106" Ref="#PWR?"  Part="1" 
AR Path="/6388E130/6381A106" Ref="#PWR052"  Part="1" 
F 0 "#PWR052" H 4600 1550 50  0001 C CNN
F 1 "GND" H 4700 1800 50  0000 C CNN
F 2 "" H 4600 1800 50  0001 C CNN
F 3 "" H 4600 1800 50  0001 C CNN
	1    4600 1800
	1    0    0    -1  
$EndComp
$Comp
L power:+5C #PWR051
U 1 1 6381AEB4
P 5900 1400
F 0 "#PWR051" H 5900 1250 50  0001 C CNN
F 1 "+5C" H 5915 1573 50  0000 C CNN
F 2 "" H 5900 1400 50  0001 C CNN
F 3 "" H 5900 1400 50  0001 C CNN
	1    5900 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND2 #PWR053
U 1 1 6381C1DB
P 5900 1800
F 0 "#PWR053" H 5900 1550 50  0001 C CNN
F 1 "GND2" H 6050 1800 50  0000 C CNN
F 2 "" H 5900 1800 50  0001 C CNN
F 3 "" H 5900 1800 50  0001 C CNN
	1    5900 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 1500 4600 1500
Wire Wire Line
	4600 1500 4600 1400
Connection ~ 4600 1500
Wire Wire Line
	4600 1700 4600 1800
Wire Wire Line
	4800 1700 4600 1700
Connection ~ 4600 1700
Wire Wire Line
	5700 1700 5900 1700
Wire Wire Line
	5900 1700 5900 1800
Connection ~ 5900 1700
Wire Wire Line
	5900 1500 5900 1400
Wire Wire Line
	5900 1500 5700 1500
Connection ~ 5900 1500
$Comp
L Isolator:SI8641BA-C-IUR U9
U 1 1 638F9E84
P 4450 4200
F 0 "U9" H 4450 4867 50  0000 C CNN
F 1 "SI8641BA-C-IUR" H 4450 4776 50  0000 C CNN
F 2 "Package_SO:QSOP-16_3.9x4.9mm_P0.635mm" H 4450 3650 50  0001 C CIN
F 3 "d:\\Users\\Designs\\Datasheets\\si864x-datasheet.pdf" H 4450 4600 50  0001 C CNN
F 4 "Skyworks Solutions Inc." H 4450 4867 50  0001 C CNN "Manufacturer"
F 5 "SI8641BA-C-IUR" H 4450 4776 50  0001 C CNN "MPN"
	1    4450 4200
	1    0    0    -1  
$EndComp
$EndSCHEMATC
