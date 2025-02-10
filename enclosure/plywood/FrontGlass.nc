(FrontGlass)
G90 G94
G17
G21
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)
G28 G91 Z0
G90

(2D Contour10)
M0
(MANUAL TOOL CHANGE TO T1)
S750 M3
G17 G90 G94
G54
G0 X1 Y69.8
Z15
G1 Z5 F50
Z0.2
Z-0.8
X133.1
G2 X133.6 Y69.3 I0 J-0.5
G1 Y1
G2 X133.1 Y0.5 I-0.5 J0
G1 X1
G2 X0.5 Y1 I0 J0.5
G1 Y69.3
G2 X1 Y69.8 I0.5 J0
G1 Z-1.6
X133.1
G2 X133.6 Y69.3 I0 J-0.5
G1 Y1
G2 X133.1 Y0.5 I-0.5 J0
G1 X1
G2 X0.5 Y1 I0 J0.5
G1 Y69.3
G2 X1 Y69.8 I0.5 J0
G1 Z-2.4
X82
Z-1.9
X84
Z-2.4
X133.1
G2 X133.6 Y69.3 I0 J-0.5
G1 Y20.2
Z-1.9
Y18.2
Z-2.4
Y1
G2 X133.1 Y0.5 I-0.5 J0
G1 X52.1
Z-1.9
X50.1
Z-2.4
X1
G2 X0.5 Y1 I0 J0.5
G1 Y50.1
Z-1.9
Y52.1
Z-2.4
Y69.3
G2 X1 Y69.8 I0.5 J0
G1 Z15

G28 G91 Z0
G90
G28 G91 X0 Y0
G90
M5
M30
