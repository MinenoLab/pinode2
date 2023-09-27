#/!bin/sh
BASEDIR=$(cd $(dirname $0); pwd)
date=$(date +"%Y %m %d %H:%M:%S")
filedate=$(date +"%Y%m%d_%H%M%S")

echo $BASEDIR
echo $date
EXEC=$(cat makefile | grep "TARGET_C =" | awk '{print $3}')

#if [ "$(whoami)" != "root" ]; then
#	echo ${date}" Require root privilege"  1>&2
#	exit 1
#fi

#if same script is running, exit the script.
SCRIPT_PID=${BASEDIR}"/run_script.pid"

ps alx | head -n 1
ps alx | grep $EXEC

if [ -f $SCRIPT_PID ]; then
	PID=$(cat $SCRIPT_PID)
	echo $PID
	if (ps -e | awk '{print $1}' | grep $PID >/dev/null); then
		echo ${date}" client is runing. exit()" 
		exit 1
	fi
fi

echo ${date}"start server" 


echo $$ > $SCRIPT_PID
chmod 777 $SCRIPT_PID
trap 'rm -f $SCRIPT_PID' EXIT

#LOGFILE=${BASEDIR}"/log/"${filedate}"_server.log"
#LOGFILE=${BASEDIR}"/log/client.log"
#./$EXEC >> $LOGFILE
#script -c ./$EXEC /dev/null | tee -a $LOGFILE
./$EXEC #>> $LOGFILE

#rm $SCRIPT_PID
exit 0
