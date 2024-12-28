(FrontGlass)
(T1 D=1 CR=0 - ZMIN=-2.4 - flat end mill)
G90 G94
G17
G21
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)
G28 G91 Z0
G90

(2D Contour3)
M0
(MANUAL TOOL CHANGE TO T1)
S1000 M3
G17 G90 G94
G54
G0 X1.2 Y68.1
Z10
G1 Z5 F75
Z0.2 F50
Z-1.1
Y68.092 Z-1.138 F75
Y68.071 Z-1.171
Y68.038 Z-1.192
Y68 Z-1.2
Y67.9
X1.208 Y67.862
X1.229 Y67.829
X1.262 Y67.808
X1.3 Y67.8
X130.1
G2 X131.4 Y66.5 I0 J-1.3
G1 Y1.3
G2 X130.1 Y0 I-1.3 J0
G1 X1.3
G2 X0 Y1.3 I0 J1.3
G1 Y66.5
G2 X1.3 Y67.8 I1.3 J0
G1 X1.338 Y67.808
X1.371 Y67.829
X1.392 Y67.862
X1.4 Y67.9
Y68
Y68.038 Z-1.192
Y68.071 Z-1.171
Y68.092 Z-1.138
Y68.1 Z-1.1
Z5
X1.2
Z-1 F50
Z-2.3
Y68.092 Z-2.338 F75
Y68.071 Z-2.371
Y68.038 Z-2.392
Y68 Z-2.4
Y67.9
X1.208 Y67.862
X1.229 Y67.829
X1.262 Y67.808
X1.3 Y67.8
X79.972
Z-1.4
X81.972
Z-2.4 F50
X130.1 F75
G2 X131.4 Y66.5 I0 J-1.3
G1 Y19.628
Z-1.4
Y17.628
Z-2.4 F50
Y1.3 F75
G2 X130.1 Y0 I-1.3 J0
G1 X51.428
Z-1.4
X49.428
Z-2.4 F50
X1.3 F75
G2 X0 Y1.3 I0 J1.3
G1 Y48.172
Z-1.4
Y50.172
Z-2.4 F50
Y66.5 F75
G2 X1.3 Y67.8 I1.3 J0
G1 X1.338 Y67.808
X1.371 Y67.829
X1.392 Y67.862
X1.4 Y67.9
Y68
Y68.038 Z-2.392
Y68.071 Z-2.371
Y68.092 Z-2.338
Y68.1 Z-2.3
Z10

G28 G91 Z0
G90
G28 G91 X0 Y0
G90
M5
M30