X = [10, 9; 2, 3; 1, 2; 7, 6.5; 3, 2.5]
X = X - 4.6
C = (transpose(X)*X)/5
[D, V] = eig(C)
