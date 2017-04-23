pkg load signal;
d_freq = 22050;
N = 600;
f0 = 80; #Hz
coef = fir1(N, f0/d_freq, 'high');
disp(N);
for i = [1:length(coef)]
	printf('%.8f\n', coef(i));
endfor
