
%Take inputs from pololu website to draw curves of the dc motor.

StallTorque = input('Please enter the stall torque in oz-inch : ');
StallCurrent = input('Please enter the stall current in mA : ');
RatedVoltage = input('Please enter the rated voltage in Volts : ');
FreeRunCurrent = input('Please enter the free run currennt in mA : ');
FreeRunSpeed = input('Please enter the free run speed in RPM : ');
Resistance = RatedVoltage / StallCurrent;

torque1 = 0;
torque2 = StallTorque*0.0720077906; % oz-inch *  (1/141.611932278)= N-m; 1 oz-in = 0,0720077906kgf-cm

current1 = FreeRunCurrent; % / 1000; %mA to Amps
current2 = StallCurrent;   %/ 1000; %mA to Amps

speed1 = FreeRunSpeed;     %RPM = 1/9.5493 radians per sec
speed2 = 0;

line([torque1 torque2], [current1 current2]);
line([torque1 torque2], [speed1 speed2]);

totSamples = 1000; %will break the torque into 200 discrete values

redlineY = zeros( 1,totSamples);
redlineX = zeros( 1,totSamples);
bluelineY = zeros( 1,totSamples);
bluelineX = zeros( 1,totSamples);
greenlineY = zeros( 1,totSamples);
greenlineX = zeros( 1,totSamples);
orangelineY = zeros( 1,totSamples);
orangelineX = zeros( 1,totSamples);

%a = (torque2 - torque1)/totSamples;
%looper = 1, torque = torque1, looper = 2, torque = torque1 + (t2-t1)/FS
Index = 1;
maxPwrIndex = 1;
maxEffIndex = 1;
lastEff = 0;
lastPwr = 0;

for T = torque1:(torque2 - torque1)/totSamples:torque2
   bluelineY(Index) =  (((speed2 - speed1) / (torque2 - torque1)) * (T - torque1)) + (speed1);
   bluelineX(Index) = T;
   
   redlineY(Index) = (((current2 - current1) / (torque2 - torque1)) * (T - torque1)) + (current1);
   redlineX(Index) = T;
   
   orangelineY(Index) = bluelineY(Index) * T; %Speed into Torque to give the Output power curve
   orangelineX(Index) = T;
   if orangelineY(Index) > lastPwr
       maxPwrIndex = Index;
       lastPwr = orangelineY(Index);
   end
   greenlineY(Index) = orangelineY(Index) / ((redlineY(Index)) * RatedVoltage); %Output power divided by curent line * Voltage (power).
   greenlineX(Index) = T;
   if greenlineY(Index) > lastEff
       maxEffIndex = Index;
       lastEff = greenlineY(Index);
   end
   
   Index = Index + 1;
end


fprintf('Imped�ncia resistiva do motor = %f\n', Resistance)
fprintf('Pot M�xima de Sa�da %f @ Corrente = %f, Torque = %f\n', orangelineY(maxPwrIndex), redlineY(maxPwrIndex), orangelineX(maxPwrIndex))
fprintf('Efici�ncia M�xima %f @ Corrente = %f, Torque = %f\n', greenlineY(maxEffIndex), redlineY(maxEffIndex), orangelineX(maxEffIndex))

grid on;
grid minor;

dim1 = 2;
dim2 = 2;
subplot(dim1,dim2,1);
plot(redlineX, redlineY);
grid on;
grid minor;
title('Corrente vs. Torque');
legend('Corrente');
xlabel('Torque (kgf.cm)');
ylabel('Corrente mA');
subplot(dim1,dim2,2);
plot(bluelineX, bluelineY);
grid on;
grid minor;
title('Velocidade vs. Torque');
legend('Velocidade');
xlabel('Torque (kgf.cm)');
ylabel('Velocidade (RPM)');
subplot(dim1,dim2,3);
grid on;
grid minor;
plot(orangelineX, orangelineY);
grid on;
grid minor;
title('Pot Sa�da');
legend('Pot Sa�da');
ylabel('Pot Sa�da (Velocidade*Torque)');
xlabel('Torque (kgf.cm)');
subplot(dim1,dim2,4);
grid on;
grid minor;
plot( greenlineX, greenlineY);
grid on;
grid minor;
title('Efici�ncia');
legend('Efici�ncia');
ylabel('Eficiencia (Pot. Sa�da/Pot Entrada)');
xlabel('Torque (kgf.cm)');
