clear;
[Y1,Fs,nbits,hfile]=wavread('2345678.wav');
figure(7)
plot(Y1);
title('truncated audio data');
% [x,y] = size(Y1);
% N = x;
% n = 0:N-1;
% y = fft(Y1,N);
% mag = abs(y);
% f = n*Fs/N;
% figure(8)
% plot(f,mag);
% title('power spectrum using matlab');