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
G0 X2 Y69.8
Z15
G1 Z5 F50
Z0.2
Z-0.8
X132.1
G2 X133.6 Y68.3 I0 J-1.5
G1 Y2
G2 X132.1 Y0.5 I-1.5 J0
G1 X2
G2 X0.5 Y2 I0 J1.5
G1 Y68.3
G2 X2 Y69.8 I1.5 J0
G1 Z-1.6
X132.1
G2 X133.6 Y68.3 I0 J-1.5
G1 Y2
G2 X132.1 Y0.5 I-1.5 J0
G1 X2
G2 X0.5 Y2 I0 J1.5
G1 Y68.3
G2 X2 Y69.8 I1.5 J0
G1 Z-2.4
X82.785
Z-1.9
X84.785
Z-2.4
X132.1
G2 X133.6 Y68.3 I0 J-1.5
G1 Y19.415
Z-1.9
Y17.415
Z-2.4
Y2
G2 X132.1 Y0.5 I-1.5 J0
G1 X51.315
Z-1.9
X49.315
Z-2.4
X2
G2 X0.5 Y2 I0 J1.5
G1 Y50.885
Z-1.9
Y52.885
Z-2.4
Y68.3
G2 X2 Y69.8 I1.5 J0
G1 Z15

G28 G91 Z0
G90
G28 G91 X0 Y0
G90
M5
M30
