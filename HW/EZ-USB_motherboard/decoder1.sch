EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "EZ-USB development board opto isolated motherboard"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:DB9_Male_MountingHoles J1
U 1 1 61C1E82F
P 2000 5100
F 0 "J1" H 2180 5009 50  0000 L CNN
F 1 "DB9_Male_MountingHoles" H 2180 5100 50  0000 L CNN
F 2 "Connector_Dsub:DSUB-9_Male_Horizontal_P2.77x2.84mm_EdgePinOffset9.90mm_Housed_MountingHolesOffset11.32mm" H 2000 5100 50  0001 C CNN
F 3 " ~" H 2000 5100 50  0001 C CNN
	1    2000 5100
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74AHC04 U1
U 4 1 61C1F943
P 3800 4450
F 0 "U1" H 3800 4767 50  0000 C CNN
F 1 "74AHC04" H 3800 4676 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3800 4675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 3800 4450 50  0001 C CNN
	4    3800 4450
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U1
U 5 1 61C203DF
P 3800 5050
F 0 "U1" H 3800 5367 50  0000 C CNN
F 1 "74AHC04" H 3800 5276 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3800 5050 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 3800 5050 50  0001 C CNN
	5    3800 5050
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U1
U 6 1 61C20FE3
P 3800 5650
F 0 "U1" H 3800 5967 50  0000 C CNN
F 1 "74AHC04" H 3800 5876 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3800 5650 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 3800 5650 50  0001 C CNN
	6    3800 5650
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U1
U 1 1 61C21906
P 3800 6200
F 0 "U1" H 3800 6517 50  0000 C CNN
F 1 "74AHC04" H 3800 6426 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3800 6200 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 3800 6200 50  0001 C CNN
	1    3800 6200
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U1
U 2 1 61C21FD8
P 3800 6750
F 0 "U1" H 3800 7067 50  0000 C CNN
F 1 "74AHC04" H 3800 6976 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3800 6750 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 3800 6750 50  0001 C CNN
	2    3800 6750
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U1
U 3 1 61C22756
P 3800 7350
F 0 "U1" H 3800 7667 50  0000 C CNN
F 1 "74AHC04" H 3800 7576 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3800 7350 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 3800 7350 50  0001 C CNN
	3    3800 7350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 4700 2450 4700
Wire Wire Line
	2450 4700 2450 4450
Wire Wire Line
	2450 4450 3500 4450
Wire Wire Line
	2300 4800 3350 4800
Wire Wire Line
	3350 4800 3350 5050
Wire Wire Line
	3350 5050 3500 5050
Wire Wire Line
	2300 4900 3250 4900
Wire Wire Line
	3250 4900 3250 5650
Wire Wire Line
	3250 5650 3500 5650
Wire Wire Line
	2300 5300 3150 5300
Wire Wire Line
	3150 5300 3150 6200
Wire Wire Line
	3150 6200 3500 6200
Wire Wire Line
	3050 5400 3050 6750
Wire Wire Line
	3050 6750 3500 6750
Wire Wire Line
	2300 5400 3050 5400
Wire Wire Line
	2300 5500 2950 5500
Wire Wire Line
	2950 5500 2950 7350
Wire Wire Line
	2950 7350 3500 7350
$Comp
L 74xx:74AHC04 U1
U 7 1 61C24EA6
P 2800 8200
F 0 "U1" H 3030 8246 50  0000 L CNN
F 1 "74AHC04" H 3030 8155 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 2800 8200 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 2800 8200 50  0001 C CNN
	7    2800 8200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 5000 2800 5000
Wire Wire Line
	2800 5000 2800 7650
Wire Wire Line
	2300 5200 2450 5200
Wire Wire Line
	2450 5200 2450 8950
Wire Wire Line
	2300 5100 2450 5100
Wire Wire Line
	2450 5100 2450 5200
Connection ~ 2450 5200
$Comp
L Device:C C1
U 1 1 61C2AF26
P 3550 8150
F 0 "C1" H 3665 8196 50  0000 L CNN
F 1 "100nF" H 3665 8105 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.0mm_P5.00mm" H 3588 8000 50  0001 C CNN
F 3 "~" H 3550 8150 50  0001 C CNN
	1    3550 8150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 7650 3550 7650
Wire Wire Line
	3550 7650 3550 8000
Connection ~ 2800 7650
Wire Wire Line
	2800 7650 2800 7700
Wire Wire Line
	3550 8300 3550 8950
Wire Wire Line
	2450 8950 2800 8950
Wire Wire Line
	2800 8700 2800 8950
Connection ~ 2800 8950
Wire Wire Line
	2800 8950 3550 8950
$Comp
L Isolator:HCPL-2630 U3
U 1 1 61C2CF6C
P 6200 4550
F 0 "U3" H 6200 5231 50  0000 C CNN
F 1 "HCPL-2630" H 6200 5140 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 6300 3830 50  0001 C CNN
F 3 "https://docs.broadcom.com/docs/AV02-0940EN" H 5800 4900 50  0001 C CNN
	1    6200 4550
	1    0    0    -1  
$EndComp
$Comp
L Isolator:HCPL-2630 U4
U 1 1 61C2FFF4
P 6200 5900
F 0 "U4" H 6200 6581 50  0000 C CNN
F 1 "HCPL-2630" H 6200 6490 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 6300 5180 50  0001 C CNN
F 3 "https://docs.broadcom.com/docs/AV02-0940EN" H 5800 6250 50  0001 C CNN
	1    6200 5900
	1    0    0    -1  
$EndComp
$Comp
L Isolator:HCPL-2630 U5
U 1 1 61C3183F
P 6200 7250
F 0 "U5" H 6200 7931 50  0000 C CNN
F 1 "HCPL-2630" H 6200 7840 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 6300 6530 50  0001 C CNN
F 3 "https://docs.broadcom.com/docs/AV02-0940EN" H 5800 7600 50  0001 C CNN
	1    6200 7250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 61C33B8C
P 4500 4450
F 0 "R1" V 4293 4450 50  0000 C CNN
F 1 "360" V 4384 4450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 4430 4450 50  0001 C CNN
F 3 "~" H 4500 4450 50  0001 C CNN
	1    4500 4450
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 61C34189
P 4500 5050
F 0 "R2" V 4293 5050 50  0000 C CNN
F 1 "360" V 4384 5050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 4430 5050 50  0001 C CNN
F 3 "~" H 4500 5050 50  0001 C CNN
	1    4500 5050
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 61C34659
P 4500 5650
F 0 "R3" V 4293 5650 50  0000 C CNN
F 1 "360" V 4384 5650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 4430 5650 50  0001 C CNN
F 3 "~" H 4500 5650 50  0001 C CNN
	1    4500 5650
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 61C34F10
P 4500 6200
F 0 "R4" V 4293 6200 50  0000 C CNN
F 1 "360" V 4384 6200 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 4430 6200 50  0001 C CNN
F 3 "~" H 4500 6200 50  0001 C CNN
	1    4500 6200
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 61C35807
P 4500 6750
F 0 "R5" V 4293 6750 50  0000 C CNN
F 1 "360" V 4384 6750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 4430 6750 50  0001 C CNN
F 3 "~" H 4500 6750 50  0001 C CNN
	1    4500 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 61C35A89
P 4500 7350
F 0 "R6" V 4293 7350 50  0000 C CNN
F 1 "360" V 4384 7350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 4430 7350 50  0001 C CNN
F 3 "~" H 4500 7350 50  0001 C CNN
	1    4500 7350
	0    1    1    0   
$EndComp
Wire Wire Line
	4100 4450 4350 4450
Wire Wire Line
	4100 5050 4350 5050
Wire Wire Line
	4100 5650 4350 5650
Wire Wire Line
	4100 6200 4350 6200
Wire Wire Line
	4100 6750 4350 6750
Wire Wire Line
	4100 7350 4350 7350
$Comp
L 74xx:74AHC04 U2
U 4 1 61C6D4FE
P 8750 4450
F 0 "U2" H 8750 4767 50  0000 C CNN
F 1 "74AHC04" H 8750 4676 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8750 4450 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8750 4450 50  0001 C CNN
	4    8750 4450
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U2
U 5 1 61C6D504
P 8750 5000
F 0 "U2" H 8750 5317 50  0000 C CNN
F 1 "74AHC04" H 8750 5226 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8750 5000 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8750 5000 50  0001 C CNN
	5    8750 5000
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U2
U 6 1 61C6D50A
P 8750 5550
F 0 "U2" H 8750 5867 50  0000 C CNN
F 1 "74AHC04" H 8750 5776 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8750 5550 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8750 5550 50  0001 C CNN
	6    8750 5550
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U2
U 1 1 61C6D510
P 8750 6200
F 0 "U2" H 8750 6517 50  0000 C CNN
F 1 "74AHC04" H 8750 6426 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8750 6200 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8750 6200 50  0001 C CNN
	1    8750 6200
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U2
U 2 1 61C6D516
P 8750 6750
F 0 "U2" H 8750 7067 50  0000 C CNN
F 1 "74AHC04" H 8750 6976 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8750 6750 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8750 6750 50  0001 C CNN
	2    8750 6750
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74AHC04 U2
U 3 1 61C6D51C
P 8750 7350
F 0 "U2" H 8750 7667 50  0000 C CNN
F 1 "74AHC04" H 8750 7576 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8750 7350 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8750 7350 50  0001 C CNN
	3    8750 7350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 61C9188C
P 6650 3600
F 0 "R7" H 6720 3646 50  0000 L CNN
F 1 "1k" H 6720 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 6580 3600 50  0001 C CNN
F 3 "~" H 6650 3600 50  0001 C CNN
	1    6650 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 61C93896
P 6900 3600
F 0 "R8" H 6970 3646 50  0000 L CNN
F 1 "1k" H 6970 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 6830 3600 50  0001 C CNN
F 3 "~" H 6900 3600 50  0001 C CNN
	1    6900 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 61C9457A
P 7150 3600
F 0 "R9" H 7220 3646 50  0000 L CNN
F 1 "1k" H 7220 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 7080 3600 50  0001 C CNN
F 3 "~" H 7150 3600 50  0001 C CNN
	1    7150 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 61C94CFD
P 7400 3600
F 0 "R10" H 7470 3646 50  0000 L CNN
F 1 "1k" H 7470 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 7330 3600 50  0001 C CNN
F 3 "~" H 7400 3600 50  0001 C CNN
	1    7400 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 61C954CF
P 7650 3600
F 0 "R11" H 7720 3646 50  0000 L CNN
F 1 "1k" H 7720 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 7580 3600 50  0001 C CNN
F 3 "~" H 7650 3600 50  0001 C CNN
	1    7650 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R12
U 1 1 61C961C3
P 7900 3600
F 0 "R12" H 7970 3646 50  0000 L CNN
F 1 "1k" H 7970 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 7830 3600 50  0001 C CNN
F 3 "~" H 7900 3600 50  0001 C CNN
	1    7900 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 4350 6650 4350
Wire Wire Line
	8250 4350 8250 4450
Wire Wire Line
	8250 4450 8450 4450
Wire Wire Line
	6400 4750 6500 4750
Wire Wire Line
	8250 4750 8250 5000
Wire Wire Line
	8250 5000 8450 5000
Wire Wire Line
	6500 5700 7150 5700
Wire Wire Line
	8250 5700 8250 5550
Wire Wire Line
	8250 5550 8450 5550
Wire Wire Line
	6500 6100 7400 6100
Wire Wire Line
	8250 6100 8250 6200
Wire Wire Line
	8250 6200 8450 6200
Wire Wire Line
	6500 7050 7650 7050
Wire Wire Line
	8250 7050 8250 6750
Wire Wire Line
	8250 6750 8450 6750
Wire Wire Line
	6500 7450 7900 7450
Wire Wire Line
	8250 7450 8250 7350
Wire Wire Line
	8250 7350 8450 7350
Connection ~ 6500 4750
Wire Wire Line
	8250 4750 6900 4750
Wire Wire Line
	6650 3750 6650 4350
Connection ~ 6650 4350
Wire Wire Line
	6650 4350 8250 4350
Wire Wire Line
	6900 3750 6900 4750
Connection ~ 6900 4750
Wire Wire Line
	6900 4750 6500 4750
Wire Wire Line
	7150 3750 7150 5700
Connection ~ 7150 5700
Wire Wire Line
	7150 5700 8250 5700
Wire Wire Line
	7400 3750 7400 6100
Connection ~ 7400 6100
Wire Wire Line
	7400 6100 8250 6100
Wire Wire Line
	7650 3750 7650 7050
Connection ~ 7650 7050
Wire Wire Line
	7650 7050 8250 7050
Wire Wire Line
	7900 3750 7900 7450
Connection ~ 7900 7450
Wire Wire Line
	7900 7450 8250 7450
Wire Wire Line
	6200 4050 6200 3200
Wire Wire Line
	6200 3200 6650 3200
Wire Wire Line
	6650 3200 6650 3450
Wire Wire Line
	6650 3200 6900 3200
Wire Wire Line
	7900 3200 7900 3450
Connection ~ 6650 3200
Wire Wire Line
	6900 3450 6900 3200
Connection ~ 6900 3200
Wire Wire Line
	6900 3200 7150 3200
Wire Wire Line
	7150 3450 7150 3200
Connection ~ 7150 3200
Wire Wire Line
	7150 3200 7400 3200
Wire Wire Line
	7400 3450 7400 3200
Connection ~ 7400 3200
Wire Wire Line
	7400 3200 7650 3200
Wire Wire Line
	7650 3450 7650 3200
Connection ~ 7650 3200
Wire Wire Line
	7650 3200 7900 3200
Wire Wire Line
	6200 5050 6200 5150
Wire Wire Line
	6200 5150 6700 5150
Wire Wire Line
	6700 5150 6700 6500
Wire Wire Line
	6200 7750 6200 7950
Wire Wire Line
	6200 7950 6700 7950
Connection ~ 6700 7950
Wire Wire Line
	6200 6400 6200 6500
Wire Wire Line
	6200 6500 6700 6500
Connection ~ 6700 6500
Wire Wire Line
	6700 6500 6700 7950
Wire Wire Line
	7900 3200 8150 3200
Connection ~ 7900 3200
Wire Wire Line
	8150 3200 8150 5350
$Comp
L 74xx:74AHC04 U2
U 7 1 61CDEC15
P 8700 8350
F 0 "U2" H 8930 8396 50  0000 L CNN
F 1 "74AHC04" H 8930 8305 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8700 8350 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT04.pdf" H 8700 8350 50  0001 C CNN
	7    8700 8350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 61CDEC1B
P 9450 8300
F 0 "C2" H 9565 8346 50  0000 L CNN
F 1 "100nF" H 9565 8255 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.0mm_P5.00mm" H 9488 8150 50  0001 C CNN
F 3 "~" H 9450 8300 50  0001 C CNN
	1    9450 8300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 7850 8700 7700
Wire Wire Line
	8700 7700 8150 7700
Connection ~ 8150 7700
Wire Wire Line
	8150 7700 8150 7950
Wire Wire Line
	8700 7700 9450 7700
Wire Wire Line
	9450 7700 9450 8150
Connection ~ 8700 7700
Wire Wire Line
	8700 8850 8700 8950
Wire Wire Line
	8700 8950 9450 8950
Wire Wire Line
	9450 8950 9450 8450
$Comp
L Device:C C3
U 1 1 61CF4E07
P 7000 8350
F 0 "C3" H 7115 8396 50  0000 L CNN
F 1 "100nF" H 7115 8305 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.0mm_P5.00mm" H 7038 8200 50  0001 C CNN
F 3 "~" H 7000 8350 50  0001 C CNN
	1    7000 8350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 61CF64A8
P 7400 8350
F 0 "C4" H 7515 8396 50  0000 L CNN
F 1 "100nF" H 7515 8305 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.0mm_P5.00mm" H 7438 8200 50  0001 C CNN
F 3 "~" H 7400 8350 50  0001 C CNN
	1    7400 8350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 61CF72A6
P 7800 8350
F 0 "C5" H 7915 8396 50  0000 L CNN
F 1 "100nF" H 7915 8305 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.0mm_P5.00mm" H 7838 8200 50  0001 C CNN
F 3 "~" H 7800 8350 50  0001 C CNN
	1    7800 8350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 8200 7000 7950
Wire Wire Line
	7000 7950 7400 7950
Wire Wire Line
	7400 8200 7400 7950
Connection ~ 7400 7950
Wire Wire Line
	7400 7950 7800 7950
Wire Wire Line
	7800 8200 7800 7950
Connection ~ 7800 7950
Wire Wire Line
	7800 7950 8150 7950
Wire Wire Line
	6700 8950 7000 8950
Wire Wire Line
	6700 7950 6700 8950
Connection ~ 8700 8950
Wire Wire Line
	7000 8500 7000 8950
Connection ~ 7000 8950
Wire Wire Line
	7000 8950 7400 8950
Wire Wire Line
	7400 8500 7400 8950
Connection ~ 7400 8950
Wire Wire Line
	7400 8950 7800 8950
Wire Wire Line
	7800 8500 7800 8950
Connection ~ 7800 8950
Wire Wire Line
	7800 8950 8700 8950
Wire Wire Line
	4650 5650 5500 5650
Wire Wire Line
	5500 5650 5500 4450
Wire Wire Line
	5500 4450 5900 4450
Wire Wire Line
	4650 5050 5350 5050
Wire Wire Line
	5350 5050 5350 4650
Wire Wire Line
	5350 4650 5900 4650
Wire Wire Line
	4650 4450 5200 4450
Wire Wire Line
	5200 4450 5200 5800
Wire Wire Line
	5200 5800 5900 5800
Wire Wire Line
	4650 6200 5500 6200
Wire Wire Line
	5500 6200 5500 7350
Wire Wire Line
	5500 7350 5900 7350
Wire Wire Line
	4650 6750 5350 6750
Wire Wire Line
	5350 6750 5350 7150
Wire Wire Line
	5350 7150 5900 7150
Wire Wire Line
	4650 7350 5200 7350
Wire Wire Line
	5200 7350 5200 6000
Wire Wire Line
	5200 6000 5900 6000
Wire Wire Line
	5900 4250 5700 4250
Wire Wire Line
	5700 4250 5700 4850
Wire Wire Line
	5700 7650 3550 7650
Connection ~ 3550 7650
Wire Wire Line
	5900 7550 5700 7550
Connection ~ 5700 7550
Wire Wire Line
	5700 7550 5700 7650
Wire Wire Line
	5900 6950 5700 6950
Connection ~ 5700 6950
Wire Wire Line
	5700 6950 5700 7550
Wire Wire Line
	5900 6200 5700 6200
Connection ~ 5700 6200
Wire Wire Line
	5700 6200 5700 6950
Wire Wire Line
	5900 5600 5700 5600
Connection ~ 5700 5600
Wire Wire Line
	5700 5600 5700 6200
Wire Wire Line
	5900 4850 5700 4850
Connection ~ 5700 4850
Wire Wire Line
	5700 4850 5700 5600
$Comp
L Connector_Generic:Conn_02x10_Odd_Even J2
U 1 1 61D7C06D
P 11250 3450
F 0 "J2" H 11300 4067 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 11300 3976 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x10_P2.54mm_Vertical" H 11250 3450 50  0001 C CNN
F 3 "~" H 11250 3450 50  0001 C CNN
	1    11250 3450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x10_Odd_Even J3
U 1 1 61D7EDE4
P 13350 3550
F 0 "J3" H 13400 2825 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 13400 2916 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x10_P2.54mm_Vertical" H 13350 3550 50  0001 C CNN
F 3 "~" H 13350 3550 50  0001 C CNN
	1    13350 3550
	-1   0    0    1   
$EndComp
Wire Notes Line
	10550 5350 14200 5350
Wire Notes Line
	14200 5350 14200 2200
Wire Notes Line
	10550 2200 10550 5350
Wire Notes Line
	11900 2200 11900 2850
Wire Notes Line
	11900 2850 12700 2850
Wire Notes Line
	12700 2850 12700 2200
Wire Notes Line
	10550 2200 14200 2200
Wire Wire Line
	11050 3750 10000 3750
Wire Wire Line
	9050 5000 10200 5000
Wire Wire Line
	10200 5000 10200 3850
Wire Wire Line
	10200 3850 11050 3850
Wire Wire Line
	11550 3750 12000 3750
Wire Wire Line
	12000 3750 12000 6200
Wire Wire Line
	12000 6200 9050 6200
Wire Wire Line
	9050 5550 10000 5550
Wire Wire Line
	11800 5550 11800 3850
Wire Wire Line
	11800 3850 11550 3850
Wire Wire Line
	9050 6750 12700 6750
Wire Wire Line
	12700 6750 12700 3950
Wire Wire Line
	12700 3950 13050 3950
Wire Wire Line
	9050 7350 14000 7350
Wire Wire Line
	14000 7350 14000 3950
Wire Wire Line
	14000 3950 13550 3950
Wire Wire Line
	11550 3950 11750 3950
Wire Wire Line
	11750 3950 11750 2550
Wire Wire Line
	11750 2550 8150 2550
Wire Wire Line
	8150 2550 8150 3200
Connection ~ 8150 3200
Wire Wire Line
	11050 3950 10700 3950
Wire Wire Line
	10700 3950 10700 8950
Wire Wire Line
	10700 8950 9450 8950
Connection ~ 9450 8950
Text Label 10900 3750 0    50   ~ 0
PB0
Text Label 10900 3850 0    50   ~ 0
PB2
Text Label 11550 3750 0    50   ~ 0
PB1
Text Label 11550 3850 0    50   ~ 0
PB3
Text Label 11550 3950 0    50   ~ 0
VCC
Text Label 10900 3950 0    50   ~ 0
GND
Text Label 13550 3950 0    50   ~ 0
PB4
Text Label 12900 3950 0    50   ~ 0
PB5
Text Label 13550 3850 0    50   ~ 0
PB6
Wire Wire Line
	13050 3850 12700 3850
Text Label 12900 3850 0    50   ~ 0
PB7
Text Label 13550 3050 0    50   ~ 0
PD3
Text Label 11550 3050 0    50   ~ 0
PD6
Text Label 11550 3150 0    50   ~ 0
GND
Text Label 11550 3250 0    50   ~ 0
GND
Text Label 13550 3150 0    50   ~ 0
PD1
Text Label 13550 3250 0    50   ~ 0
PA7
Text Label 13550 3350 0    50   ~ 0
PA5
Text Label 13550 3450 0    50   ~ 0
PA3
Text Label 13550 3550 0    50   ~ 0
PA1
Text Label 13550 3650 0    50   ~ 0
CTL2
Text Label 13550 3750 0    50   ~ 0
CTL0
Text Label 11550 3350 0    50   ~ 0
RDY0
Text Label 11550 3450 0    50   ~ 0
VCC
Text Label 11550 3550 0    50   ~ 0
IFCLK
Text Label 11550 3650 0    50   ~ 0
SDA
Text Notes 12200 2500 0    50   ~ 0
USB
$Comp
L Jumper:Jumper_2_Open JP2
U 1 1 61E61CF1
P 5100 8950
F 0 "JP2" H 5100 9185 50  0000 C CNN
F 1 "Jumper_2_Open" H 5100 9094 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5100 8950 50  0001 C CNN
F 3 "~" H 5100 8950 50  0001 C CNN
	1    5100 8950
	1    0    0    -1  
$EndComp
$Comp
L Jumper:Jumper_2_Open JP1
U 1 1 61E62007
P 5250 3200
F 0 "JP1" H 5250 3435 50  0000 C CNN
F 1 "Jumper_2_Open" H 5250 3344 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5250 3200 50  0001 C CNN
F 3 "~" H 5250 3200 50  0001 C CNN
	1    5250 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3200 6200 3200
Connection ~ 6200 3200
Wire Wire Line
	3550 8950 4900 8950
Connection ~ 3550 8950
Wire Wire Line
	5300 8950 6700 8950
Connection ~ 6700 8950
Wire Wire Line
	2800 5000 2800 3200
Wire Wire Line
	2800 3200 5050 3200
Connection ~ 2800 5000
Wire Wire Line
	6200 5400 6200 5350
Wire Wire Line
	6200 5350 8150 5350
Connection ~ 8150 5350
Wire Wire Line
	8150 5350 8150 6700
Wire Wire Line
	6200 6750 6200 6700
Wire Wire Line
	6200 6700 8150 6700
Connection ~ 8150 6700
Wire Wire Line
	8150 6700 8150 7700
$Comp
L Mechanical:MountingHole H1
U 1 1 61EEB3EC
P 2100 9500
F 0 "H1" H 2200 9546 50  0000 L CNN
F 1 "MountingHole" H 2200 9455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2100 9500 50  0001 C CNN
F 3 "~" H 2100 9500 50  0001 C CNN
	1    2100 9500
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 61EEB7A1
P 2100 9750
F 0 "H2" H 2200 9796 50  0000 L CNN
F 1 "MountingHole" H 2200 9705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2100 9750 50  0001 C CNN
F 3 "~" H 2100 9750 50  0001 C CNN
	1    2100 9750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 61EEC5FF
P 2100 10050
F 0 "H3" H 2200 10096 50  0000 L CNN
F 1 "MountingHole" H 2200 10005 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2100 10050 50  0001 C CNN
F 3 "~" H 2100 10050 50  0001 C CNN
	1    2100 10050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 61EECED5
P 2100 10350
F 0 "H4" H 2200 10396 50  0000 L CNN
F 1 "MountingHole" H 2200 10305 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2100 10350 50  0001 C CNN
F 3 "~" H 2100 10350 50  0001 C CNN
	1    2100 10350
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H5
U 1 1 61EEE3C9
P 3150 9500
F 0 "H5" H 3250 9546 50  0000 L CNN
F 1 "MountingHole" H 3250 9455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 3150 9500 50  0001 C CNN
F 3 "~" H 3150 9500 50  0001 C CNN
	1    3150 9500
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H6
U 1 1 61EEE3CF
P 3150 9750
F 0 "H6" H 3250 9796 50  0000 L CNN
F 1 "MountingHole" H 3250 9705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 3150 9750 50  0001 C CNN
F 3 "~" H 3150 9750 50  0001 C CNN
	1    3150 9750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H7
U 1 1 61EEE3D5
P 3150 10050
F 0 "H7" H 3250 10096 50  0000 L CNN
F 1 "MountingHole" H 3250 10005 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 3150 10050 50  0001 C CNN
F 3 "~" H 3150 10050 50  0001 C CNN
	1    3150 10050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H8
U 1 1 61EEE3DB
P 3150 10350
F 0 "H8" H 3250 10396 50  0000 L CNN
F 1 "MountingHole" H 3250 10305 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 3150 10350 50  0001 C CNN
F 3 "~" H 3150 10350 50  0001 C CNN
	1    3150 10350
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H9
U 1 1 61EF791B
P 4050 9500
F 0 "H9" H 4150 9546 50  0000 L CNN
F 1 "MountingHole" H 4150 9455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4050 9500 50  0001 C CNN
F 3 "~" H 4050 9500 50  0001 C CNN
	1    4050 9500
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H10
U 1 1 61EF7921
P 4050 9750
F 0 "H10" H 4150 9796 50  0000 L CNN
F 1 "MountingHole" H 4150 9705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4050 9750 50  0001 C CNN
F 3 "~" H 4050 9750 50  0001 C CNN
	1    4050 9750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 3750 10000 5550
Wire Wire Line
	9050 4450 10400 4450
Wire Wire Line
	10400 4450 10400 5550
Wire Wire Line
	10400 5550 11800 5550
Text Label 2300 4700 0    50   ~ 0
ZOOM_A
Text Label 2300 5500 0    50   ~ 0
ZOOM_B
Text Label 2300 4800 0    50   ~ 0
FOCUS_A
Text Label 2300 4900 0    50   ~ 0
FOCUS_B
Text Label 2300 5300 0    50   ~ 0
IRIS_A
Text Label 2300 5400 0    50   ~ 0
IRIS_B
Text Label 2300 5200 0    50   ~ 0
GND
Text Label 2300 5000 0    50   ~ 0
PWR
Text Label 2300 5100 0    50   ~ 0
VIRT_ENA
$EndSCHEMATC
