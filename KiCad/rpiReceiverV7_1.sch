EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Raspberry Pi Receiver"
Date "2020-02-26"
Rev "7.1"
Comp "HTL Steyr"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Memory_EEPROM_:CAT24C512 U1
U 1 1 5E0F6561
P 1750 4600
F 0 "U1" H 1500 4850 50  0000 C CNN
F 1 "CAT24C512" H 2000 4850 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 1750 4600 50  0001 C CNN
F 3 "https://www.onsemi.cn/PowerSolutions/document/CAT24C512-D.PDF" H 1750 4600 50  0001 C CNN
F 4 "EEPROM Serial 512-Kb I2C" H -2350 -500 50  0001 C CNN "Description"
F 5 "ON Semiconductor" H -2350 -500 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 -500 50  0001 C CNN "Vendor"
F 7 "755-9843" H -2350 -500 50  0001 C CNN "Vendor Part Number"
	1    1750 4600
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR03
U 1 1 5E0F6EEC
P 2850 3850
F 0 "#PWR03" H 2850 3700 50  0001 C CNN
F 1 "+3.3V" H 2865 4023 50  0000 C CNN
F 2 "" H 2850 3850 50  0001 C CNN
F 3 "" H 2850 3850 50  0001 C CNN
	1    2850 3850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5E0F74B2
P 1750 5050
F 0 "#PWR04" H 1750 4800 50  0001 C CNN
F 1 "GND" H 1755 4877 50  0000 C CNN
F 2 "" H 1750 5050 50  0001 C CNN
F 3 "" H 1750 5050 50  0001 C CNN
	1    1750 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 5050 1750 4950
$Comp
L Mechanical:MountingHole H3
U 1 1 5E11B529
P 9750 5800
F 0 "H3" H 9850 5846 50  0000 L CNN
F 1 "MountingHole" H 9850 5755 50  0000 L CNN
F 2 "MountingHole_:MountingHole_2.7mm_M2.5" H 9750 5800 50  0001 C CNN
F 3 "~" H 9750 5800 50  0001 C CNN
	1    9750 5800
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5E11BBEA
P 10450 6000
F 0 "H2" H 10550 6046 50  0000 L CNN
F 1 "MountingHole" H 10550 5955 50  0000 L CNN
F 2 "MountingHole_:MountingHole_2.7mm_M2.5" H 10450 6000 50  0001 C CNN
F 3 "~" H 10450 6000 50  0001 C CNN
	1    10450 6000
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 5E11CA4E
P 9750 6000
F 0 "H1" H 9850 6046 50  0000 L CNN
F 1 "MountingHole" H 9850 5955 50  0000 L CNN
F 2 "MountingHole_:MountingHole_2.7mm_M2.5" H 9750 6000 50  0001 C CNN
F 3 "~" H 9750 6000 50  0001 C CNN
	1    9750 6000
	1    0    0    -1  
$EndComp
$Comp
L Connector_RaspberryPi:PI4HAT DNP_J1
U 1 1 5E122BCB
P 2150 750
F 0 "DNP_J1" H 2150 975 50  0000 C CNN
F 1 "PI4HAT" H 2150 884 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x02_P2.54mm_Vertical" H 2150 950 50  0001 C CNN
F 3 "http://www.assmann-wsw.com/wo/de/produkte/connectors/header/detail/1455224/" H 1450 750 50  0001 C CNN
F 4 "M20-7830246" H 2250 300 50  0001 L CNN "Description"
F 5 "HARWIN" H 2200 400 50  0001 L CNN "Manufacturer"
F 6 "RS Components" H 2200 200 50  0001 L CNN "Vendor"
F 7 "681-6839" H 2250 100 50  0001 L CNN "Vendor Part Number"
	1    2150 750 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5E126DC7
P 1450 3250
F 0 "#PWR02" H 1450 3000 50  0001 C CNN
F 1 "GND" H 1455 3077 50  0000 C CNN
F 2 "" H 1450 3250 50  0001 C CNN
F 3 "" H 1450 3250 50  0001 C CNN
	1    1450 3250
	1    0    0    -1  
$EndComp
NoConn ~ 1550 750 
NoConn ~ 1550 850 
NoConn ~ 2750 750 
NoConn ~ 2750 850 
$Comp
L power:+3V3 #PWR01
U 1 1 5E1242A4
P 1350 1150
F 0 "#PWR01" H 1350 1000 50  0001 C CNN
F 1 "+3V3" H 1365 1323 50  0000 C CNN
F 2 "" H 1350 1150 50  0001 C CNN
F 3 "" H 1350 1150 50  0001 C CNN
	1    1350 1150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 5E1246B3
P 2900 1150
F 0 "#PWR06" H 2900 1000 50  0001 C CNN
F 1 "+5V" H 2915 1323 50  0000 C CNN
F 2 "" H 2900 1150 50  0001 C CNN
F 3 "" H 2900 1150 50  0001 C CNN
	1    2900 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1250 2900 1250
Wire Wire Line
	2900 1250 2900 1150
Wire Wire Line
	2750 1350 2900 1350
Wire Wire Line
	2900 1350 2900 1250
Connection ~ 2900 1250
Wire Wire Line
	1450 3250 1450 3150
Wire Wire Line
	1450 3150 1550 3150
Wire Wire Line
	1450 3150 1450 2450
Wire Wire Line
	1450 2450 1550 2450
Connection ~ 1450 3150
Wire Wire Line
	1450 2450 1450 1650
Wire Wire Line
	1450 1650 1550 1650
Connection ~ 1450 2450
$Comp
L power:GND #PWR05
U 1 1 5E12724B
P 2850 3250
F 0 "#PWR05" H 2850 3000 50  0001 C CNN
F 1 "GND" H 2855 3077 50  0000 C CNN
F 2 "" H 2850 3250 50  0001 C CNN
F 3 "" H 2850 3250 50  0001 C CNN
	1    2850 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 2850 2850 2850
Wire Wire Line
	2850 2850 2850 3250
Wire Wire Line
	2750 2650 2850 2650
Wire Wire Line
	2850 2650 2850 2850
Connection ~ 2850 2850
Wire Wire Line
	2750 2150 2850 2150
Wire Wire Line
	2850 2150 2850 2650
Connection ~ 2850 2650
Wire Wire Line
	2750 1850 2850 1850
Wire Wire Line
	2850 1850 2850 2150
Connection ~ 2850 2150
Wire Wire Line
	2750 1450 2850 1450
Wire Wire Line
	2850 1450 2850 1850
Connection ~ 2850 1850
Wire Wire Line
	1350 4500 1250 4500
Wire Wire Line
	1250 4500 1250 4600
Wire Wire Line
	1250 4950 1750 4950
Connection ~ 1750 4950
Wire Wire Line
	1750 4950 1750 4900
Wire Wire Line
	1350 4600 1250 4600
Connection ~ 1250 4600
Wire Wire Line
	1250 4600 1250 4700
Wire Wire Line
	1350 4700 1250 4700
Connection ~ 1250 4700
Wire Wire Line
	1250 4700 1250 4950
$Comp
L Connector_Generic:Conn_01x02 DNP_J3
U 1 1 5E12A2EA
P 3100 4700
F 0 "DNP_J3" H 3180 4692 50  0000 L CNN
F 1 "Conn_01x02" H 2950 4500 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3100 4700 50  0001 C CNN
F 3 "~" H 3100 4700 50  0001 C CNN
	1    3100 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 4800 2900 4950
Wire Bus Line
	750  3500 3550 3500
Wire Wire Line
	1550 2550 850  2550
Text Label 850  2550 0    50   ~ 0
ID_SD_EEPROM
Wire Wire Line
	2750 2550 3450 2550
Text Label 2900 2550 0    50   ~ 0
ID_SC_EEPROM
Text Label 2900 4500 0    50   ~ 0
ID_SD_EEPROM
Text Label 2900 4600 0    50   ~ 0
ID_SC_EEPROM
$Comp
L Device:R R2
U 1 1 5E13A324
P 2550 4100
F 0 "R2" H 2480 4054 50  0000 R CNN
F 1 "3k9" H 2480 4145 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2480 4100 50  0001 C CNN
F 3 "~" H 2550 4100 50  0001 C CNN
F 4 "804-6492" H -2350 -500 50  0001 C CNN "Description"
F 5 "RS Pro" H -2350 -500 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 -500 50  0001 C CNN "Vendor"
F 7 "804-6492" H -2350 -500 50  0001 C CNN "Vendor Part Number"
	1    2550 4100
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 5E145FF3
P 2250 4100
F 0 "R1" H 2180 4054 50  0000 R CNN
F 1 "3k9" H 2180 4145 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2180 4100 50  0001 C CNN
F 3 "~" H 2250 4100 50  0001 C CNN
F 4 "804-6492" H -2350 -500 50  0001 C CNN "Description"
F 5 "RS Pro" H -2350 -500 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 -500 50  0001 C CNN "Vendor"
F 7 "804-6492" H -2350 -500 50  0001 C CNN "Vendor Part Number"
	1    2250 4100
	-1   0    0    1   
$EndComp
$Comp
L Device:R R3
U 1 1 5E1461EB
P 2850 4100
F 0 "R3" H 2780 4054 50  0000 R CNN
F 1 "3k9" H 2780 4145 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2780 4100 50  0001 C CNN
F 3 "~" H 2850 4100 50  0001 C CNN
F 4 "804-6492" H -2350 -500 50  0001 C CNN "Description"
F 5 "RS Pro" H -2350 -500 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 -500 50  0001 C CNN "Vendor"
F 7 "804-6492" H -2350 -500 50  0001 C CNN "Vendor Part Number"
	1    2850 4100
	-1   0    0    1   
$EndComp
Wire Wire Line
	2250 3950 2250 3900
Wire Wire Line
	2250 3900 1750 3900
Connection ~ 1750 3900
Wire Wire Line
	2250 3900 2550 3900
Wire Wire Line
	2550 3900 2550 3950
Connection ~ 2250 3900
Wire Wire Line
	2550 3900 2850 3900
Wire Wire Line
	2850 3900 2850 3950
Connection ~ 2550 3900
Wire Wire Line
	2150 4500 2250 4500
Wire Wire Line
	2150 4600 2550 4600
Wire Wire Line
	2150 4700 2850 4700
Wire Wire Line
	1750 4950 2900 4950
Wire Wire Line
	2250 4250 2250 4500
Connection ~ 2250 4500
Wire Wire Line
	2250 4500 3450 4500
Wire Wire Line
	2550 4250 2550 4600
Connection ~ 2550 4600
Wire Wire Line
	2550 4600 3450 4600
Wire Wire Line
	2850 4250 2850 4700
Connection ~ 2850 4700
Wire Wire Line
	2850 4700 2900 4700
NoConn ~ 2750 1550
NoConn ~ 2750 1650
NoConn ~ 1550 2250
NoConn ~ 1550 2350
NoConn ~ 2750 2250
NoConn ~ 2750 2350
$Comp
L Device:C C1
U 1 1 5E16926E
P 1250 4150
F 0 "C1" H 1365 4196 50  0000 L CNN
F 1 "100nF" H 1365 4105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1288 4000 50  0001 C CNN
F 3 "~" H 1250 4150 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 8700 1000 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 8700 1000 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 8700 1000 50  0001 C CNN "Vendor"
F 7 "766-5648" H 8700 1000 50  0001 C CNN "Vendor Part Number"
	1    1250 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 4000 1250 3900
Wire Wire Line
	1250 3900 1750 3900
Wire Wire Line
	1250 4300 1250 4500
Connection ~ 1250 4500
Wire Wire Line
	1750 4300 1750 3900
Wire Wire Line
	1550 1350 850  1350
Wire Wire Line
	1550 1450 850  1450
Entry Wire Line
	750  1250 850  1350
Entry Wire Line
	750  1350 850  1450
Entry Wire Line
	750  2450 850  2550
Entry Wire Line
	3450 2550 3550 2650
Entry Wire Line
	3450 4500 3550 4600
Entry Wire Line
	3450 4600 3550 4700
Text Label 900  1350 0    50   ~ 0
SDA1
Text Label 900  1450 0    50   ~ 0
SCL1
Entry Wire Line
	3550 1500 3650 1600
Entry Wire Line
	3550 1600 3650 1700
Wire Wire Line
	3650 1600 3950 1600
Wire Wire Line
	3650 1700 4250 1700
Text Label 3700 1600 0    50   ~ 0
SCL1
Text Label 3700 1700 0    50   ~ 0
SDA1
$Comp
L Device:R R4
U 1 1 5E1319DD
P 3950 1250
F 0 "R4" H 3880 1204 50  0000 R CNN
F 1 "3k9" H 3880 1295 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 3880 1250 50  0001 C CNN
F 3 "~" H 3950 1250 50  0001 C CNN
F 4 "804-6492" H -2350 200 50  0001 C CNN "Description"
F 5 "RS Pro" H -2350 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 200 50  0001 C CNN "Vendor"
F 7 "804-6492" H -2350 200 50  0001 C CNN "Vendor Part Number"
	1    3950 1250
	-1   0    0    1   
$EndComp
$Comp
L Device:R R5
U 1 1 5E13217E
P 4250 1250
F 0 "R5" H 4180 1204 50  0000 R CNN
F 1 "3k9" H 4180 1295 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 4180 1250 50  0001 C CNN
F 3 "~" H 4250 1250 50  0001 C CNN
F 4 "804-6492" H -2350 200 50  0001 C CNN "Description"
F 5 "RS Pro" H -2350 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 200 50  0001 C CNN "Vendor"
F 7 "804-6492" H -2350 200 50  0001 C CNN "Vendor Part Number"
	1    4250 1250
	-1   0    0    1   
$EndComp
Wire Wire Line
	3950 1600 3950 1400
Wire Wire Line
	4250 1700 4250 1400
$Comp
L power:+3V3 #PWR014
U 1 1 5E138CC3
P 4850 1250
F 0 "#PWR014" H 4850 1100 50  0001 C CNN
F 1 "+3V3" H 4865 1423 50  0000 C CNN
F 2 "" H 4850 1250 50  0001 C CNN
F 3 "" H 4850 1250 50  0001 C CNN
	1    4850 1250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR09
U 1 1 5E13B009
P 4250 1000
F 0 "#PWR09" H 4250 850 50  0001 C CNN
F 1 "+3V3" H 4265 1173 50  0000 C CNN
F 2 "" H 4250 1000 50  0001 C CNN
F 3 "" H 4250 1000 50  0001 C CNN
	1    4250 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 1100 3950 1050
Wire Wire Line
	3950 1050 4250 1050
Wire Wire Line
	4250 1050 4250 1000
Wire Wire Line
	4250 1100 4250 1050
Connection ~ 4250 1050
$Comp
L Device:C C2
U 1 1 5E140FC8
P 5150 1300
F 0 "C2" H 5265 1346 50  0000 L CNN
F 1 "100nF" H 5265 1255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5188 1150 50  0001 C CNN
F 3 "~" H 5150 1300 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 8500 1750 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 8500 1750 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 8500 1750 50  0001 C CNN "Vendor"
F 7 "766-5648" H 8500 1750 50  0001 C CNN "Vendor Part Number"
	1    5150 1300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5E146E5F
P 5450 1300
F 0 "#PWR013" H 5450 1050 50  0001 C CNN
F 1 "GND" H 5455 1127 50  0000 C CNN
F 2 "" H 5450 1300 50  0001 C CNN
F 3 "" H 5450 1300 50  0001 C CNN
	1    5450 1300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5E181CD1
P 4850 2250
F 0 "#PWR07" H 4850 2000 50  0001 C CNN
F 1 "GND" H 4855 2077 50  0000 C CNN
F 2 "" H 4850 2250 50  0001 C CNN
F 3 "" H 4850 2250 50  0001 C CNN
	1    4850 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 1550 850  1550
Entry Wire Line
	750  1450 850  1550
Text Label 900  1550 0    50   ~ 0
GPIO04
Wire Wire Line
	1550 1850 850  1850
Entry Wire Line
	750  1750 850  1850
Text Label 900  1850 0    50   ~ 0
GPIO27
Wire Wire Line
	1550 2850 850  2850
Entry Wire Line
	750  2750 850  2850
Text Label 900  2850 0    50   ~ 0
GPIO13
Wire Wire Line
	1550 3050 850  3050
Entry Wire Line
	750  2950 850  3050
Text Label 900  3050 0    50   ~ 0
GPIO26
Connection ~ 3950 1600
Wire Wire Line
	3950 1600 4350 1600
Connection ~ 4250 1700
Wire Wire Line
	4250 1700 4350 1700
Wire Wire Line
	4850 1250 4850 1300
Wire Wire Line
	5000 1300 4850 1300
Connection ~ 4850 1300
Wire Wire Line
	5300 1300 5450 1300
$Comp
L Graphic:Logo_Open_Hardware_Small Logo1
U 1 1 5E2DF9F9
P 9750 6350
F 0 "Logo1" H 9950 6200 50  0000 L CNN
F 1 " " H 9750 6125 50  0001 C CNN
F 2 "Symbol:OSHW-Logo2_7.3x6mm_Copper" H 9750 6350 50  0001 C CNN
F 3 "~" H 9750 6350 50  0001 C CNN
	1    9750 6350
	1    0    0    -1  
$EndComp
$Comp
L Connector_RaspberryPi:PI40HAT J2
U 1 1 5E11BD5C
P 2150 1250
F 0 "J2" H 2150 1475 50  0000 C CNN
F 1 "PI40HAT" H 2150 1384 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 2150 1450 50  0001 C CNN
F 3 "http://www.assmann-wsw.com/wo/de/produkte/connectors/header/detail/1455276/" H 2150 1383 50  0001 C CNN
F 4 "A-BL254-DG-G40D" H 2250 800 50  0001 L CNN "Description"
F 5 "ASSMANN WSW" H 2200 900 50  0001 L CNN "Manufacturer"
F 6 "RS Components" H 2200 700 50  0001 L CNN "Vendor"
F 7 "674-2369" H 2250 600 50  0001 L CNN "Vendor Part Number"
	1    2150 1250
	1    0    0    -1  
$EndComp
Entry Wire Line
	750  1850 850  1950
Wire Wire Line
	850  1950 1550 1950
Text Label 900  1950 0    50   ~ 0
GPIO22
Entry Wire Line
	3450 2950 3550 3050
Wire Wire Line
	3450 2950 2750 2950
Text Label 2900 2950 0    50   ~ 0
GPIO16
NoConn ~ 1550 2150
$Comp
L Timer_RTC:DS3231MZ U2
U 1 1 5E40489A
P 4850 1800
F 0 "U2" H 4500 2150 50  0000 C CNN
F 1 "DS3231MZ" H 5100 2150 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 4850 1300 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS3231M.pdf" H 4850 1200 50  0001 C CNN
F 4 "DS3231MZ+" H -200 200 50  0001 C CNN "Description"
F 5 "Maxim Integrated" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "189-8391" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    4850 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1300 4850 1400
$Comp
L Connector:TestPoint TP1
U 1 1 5E40AAF3
P 3900 2000
F 0 "TP1" H 3958 2118 50  0000 L CNN
F 1 "TestPoint" H 3958 2027 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 4100 2000 50  0001 C CNN
F 3 "~" H 4100 2000 50  0001 C CNN
	1    3900 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 2000 3900 2000
$Comp
L Device_:Battery_Cell BT1
U 1 1 5E413244
P 5100 800
F 0 "BT1" V 5200 1000 50  0000 C CNN
F 1 "Battery_Cell" V 5200 550 50  0000 C CNN
F 2 "Battery:BatteryHolder_Keystone_3002_1x2032" V 5100 860 50  0001 C CNN
F 3 "~" V 5100 860 50  0001 C CNN
F 4 "185-4652" H -200 200 50  0001 C CNN "Description"
F 5 "RS Pro" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "185-4652" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    5100 800 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5450 800  5450 1300
Connection ~ 5450 1300
Wire Wire Line
	4750 1400 4750 800 
Wire Wire Line
	4850 2250 4850 2200
$Comp
L Connector:TestPoint TP2
U 1 1 5E42017C
P 5650 1600
F 0 "TP2" H 5708 1718 50  0000 L CNN
F 1 "TestPoint" H 5708 1627 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 5850 1600 50  0001 C CNN
F 3 "~" H 5850 1600 50  0001 C CNN
	1    5650 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 1600 5650 1600
$Comp
L Device:R R6
U 1 1 5E422D36
P 6100 1250
F 0 "R6" H 6030 1204 50  0000 R CNN
F 1 "3k9" H 6030 1295 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 6030 1250 50  0001 C CNN
F 3 "~" H 6100 1250 50  0001 C CNN
F 4 "804-6492" H -200 200 50  0001 C CNN "Description"
F 5 "RS Pro" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "804-6492" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    6100 1250
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR08
U 1 1 5E423517
P 6100 1000
F 0 "#PWR08" H 6100 850 50  0001 C CNN
F 1 "+3V3" H 6115 1173 50  0000 C CNN
F 2 "" H 6100 1000 50  0001 C CNN
F 3 "" H 6100 1000 50  0001 C CNN
	1    6100 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 1000 6100 1100
Wire Wire Line
	6100 1400 6100 1900
Wire Wire Line
	6100 1900 5350 1900
Wire Wire Line
	6100 2500 3650 2500
Connection ~ 6100 1900
Entry Wire Line
	3550 2400 3650 2500
Wire Wire Line
	5200 800  5450 800 
Wire Wire Line
	4900 800  4750 800 
Wire Wire Line
	5100 2950 6100 2950
Wire Wire Line
	6100 2950 6100 2500
Connection ~ 6100 2500
Wire Wire Line
	4600 2950 3650 2950
Entry Wire Line
	3550 2850 3650 2950
Wire Wire Line
	6100 1900 6100 2500
Text Label 3700 2950 0    50   ~ 0
SCL1
Wire Wire Line
	5100 3450 3650 3450
Entry Wire Line
	3550 3350 3650 3450
$Comp
L 74xGxx_:74AHC1G66 U3
U 1 1 5E401B8C
P 4850 2950
F 0 "U3" H 5050 3100 50  0000 C CNN
F 1 "74AHC1G66" H 5150 2800 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-353_SC-70-5" H 4850 2950 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT1G66.pdf" H 4850 2950 50  0001 C CNN
F 4 "74AHC1G66GW,125" H -200 200 50  0001 C CNN "Description"
F 5 "Nexperia" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "813-9222" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    4850 2950
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 5E4032C1
P 4800 3150
F 0 "#PWR011" H 4800 2900 50  0001 C CNN
F 1 "GND" H 4805 2977 50  0000 C CNN
F 2 "" H 4800 3150 50  0001 C CNN
F 3 "" H 4800 3150 50  0001 C CNN
	1    4800 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 3150 4800 3100
Wire Wire Line
	5100 3100 5100 3450
$Comp
L power:+3V3 #PWR010
U 1 1 5E40EC86
P 4800 2750
F 0 "#PWR010" H 4800 2600 50  0001 C CNN
F 1 "+3V3" H 4815 2923 50  0000 C CNN
F 2 "" H 4800 2750 50  0001 C CNN
F 3 "" H 4800 2750 50  0001 C CNN
	1    4800 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 2800 4800 2750
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 5E402AF2
P 10550 1750
F 0 "J5" H 10630 1742 50  0000 L CNN
F 1 "Conn_01x02" H 10630 1651 50  0000 L CNN
F 2 "Connector_TE-Connectivity_:TE_2834085-1_1x02_P2.54mm_45deg" H 10550 1750 50  0001 C CNN
F 3 "~" H 10550 1750 50  0001 C CNN
F 4 "2834085-1" H -200 200 50  0001 C CNN "Description"
F 5 "TE Connectivity" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "123-2029" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    10550 1750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 5E403469
P 10550 1450
F 0 "J4" H 10630 1442 50  0000 L CNN
F 1 "Conn_01x02" H 10630 1351 50  0000 L CNN
F 2 "Connector_TE-Connectivity_:TE_2834085-1_1x02_P2.54mm_45deg" H 10550 1450 50  0001 C CNN
F 3 "~" H 10550 1450 50  0001 C CNN
F 4 "2834085-1" H -200 200 50  0001 C CNN "Description"
F 5 "TE Connectivity" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "123-2029" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    10550 1450
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Optical_:OPT3001 U5
U 1 1 5E40628A
P 1650 5900
F 0 "U5" H 1450 6150 50  0000 R CNN
F 1 "OPT3001" H 2150 6150 50  0000 R CNN
F 2 "Package_DFN_QFN_:DNP-6-1EP_2.1x2.1mm_P0.65mm_EP0.65x1.35mm" H 1650 5400 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/opt3001.pdf" H 2400 6200 50  0001 C CNN
F 4 "OPT3001DNPT" H -200 200 50  0001 C CNN "Description"
F 5 "Texas Instruments" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "900-9810" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    1650 5900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5E4072F7
P 2850 5500
F 0 "R7" H 2780 5454 50  0000 R CNN
F 1 "3k9" H 2780 5545 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2780 5500 50  0001 C CNN
F 3 "~" H 2850 5500 50  0001 C CNN
F 4 "804-6492" H -2350 900 50  0001 C CNN "Description"
F 5 "RS Pro" H -2350 900 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2350 900 50  0001 C CNN "Vendor"
F 7 "804-6492" H -2350 900 50  0001 C CNN "Vendor Part Number"
	1    2850 5500
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR015
U 1 1 5E408491
P 2850 5300
F 0 "#PWR015" H 2850 5150 50  0001 C CNN
F 1 "+3.3V" H 2865 5473 50  0000 C CNN
F 2 "" H 2850 5300 50  0001 C CNN
F 3 "" H 2850 5300 50  0001 C CNN
	1    2850 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5E4093B8
P 1200 5500
F 0 "C3" H 1315 5546 50  0000 L CNN
F 1 "100nF" H 1315 5455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1238 5350 50  0001 C CNN
F 3 "~" H 1200 5500 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 8650 2350 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 8650 2350 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 8650 2350 50  0001 C CNN "Vendor"
F 7 "766-5648" H 8650 2350 50  0001 C CNN "Vendor Part Number"
	1    1200 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 5350 1750 5350
$Comp
L power:GND #PWR012
U 1 1 5E4109E3
P 1550 6400
F 0 "#PWR012" H 1550 6150 50  0001 C CNN
F 1 "GND" H 1555 6227 50  0000 C CNN
F 2 "" H 1550 6400 50  0001 C CNN
F 3 "" H 1550 6400 50  0001 C CNN
	1    1550 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 5650 1200 6300
Wire Wire Line
	1200 6300 1550 6300
Wire Wire Line
	1750 6300 1750 6200
Wire Wire Line
	1550 6200 1550 6300
Connection ~ 1550 6300
Wire Wire Line
	1550 6300 1750 6300
Wire Wire Line
	1750 5600 1750 5350
Connection ~ 1750 5350
Wire Wire Line
	1550 6400 1550 6300
Wire Wire Line
	2050 6000 2850 6000
Wire Wire Line
	2850 6000 2850 5650
Wire Wire Line
	2850 6000 3450 6000
Connection ~ 2850 6000
Wire Wire Line
	2050 5900 3450 5900
Wire Wire Line
	2050 5800 3450 5800
Entry Wire Line
	3450 5800 3550 5900
Entry Wire Line
	3450 5900 3550 6000
Entry Wire Line
	3450 6000 3550 6100
Text Label 2900 5800 0    50   ~ 0
SDA1
Text Label 2900 5900 0    50   ~ 0
SCL1
Wire Wire Line
	1750 5350 2850 5350
Wire Wire Line
	2850 3850 2850 3900
Connection ~ 2850 3900
Wire Wire Line
	2850 5300 2850 5350
Connection ~ 2850 5350
Connection ~ 3550 3500
$Comp
L Device:R R8
U 1 1 5E46EDA0
P 2650 6900
F 0 "R8" V 2550 6850 50  0000 R CNN
F 1 "100" V 2550 7050 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2580 6900 50  0001 C CNN
F 3 "~" H 2650 6900 50  0001 C CNN
F 4 "CRG0805F100R" H -2550 2300 50  0001 C CNN "Description"
F 5 "TE Connectivity" H -2550 2300 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -2550 2300 50  0001 C CNN "Vendor"
F 7 "134-5462" H -2550 2300 50  0001 C CNN "Vendor Part Number"
	1    2650 6900
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C4
U 1 1 5E46F422
P 2300 6750
F 0 "C4" H 2415 6796 50  0000 L CNN
F 1 "100nF" H 2415 6705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2338 6600 50  0001 C CNN
F 3 "~" H 2300 6750 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 9750 3600 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 9750 3600 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 9750 3600 50  0001 C CNN "Vendor"
F 7 "766-5648" H 9750 3600 50  0001 C CNN "Vendor Part Number"
	1    2300 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 5E46FD4C
P 2200 7400
F 0 "#PWR017" H 2200 7150 50  0001 C CNN
F 1 "GND" H 2205 7227 50  0000 C CNN
F 2 "" H 2200 7400 50  0001 C CNN
F 3 "" H 2200 7400 50  0001 C CNN
	1    2200 7400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5E47022E
P 2050 6550
F 0 "#PWR016" H 2050 6300 50  0001 C CNN
F 1 "GND" H 2055 6377 50  0000 C CNN
F 2 "" H 2050 6550 50  0001 C CNN
F 3 "" H 2050 6550 50  0001 C CNN
	1    2050 6550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR018
U 1 1 5E47052D
P 2900 6800
F 0 "#PWR018" H 2900 6650 50  0001 C CNN
F 1 "+3.3V" H 2915 6973 50  0000 C CNN
F 2 "" H 2900 6800 50  0001 C CNN
F 3 "" H 2900 6800 50  0001 C CNN
	1    2900 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 7300 2200 7300
Wire Wire Line
	2200 7300 2200 7400
Wire Wire Line
	2100 6900 2300 6900
Connection ~ 2300 6900
Wire Wire Line
	2300 6900 2500 6900
Wire Wire Line
	2050 6550 2300 6550
Wire Wire Line
	2300 6550 2300 6600
Wire Wire Line
	2800 6900 2900 6900
Wire Wire Line
	2900 6900 2900 6800
Wire Wire Line
	2100 7100 3450 7100
Entry Wire Line
	3450 7100 3550 7200
Text Label 2900 7100 0    50   ~ 0
GPIO26
$Comp
L Interface_Optical_:TSOP43xx U4
U 1 1 5E4090FD
P 1700 7100
F 0 "U4" H 1688 7525 50  0000 C CNN
F 1 "TSOP43xx" H 1688 7434 50  0000 C CNN
F 2 "OptoDevice:Vishay_MOLD-3Pin" H 1650 6725 50  0001 C CNN
F 3 "http://www.vishay.com/docs/82460/tsop45.pdf" H 2350 7400 50  0001 C CNN
F 4 "TSOP4138" H -200 200 50  0001 C CNN "Description"
F 5 "Vishay" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "708-5109" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    1700 7100
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Audio_:SSM2518 U6
U 1 1 5E40983E
P 8250 1650
F 0 "U6" H 8250 2200 50  0000 C CNN
F 1 "SSM2518" H 7900 2200 50  0000 C CNN
F 2 "Package_CSP_:LFCSP-20-1EP_4x4mm_P0.5mm_EP2.5x2.5mm_ThermalVias" H 8250 1600 50  0001 C CNN
F 3 "https://www.analog.com/media/en/technical-documentation/data-sheets/SSM2518.pdf" H 8200 1650 50  0001 C CNN
F 4 "SSM2518CPZ" H -200 200 50  0001 C CNN "Description"
F 5 "Analog Devices" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "820-9413" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    8250 1650
	1    0    0    -1  
$EndComp
Wire Bus Line
	3550 3500 6450 3500
Wire Wire Line
	7750 1250 6550 1250
Entry Wire Line
	6450 1150 6550 1250
Wire Wire Line
	7750 1350 6550 1350
Entry Wire Line
	6450 1250 6550 1350
Wire Wire Line
	7750 1450 6550 1450
Entry Wire Line
	6450 1350 6550 1450
Wire Wire Line
	7750 1550 6550 1550
Entry Wire Line
	6450 1450 6550 1550
Wire Wire Line
	7750 1750 6550 1750
Entry Wire Line
	6450 1650 6550 1750
Wire Wire Line
	7750 1850 6550 1850
Entry Wire Line
	6450 1750 6550 1850
$Comp
L Device:C C5
U 1 1 5E447DF7
P 7600 850
F 0 "C5" H 7715 896 50  0000 L CNN
F 1 "100nF" H 7715 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 7638 700 50  0001 C CNN
F 3 "~" H 7600 850 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 10950 1300 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 10950 1300 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 10950 1300 50  0001 C CNN "Vendor"
F 7 "766-5648" H 10950 1300 50  0001 C CNN "Vendor Part Number"
	1    7600 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 5E44DB20
P 8850 850
F 0 "C6" H 8965 896 50  0000 L CNN
F 1 "100nF" H 8965 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8888 700 50  0001 C CNN
F 3 "~" H 8850 850 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 12200 1300 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 12200 1300 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 12200 1300 50  0001 C CNN "Vendor"
F 7 "766-5648" H 12200 1300 50  0001 C CNN "Vendor Part Number"
	1    8850 850 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5E44E120
P 7600 1000
F 0 "#PWR020" H 7600 750 50  0001 C CNN
F 1 "GND" H 7605 827 50  0000 C CNN
F 2 "" H 7600 1000 50  0001 C CNN
F 3 "" H 7600 1000 50  0001 C CNN
	1    7600 1000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 5E44E567
P 8850 1100
F 0 "#PWR022" H 8850 850 50  0001 C CNN
F 1 "GND" H 8855 927 50  0000 C CNN
F 2 "" H 8850 1100 50  0001 C CNN
F 3 "" H 8850 1100 50  0001 C CNN
	1    8850 1100
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR019
U 1 1 5E453C15
P 7600 700
F 0 "#PWR019" H 7600 550 50  0001 C CNN
F 1 "+3V3" H 7615 873 50  0000 C CNN
F 2 "" H 7600 700 50  0001 C CNN
F 3 "" H 7600 700 50  0001 C CNN
	1    7600 700 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 700  8150 700 
Wire Wire Line
	8150 700  8150 1050
Connection ~ 7600 700 
Wire Wire Line
	8450 1050 8450 700 
Wire Wire Line
	8450 700  8550 700 
Wire Wire Line
	8550 1050 8550 700 
Connection ~ 8550 700 
Wire Wire Line
	8550 700  8850 700 
$Comp
L power:GND #PWR021
U 1 1 5E464B17
P 8150 2350
F 0 "#PWR021" H 8150 2100 50  0001 C CNN
F 1 "GND" H 8155 2177 50  0000 C CNN
F 2 "" H 8150 2350 50  0001 C CNN
F 3 "" H 8150 2350 50  0001 C CNN
	1    8150 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 1950 7650 1950
Wire Wire Line
	7650 1950 7650 2050
Wire Wire Line
	7650 2300 8150 2300
Wire Wire Line
	8550 2300 8550 2250
Wire Wire Line
	7750 2050 7650 2050
Connection ~ 7650 2050
Wire Wire Line
	7650 2050 7650 2300
Wire Wire Line
	8150 2250 8150 2300
Connection ~ 8150 2300
Wire Wire Line
	8150 2300 8250 2300
Wire Wire Line
	8150 2350 8150 2300
Wire Wire Line
	8250 2250 8250 2300
Connection ~ 8250 2300
Wire Wire Line
	8250 2300 8350 2300
Wire Wire Line
	8350 2250 8350 2300
Connection ~ 8350 2300
Wire Wire Line
	8350 2300 8450 2300
Wire Wire Line
	8450 2250 8450 2300
Connection ~ 8450 2300
Wire Wire Line
	8450 2300 8550 2300
Text Label 6600 1350 0    50   ~ 0
SCL1
Text Label 6600 1450 0    50   ~ 0
SDA1
$Comp
L Device:CP C7
U 1 1 5E495D2E
P 9350 850
F 0 "C7" H 9468 896 50  0000 L CNN
F 1 "4u7/25V" H 9468 805 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_4x5.4" H 9388 700 50  0001 C CNN
F 3 "~" H 9350 850 50  0001 C CNN
F 4 "UWX1E4R7MCL1GB" H -200 200 50  0001 C CNN "Description"
F 5 "Nichicon WX" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "520-1703" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    9350 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 700  9350 700 
Connection ~ 8850 700 
$Comp
L Connector:TestPoint TP3
U 1 1 5E40D73C
P 7650 1650
F 0 "TP3" V 7700 1800 50  0000 L CNN
F 1 "TestPoint" V 7600 1800 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 7850 1650 50  0001 C CNN
F 3 "~" H 7850 1650 50  0001 C CNN
	1    7650 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7650 1650 7750 1650
$Comp
L power:+5V #PWR0101
U 1 1 5E414DD9
P 8550 700
F 0 "#PWR0101" H 8550 550 50  0001 C CNN
F 1 "+5V" H 8565 873 50  0000 C CNN
F 2 "" H 8550 700 50  0001 C CNN
F 3 "" H 8550 700 50  0001 C CNN
	1    8550 700 
	1    0    0    -1  
$EndComp
$Comp
L Device:Ferrite_Bead_Small FB1
U 1 1 5E418166
P 9050 1450
F 0 "FB1" V 9000 1300 50  0000 C CNN
F 1 "Ferrite_Bead_Small" V 9000 2000 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" V 8980 1450 50  0001 C CNN
F 3 "~" H 9050 1450 50  0001 C CNN
F 4 "BLM18KG471SN1D" H -200 200 50  0001 C CNN "Description"
F 5 "Murata" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "747-4726" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    9050 1450
	0    1    1    0   
$EndComp
$Comp
L Device:Ferrite_Bead_Small FB3
U 1 1 5E418B71
P 9150 1550
F 0 "FB3" V 9100 1300 50  0000 C CNN
F 1 "Ferrite_Bead_Small" V 9100 2000 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" V 9080 1550 50  0001 C CNN
F 3 "~" H 9150 1550 50  0001 C CNN
F 4 "BLM18KG471SN1D" H -200 200 50  0001 C CNN "Description"
F 5 "Murata" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "747-4726" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    9150 1550
	0    1    1    0   
$EndComp
$Comp
L Device:Ferrite_Bead_Small FB2
U 1 1 5E418EBA
P 9050 1750
F 0 "FB2" V 9000 1600 50  0000 C CNN
F 1 "Ferrite_Bead_Small" V 9000 2300 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" V 8980 1750 50  0001 C CNN
F 3 "~" H 9050 1750 50  0001 C CNN
F 4 "BLM18KG471SN1D" H -200 200 50  0001 C CNN "Description"
F 5 "Murata" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "747-4726" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    9050 1750
	0    1    1    0   
$EndComp
$Comp
L Device:Ferrite_Bead_Small FB4
U 1 1 5E4193C5
P 9150 1850
F 0 "FB4" V 9100 1600 50  0000 C CNN
F 1 "Ferrite_Bead_Small" V 9100 2300 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" V 9080 1850 50  0001 C CNN
F 3 "~" H 9150 1850 50  0001 C CNN
F 4 "BLM18KG471SN1D" H -200 200 50  0001 C CNN "Description"
F 5 "Murata" H -200 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H -200 200 50  0001 C CNN "Vendor"
F 7 "747-4726" H -200 200 50  0001 C CNN "Vendor Part Number"
	1    9150 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	8750 1450 8950 1450
Wire Wire Line
	8750 1550 9050 1550
Wire Wire Line
	8750 1750 8950 1750
Wire Wire Line
	8750 1850 9050 1850
Text Label 6600 1550 0    50   ~ 0
GPIO18
Text Label 6600 1750 0    50   ~ 0
GPIO19
Text Label 6600 1850 0    50   ~ 0
GPIO21
Text Label 6600 1250 0    50   ~ 0
GPIO27
$Comp
L Device:C C8
U 1 1 5E476015
P 10050 2100
F 0 "C8" V 10000 2150 50  0000 L CNN
F 1 "470p" V 10000 1850 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10088 1950 50  0001 C CNN
F 3 "~" H 10050 2100 50  0001 C CNN
F 4 "Ceramic capacitor 470pF/50V  0805CG471J9BB" H 13400 2550 50  0001 C CNN "Description"
F 5 "EPCOS" H 13400 2550 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 13400 2550 50  0001 C CNN "Vendor"
F 7 "378-700" H 13400 2550 50  0001 C CNN "Vendor Part Number"
	1    10050 2100
	0    1    1    0   
$EndComp
$Comp
L Device:C C9
U 1 1 5E485EB4
P 10050 2300
F 0 "C9" V 10000 2350 50  0000 L CNN
F 1 "470p" V 10000 2050 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10088 2150 50  0001 C CNN
F 3 "~" H 10050 2300 50  0001 C CNN
F 4 "Ceramic capacitor 470pF/50V  0805CG471J9BB" H 13400 2750 50  0001 C CNN "Description"
F 5 "EPCOS" H 13400 2750 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 13400 2750 50  0001 C CNN "Vendor"
F 7 "378-700" H 13400 2750 50  0001 C CNN "Vendor Part Number"
	1    10050 2300
	0    1    1    0   
$EndComp
$Comp
L Device:C C10
U 1 1 5E4864D3
P 10050 1250
F 0 "C10" V 10000 1300 50  0000 L CNN
F 1 "470p" V 10000 1000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10088 1100 50  0001 C CNN
F 3 "~" H 10050 1250 50  0001 C CNN
F 4 "Ceramic capacitor 470pF/50V  0805CG471J9BB" H 13400 1700 50  0001 C CNN "Description"
F 5 "EPCOS" H 13400 1700 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 13400 1700 50  0001 C CNN "Vendor"
F 7 "378-700" H 13400 1700 50  0001 C CNN "Vendor Part Number"
	1    10050 1250
	0    1    1    0   
$EndComp
$Comp
L Device:C C11
U 1 1 5E486741
P 10050 1050
F 0 "C11" V 10000 1100 50  0000 L CNN
F 1 "470p" V 10000 800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10088 900 50  0001 C CNN
F 3 "~" H 10050 1050 50  0001 C CNN
F 4 "Ceramic capacitor 470pF/50V  0805CG471J9BB" H 13400 1500 50  0001 C CNN "Description"
F 5 "EPCOS" H 13400 1500 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 13400 1500 50  0001 C CNN "Vendor"
F 7 "378-700" H 13400 1500 50  0001 C CNN "Vendor Part Number"
	1    10050 1050
	0    1    1    0   
$EndComp
Wire Wire Line
	9900 2100 9700 2100
Wire Wire Line
	10200 2100 10200 1850
Connection ~ 10200 1850
Wire Wire Line
	10200 2300 10300 2300
Wire Wire Line
	10300 2300 10300 1750
Connection ~ 10300 1750
Wire Wire Line
	10300 1750 10350 1750
Wire Wire Line
	10200 1850 10350 1850
Entry Wire Line
	3450 1750 3550 1850
Wire Wire Line
	2750 1750 3450 1750
Entry Wire Line
	3450 3150 3550 3250
Wire Wire Line
	2750 3150 3450 3150
Text Label 2900 3150 0    50   ~ 0
GPIO21
Text Label 2900 1750 0    50   ~ 0
GPIO18
Wire Wire Line
	1550 2950 850  2950
Entry Wire Line
	750  2850 850  2950
Text Label 900  2950 0    50   ~ 0
GPIO19
$Comp
L Graphic:Logo_Open_Hardware_Small Logo2
U 1 1 5E42B44F
P 10450 6350
F 0 "Logo2" H 10650 6200 50  0000 L CNN
F 1 " " H 10450 6125 50  0001 C CNN
F 2 "Symbol_:HTL-Steyr-Logo_4x4mm_Copper" H 10450 6350 50  0001 C CNN
F 3 "~" H 10450 6350 50  0001 C CNN
	1    10450 6350
	1    0    0    -1  
$EndComp
$Comp
L Graphic:Logo_Open_Hardware_Small Logo3
U 1 1 5E435930
P 10450 6350
F 0 "Logo3" H 10650 6200 50  0000 L CNN
F 1 " " H 10450 6125 50  0001 C CNN
F 2 "Symbol_:HTL-Steyr-Logo_4x4mm_Copper" H 10450 6350 50  0001 C CNN
F 3 "~" H 10450 6350 50  0001 C CNN
	1    10450 6350
	1    0    0    -1  
$EndComp
$Comp
L Graphic:Logo_Open_Hardware_Small Logo4
U 1 1 5E435936
P 10450 6350
F 0 "Logo4" H 10650 6200 50  0000 L CNN
F 1 " " H 10450 6125 50  0001 C CNN
F 2 "Symbol_:HTL-Steyr-Logo_4x4mm_SilkScreen" H 10450 6350 50  0001 C CNN
F 3 "~" H 10450 6350 50  0001 C CNN
	1    10450 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 1650 7650 1950
Connection ~ 7650 1650
Connection ~ 7650 1950
Text Label 2900 6000 0    50   ~ 0
GPIO23
Text Label 3700 2500 0    50   ~ 0
GPIO22
Text Label 3700 3450 0    50   ~ 0
GPIO04
$Comp
L RF_FM_DAB:Si4688-A10-GM U7
U 1 1 5E4ED1A6
P 5900 5350
F 0 "U7" H 5350 6200 50  0000 C CNN
F 1 "Si4688-A10-GM" H 5900 5400 50  0000 C CNN
F 2 "Package_DFN_QFN_:QFN-48-1EP_7x7mm_P0.5mm_EP5.3x5.3mm_ThermalVias" H 4900 4500 50  0001 L CNN
F 3 "https://www.silabs.com/documents/public/data-shorts/Si4688-short.pdf" H 5400 4650 50  0001 C CNN
F 4 "SI4688-A10-GM" H 300 200 50  0001 C CNN "Description"
F 5 "Silicon Labs" H 300 200 50  0001 C CNN "Manufacturer"
F 6 "Mouser" H 300 200 50  0001 C CNN "Vendor"
F 7 "634-SI4688-A10-GM" H 300 200 50  0001 C CNN "Vendor Part Number"
	1    5900 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR024
U 1 1 5E4D9CC2
P 6000 6450
F 0 "#PWR024" H 6000 6200 50  0001 C CNN
F 1 "GND" H 6005 6277 50  0000 C CNN
F 2 "" H 6000 6450 50  0001 C CNN
F 3 "" H 6000 6450 50  0001 C CNN
	1    6000 6450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 6400 6400 6300
Wire Wire Line
	6000 6300 6000 6400
Connection ~ 6000 6400
Wire Wire Line
	6000 6400 6100 6400
Wire Wire Line
	6000 6400 6000 6450
Connection ~ 6100 6400
Wire Wire Line
	6100 6400 6200 6400
Wire Wire Line
	6100 6300 6100 6400
Wire Wire Line
	6200 6300 6200 6400
Connection ~ 6200 6400
Wire Wire Line
	6200 6400 6300 6400
Wire Wire Line
	6300 6300 6300 6400
Connection ~ 6300 6400
Wire Wire Line
	6300 6400 6400 6400
$Comp
L Memory_Flash_:SST25VF016B-50-4x-S2Ax U8
U 1 1 5E503327
P 8450 5750
F 0 "U8" H 8150 6100 50  0000 L CNN
F 1 "SST25VF016B-50-4x-S2Ax" H 8500 5400 50  0000 L CNN
F 2 "Package_SO:SOIC-8_5.275x5.275mm_P1.27mm" H 8450 5050 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/20005044C.pdf" H 8400 6300 50  0001 C CNN
F 4 "SST25VF016B-50-4C-S2AF" H 800 200 50  0001 C CNN "Description"
F 5 "Microchip" H 800 200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 800 200 50  0001 C CNN "Vendor"
F 7 "798-5371" H 800 200 50  0001 C CNN "Vendor Part Number"
	1    8450 5750
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR026
U 1 1 5E5195AD
P 8450 5300
F 0 "#PWR026" H 8450 5150 50  0001 C CNN
F 1 "+3.3V" H 8465 5473 50  0000 C CNN
F 2 "" H 8450 5300 50  0001 C CNN
F 3 "" H 8450 5300 50  0001 C CNN
	1    8450 5300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR027
U 1 1 5E519CC5
P 8450 6200
F 0 "#PWR027" H 8450 5950 50  0001 C CNN
F 1 "GND" H 8455 6027 50  0000 C CNN
F 2 "" H 8450 6200 50  0001 C CNN
F 3 "" H 8450 6200 50  0001 C CNN
	1    8450 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 6200 8450 6150
Wire Wire Line
	8450 5350 8450 5300
$Comp
L Device:C C12
U 1 1 5E52FA04
P 9200 5350
F 0 "C12" H 9315 5396 50  0000 L CNN
F 1 "100nF" H 9315 5305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9238 5200 50  0001 C CNN
F 3 "~" H 9200 5350 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 16650 2200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 16650 2200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16650 2200 50  0001 C CNN "Vendor"
F 7 "766-5648" H 16650 2200 50  0001 C CNN "Vendor Part Number"
	1    9200 5350
	0    1    1    0   
$EndComp
Wire Wire Line
	9050 5350 8450 5350
Connection ~ 8450 5350
$Comp
L power:GND #PWR028
U 1 1 5E53BD88
P 9350 5350
F 0 "#PWR028" H 9350 5100 50  0001 C CNN
F 1 "GND" H 9355 5177 50  0000 C CNN
F 2 "" H 9350 5350 50  0001 C CNN
F 3 "" H 9350 5350 50  0001 C CNN
	1    9350 5350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8450 5350 8000 5350
Wire Wire Line
	8000 5350 8000 5950
Wire Wire Line
	8000 5950 8050 5950
$Comp
L Device:R R9
U 1 1 5E54772E
P 7850 6050
F 0 "R9" H 7780 6004 50  0000 R CNN
F 1 "3k9" H 7780 6095 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7780 6050 50  0001 C CNN
F 3 "~" H 7850 6050 50  0001 C CNN
F 4 "804-6492" H 2650 1450 50  0001 C CNN "Description"
F 5 "RS Pro" H 2650 1450 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2650 1450 50  0001 C CNN "Vendor"
F 7 "804-6492" H 2650 1450 50  0001 C CNN "Vendor Part Number"
	1    7850 6050
	-1   0    0    1   
$EndComp
Wire Wire Line
	8050 5850 7850 5850
Wire Wire Line
	7850 5850 7850 5900
$Comp
L power:GND #PWR025
U 1 1 5E553637
P 7850 6200
F 0 "#PWR025" H 7850 5950 50  0001 C CNN
F 1 "GND" H 7855 6027 50  0000 C CNN
F 2 "" H 7850 6200 50  0001 C CNN
F 3 "" H 7850 6200 50  0001 C CNN
	1    7850 6200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 DNP_J6
U 1 1 5E5539F1
P 7700 6050
F 0 "DNP_J6" H 7780 6042 50  0000 L CNN
F 1 "Conn_01x02" H 7550 5850 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 7700 6050 50  0001 C CNN
F 3 "~" H 7700 6050 50  0001 C CNN
	1    7700 6050
	0    1    1    0   
$EndComp
Wire Wire Line
	7700 5850 7850 5850
Connection ~ 7850 5850
Wire Wire Line
	7600 5850 7600 5350
Wire Wire Line
	7600 5350 8000 5350
Connection ~ 8000 5350
Wire Wire Line
	7500 5850 7500 5550
Wire Wire Line
	7500 5550 8050 5550
Wire Wire Line
	7400 5750 7400 5550
Wire Wire Line
	7400 5750 8050 5750
Wire Wire Line
	8850 5550 9000 5550
Wire Wire Line
	9000 5550 9000 6450
Wire Wire Line
	9000 6450 7300 6450
Wire Wire Line
	7300 6450 7300 5750
$Comp
L Regulator_Linear:AP2127N-1.8 U9
U 1 1 5E50F80D
P 10250 3600
F 0 "U9" H 10250 3842 50  0000 C CNN
F 1 "AP2127N-1.8" H 10250 3751 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10250 3825 50  0001 C CIN
F 3 "https://www.diodes.com/assets/Datasheets/AP2127.pdf" H 10250 3600 50  0001 C CNN
F 4 "AP2127N-1.8TRG1" H 2000 -150 50  0001 C CNN "Description"
F 5 "DiodesZetex" H 2000 -150 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2000 -150 50  0001 C CNN "Vendor"
F 7 "828-7369" H 2000 -150 50  0001 C CNN "Vendor Part Number"
	1    10250 3600
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR030
U 1 1 5E5131A7
P 10700 3600
F 0 "#PWR030" H 10700 3450 50  0001 C CNN
F 1 "+3.3V" H 10715 3773 50  0000 C CNN
F 2 "" H 10700 3600 50  0001 C CNN
F 3 "" H 10700 3600 50  0001 C CNN
	1    10700 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C13
U 1 1 5E51368D
P 10700 3750
F 0 "C13" H 10815 3796 50  0000 L CNN
F 1 "100nF" H 10815 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10738 3600 50  0001 C CNN
F 3 "~" H 10700 3750 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 18150 600 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 18150 600 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 18150 600 50  0001 C CNN "Vendor"
F 7 "766-5648" H 18150 600 50  0001 C CNN "Vendor Part Number"
	1    10700 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 3900 10700 3900
Wire Wire Line
	10550 3600 10700 3600
Connection ~ 10700 3600
$Comp
L power:GND #PWR029
U 1 1 5E52EB8B
P 10250 3900
F 0 "#PWR029" H 10250 3650 50  0001 C CNN
F 1 "GND" H 10255 3727 50  0000 C CNN
F 2 "" H 10250 3900 50  0001 C CNN
F 3 "" H 10250 3900 50  0001 C CNN
	1    10250 3900
	1    0    0    -1  
$EndComp
Connection ~ 10250 3900
$Comp
L Device:C C15
U 1 1 5E51A038
P 9650 3750
F 0 "C15" H 9765 3796 50  0000 L CNN
F 1 "1uF" H 9765 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9688 3600 50  0001 C CNN
F 3 "~" H 9650 3750 50  0001 C CNN
F 4 "Ceramic capacitor 1uF/25V C0805C105K3RACAUTO" H 17100 600 50  0001 C CNN "Description"
F 5 "KEMET" H 17100 600 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 17100 600 50  0001 C CNN "Vendor"
F 7 "133-5653" H 17100 600 50  0001 C CNN "Vendor Part Number"
	1    9650 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:C C14
U 1 1 5E51AA1C
P 9200 3750
F 0 "C14" H 9315 3796 50  0000 L CNN
F 1 "2n2" H 9315 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9238 3600 50  0001 C CNN
F 3 "~" H 9200 3750 50  0001 C CNN
F 4 "Ceramic capacitor 2n2F/50V  C0805C222K5RACTU " H 16650 600 50  0001 C CNN "Description"
F 5 "KEMET" H 16650 600 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16650 600 50  0001 C CNN "Vendor"
F 7 "264-4359" H 16650 600 50  0001 C CNN "Vendor Part Number"
	1    9200 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9950 3600 9650 3600
Wire Wire Line
	9650 3600 9200 3600
Connection ~ 9650 3600
Wire Wire Line
	9200 3900 9650 3900
$Comp
L Regulator_Linear:AP2127N-1.8 U10
U 1 1 5E59E04B
P 10250 2800
F 0 "U10" H 10250 3042 50  0000 C CNN
F 1 "AP2127N-1.8" H 10250 2951 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10250 3025 50  0001 C CIN
F 3 "https://www.diodes.com/assets/Datasheets/AP2127.pdf" H 10250 2800 50  0001 C CNN
F 4 "AP2127N-1.8TRG1" H 2000 -950 50  0001 C CNN "Description"
F 5 "DiodesZetex" H 2000 -950 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2000 -950 50  0001 C CNN "Vendor"
F 7 "828-7369" H 2000 -950 50  0001 C CNN "Vendor Part Number"
	1    10250 2800
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR033
U 1 1 5E59E051
P 10700 2800
F 0 "#PWR033" H 10700 2650 50  0001 C CNN
F 1 "+3.3V" H 10715 2973 50  0000 C CNN
F 2 "" H 10700 2800 50  0001 C CNN
F 3 "" H 10700 2800 50  0001 C CNN
	1    10700 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C20
U 1 1 5E59E05B
P 10700 2950
F 0 "C20" H 10815 2996 50  0000 L CNN
F 1 "100nF" H 10815 2905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10738 2800 50  0001 C CNN
F 3 "~" H 10700 2950 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 18150 -200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 18150 -200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 18150 -200 50  0001 C CNN "Vendor"
F 7 "766-5648" H 18150 -200 50  0001 C CNN "Vendor Part Number"
	1    10700 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 3100 10700 3100
Wire Wire Line
	10550 2800 10700 2800
Connection ~ 10700 2800
$Comp
L power:GND #PWR031
U 1 1 5E59E064
P 10250 3100
F 0 "#PWR031" H 10250 2850 50  0001 C CNN
F 1 "GND" H 10255 2927 50  0000 C CNN
F 2 "" H 10250 3100 50  0001 C CNN
F 3 "" H 10250 3100 50  0001 C CNN
	1    10250 3100
	1    0    0    -1  
$EndComp
Connection ~ 10250 3100
$Comp
L Device:C C18
U 1 1 5E59E06F
P 9650 2950
F 0 "C18" H 9765 2996 50  0000 L CNN
F 1 "1uF" H 9765 2905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9688 2800 50  0001 C CNN
F 3 "~" H 9650 2950 50  0001 C CNN
F 4 "Ceramic capacitor 1uF/25V C0805C105K3RACAUTO" H 17100 -200 50  0001 C CNN "Description"
F 5 "KEMET" H 17100 -200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 17100 -200 50  0001 C CNN "Vendor"
F 7 "133-5653" H 17100 -200 50  0001 C CNN "Vendor Part Number"
	1    9650 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C16
U 1 1 5E59E079
P 9200 2950
F 0 "C16" H 9315 2996 50  0000 L CNN
F 1 "2n2" H 9315 2905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9238 2800 50  0001 C CNN
F 3 "~" H 9200 2950 50  0001 C CNN
F 4 "Ceramic capacitor 2n2F/50V  C0805C222K5RACTU " H 16650 -200 50  0001 C CNN "Description"
F 5 "KEMET" H 16650 -200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16650 -200 50  0001 C CNN "Vendor"
F 7 "264-4359" H 16650 -200 50  0001 C CNN "Vendor Part Number"
	1    9200 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9950 2800 9650 2800
Connection ~ 9650 2800
Wire Wire Line
	9200 3100 9650 3100
$Comp
L Regulator_Linear:AP2127N-1.8 U11
U 1 1 5E5C3D93
P 10250 4400
F 0 "U11" H 10250 4642 50  0000 C CNN
F 1 "AP2127N-1.8" H 10250 4551 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10250 4625 50  0001 C CIN
F 3 "https://www.diodes.com/assets/Datasheets/AP2127.pdf" H 10250 4400 50  0001 C CNN
F 4 "AP2127N-1.8TRG1" H 2000 650 50  0001 C CNN "Description"
F 5 "DiodesZetex" H 2000 650 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2000 650 50  0001 C CNN "Vendor"
F 7 "828-7369" H 2000 650 50  0001 C CNN "Vendor Part Number"
	1    10250 4400
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR034
U 1 1 5E5C3D99
P 10700 4400
F 0 "#PWR034" H 10700 4250 50  0001 C CNN
F 1 "+3.3V" H 10715 4573 50  0000 C CNN
F 2 "" H 10700 4400 50  0001 C CNN
F 3 "" H 10700 4400 50  0001 C CNN
	1    10700 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C21
U 1 1 5E5C3DA3
P 10700 4550
F 0 "C21" H 10815 4596 50  0000 L CNN
F 1 "100nF" H 10815 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10738 4400 50  0001 C CNN
F 3 "~" H 10700 4550 50  0001 C CNN
F 4 "Ceramic capacitor 100nF/25V CL21B104KACNNNC" H 18150 1400 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 18150 1400 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 18150 1400 50  0001 C CNN "Vendor"
F 7 "766-5648" H 18150 1400 50  0001 C CNN "Vendor Part Number"
	1    10700 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 4700 10700 4700
Wire Wire Line
	10550 4400 10700 4400
Connection ~ 10700 4400
$Comp
L power:GND #PWR032
U 1 1 5E5C3DAC
P 10250 4700
F 0 "#PWR032" H 10250 4450 50  0001 C CNN
F 1 "GND" H 10255 4527 50  0000 C CNN
F 2 "" H 10250 4700 50  0001 C CNN
F 3 "" H 10250 4700 50  0001 C CNN
	1    10250 4700
	1    0    0    -1  
$EndComp
Connection ~ 10250 4700
$Comp
L Device:C C19
U 1 1 5E5C3DB7
P 9650 4550
F 0 "C19" H 9765 4596 50  0000 L CNN
F 1 "1uF" H 9765 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9688 4400 50  0001 C CNN
F 3 "~" H 9650 4550 50  0001 C CNN
F 4 "Ceramic capacitor 1uF/25V C0805C105K3RACAUTO" H 17100 1400 50  0001 C CNN "Description"
F 5 "KEMET" H 17100 1400 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 17100 1400 50  0001 C CNN "Vendor"
F 7 "133-5653" H 17100 1400 50  0001 C CNN "Vendor Part Number"
	1    9650 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C17
U 1 1 5E5C3DC1
P 9200 4550
F 0 "C17" H 9315 4596 50  0000 L CNN
F 1 "2n2" H 9315 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9238 4400 50  0001 C CNN
F 3 "~" H 9200 4550 50  0001 C CNN
F 4 "Ceramic capacitor 2n2F/50V  C0805C222K5RACTU " H 16650 1400 50  0001 C CNN "Description"
F 5 "KEMET" H 16650 1400 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16650 1400 50  0001 C CNN "Vendor"
F 7 "264-4359" H 16650 1400 50  0001 C CNN "Vendor Part Number"
	1    9200 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9950 4400 9650 4400
Wire Wire Line
	9650 4400 9200 4400
Connection ~ 9650 4400
Wire Wire Line
	9200 4700 9650 4700
$Comp
L Device:C C22
U 1 1 5E63AB04
P 7750 4950
F 0 "C22" V 7800 5000 50  0000 L CNN
F 1 "1uF" V 7800 4750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 7788 4800 50  0001 C CNN
F 3 "~" H 7750 4950 50  0001 C CNN
F 4 "Ceramic capacitor 1uF/25V C0805C105K3RACAUTO" H 15200 1800 50  0001 C CNN "Description"
F 5 "KEMET" H 15200 1800 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 15200 1800 50  0001 C CNN "Vendor"
F 7 "133-5653" H 15200 1800 50  0001 C CNN "Vendor Part Number"
	1    7750 4950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR035
U 1 1 5E63B77C
P 7900 4950
F 0 "#PWR035" H 7900 4700 50  0001 C CNN
F 1 "GND" H 7905 4777 50  0000 C CNN
F 2 "" H 7900 4950 50  0001 C CNN
F 3 "" H 7900 4950 50  0001 C CNN
	1    7900 4950
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint_Flag TP4
U 1 1 5E64D89E
P 7650 4750
F 0 "TP4" H 7910 4844 50  0000 L CNN
F 1 "TestPoint_Flag" H 8050 4850 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 7850 4750 50  0001 C CNN
F 3 "~" H 7850 4750 50  0001 C CNN
	1    7650 4750
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_Flag TP5
U 1 1 5E64DD11
P 7650 4850
F 0 "TP5" H 7910 4944 50  0000 L CNN
F 1 "TestPoint_Flag" H 8050 4950 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 7850 4850 50  0001 C CNN
F 3 "~" H 7850 4850 50  0001 C CNN
	1    7650 4850
	1    0    0    -1  
$EndComp
Connection ~ 8550 2300
Wire Wire Line
	10300 1450 10300 1050
Wire Wire Line
	10300 1050 10200 1050
Connection ~ 10300 1450
Wire Wire Line
	10200 1250 10200 1550
Connection ~ 10200 1550
Wire Wire Line
	10200 1550 10350 1550
Wire Wire Line
	10300 1450 10350 1450
Wire Wire Line
	9150 1450 10300 1450
Wire Wire Line
	9250 1550 10200 1550
Wire Wire Line
	9150 1750 10300 1750
Wire Wire Line
	9250 1850 10200 1850
Wire Wire Line
	8550 2300 9700 2300
Wire Wire Line
	9700 2100 9700 2300
Connection ~ 9700 2300
Wire Wire Line
	9700 2300 9900 2300
Wire Wire Line
	9700 1250 9900 1250
Wire Wire Line
	9900 1050 9700 1050
Connection ~ 9700 1050
Wire Wire Line
	9700 1050 9700 1250
Wire Wire Line
	9700 1050 9350 1050
Wire Wire Line
	8850 1050 8850 1100
Wire Wire Line
	8850 1000 8850 1050
Connection ~ 8850 1050
Wire Wire Line
	9350 1000 9350 1050
Connection ~ 9350 1050
Wire Wire Line
	9350 1050 8850 1050
Wire Wire Line
	6700 4750 7650 4750
Wire Wire Line
	6700 4850 7650 4850
Wire Wire Line
	6700 4950 7600 4950
Wire Wire Line
	6700 5550 7400 5550
Wire Wire Line
	6700 5650 8050 5650
Wire Wire Line
	6700 5750 7300 5750
Wire Wire Line
	6700 5850 7500 5850
$Comp
L Device:R R11
U 1 1 5E9533BB
P 7350 4300
F 0 "R11" V 7400 4550 50  0000 R CNN
F 1 "3k9" V 7400 4150 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7280 4300 50  0001 C CNN
F 3 "~" H 7350 4300 50  0001 C CNN
F 4 "804-6492" H 2150 -300 50  0001 C CNN "Description"
F 5 "RS Pro" H 2150 -300 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2150 -300 50  0001 C CNN "Vendor"
F 7 "804-6492" H 2150 -300 50  0001 C CNN "Vendor Part Number"
	1    7350 4300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R10
U 1 1 5E9545B7
P 7250 4300
F 0 "R10" V 7300 4550 50  0000 R CNN
F 1 "3k9" V 7300 4150 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7180 4300 50  0001 C CNN
F 3 "~" H 7250 4300 50  0001 C CNN
F 4 "804-6492" H 2050 -300 50  0001 C CNN "Description"
F 5 "RS Pro" H 2050 -300 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2050 -300 50  0001 C CNN "Vendor"
F 7 "804-6492" H 2050 -300 50  0001 C CNN "Vendor Part Number"
	1    7250 4300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R12
U 1 1 5E955565
P 7450 4300
F 0 "R12" V 7400 4150 50  0000 R CNN
F 1 "100" V 7400 4600 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7380 4300 50  0001 C CNN
F 3 "~" H 7450 4300 50  0001 C CNN
F 4 "CRG0805F100R" H 2250 -300 50  0001 C CNN "Description"
F 5 "TE Connectivity" H 2250 -300 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 2250 -300 50  0001 C CNN "Vendor"
F 7 "134-5462" H 2250 -300 50  0001 C CNN "Vendor Part Number"
	1    7450 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 5350 7450 5350
Wire Wire Line
	7450 5350 7450 4450
Wire Wire Line
	6700 5250 7350 5250
Wire Wire Line
	7350 5250 7350 4450
Wire Wire Line
	6700 5150 7250 5150
Wire Wire Line
	7250 5150 7250 4450
Entry Wire Line
	6450 3400 6550 3500
Entry Wire Line
	6450 3300 6550 3400
Entry Wire Line
	6450 3200 6550 3300
Wire Wire Line
	6550 3500 7250 3500
Wire Wire Line
	6550 3400 7350 3400
Wire Wire Line
	6550 3300 7450 3300
Text Label 6600 3300 0    50   ~ 0
GPIO20
Text Label 6600 3500 0    50   ~ 0
GPIO18
Text Label 6600 3400 0    50   ~ 0
GPIO19
Wire Wire Line
	5100 5350 3650 5350
Entry Wire Line
	3550 5250 3650 5350
Wire Wire Line
	5100 5250 3650 5250
Entry Wire Line
	3550 5150 3650 5250
Text Label 3700 5350 0    50   ~ 0
GPIO16
Text Label 3700 5250 0    50   ~ 0
GPIO13
Wire Wire Line
	6300 4400 6300 3900
Wire Wire Line
	6300 3900 8650 3900
Connection ~ 9200 3900
Connection ~ 9200 4700
Wire Wire Line
	6200 4400 6200 4000
Wire Wire Line
	6100 4400 6100 3800
Connection ~ 9200 3600
Connection ~ 9200 2800
Wire Wire Line
	7250 3500 7250 4150
Wire Wire Line
	7350 3400 7350 4150
Wire Wire Line
	5800 3600 8300 3600
Wire Wire Line
	5800 3600 5800 4400
Wire Wire Line
	5900 3700 8400 3700
Wire Wire Line
	8800 3100 9200 3100
Connection ~ 9200 3100
Wire Wire Line
	9200 2800 9650 2800
$Comp
L Device:C C23
U 1 1 5E66CB96
P 6700 4400
F 0 "C23" H 6550 4500 50  0000 L CNN
F 1 "2n2" H 6550 4300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6738 4250 50  0001 C CNN
F 3 "~" H 6700 4400 50  0001 C CNN
F 4 "Ceramic capacitor 2n2F/50V  C0805C222K5RACTU " H 14150 1250 50  0001 C CNN "Description"
F 5 "KEMET" H 14150 1250 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 14150 1250 50  0001 C CNN "Vendor"
F 7 "264-4359" H 14150 1250 50  0001 C CNN "Vendor Part Number"
	1    6700 4400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5E66DBAD
P 6700 4550
F 0 "#PWR0102" H 6700 4300 50  0001 C CNN
F 1 "GND" H 6800 4450 50  0000 C CNN
F 2 "" H 6700 4550 50  0001 C CNN
F 3 "" H 6700 4550 50  0001 C CNN
	1    6700 4550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0103
U 1 1 5E66E03C
P 6700 4250
F 0 "#PWR0103" H 6700 4100 50  0001 C CNN
F 1 "+3.3V" H 6715 4423 50  0000 C CNN
F 2 "" H 6700 4250 50  0001 C CNN
F 3 "" H 6700 4250 50  0001 C CNN
	1    6700 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 4250 6000 4250
Connection ~ 6700 4250
Wire Wire Line
	5900 4400 5900 3700
Wire Wire Line
	6100 3800 8650 3800
Wire Wire Line
	6000 4250 6000 4400
$Comp
L Device:C C24
U 1 1 5E6EF0E3
P 8800 2950
F 0 "C24" H 8915 2996 50  0000 L CNN
F 1 "8p2" H 8915 2905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8838 2800 50  0001 C CNN
F 3 "~" H 8800 2950 50  0001 C CNN
F 4 "Ceramic capacitor 8p2F/100V 08051A8R2CAT2A" H 16250 -200 50  0001 C CNN "Description"
F 5 "AVX" H 16250 -200 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16250 -200 50  0001 C CNN "Vendor"
F 7 "136-4698" H 16250 -200 50  0001 C CNN "Vendor Part Number"
	1    8800 2950
	1    0    0    -1  
$EndComp
Connection ~ 8800 2800
Wire Wire Line
	8800 2800 9200 2800
$Comp
L Device:C C25
U 1 1 5E6EFF2B
P 8800 3750
F 0 "C25" H 8915 3796 50  0000 L CNN
F 1 "8p2" H 8915 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8838 3600 50  0001 C CNN
F 3 "~" H 8800 3750 50  0001 C CNN
F 4 "Ceramic capacitor 8p2F/100V 08051A8R2CAT2A" H 16250 600 50  0001 C CNN "Description"
F 5 "AVX" H 16250 600 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16250 600 50  0001 C CNN "Vendor"
F 7 "136-4698" H 16250 600 50  0001 C CNN "Vendor Part Number"
	1    8800 3750
	1    0    0    -1  
$EndComp
Connection ~ 8800 3900
$Comp
L Device:C C26
U 1 1 5E6F05F9
P 8800 4550
F 0 "C26" H 8915 4596 50  0000 L CNN
F 1 "8p2" H 8915 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8838 4400 50  0001 C CNN
F 3 "~" H 8800 4550 50  0001 C CNN
F 4 "Ceramic capacitor 8p2F/100V 08051A8R2CAT2A" H 16250 1400 50  0001 C CNN "Description"
F 5 "AVX" H 16250 1400 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 16250 1400 50  0001 C CNN "Vendor"
F 7 "136-4698" H 16250 1400 50  0001 C CNN "Vendor Part Number"
	1    8800 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 3600 8300 2800
Wire Wire Line
	8300 2800 8800 2800
Wire Wire Line
	8400 3700 8400 3100
Wire Wire Line
	8400 3100 8800 3100
Connection ~ 8800 3100
Wire Wire Line
	8800 3900 9200 3900
Wire Wire Line
	8800 4700 9200 4700
Wire Wire Line
	6200 4000 8800 4000
Wire Wire Line
	8800 4700 8650 4700
Wire Wire Line
	8650 4700 8650 3900
Connection ~ 8800 4700
Connection ~ 8650 3900
Wire Wire Line
	8650 3900 8800 3900
Wire Wire Line
	8800 4400 8800 4000
Connection ~ 8800 3600
Wire Wire Line
	8650 3800 8650 3600
Wire Wire Line
	8650 3600 8800 3600
Wire Wire Line
	8800 3600 9200 3600
Wire Wire Line
	9200 4400 8800 4400
Connection ~ 9200 4400
Connection ~ 8800 4400
$Comp
L Device:C C27
U 1 1 5E7C732D
P 6950 4400
F 0 "C27" H 6800 4500 50  0000 L CNN
F 1 "1uF" H 6800 4300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6988 4250 50  0001 C CNN
F 3 "~" H 6950 4400 50  0001 C CNN
F 4 "Ceramic capacitor 1uF/25V C0805C105K3RACAUTO" H 14400 1250 50  0001 C CNN "Description"
F 5 "KEMET" H 14400 1250 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 14400 1250 50  0001 C CNN "Vendor"
F 7 "133-5653" H 14400 1250 50  0001 C CNN "Vendor Part Number"
	1    6950 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 4550 6700 4550
Connection ~ 6700 4550
Wire Wire Line
	6950 4250 6700 4250
$Comp
L Device:Crystal_GND24_Small Y1
U 1 1 5E7F5058
P 5400 3850
F 0 "Y1" H 5250 4000 50  0000 L CNN
F 1 "Crystal_GND24_Small" H 5050 3700 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_Abracon_ABM8G-4Pin_3.2x2.5mm" H 5400 3850 50  0001 C CNN
F 3 "~" H 5400 3850 50  0001 C CNN
F 4 "ABM8-19.200MHZ-10-1-U-T" H 14400 1050 50  0001 C CNN "Description"
F 5 "ABRACON" H 14400 1050 50  0001 C CNN "Manufacturer"
F 6 "Mouser" H 14400 1050 50  0001 C CNN "Vendor"
F 7 "815-ABM8-19.2-T" H 14400 1050 50  0001 C CNN "Vendor Part Number"
	1    5400 3850
	-1   0    0    1   
$EndComp
Wire Wire Line
	5600 3850 5500 3850
Wire Wire Line
	5300 3850 5200 3850
Wire Wire Line
	5200 4400 5500 4400
$Comp
L power:GND #PWR023
U 1 1 5E839FC9
P 5400 4000
F 0 "#PWR023" H 5400 3750 50  0001 C CNN
F 1 "GND" H 5400 3850 50  0000 C CNN
F 2 "" H 5400 4000 50  0001 C CNN
F 3 "" H 5400 4000 50  0001 C CNN
	1    5400 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4000 5400 3950
Wire Wire Line
	5400 4000 5250 4000
Wire Wire Line
	5250 4000 5250 3750
Wire Wire Line
	5250 3750 5400 3750
Connection ~ 5400 4000
Wire Wire Line
	5200 3850 5200 4400
Wire Wire Line
	5600 3850 5600 4400
Wire Wire Line
	5000 4250 5000 5450
Connection ~ 6000 4250
Wire Wire Line
	5100 5650 3650 5650
Entry Wire Line
	3550 5550 3650 5650
Text Label 3700 5650 0    50   ~ 0
SCL1
Wire Wire Line
	5100 5750 3650 5750
Entry Wire Line
	3550 5650 3650 5750
Text Label 3700 5750 0    50   ~ 0
SDA1
Wire Wire Line
	6000 4250 5000 4250
Wire Wire Line
	5100 5450 5000 5450
Wire Wire Line
	5000 5550 5100 5550
NoConn ~ 1550 2650
NoConn ~ 1550 2750
NoConn ~ 2750 2750
Wire Wire Line
	5000 5550 5000 5850
Wire Wire Line
	5100 5850 5000 5850
Connection ~ 5000 5850
Wire Wire Line
	5000 5850 5000 6400
Wire Wire Line
	7450 3300 7450 4150
Entry Wire Line
	3450 3050 3550 3150
Wire Wire Line
	2750 3050 3450 3050
Text Label 2900 3050 0    50   ~ 0
GPIO20
NoConn ~ 2750 2450
NoConn ~ 2750 2050
Wire Wire Line
	1350 1150 1350 1250
Wire Wire Line
	1350 2050 1550 2050
Wire Wire Line
	1350 1250 1550 1250
Connection ~ 1350 1250
Wire Wire Line
	1350 1250 1350 2050
Entry Wire Line
	3450 1950 3550 2050
Wire Wire Line
	2750 1950 3450 1950
Text Label 2900 1950 0    50   ~ 0
GPIO23
NoConn ~ 1550 1750
$Comp
L Power_Protection:CM1213A-01SO D1
U 1 1 5EBDA060
P 4000 3850
F 0 "D1" H 4000 4092 50  0000 C CNN
F 1 "CM1213A-01SO" H 4000 4001 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4050 3680 50  0001 L CNN
F 3 "http://www.onsemi.com/pub_link/Collateral/CM1213A-D.PDF" V 3925 3930 50  0001 C CNN
F 4 "CM1213A-01SO" H 14400 1250 50  0001 C CNN "Description"
F 5 "ON Semiconductor" H 14400 1250 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 14400 1250 50  0001 C CNN "Vendor"
F 7 "184-4869" H 14400 1250 50  0001 C CNN "Vendor Part Number"
	1    4000 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:L_Core_Ferrite_Small L2
U 1 1 5EBDB91A
P 4500 6150
F 0 "L2" H 4568 6196 50  0000 L CNN
F 1 "120nH" H 4568 6105 50  0000 L CNN
F 2 "Inductor_SMD:L_0402_1005Metric" H 4500 6150 50  0001 C CNN
F 3 "~" H 4500 6150 50  0001 C CNN
F 4 "LQW15ANR12J00D" H 14400 1250 50  0001 C CNN "Description"
F 5 "Murata" H 14400 1250 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 14400 1250 50  0001 C CNN "Vendor"
F 7 "792-6491" H 14400 1250 50  0001 C CNN "Vendor Part Number"
	1    4500 6150
	1    0    0    -1  
$EndComp
$Comp
L Device:L_Core_Ferrite_Small L1
U 1 1 5EBDCFED
P 4000 4600
F 0 "L1" H 3956 4554 50  0000 R CNN
F 1 "62nH" H 3956 4645 50  0000 R CNN
F 2 "Inductor_SMD:L_0402_1005Metric" H 4000 4600 50  0001 C CNN
F 3 "~" H 4000 4600 50  0001 C CNN
F 4 "LQW15AN62NG00D" H 14400 1250 50  0001 C CNN "Description"
F 5 "Murata" H 14400 1250 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 14400 1250 50  0001 C CNN "Vendor"
F 7 "785-5150" H 14400 1250 50  0001 C CNN "Vendor Part Number"
	1    4000 4600
	-1   0    0    1   
$EndComp
$Comp
L Device:C C28
U 1 1 5EBDD763
P 4000 4900
F 0 "C28" H 4115 4946 50  0000 L CNN
F 1 "10pF" H 4115 4855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4038 4750 50  0001 C CNN
F 3 "~" H 4000 4900 50  0001 C CNN
F 4 "GRM1885C1H100FA01D" H 14400 1250 50  0001 C CNN "Description"
F 5 "Murata" H 14400 1250 50  0001 C CNN "Manufacturer"
F 6 "RS Components" H 14400 1250 50  0001 C CNN "Vendor"
F 7 "723-5770" H 14400 1250 50  0001 C CNN "Vendor Part Number"
	1    4000 4900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR036
U 1 1 5EC2E667
P 4250 4100
F 0 "#PWR036" H 4250 3850 50  0001 C CNN
F 1 "GND" H 4250 3950 50  0000 C CNN
F 2 "" H 4250 4100 50  0001 C CNN
F 3 "" H 4250 4100 50  0001 C CNN
	1    4250 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3850 4250 3850
Wire Wire Line
	4250 4100 3800 4100
Wire Wire Line
	3800 4100 3800 3850
Wire Wire Line
	4250 3850 4250 4100
Connection ~ 4250 4100
Wire Wire Line
	4000 5050 5100 5050
Wire Wire Line
	4000 4750 4000 4700
Wire Wire Line
	5000 6400 6000 6400
Wire Wire Line
	5800 6300 5700 6300
Wire Wire Line
	4500 6300 4500 6250
Connection ~ 5500 6300
Wire Wire Line
	5500 6300 4500 6300
Connection ~ 5600 6300
Wire Wire Line
	5600 6300 5500 6300
Connection ~ 5700 6300
Wire Wire Line
	5700 6300 5600 6300
$Comp
L power:GND #PWR037
U 1 1 5ED5BD26
P 4500 6300
F 0 "#PWR037" H 4500 6050 50  0001 C CNN
F 1 "GND" H 4505 6127 50  0000 C CNN
F 2 "" H 4500 6300 50  0001 C CNN
F 3 "" H 4500 6300 50  0001 C CNN
	1    4500 6300
	1    0    0    -1  
$EndComp
Connection ~ 4500 6300
Wire Wire Line
	4500 6050 4500 4850
Wire Wire Line
	4500 4850 5100 4850
Connection ~ 4500 4850
Wire Wire Line
	4500 4750 4000 4750
Wire Wire Line
	4500 4750 4500 4850
Connection ~ 4000 4750
$Comp
L Device:Antenna AE1
U 1 1 5EDAE5FF
P 4750 4100
F 0 "AE1" H 4800 4150 50  0000 L CNN
F 1 "Antenna" H 4800 4050 50  0000 L CNN
F 2 "Connector_Pin:Pin_D1.0mm_L10.0mm" H 4750 4100 50  0001 C CNN
F 3 "~" H 4750 4100 50  0001 C CNN
	1    4750 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial DNP_J2
U 1 1 5E53BD0F
P 4450 4100
F 0 "DNP_J2" V 4550 4000 50  0000 L CNN
F 1 "Conn_Coaxial" V 4650 3950 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Molex_73251-1153_EdgeMount_Horizontal" H 4450 4100 50  0001 C CNN
F 3 " ~" H 4450 4100 50  0001 C CNN
	1    4450 4100
	0    1    -1   0   
$EndComp
Wire Wire Line
	4000 4050 4000 4300
Wire Wire Line
	4450 4300 4000 4300
Connection ~ 4000 4300
Wire Wire Line
	4000 4300 4000 4500
Wire Wire Line
	4450 4300 4750 4300
Connection ~ 4450 4300
Wire Bus Line
	6450 1150 6450 3500
Wire Bus Line
	3550 3500 3550 7200
Wire Bus Line
	3550 1500 3550 3500
Wire Bus Line
	750  1250 750  3500
$EndSCHEMATC
