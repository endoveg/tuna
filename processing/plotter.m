A = load("output.dat");
f = figure('visible','off');
filename = "yin.png";
plot(A);
print -dpng filename;	
