y = wavread('capturedaudio.wav');
length=size(y,1);
cursor=50;
window=512;
overall_res=[];
h=hamming(512);
while cursor<length-window
    fft_res=abs(fft(y(cursor:cursor+window-1).*h,512));
    overall_res=[overall_res fft_res];
    cursor=cursor+window;
end
%max(max(overall_res))
figure,pcolor(overall_res);