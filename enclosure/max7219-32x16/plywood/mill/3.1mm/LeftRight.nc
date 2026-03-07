(LeftRight)
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
G0 X0.895 Y3.74
Z18.1
G1 Z8.1 F200
Z3.3
Z2.325
Y3.759
Y69.056
X0.901 Y69.106
X0.92 Y69.153
X0.95 Y69.193
X0.989 Y69.225
X1.035 Y69.246
X1.135 Y69.278
G2 X1.395 Y69.087 I0.06 J-0.191
G1 Y3.712
G3 X1.595 Y3.512 I0.2 J0
G1 X1.695
G3 X1.895 Y3.712 I0 J0.2
G1 Y69.087
G2 X2.095 Y69.287 I0.2 J0
G1 X2.195
G2 X2.395 Y69.087 I0 J-0.2
G1 Y3.712
G3 X2.602 Y3.512 I0.2 J0
G1 X2.707 Y3.516
G3 X2.9 Y3.716 I-0.007 J0.2
G1 Y69.091
G3 X2.696 Y69.291 I-0.2 J0
G1 X1.091 Y69.26
G3 X0.895 Y69.06 I0.004 J-0.2
G1 Z1.55
Y69.056
Y3.759
X0.9 Y3.714
X0.915 Y3.671
X0.939 Y3.633
X0.972 Y3.601
X1.01 Y3.578
X1.11 Y3.531
G3 X1.395 Y3.712 I0.085 J0.181
G1 Y69.087
G2 X1.595 Y69.287 I0.2 J0
G1 X1.695
G2 X1.895 Y69.087 I0 J-0.2
G1 Y3.712
G3 X2.095 Y3.512 I0.2 J0
G1 X2.195
G3 X2.395 Y3.712 I0 J0.2
G1 Y69.087
G2 X2.588 Y69.287 I0.2 J0
G1 X2.693 Y69.29
G2 X2.9 Y69.091 I0.007 J-0.2
G1 Y4.1
Y3.716
Z18.1
(When using Fusion for Personal Use, the feedrate of rapid)
(moves is reduced to match the feedrate of cutting moves,)
(which can increase machining time. Unrestricted rapid moves)
(are available with a Fusion Subscription.)

(2D Contour7)
G0 X42.9 Y47.567
G1 Z18.1 F200
Z8.1
Z3.3
Z2.05
Y25.233
X39.8
Y4.1
Y3.6
X29
Y1
Y0.5
X14.4
Y3.6
X1
X0.5
Y69.2
X14.4
Y71.8
Y72.3
X29
Y69.2
X39.3
X39.8
Y47.567
X42.4
X42.9
Z1
Y25.233
X39.8
Y16.409
Z2
Y14.409
Z1
Y3.6
X29
Y0.5
X14.4
Y3.6
X10.407
Z2
X8.407
Z1
X0.5
Y69.2
X8.199
Z2
X10.199
Z1
X14.4
Y72.3
X29
Y69.2
X39.8
Y56.716
Z2
Y54.716
Z1
Y47.567
X42.9
Z0
Y25.233
X39.8
Y16.409
Z2
Y14.409
Z0
Y3.6
X29
Y0.5
X14.4
Y3.6
X10.407
Z2
X8.407
Z0
X0.5
Y69.2
X8.199
Z2
X10.199
Z0
X14.4
Y72.3
X29
Y69.2
X39.8
Y56.716
Z2
Y54.716
Z0
Y47.567
X42.9
Z8.1
Z18.1

G28 G91 Z0
G90
G28 G91 X0 Y0
G90
M5
M30
