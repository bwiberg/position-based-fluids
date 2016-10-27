filename = 'large-dam-break.txt';

file = fopen(filename, 'w');

r = 0.05;

bottom_left = [-0.8 0.0 -0.8];
top_right = [0.2 0.9 0.2];

X = bottom_left(1):r:top_right(1);
Y = bottom_left(2):r:top_right(2);
Z = bottom_left(3):r:top_right(3);

length(X)*length(Y)*length(Z);

[Xs, Ys, Zs] = meshgrid(X, Y, Z);

Xv = Xs(:);
Yv = Ys(:);
Zv = Zs(:);

formatspec = '%f %f %f\n';

fprintf(file, '%d \n', length(Xv));

for i=1:length(Xv)
    fprintf(file, formatspec, Xv(i), Yv(i), Zv(i));
end