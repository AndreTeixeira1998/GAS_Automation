#Setup sockets
socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
P1=$!
read -p "Enter first socat in: " firstin
read -p "Enter first socat out: " firstout

socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
P2=$!
read -p "Enter second socat in: " secondin
read -p "Enter second socat out: " secondout

#Start Simulators
echo $PWD &
python $PWD/sims/MsgCreator.py > /dev/pts/$firstin &
P3=$!

python $PWD/sims/RGBMatrix.py < /dev/pts/$secondout &
P4=$!

bash -c "./GAS.out /dev/pts/$firstout /dev/pts/$secondin"
#python sims/write_matrix.py > /dev/pts/$secondin
P5=$!

wait $P1 $P2 $P3 $P4 $P5
