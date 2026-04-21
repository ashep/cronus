(LeftRightHoriz)
G90 G94
G17
G21
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)
G28 G91 Z0
G90

(2D Pocket3 2)
S10000 M3
G17 G90 G94
G54
G0 X3.696 Y42.005
Z15
G1 Z5 F200
Z0.2
Z-0.775
X3.715
X69.09
G2 X69.29 Y41.805 I0 J-0.2
G1 Y41.705
G2 X69.09 Y41.505 I-0.2 J0
G1 X3.715
G3 X3.515 Y41.305 I0 J-0.2
G1 Y41.205
G3 X3.715 Y41.005 I0.2 J0
G1 X69.09
G2 X69.29 Y40.806 I0 J-0.2
G1 X69.291 Y40.701
G2 X69.091 Y40.5 I-0.2 J-0.001
G1 X3.703
G2 X3.503 Y40.699 I0 J0.2
G1 X3.496 Y41.804
G2 X3.696 Y42.005 I0.2 J0.001
G1 Z-1.55
X3.715
X69.09
G2 X69.29 Y41.805 I0 J-0.2
G1 Y41.705
G2 X69.09 Y41.505 I-0.2 J0
G1 X3.715
G3 X3.515 Y41.305 I0 J-0.2
G1 Y41.205
G3 X3.715 Y41.005 I0.2 J0
G1 X69.09
G2 X69.29 Y40.806 I0 J-0.2
G1 X69.291 Y40.701
G2 X69.091 Y40.5 I-0.2 J-0.001
G1 X3.703
Z15
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)

(2D Contour7 2)
G0 X47.517 Y0.5
G1 Z15 F200
Z5
Z0.2
Z-1.05
X25.283
Y3.6
X4.1
X3.6
Y14.35
X1
X0.5
Y28.75
X3.6
Y42.1
Y42.6
X69.2
Y28.75
X71.8
X72.3
Y14.35
X69.2
Y4.1
Y3.6
X47.517
Y1
Y0.5
Z-2.1
X25.283
Y3.6
X16.435
Z-1.1
X14.436
Z-2.1
X3.6
Y14.35
X0.5
Y28.75
X3.6
Y32.723
Z-1.1
Y34.723
Z-2.1
Y42.6
X69.2
Y34.931
Z-1.1
Y32.931
Z-2.1
Y28.75
X72.3
Y14.35
X69.2
Y3.6
X56.686
Z-1.1
X54.686
Z-2.1
X47.517
Y0.5
Z-3.1
X25.283
Y3.6
X16.435
Z-1.1
X14.436
Z-3.1
X3.6
Y14.35
X0.5
Y28.75
X3.6
Y32.723
Z-1.1
Y34.723
Z-3.1
Y42.6
X69.2
Y34.931
Z-1.1
Y32.931
Z-3.1
Y28.75
X72.3
Y14.35
X69.2
Y3.6
X56.686
Z-1.1
X54.686
Z-3.1
X47.517
Y0.5
Z5
Z15

G28 G91 Z0
G90
G28 G91 X0 Y0
G90
M5
M30
