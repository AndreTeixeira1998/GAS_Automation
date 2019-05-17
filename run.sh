#Setup sockets
socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
P1=$!
sleep 0.25
read -p "Enter first socat in: " firstin
read -p "Enter first socat out: " firstout

socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
P2=$!
sleep 0.25
read -p "Enter second socat in: " secondin
read -p "Enter second socat out: " secondout

#Start Simulators
echo $PWD &
python $PWD/sims/MsgCreator.py > /dev/pts/$firstin &
P3=$!

python $PWD/sims/RGBMatrix.py < /dev/pts/$secondout &
P4=$!

sleep 3

if getopts v: option
then
    bash -c "valgrind ./GAS.out GASconfig.json connString.conf /dev/pts/$firstout /dev/pts/$secondin"
else
    bash -c "./GAS.out GASconfig.json connString.conf /dev/pts/$firstout /dev/pts/$secondin"
fi

#python sims/write_matrix.py > /dev/pts/$secondin
P5=$!

kill $P4
kill $P3
kill $P2
kill $P1
