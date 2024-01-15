# Smarc_SRC
C implementation of float for Smarc_SRC (Smarc audio sampling rate converter)
https://audio-smarc.sourceforge.net/

## Table of contents
1. [Development environments](#dev_env)
2. [Usages](#usage)
3. [To do](#to_do)

## Development environments <a name="dev_env"></a>
* Coding language: C
* IDE: VSCode
* Extensions: C/C++

## Usages <a name="usage"></a>
* Input: input.wav (Bit Depth : 16bit)
* Output: output.wav (Bit Depth : 16bit)
```c
cd ~/Smarc_SRC
./ gcc ./*.c -o main
./ main.exe input.wav output.wav target_samplerate
for example : ./ main.exe input_16kHz.wav output_48kHz.wav 48000
```

## To do <a name="to_do"></a>
* C implementation of fixed-point for Smarc_SRC