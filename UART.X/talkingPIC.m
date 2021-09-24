port = 'COM5'; % Edit this with the correct name of your PORT.

% Makes sure port is closed
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
fprintf('Opening port %s....\n',port);

% Defining serial variable
mySerial = serial(port, 'BaudRate', 230400);

% Opening serial connection
fopen(mySerial);

%fprintf(mySerial,'%s','\n');

% Writing some data to the serial port
%fprintf(mySerial,'U\n');
while 1
% Reading the echo from the PIC32 to verify correct communication
data_read = fscanf(mySerial,'%s');
%disp('fdfs')
%fprinf('%c',data_read);
disp(data_read);
end

% Closing serial connection
fclose(mySerial);
