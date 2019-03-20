#Setup sockets
socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
P1=$!
sleep 0.5
read -p "Enter first socat in: " firstin
read -p "Enter first socat out: " firstout

socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
P2=$!
sleep 0.5
read -p "Enter second socat in: " secondin
read -p "Enter second socat out: " secondout

#Start Simulators
echo $PWD &
python3 $PWD/sims/MsgCreator.py > /dev/pts/$firstin &
P3=$!

python3 $PWD/sims/RGBMatrix.py < /dev/pts/$secondout &
P4=$!

sleep 3

bash -c "./GAS.out GASconfig.json /dev/pts/$firstout /dev/pts/$secondin"
#python3 sims/write_matrix.py > /dev/pts/$secondin
P5=$!

wait $P1 $P2 $P3 $P4 $P5
