#!/home/bvalarezo/hw3-bvalarezo/smash -d
echo "I am testing batch mode on smash"
echo "You should see some debug messages in 1.log"
#I should be ignored
pwd
ls -l
uname -a > /home/bvalarezo/hw3-bvalarezo/tests/rdump/1.txt
echo "Check 1.txt in tests/rdump/"
echo $?
echo Goodbye!
exit