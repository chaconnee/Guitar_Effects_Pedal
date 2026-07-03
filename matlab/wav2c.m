%% wav2c.m —— WAV 文件转 C 头文件 (用于 cab_sim 卷积)
%
% 功能: 读取 IR WAV 文件，重采样到 32kHz，截断，归一化，输出 .h 文件
% 用法: 修改下方参数区，运行脚本即可生成头文件
%
% 对应 C 代码: APP/effects/cab_sim/cab_sim.c cab_init()

clear; clc; close all;

% 检测是否为 Octave 环境
isOctave = exist('OCTAVE_VERSION', 'builtin') > 0;
if isOctave
    pkg load signal;
    output_precision(10);
end

%% ========== 参数区 (按需修改) ==========

% 输入 WAV 文件路径
IR_FILE    = 'IR/OwnHammer_412_MES-ST_BS-90_57-00.wav';

% 输出头文件路径
OUTPUT_DIR = '../APP/effects/cab_sim';
OUTPUT_NAME = 'ir_OwnHammer_412';   % 变量名和文件名

% 目标参数
FS         = 44100;    % 目标采样率 (Hz)
IR_SIZE    = 897;      % 截断长度 (FFT_SIZE - BLOCK_SIZE + 1)
IR_PEAK    = 0.5;      % 峰值归一化目标

% 显示设置
PLOT_EN    = true;     % 是否绘制分析图
COLS       = 10;       % 每行输出的样本数


%% ========== 1. 加载 IR ==========
fprintf('\n[1/4] 加载 IR: %s\n', IR_FILE);

[ir_raw, fs_orig] = audioread(IR_FILE);
fprintf('      原始采样率: %d Hz, 样本数: %d, 时长: %.1f ms\n', ...
        fs_orig, length(ir_raw), length(ir_raw)/fs_orig*1000);

% 多声道取第一声道
if size(ir_raw, 2) > 1
    ir_raw = ir_raw(:, 1);
    fprintf('      已截取为单声道\n');
end


%% ========== 2. 重采样 ==========
fprintf('\n[2/4] 重采样: %d Hz -> %d Hz\n', fs_orig, FS);

if fs_orig ~= FS
    [p, q] = rat(FS / fs_orig);
    ir_resampled = resample(ir_raw, p, q);
    fprintf('      重采样完成: %d -> %d 样本\n', length(ir_raw), length(ir_resampled));
else
    ir_resampled = ir_raw;
    fprintf('      采样率一致，无需重采样\n');
end


%% ========== 3. 截断 + 归一化 ==========
fprintf('\n[3/4] 截断 + 峰值归一化\n');

% 截断到 IR_SIZE
ir_len = min(IR_SIZE, length(ir_resampled));
ir_used = ir_resampled(1:ir_len);
fprintf('      截断: %d 样本 (%.1f ms)\n', ir_len, ir_len/FS*1000);

% 去除直流偏移
dc_offset = mean(ir_used);
ir_used = ir_used - dc_offset;
fprintf('      直流偏移: %.6f (已去除)\n', dc_offset);

% 峰值归一化到 0.8 (参考 NeuralRack)
peak_before = max(abs(ir_used));
ir_used = ir_used / peak_before * 0.8;
fprintf('      峰值归一化: %.6f -> 0.8\n', peak_before);

% 功率归一化 (参考 NeuralRack, norm=0)
ir_pow = sum(ir_used.*ir_used);  % 总功率，不是平均功率
ir_gain = 1.5 / ir_pow;  % NeuralRack norm=0 时的增益
ir_used = ir_used * ir_gain;
fprintf('      功率归一化: 总功率 %.6f, 增益 %.6f\n', ir_pow, ir_gain);
fprintf('      归一化后峰值: %.6f\n', max(abs(ir_used)));

% IR 能量分析
cum_e = cumsum(ir_used.^2);
total_e = sum(ir_used.^2);
used_pct = cum_e(end) / total_e * 100;
fprintf('      能量覆盖: %.2f%% (@ %d 样本)\n', used_pct, ir_len);


%% ========== 4. 输出 C 头文件 ==========
fprintf('\n[4/4] 输出头文件\n');

% 创建输出目录
if ~exist(OUTPUT_DIR, 'dir')
    mkdir(OUTPUT_DIR);
end

% 变量名处理 (替换特殊字符为下划线)
var_name = regexprep(OUTPUT_NAME, '[^a-zA-Z0-9_]', '_');
macro_name = upper(var_name);

% 文件路径
filepath = fullfile(OUTPUT_DIR, [OUTPUT_NAME '.h']);
fid = fopen(filepath, 'w');

% 写入头文件保护
fprintf(fid, '#ifndef %s_H\n', macro_name);
fprintf(fid, '#define %s_H\n\n', macro_name);
fprintf(fid, '#include <stdint.h>\n\n');

% 写入长度宏
fprintf(fid, '#define %s_LENGTH %d\n\n', macro_name, ir_len);

% 写入数组声明
fprintf(fid, 'static const float %s[%s_LENGTH] = {\n', var_name, macro_name);

% 写入样本数据
for i = 1:ir_len
    if mod(i, COLS) == 1
        fprintf(fid, '    ');
    end
    
    if i < ir_len
        fprintf(fid, '%.10ff,', ir_used(i));
    else
        fprintf(fid, '%.10ff', ir_used(i));  % 最后一个不加逗号
    end
    
    if mod(i, COLS) == 0 || i == ir_len
        fprintf(fid, '\n');
    end
end

fprintf(fid, '};\n\n');
fprintf(fid, '#endif /* %s_H */\n', macro_name);
fclose(fid);

fprintf('      已生成: %s\n', filepath);
fprintf('      变量名: %s\n', var_name);
fprintf('      宏前缀: %s\n', macro_name);
fprintf('      样本数: %d, 文件大小: %.1f KB\n', ir_len, ir_len*11/1024);


%% ========== 可选: 绘制分析图 ==========
if PLOT_EN
    figure('Name', 'IR 分析', 'Position', [50 50 1200 500]);
    
    % 时域波形
    subplot(1,3,1);
    t = (0:ir_len-1) / FS * 1000;
    plot(t, ir_used, 'b');
    xlabel('时间 (ms)'); ylabel('幅度');
    title(sprintf('IR 时域波形 (%d Hz, %d 样本)', FS, ir_len));
    grid on;
    
    % 频率响应
    subplot(1,3,2);
    Nfft = 4096;
    H = fft(ir_used, Nfft);
    freqs = (0:Nfft/2-1) * FS / Nfft;
    semilogx(freqs, 20*log10(abs(H(1:Nfft/2))+1e-10), 'b');
    xlabel('频率 (Hz)'); ylabel('幅度 (dB)');
    title('IR 频率响应');
    grid on; xlim([20 FS/2]); ylim([-60 10]);
    
    % 能量累积
    subplot(1,3,3);
    plot(t, cum_e/total_e*100, 'r', 'LineWidth', 1.5);
    xlabel('时间 (ms)'); ylabel('累积能量 (%)');
    title(sprintf('能量累积 (%.1f%% @ %d 样本)', used_pct, ir_len));
    grid on;
    
    sgtitle(sprintf('WAV -> C 转换: %s', OUTPUT_NAME), 'FontSize', 12);
end

fprintf('\n完成!\n');
