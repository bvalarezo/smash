#!/home/bvalarezo/hw3-bvalarezo/smash
echo "I am testing job control mode on smash"
echo "You should see some debug messages in 2.log"
sleep 5 &
kill -19 1
sleep 4 &
sleep 3 &
sleep 2 &
sleep 1 &
jobs > /home/bvalarezo/hw3-bvalarezo/tests/rdump/2a.txt
bg 1
echo "Going to call 'sleep 10' here to wait for all the jobs to finish. Please hold :)"
sleep 10
jobs > /home/bvalarezo/hw3-bvalarezo/tests/rdump/2b.txt
echo "Check 2a.txt and 2b.txt in tests/rdump/"
exit