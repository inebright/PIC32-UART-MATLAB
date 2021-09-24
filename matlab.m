port='COM7'; % Edit this with the correct name of your PORT.

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

% Writing some data to the serial port
%overdamped = "OVERDAMPED";
%fprintf(mySerial, overdamped);

underdamped = "UNDERDAMPED"
fprintf(mySerial, underdamped)

% Reading the echo from the PIC32 to verify correct communication
len = 100;
data = zeros(len,1);

referenceData = zeros(len,1);

for i = 1:len
    data(i) = fscanf(mySerial,'%d');
    %disp("test")
    %data(i) = data_read
    referenceData(i) = 180;
end
disp("Before");
disp(data);
disp("finished with collect");
%disp(data)
% Closing serial connection
fclose(mySerial);



plot(1:len,data)
hold on
plot(1:len,referenceData)
hold off
title('Plot of Underdamped Response')
ylabel(' Encoder Angle ')
xlabel(' Number of Samples ');