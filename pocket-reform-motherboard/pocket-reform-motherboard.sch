EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A2 23386 16535
encoding utf-8
Sheet 1 1
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
L reform2:reform2-som-connector U1
U 1 1 6089512C
P 4750 8350
F 0 "U1" H 4800 13417 50  0000 C CNN
F 1 "reform2-som-connector" H 4800 13326 50  0000 C CNN
F 2 "footprints:TE_1473149-4" H 4550 11550 50  0001 C CNN
F 3 "" H 4550 11550 50  0001 C CNN
	1    4750 8350
	1    0    0    -1  
$EndComp
$Comp
L reform2-motherboard-rescue:MPCIE-Socket-reform-motherboard-rescue U6
U 1 1 6089CFD0
P 9250 7050
F 0 "U6" H 9250 8487 60  0000 C CNN
F 1 "MPCIE-Socket-reform-motherboard-rescue" H 9250 8381 60  0000 C CNN
F 2 "footprints:MOLEX_67910-5700" H 9350 6050 60  0001 C CNN
F 3 "" H 9350 6050 60  0000 C CNN
	1    9250 7050
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_2Pole TP1
U 1 1 608A10DB
P 9250 9000
F 0 "TP1" H 9250 9195 50  0000 C CNN
F 1 "TestPoint_2Pole" H 9250 9104 50  0000 C CNN
F 2 "footprints:MOLEX_48099-5701" H 9250 9000 50  0001 C CNN
F 3 "~" H 9250 9000 50  0001 C CNN
	1    9250 9000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 608A17C2
P 9000 9100
F 0 "#PWR0101" H 9000 8850 50  0001 C CNN
F 1 "GND" H 9005 8927 50  0000 C CNN
F 2 "" H 9000 9100 50  0001 C CNN
F 3 "" H 9000 9100 50  0001 C CNN
	1    9000 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 9100 9000 9000
Wire Wire Line
	9000 9000 9050 9000
$Comp
L power:GND #PWR0102
U 1 1 608A1D87
P 9500 9100
F 0 "#PWR0102" H 9500 8850 50  0001 C CNN
F 1 "GND" H 9505 8927 50  0000 C CNN
F 2 "" H 9500 9100 50  0001 C CNN
F 3 "" H 9500 9100 50  0001 C CNN
	1    9500 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 9100 9500 9000
Wire Wire Line
	9500 9000 9450 9000
$Comp
L Connector:USB_C_Receptacle J3
U 1 1 60A8D380
P 11650 3150
F 0 "J3" H 11757 4417 50  0000 C CNN
F 1 "USB_C_Receptacle" H 11757 4326 50  0000 C CNN
F 2 "Connector_USB:USB_C_Receptacle_Amphenol_12401548E4-2A_CircularHoles" H 11800 3150 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 11800 3150 50  0001 C CNN
	1    11650 3150
	1    0    0    -1  
$EndComp
$Comp
L Connector:Micro_SD_Card_Det J2
U 1 1 60A91E14
P 9100 3350
F 0 "J2" H 9050 4167 50  0000 C CNN
F 1 "Micro_SD_Card_Det" H 9050 4076 50  0000 C CNN
F 2 "footprints:HRS_DM3CS-SF" H 11150 4050 50  0001 C CNN
F 3 "https://www.hirose.com/product/en/download_file/key_name/DM3/category/Catalog/doc_file_id/49662/?file_category_id=4&item_id=195&is_series=1" H 9100 3450 50  0001 C CNN
	1    9100 3350
	1    0    0    -1  
$EndComp
Text Notes 10950 10800 0    50   ~ 0
TODO: import the audio filters etc
$Comp
L Connector:AudioJack4 J4
U 1 1 60A93F5E
P 10550 12800
F 0 "J4" H 10507 13125 50  0000 C CNN
F 1 "AudioJack4" H 10507 13034 50  0000 C CNN
F 2 "Connector_Audio:Jack_3.5mm_CUI_SJ-3524-SMT_Horizontal" H 10550 12800 50  0001 C CNN
F 3 "~" H 10550 12800 50  0001 C CNN
	1    10550 12800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J1
U 1 1 60A9494A
P 6350 2000
F 0 "J1" H 6458 2281 50  0000 C CNN
F 1 "PWR" H 6458 2190 50  0000 C CNN
F 2 "Connector_JST:JST_PH_S4B-PH-SM4-TB_1x04-1MP_P2.00mm_Horizontal" H 6350 2000 50  0001 C CNN
F 3 "~" H 6350 2000 50  0001 C CNN
	1    6350 2000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 60A969B9
P 6900 1800
F 0 "#PWR01" H 6900 1650 50  0001 C CNN
F 1 "+5V" H 6915 1973 50  0000 C CNN
F 2 "" H 6900 1800 50  0001 C CNN
F 3 "" H 6900 1800 50  0001 C CNN
	1    6900 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 1900 6900 1900
Wire Wire Line
	6900 1900 6900 1800
Wire Wire Line
	6550 2000 6900 2000
Wire Wire Line
	6900 2000 6900 1900
Connection ~ 6900 1900
$Comp
L power:GND #PWR02
U 1 1 60A97A09
P 6900 2300
F 0 "#PWR02" H 6900 2050 50  0001 C CNN
F 1 "GND" H 6905 2127 50  0000 C CNN
F 2 "" H 6900 2300 50  0001 C CNN
F 3 "" H 6900 2300 50  0001 C CNN
	1    6900 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 2300 6900 2200
Wire Wire Line
	6900 2200 6550 2200
Wire Wire Line
	6550 2100 6900 2100
Wire Wire Line
	6900 2100 6900 2200
Connection ~ 6900 2200
$Comp
L power:+5V #PWR03
U 1 1 60A9876C
P 3450 3250
F 0 "#PWR03" H 3450 3100 50  0001 C CNN
F 1 "+5V" H 3465 3423 50  0000 C CNN
F 2 "" H 3450 3250 50  0001 C CNN
F 3 "" H 3450 3250 50  0001 C CNN
	1    3450 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3550 3950 3550
Wire Wire Line
	3450 3250 3450 3550
Wire Wire Line
	3950 3650 3450 3650
Wire Wire Line
	3450 3650 3450 3550
Connection ~ 3450 3550
Wire Wire Line
	3950 3750 3450 3750
Wire Wire Line
	3450 3750 3450 3650
Connection ~ 3450 3650
$Comp
L power:+5V #PWR04
U 1 1 60A9A252
P 6150 3250
F 0 "#PWR04" H 6150 3100 50  0001 C CNN
F 1 "+5V" H 6165 3423 50  0000 C CNN
F 2 "" H 6150 3250 50  0001 C CNN
F 3 "" H 6150 3250 50  0001 C CNN
	1    6150 3250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6150 3550 5650 3550
Wire Wire Line
	6150 3250 6150 3550
Wire Wire Line
	5650 3650 6150 3650
Wire Wire Line
	6150 3650 6150 3550
Connection ~ 6150 3550
Wire Wire Line
	5650 3750 6150 3750
Wire Wire Line
	6150 3750 6150 3650
Connection ~ 6150 3650
NoConn ~ 3950 5650
NoConn ~ 3950 5750
NoConn ~ 3950 5950
NoConn ~ 3950 6050
NoConn ~ 3950 6250
NoConn ~ 3950 6350
NoConn ~ 3950 6550
NoConn ~ 3950 6650
NoConn ~ 3950 6850
NoConn ~ 3950 6950
NoConn ~ 3950 7650
NoConn ~ 3950 7550
NoConn ~ 3950 7450
NoConn ~ 3950 7350
NoConn ~ 3950 7250
NoConn ~ 3950 7150
NoConn ~ 3950 7750
NoConn ~ 3950 8350
NoConn ~ 3950 8150
NoConn ~ 3950 8050
NoConn ~ 3950 7950
Text GLabel 5650 4050 2    50   Input ~ 0
SD_D0
Text GLabel 5650 4150 2    50   Input ~ 0
SD_D1
Text GLabel 5650 4250 2    50   Input ~ 0
SD_D2
Text GLabel 5650 4350 2    50   Input ~ 0
SD_D3
Text GLabel 5650 4450 2    50   Input ~ 0
SD_CMD
Text GLabel 5650 4550 2    50   Input ~ 0
SD_CLK
NoConn ~ 5650 4950
NoConn ~ 5650 5050
NoConn ~ 5650 5150
NoConn ~ 5650 5250
NoConn ~ 3950 11450
NoConn ~ 3950 11350
NoConn ~ 3950 11250
NoConn ~ 3950 11150
NoConn ~ 3950 11050
NoConn ~ 3950 10950
NoConn ~ 3950 10850
NoConn ~ 3950 10350
NoConn ~ 3950 10250
NoConn ~ 3950 10150
Text GLabel 8200 3550 0    50   Input ~ 0
SD_D0
Text GLabel 8200 3650 0    50   Input ~ 0
SD_D1
Text GLabel 8200 2950 0    50   Input ~ 0
SD_D2
Text GLabel 8200 3050 0    50   Input ~ 0
SD_D3
Text GLabel 8200 3150 0    50   Input ~ 0
SD_CMD
Text GLabel 8200 3350 0    50   Input ~ 0
SD_CLK
Text Notes 8050 4250 0    50   ~ 0
TODO: level shifter!
NoConn ~ 5650 8250
NoConn ~ 5650 8050
NoConn ~ 5650 7950
NoConn ~ 5650 7850
NoConn ~ 5650 7750
NoConn ~ 5650 7650
NoConn ~ 5650 7550
Text Notes 7000 2000 0    50   ~ 0
TODO: combined power + uart?
NoConn ~ 3950 5150
NoConn ~ 3950 5250
Text Notes 7400 5050 0    50   ~ 0
TODO: 3V3 Power
Text Notes 7400 5150 0    50   ~ 0
TODO: 1V5 Power
Text GLabel 14350 7750 0    50   Input ~ 0
USB1_DN
Text GLabel 14350 7650 0    50   Input ~ 0
USB1_DP
Text GLabel 9900 6700 2    50   Input ~ 0
USB11_DN
Text GLabel 9900 6600 2    50   Input ~ 0
USB11_DP
$Comp
L reform2-motherboard-rescue:MPCIE-Socket-reform-motherboard-rescue U2
U 1 1 60AB80A1
P 11600 7050
F 0 "U2" H 11600 8487 60  0000 C CNN
F 1 "MPCIE-Socket-reform-motherboard-rescue" H 11600 8381 60  0000 C CNN
F 2 "footprints:MOLEX_67910-5700" H 11700 6050 60  0001 C CNN
F 3 "" H 11700 6050 60  0000 C CNN
	1    11600 7050
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_2Pole TP2
U 1 1 60AB848D
P 11600 9000
F 0 "TP2" H 11600 9195 50  0000 C CNN
F 1 "TestPoint_2Pole" H 11600 9104 50  0000 C CNN
F 2 "footprints:MOLEX_48099-5701" H 11600 9000 50  0001 C CNN
F 3 "~" H 11600 9000 50  0001 C CNN
	1    11600 9000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 60AB8497
P 11350 9100
F 0 "#PWR0103" H 11350 8850 50  0001 C CNN
F 1 "GND" H 11355 8927 50  0000 C CNN
F 2 "" H 11350 9100 50  0001 C CNN
F 3 "" H 11350 9100 50  0001 C CNN
	1    11350 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	11350 9100 11350 9000
Wire Wire Line
	11350 9000 11400 9000
$Comp
L power:GND #PWR0104
U 1 1 60AB84A3
P 11850 9100
F 0 "#PWR0104" H 11850 8850 50  0001 C CNN
F 1 "GND" H 11855 8927 50  0000 C CNN
F 2 "" H 11850 9100 50  0001 C CNN
F 3 "" H 11850 9100 50  0001 C CNN
	1    11850 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	11850 9100 11850 9000
Wire Wire Line
	11850 9000 11800 9000
$Comp
L Connector:USB_C_Receptacle J5
U 1 1 60AD0545
P 13750 3150
F 0 "J5" H 13857 4417 50  0000 C CNN
F 1 "USB_C_Receptacle" H 13857 4326 50  0000 C CNN
F 2 "Connector_USB:USB_C_Receptacle_Amphenol_12401548E4-2A_CircularHoles" H 13900 3150 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 13900 3150 50  0001 C CNN
	1    13750 3150
	1    0    0    -1  
$EndComp
$Comp
L Interface_USB:TUSB4041I U3
U 1 1 60AD3207
P 15650 8350
F 0 "U3" H 15650 6161 50  0000 C CNN
F 1 "TUSB4041I" H 15650 6070 50  0000 C CNN
F 2 "Package_QFP:HTQFP-64-1EP_10x10mm_P0.5mm_EP8x8mm_Mask4.4x4.4mm_ThermalVias" H 16850 10350 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tusb4041i.pdf" H 15350 8550 50  0001 C CNN
	1    15650 8350
	1    0    0    -1  
$EndComp
NoConn ~ 14350 9450
NoConn ~ 14350 9250
NoConn ~ 14350 9350
NoConn ~ 14350 10150
Text Notes 7400 5250 0    50   ~ 0
TODO: 1V1 Power
$Comp
L Connector:Conn_01x04_Male J6
U 1 1 60ADE021
P 17700 9600
F 0 "J6" H 17808 9881 50  0000 C CNN
F 1 "USBINT" H 17808 9790 50  0000 C CNN
F 2 "Connector_JST:JST_PH_S4B-PH-SM4-TB_1x04-1MP_P2.00mm_Horizontal" H 17700 9600 50  0001 C CNN
F 3 "~" H 17700 9600 50  0001 C CNN
	1    17700 9600
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0105
U 1 1 60AE3EC5
P 14750 5950
F 0 "#PWR0105" H 14750 5800 50  0001 C CNN
F 1 "+3V3" H 14765 6123 50  0000 C CNN
F 2 "" H 14750 5950 50  0001 C CNN
F 3 "" H 14750 5950 50  0001 C CNN
	1    14750 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 5950 14750 6100
Wire Wire Line
	14750 6100 14850 6100
Wire Wire Line
	15050 6100 15050 6250
Connection ~ 14750 6100
Wire Wire Line
	14750 6100 14750 6250
Wire Wire Line
	14950 6250 14950 6100
Connection ~ 14950 6100
Wire Wire Line
	14950 6100 15050 6100
Wire Wire Line
	14850 6250 14850 6100
Connection ~ 14850 6100
Wire Wire Line
	14850 6100 14950 6100
$Comp
L power:+1V1 #PWR0106
U 1 1 60AE7879
P 15250 5850
F 0 "#PWR0106" H 15250 5700 50  0001 C CNN
F 1 "+1V1" H 15265 6023 50  0000 C CNN
F 2 "" H 15250 5850 50  0001 C CNN
F 3 "" H 15250 5850 50  0001 C CNN
	1    15250 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	15250 5850 15250 6100
Wire Wire Line
	15250 6100 15350 6100
Wire Wire Line
	15850 6100 15850 6250
Connection ~ 15250 6100
Wire Wire Line
	15250 6100 15250 6250
Wire Wire Line
	15750 6250 15750 6100
Connection ~ 15750 6100
Wire Wire Line
	15750 6100 15850 6100
Wire Wire Line
	15650 6250 15650 6100
Connection ~ 15650 6100
Wire Wire Line
	15650 6100 15750 6100
Wire Wire Line
	15550 6100 15550 6250
Connection ~ 15550 6100
Wire Wire Line
	15550 6100 15650 6100
Wire Wire Line
	15450 6250 15450 6100
Connection ~ 15450 6100
Wire Wire Line
	15450 6100 15550 6100
Wire Wire Line
	15350 6100 15350 6250
Connection ~ 15350 6100
Wire Wire Line
	15350 6100 15450 6100
Text GLabel 16950 6850 2    50   Input ~ 0
USB11_DN
Text GLabel 16950 6750 2    50   Input ~ 0
USB11_DP
Text GLabel 16950 7350 2    50   Input ~ 0
USB12_DN
Text GLabel 16950 7250 2    50   Input ~ 0
USB12_DP
Text GLabel 5650 8450 2    50   Input ~ 0
USB1_DN
Text GLabel 5650 8550 2    50   Input ~ 0
USB1_DP
Text GLabel 3950 9750 0    50   Input ~ 0
USB2_DN
Text GLabel 3950 9850 0    50   Input ~ 0
USB2_DP
Text GLabel 14350 2650 2    50   Input ~ 0
USB2_DN
Text GLabel 14350 2950 2    50   Input ~ 0
USB2_DP
Text GLabel 14350 2850 2    50   Input ~ 0
USB2_DP
Text GLabel 14350 2750 2    50   Input ~ 0
USB2_DN
Text GLabel 12250 2750 2    50   Input ~ 0
USB12_DN
Text GLabel 12250 2850 2    50   Input ~ 0
USB12_DP
Text GLabel 12250 2650 2    50   Input ~ 0
USB12_DN
Text GLabel 12250 2950 2    50   Input ~ 0
USB12_DP
Text GLabel 17900 9700 2    50   Input ~ 0
USB13_DN
Text GLabel 17900 9600 2    50   Input ~ 0
USB13_DP
Text GLabel 5650 8750 2    50   Input ~ 0
USB1_TXN
Text GLabel 5650 8850 2    50   Input ~ 0
USB1_TXP
Text GLabel 5650 9050 2    50   Input ~ 0
USB1_RXN
Text GLabel 5650 9150 2    50   Input ~ 0
USB1_RXP
Text GLabel 3950 9250 0    50   Input ~ 0
USB2_TXN
Text GLabel 3950 9150 0    50   Input ~ 0
USB2_TXP
Text GLabel 3950 9550 0    50   Input ~ 0
USB2_RXN
Text GLabel 3950 9450 0    50   Input ~ 0
USB2_RXP
Text GLabel 12250 4050 2    50   Input ~ 0
USB2_TXN
Text GLabel 12250 4150 2    50   Input ~ 0
USB2_TXP
Text GLabel 12250 3750 2    50   Input ~ 0
USB2_RXN
Text GLabel 12250 3850 2    50   Input ~ 0
USB2_RXP
Text GLabel 14350 3450 2    50   Input ~ 0
USB1_TXN
Text GLabel 14350 3550 2    50   Input ~ 0
USB1_TXP
Text GLabel 14350 3750 2    50   Input ~ 0
USB1_RXN
Text GLabel 14350 3850 2    50   Input ~ 0
USB1_RXP
Text Notes 12500 5100 0    50   ~ 0
TODO: AC caps on USB3/PCIe? RX/TX correct?
Text GLabel 14350 4050 2    50   Input ~ 0
USB1_TXN
Text GLabel 14350 4150 2    50   Input ~ 0
USB1_TXP
Text GLabel 14350 3150 2    50   Input ~ 0
USB1_RXN
Text GLabel 14350 3250 2    50   Input ~ 0
USB1_RXP
Text GLabel 12250 3150 2    50   Input ~ 0
USB2_RXN
Text GLabel 12250 3250 2    50   Input ~ 0
USB2_RXP
Text GLabel 12250 3450 2    50   Input ~ 0
USB2_TXN
Text GLabel 12250 3550 2    50   Input ~ 0
USB2_TXP
Text GLabel 5650 5750 2    50   Input ~ 0
PCIE1_TXP
Text GLabel 5650 5850 2    50   Input ~ 0
PCIE1_TXN
Text GLabel 5650 6050 2    50   Input ~ 0
PCIE1_RXP
Text GLabel 5650 6150 2    50   Input ~ 0
PCIE1_RXN
Text GLabel 8600 6800 0    50   Input ~ 0
PCIE1_TXP
Text GLabel 8600 6900 0    50   Input ~ 0
PCIE1_TXN
Text GLabel 8600 7200 0    50   Input ~ 0
PCIE1_RXP
Text GLabel 8600 7300 0    50   Input ~ 0
PCIE1_RXN
$Comp
L Connector:HDMI_D_1.4 J7
U 1 1 60B1DC71
P 16650 3300
F 0 "J7" H 17080 3346 50  0000 L CNN
F 1 "HDMI_D_1.4" H 17080 3255 50  0000 L CNN
F 2 "Connector_HDMI:HDMI_Micro-D_Molex_46765-0x01" H 16675 3300 50  0001 C CNN
F 3 "http://pinoutguide.com/PortableDevices/micro_hdmi_type_d_pinout.shtml" H 16675 3300 50  0001 C CNN
	1    16650 3300
	1    0    0    -1  
$EndComp
Text GLabel 3950 11650 0    50   Input ~ 0
HDMI_CLKN
Text GLabel 3950 11750 0    50   Input ~ 0
HDMI_CLKP
Text GLabel 3950 11950 0    50   Input ~ 0
HDMI_TX0N
Text GLabel 3950 12050 0    50   Input ~ 0
HDMI_TX0P
Text GLabel 3950 12250 0    50   Input ~ 0
HDMI_TX1N
Text GLabel 3950 12350 0    50   Input ~ 0
HDMI_TX1P
Text GLabel 3950 12550 0    50   Input ~ 0
HDMI_TX2N
Text GLabel 3950 12650 0    50   Input ~ 0
HDMI_TX2P
Text GLabel 3950 12850 0    50   Input ~ 0
HDMI_AUXP
Text GLabel 3950 12950 0    50   Input ~ 0
HDMI_AUXN
Text GLabel 16250 3200 0    50   Input ~ 0
HDMI_CLKN
Text GLabel 16250 3100 0    50   Input ~ 0
HDMI_CLKP
Text GLabel 16250 3000 0    50   Input ~ 0
HDMI_TX0N
Text GLabel 16250 2900 0    50   Input ~ 0
HDMI_TX0P
Text GLabel 16250 2800 0    50   Input ~ 0
HDMI_TX1N
Text GLabel 16250 2700 0    50   Input ~ 0
HDMI_TX1P
Text GLabel 16250 2600 0    50   Input ~ 0
HDMI_TX2N
Text GLabel 16250 2500 0    50   Input ~ 0
HDMI_TX2P
Text GLabel 16400 4950 0    50   Input ~ 0
HDMI_AUXP
Text GLabel 16400 5050 0    50   Input ~ 0
HDMI_AUXN
Text Notes 15550 4750 0    50   ~ 0
TODO: port protector
Text Notes 2250 4550 0    50   ~ 0
TODO: break out ETH to FPC header?
Text GLabel 16950 7850 2    50   Input ~ 0
USB13_DN
Text GLabel 16950 7750 2    50   Input ~ 0
USB13_DP
Text GLabel 16950 8350 2    50   Input ~ 0
USB14_DN
Text GLabel 16950 8250 2    50   Input ~ 0
USB14_DP
Text Notes 17250 10000 0    50   ~ 0
TODO: check pinout
$Comp
L reform2:WM8960 U4
U 1 1 60B34231
P 12000 12550
F 0 "U4" H 12000 14165 50  0000 C CNN
F 1 "WM8960" H 12000 14074 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-32-1EP_5x5mm_P0.5mm_EP3.3x3.3mm" H 11950 14300 50  0001 C CNN
F 3 "" H 11950 14300 50  0001 C CNN
	1    12000 12550
	1    0    0    -1  
$EndComp
Text GLabel 10950 6800 0    50   Input ~ 0
PCIE2_TXP
Text GLabel 10950 6900 0    50   Input ~ 0
PCIE2_TXN
Text GLabel 10950 7200 0    50   Input ~ 0
PCIE2_RXP
Text GLabel 10950 7300 0    50   Input ~ 0
PCIE2_RXN
Text Notes 8900 9450 0    50   ~ 0
TODO: UIM
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 60B6E428
P 13850 11750
F 0 "H1" H 13950 11799 50  0000 L CNN
F 1 "MountingHole_Pad" H 13950 11708 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_DIN965_Pad" H 13850 11750 50  0001 C CNN
F 3 "~" H 13850 11750 50  0001 C CNN
	1    13850 11750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 60B6E5DB
P 13950 12000
F 0 "H2" H 14050 12049 50  0000 L CNN
F 1 "MountingHole_Pad" H 14050 11958 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_DIN965_Pad" H 13950 12000 50  0001 C CNN
F 3 "~" H 13950 12000 50  0001 C CNN
	1    13950 12000
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 60B6E9EA
P 14100 12250
F 0 "H3" H 14200 12299 50  0000 L CNN
F 1 "MountingHole_Pad" H 14200 12208 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_DIN965_Pad" H 14100 12250 50  0001 C CNN
F 3 "~" H 14100 12250 50  0001 C CNN
	1    14100 12250
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 60B6EB43
P 14250 12500
F 0 "H4" H 14350 12549 50  0000 L CNN
F 1 "MountingHole_Pad" H 14350 12458 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_DIN965_Pad" H 14250 12500 50  0001 C CNN
F 3 "~" H 14250 12500 50  0001 C CNN
	1    14250 12500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 60B70900
P 13850 12750
F 0 "#PWR0107" H 13850 12500 50  0001 C CNN
F 1 "GND" H 13855 12577 50  0000 C CNN
F 2 "" H 13850 12750 50  0001 C CNN
F 3 "" H 13850 12750 50  0001 C CNN
	1    13850 12750
	1    0    0    -1  
$EndComp
Wire Wire Line
	14250 12600 13850 12600
Wire Wire Line
	13850 11850 13850 12100
Connection ~ 13850 12600
Wire Wire Line
	13850 12600 13850 12750
Wire Wire Line
	14100 12350 13850 12350
Connection ~ 13850 12350
Wire Wire Line
	13850 12350 13850 12600
Wire Wire Line
	13950 12100 13850 12100
Connection ~ 13850 12100
Wire Wire Line
	13850 12100 13850 12350
$Comp
L Mechanical:MountingHole GFX1
U 1 1 60B85544
P 15450 12000
F 0 "GFX1" H 15550 12046 50  0000 L CNN
F 1 "MountingHole" H 15550 11955 50  0000 L CNN
F 2 "footprints:pocket-reform" H 15450 12000 50  0001 C CNN
F 3 "~" H 15450 12000 50  0001 C CNN
	1    15450 12000
	1    0    0    -1  
$EndComp
$Comp
L Connector:SIM_Card J8
U 1 1 60B8EA2D
P 9200 10150
F 0 "J8" H 9830 10250 50  0000 L CNN
F 1 "SIM_Card" H 9830 10159 50  0000 L CNN
F 2 "footprints:TE_2199337-5" H 9200 10500 50  0001 C CNN
F 3 " ~" H 9150 10150 50  0001 C CNN
	1    9200 10150
	1    0    0    -1  
$EndComp
Text GLabel 9900 8000 2    50   Input ~ 0
UIM_CLK
Text GLabel 9900 7900 2    50   Input ~ 0
UIM_RST
Text GLabel 9900 8100 2    50   Input ~ 0
UIM_IO
Text GLabel 9900 7800 2    50   Input ~ 0
UIM_VPP
Text GLabel 8700 10050 0    50   Input ~ 0
UIM_CLK
Text GLabel 8700 9950 0    50   Input ~ 0
UIM_RST
Text GLabel 8700 10350 0    50   Input ~ 0
UIM_IO
Text GLabel 8700 10250 0    50   Input ~ 0
UIM_VPP
Text Notes 8850 2350 0    50   ~ 0
DM3CS-SF
$EndSCHEMATC
