% this BAT file is used to convert the Microsoft's COFF library format
% to Borland's OMF library format.
%
implib -a debug\libopen_bcb.lib debug\libopen.dll
impdef -a debug\libopen_bcb.def debug\libopen.dll

