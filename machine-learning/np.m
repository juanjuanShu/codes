A = [
0         -2.1/8.3    1.2/8.3    -0.5/8.3;
-0.8/10.2  0          -3.5/10.2   1.8/10.2
-1.2/4    -0.2/4      0          0.5/4;
0.2/2     -0.3/2      -0.4/2      0
]

V = [-3.02/8.3 ;4.79/10.2; -6.72/4;8.89/2]


X0 = [0 ; 0 ;0;0];
X = [0.5; -1.1;2.3;-4.2];
i = 0;
while(i < 7)
  X0 = A * X0 + V
  i++;
end