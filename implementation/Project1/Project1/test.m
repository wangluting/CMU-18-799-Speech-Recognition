%Captured audio waveform
[y,Fs,bits] = wavread('capturedaudio.wav');
figure;
plot(y);
xlabel('time');
ylabel('Frequency');
title('Captured audio waveform');
print('-dpng','Audio Wave.jpg'); 

%power spectrum
m = importdata('power spectrum.txt');
figure;
[x, y] = size(m);
[x1, y1] = meshgrid(1:x, 1:y);
gpa = pcolor(x1, y1, m');
title('power spectrum');
set(gpa, 'LineStyle', 'none');
colormap(jet);
xlabel('time');
ylabel('No. of samples');
print('-dpng','power spectrum.jpg'); 

%MFCC cepstrum 
m = importdata('mel cepstrum.txt');
figure;
[x, y] = size(m);
[x1, y1] = meshgrid(1:x, 1:y);
gpa = pcolor(x1, y1, m');
title('MFCC cepstrum');
set(gpa, 'LineStyle', 'none');
colormap(jet);
xlabel('time');
ylabel('No. of features');
print('-dpng','mfcc cepstrum.jpg'); 

%log mel energy before DCT
m = importdata('log mel energy before DCT.txt');
figure;
[x, y] = size(m);
[x1, y1] = meshgrid(1:x, 1:y);
gpa = pcolor(x1, y1, m');
title('log mel energy before DCT');
set(gpa, 'LineStyle', 'none');
colormap(jet);
xlabel('time');
ylabel('No. of windows');
print('-dpng','log mel energy before DCT.jpg'); 

%log mel after IDCT
m = importdata('log mel energy after IDCT.txt');
figure;
[x, y] = size(m);
[x1, y1] = meshgrid(1:x, 1:y);
gpa = pcolor(x1, y1, m');
title('log mel after IDCT');
set(gpa, 'LineStyle', 'none');
colormap(jet);
xlabel('time');
ylabel('No. of windows');
print('-dpng','log mel after IDCT.jpg'); 


