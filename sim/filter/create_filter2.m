% Filter Setup
N = 32;
Fs = 1000;
Fn = Fs/2;

%% Create Filters
lp1 = fir1(N, 60/Fn, 'low');
lp2 = fir1(N, 120/Fn, 'low');
lp3 = fir1(N, 180/Fn, 'low');
lp4 = fir1(N, 240/Fn, 'low');
lp5 = fir1(N, 300/Fn, 'low');
lp6 = fir1(N, 360/Fn, 'low');
lp7 = fir1(N, 420/Fn, 'low');

hp1 = fir1(N, 60/Fn, 'high');
hp2 = fir1(N, 120/Fn, 'high');
hp3 = fir1(N, 180/Fn, 'high');
hp4 = fir1(N, 240/Fn, 'high');
hp5 = fir1(N, 300/Fn, 'high');
hp6 = fir1(N, 360/Fn, 'high');
hp7 = fir1(N, 420/Fn, 'high');

bp1 = fir1(N, [60/Fn, 180/Fn], 'bandpass');
bp2 = fir1(N, [120/Fn, 240/Fn], 'bandpass');
bp3 = fir1(N, [180/Fn, 300/Fn], 'bandpass');
bp4 = fir1(N, [240/Fn, 360/Fn], 'bandpass');
bp5 = fir1(N, [300/Fn, 420/Fn], 'bandpass');

bs1 = fir1(N, [60/Fn, 180/Fn], 'stop');
bs2 = fir1(N, [120/Fn, 240/Fn], 'stop');
bs3 = fir1(N, [180/Fn, 300/Fn], 'stop');
bs4 = fir1(N, [240/Fn, 360/Fn], 'stop');
bs5 = fir1(N, [300/Fn, 420/Fn], 'stop');

%% Convert to fractional number
% lpf1 = ceil(lp1*2^8);
% lpf2 = ceil(lp2*2^8);
% lpf3 = ceil(lp3*2^8);
% lpf4 = ceil(lp4*2^8);
% lpf5 = ceil(lp5*2^8);
% lpf6 = ceil(lp6*2^8);
% lpf7 = ceil(lp7*2^8);

% hpf1 = ceil(hp1*2^8);
% hpf2 = ceil(hp2*2^8);
% hpf3 = ceil(hp3*2^8);
% hpf4 = ceil(hp4*2^8);
% hpf5 = ceil(hp5*2^8);
% hpf6 = ceil(hp6*2^8);
% hpf7 = ceil(hp7*2^8);

% bpf1 = ceil(bp1*2^8);
% bpf2 = ceil(bp2*2^8);
% bpf3 = ceil(bp3*2^8);
% bpf4 = ceil(bp4*2^8);
% bpf5 = ceil(bp5*2^8);

% bsf1 = ceil(bs1*2^8);
% bsf2 = ceil(bs2*2^8);
% bsf3 = ceil(bs3*2^8);
% bsf4 = ceil(bs4*2^8);
% bsf5 = ceil(bs5*2^8);

%% Plot Filter
% Fs = 8000;
% freqz(lpf1,1, 256, Fs);

%% Print to file
fileID = fopen('filterf_2.csv','w');
fprintf(fileID,'%6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s, %6s\n', 'lpf1', 'lpf2', 'lpf3', 'lpf4', 'lpf5', 'lpf6', 'lpf7', 'hpf1', 'hpf2', 'hpf3', 'hpf4', 'hpf5', 'hpf6', 'hpf7', 'bpf1', 'bpf2', 'bpf3', 'bpf4', 'bpf5', 'bsf1', 'bsf2', 'bsf3', 'bsf4', 'bsf5');
fprintf(fileID,'%6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f, %6f\n', [lp1; lp2; lp3; lp4; lp5; lp6; lp7; hp1; hp2; hp3; hp4; hp5; hp6; hp7; bp1; bp2; bp3; bp4; bp5; bs1; bs2; bs3; bs4; bs5]);
%fprintf(fileID,'%6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d\n', lpf1, lpf2, lpf3, lpf4, lpf5, lpf6, lpf7, hpf1, hpf2, hpf3, hpf4, hpf5, hpf6, hpf7, bpf1, bpf2, bpf3, bpf4, bpf5, bsf1, bsf2, bsf3, bsf4, bsf5);
fclose(fileID);

%% Test Filter
% t = (0:1:199)/Fs;
% f = 1000;
% s = sin(2*pi*f*t);
% filt = filter(lp1,1,s);

% subplot(2,1,1);
% plot(s);

% subplot(2,1,2);
% plot(filt);