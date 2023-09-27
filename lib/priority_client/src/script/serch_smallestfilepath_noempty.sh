#!/bin/bash

check_file() {
	echo "$1" | while read line
        do
                ret=`echo $line | awk '{print $7}'`
		if [ -n "$ret" ] && [ ! -L ${ret} ] && [ -s ${ret} ] ; then
			echo $ret
			end_flag=1
			exit 0
		fi 
        done
}

result_ls=`ls -lSr --time-style="+%s" $1* `

result_check=`check_file "$result_ls" 2>/dev/null`
if [ -n "$result_check" ] ; then
	echo $result_check
fi
