% m = importdata('zero.txt');
% num = size(m, 1);
% n = zeros(num-4, 39);
% 
% for i = 3:num-2
%     n(i-2, 1:13) = m(i, :);
%     n(i-2, 14:26) = m(i+1, :) - m(i-1, :);
%     n(i-2, 27:39) = m(i+2, :) - m(i-2, :);  
% end

% save mfcc39.txt -ascii n
% 
% 

m = importdata('one1.txt');
figure;
[x, y] = size(m);
[x1, y1] = meshgrid(1:x, 1:y);
gpa = pcolor(x1, y1, m');
title('zero MFCC cepstrum');
set(gpa, 'LineStyle', 'none');
colormap(jet);
xlabel('time');
ylabel('No. of features');
print('-dpng','mfcc cepstrum.jpg'); 

%m = importdata('normilized mel cepstrum.txt');
%n= importdata('test.txt');