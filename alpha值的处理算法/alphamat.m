 r = importdata('G:\flipcode\flipcode\newbalpha.txt');
%  b = importdata('EKBÎÄ¼þ16.09.23retricingraytracingnewralpha.txt');
%   c = importdata('EKBÎÄ¼þ16.09.23retricingraytracingnewgalpha.txt');
%    gray = (30r+59b+11c)100;
 
  r=uint8(255);
for i=512
for j = 512
if r(i,j)>0&&r(i,j)<255
    
    r(i,j)=128;
end
% if gray(i,j)=240
%     gray(i,j)=255;
% end
% if gray(i,j)~=0 && gray(i,j)~=255
% gray(i,j)=128;
% end
end
end
imshow(r);
 
% a=imread('CUserskbDesktop16.09.23retricingraytracing555.png');
%  for i=1660
% for j = 11000
% for m=13
% a(i,j,m)=a(i,j,m)gray(i,j);
% 
% end
%  end
%  end
%  imshow(a);
% 
%  

