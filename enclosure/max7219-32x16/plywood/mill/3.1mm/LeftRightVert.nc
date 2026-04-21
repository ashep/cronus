(LeftRightVert)
G90 G94
G17
G21
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)
G28 G91 Z0
G90

(2D Pocket3)
S10000 M3
G17 G90 G94
G54
G0 X1.095 Y3.696
Z15
G1 Z5 F200
Z0.2
Z-0.775
Y3.715
Y69.09
G2 X1.295 Y69.29 I0.2 J0
G1 X1.395
G2 X1.595 Y69.09 I0 J-0.2
G1 Y3.715
G3 X1.795 Y3.515 I0.2 J0
G1 X1.895
G3 X2.095 Y3.715 I0 J0.2
G1 Y69.09
G2 X2.294 Y69.29 I0.2 J0
G1 X2.399 Y69.291
G2 X2.6 Y69.091 I0.001 J-0.2
G1 Y3.703
G2 X2.401 Y3.503 I-0.2 J0
G1 X1.296 Y3.496
G2 X1.095 Y3.696 I-0.001 J0.2
G1 Z-1.55
Y3.715
Y69.09
G2 X1.295 Y69.29 I0.2 J0
G1 X1.395
G2 X1.595 Y69.09 I0 J-0.2
G1 Y3.715
G3 X1.795 Y3.515 I0.2 J0
G1 X1.895
G3 X2.095 Y3.715 I0 J0.2
G1 Y69.09
G2 X2.294 Y69.29 I0.2 J0
G1 X2.399 Y69.291
G2 X2.6 Y69.091 I0.001 J-0.2
G1 Y3.703
Z15
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)

(2D Contour7)
G0 X42.6 Y47.517
G1 Z15 F200
Z5
Z0.2
Z-1.05
Y25.283
X39.5
Y4.1
Y3.6
X28.75
Y1
Y0.5
X14.35
Y3.6
X1
X0.5
Y69.2
X14.35
Y71.8
Y72.3
X28.75
Y69.2
X39
X39.5
Y47.517
X42.1
X42.6
Z-2.1
Y25.283
X39.5
Y16.435
Z-1.1
Y14.436
Z-2.1
Y3.6
X28.75
Y0.5
X14.35
Y3.6
X10.377
Z-1.1
X8.377
Z-2.1
X0.5
Y69.2
X8.169
Z-1.1
X10.169
Z-2.1
X14.35
Y72.3
X28.75
Y69.2
X39.5
Y56.686
Z-1.1
Y54.686
Z-2.1
Y47.517
X42.6
Z-3.1
Y25.283
X39.5
Y16.435
Z-1.1
Y14.436
Z-3.1
Y3.6
X28.75
Y0.5
X14.35
Y3.6
X10.377
Z-1.1
X8.377
Z-3.1
X0.5
Y69.2
X8.169
Z-1.1
X10.169
Z-3.1
X14.35
Y72.3
X28.75
Y69.2
X39.5
Y56.686
Z-1.1
Y54.686
Z-3.1
Y47.517
X42.6
Z5
Z15

G28 G91 Z0
G90
G28 G91 X0 Y0
G90
M5
M30
