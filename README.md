# byggern26

How to run: 
make flash # builds and sends the code to the avr


sudo picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyS0 # see output from avr in picocom terminal


for lab 2.2: 
    - de-cross the adress and data cables to the SRAM
    - put lowpass filter on the ALE signal
    - fix star ground and voltage supply