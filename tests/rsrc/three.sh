#!/home/bvalarezo/hw3-bvalarezo/smash
echo "I am some of the built-in commands in smash"
echo "You should see some debug messages in 3.log"
sleep 5 &
kill -19 1
sleep 60 &
kill -9 2 #send SIGKILL
bg 1
jobs > /home/bvalarezo/hw3-bvalarezo/tests/rdump/3a.txt
cd tests/rdump/
pwd > 3b.txt
echo "testing echo to a file called 3c.txt" > 3c.txt
echo $?
echo $0
echo "Check *.txt in tests/rdump/"
jobs > 3d.txt
exit