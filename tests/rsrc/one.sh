#!/home/bvalarezo/hw3-bvalarezo/smash -d
echo "I am testing batch mode on smash"
echo "You should see some debug messages in one.log"
#I should be ignored
pwd
ls -l
uname -a > /home/bvalarezo/hw3-bvalarezo/tests/rdump/one.txt
echo "Check one.txt in tests/rdump/"
echo $?
echo Goodbye!
exit