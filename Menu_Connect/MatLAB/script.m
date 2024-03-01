clear;

availablePorts = serialportlist;

s = serialport(availablePorts, 115200);
configureTerminator(s, "LF");


% sending data block
fileSample = fopen('Sample.txt', 'r');
try
    % Check if the serial port is open
    if s.Status == "open"
        info = fscanf(fileSample, '%d');% Data read from sample
        index = 1;
        while(info(index)~=-1)
            infoPart = num2str(info(index)); %Data to send
            writeline(s, infoPart); % Write the data with a newline character
            index = index + 1;
        end
        disp('Data sent.');
    else
        disp('Serial port is not open.');
    end
    
catch
    clear s;
    error('Error communicating with Arduino.');
end

pause(1000); % think how to realise 
%reading data block  -rework

dataArray = zeros(1, 10);

fileID = fopen('Data.txt','w');

while(str2double(readline(s)) == 0)

    try     %basic error catch
        disp("Start of reading");
    
        % read data from Serial and add to array
    
        for i = 1:arraySize
            pause(1);
            data = readline(s); % Read data
            dataArray(i) = data;
            fprintf(fileID, '%d\n', dataArray(i));  %write array in textfile
        end
        disp("End of reading");
    
    catch
        clear s;
        error('Error communicating with Arduino.');
    end
    
end