#!/bin/bash
function printusage
{
    echo "wrong input parameters!"
    echo "Usage:"
    echo "./startdatacall.sh Operation mode:start/stop/clean [IP Version:4/6/7 | multiple PDN(two ipv4): 4,4] [qcqmi number:0-8(default:0)] "
	echo "[auto mtu update(default:enable)] [auto ip assign(default:enable]"
    echo -e "Default: OP:$OP, IP:$IPVERSION, /dev/qcqmi$QCQMI, MTU UPDATE:$MTUUPDATE, AUTO IP ASSIGN:$IPASSIGN, CONFIG FILE:$CONFIGFILE\n"
    echo "Example: start one data call, ip version:4, qcqmi index 0, mtu auto update:1, auto ip assign:1, file name:testconfig.txt"
    echo -e "./startdatacall.sh start 4 0 1 1 testconfig.txt\n"
    echo "QMAP Mode Example: stop 1st data call"
    echo -e "./startdatacall.sh stop 1\n"
    echo "Example: start two ipv4 data call, qcqmi index 0, disable mtu auto update, disable auto ip assignment"
    echo -e "./startdatacall.sh start 4,4 0 0 0 testconfig.txt\n"
    echo "QMAP Mode Example: stop 2nd data call"
    echo -e "./startdatacall.sh stop 2\n"
    echo "kill all the processes launched and temporarily created files by startdatacall.sh"
    echo -e "./startdatacall.sh clean\n"
    exit 1;
} 
OP=${1:-start}
IPVERSION=${2:-4}
QCQMI=${3:-0}
MTUUPDATE=${4:-1}
IPASSIGN=${5:-1}
CONFIGFILE=${6:-testconfig.txt}

if [ "$1" == "start" ]; then
    /tmp/packingdemohostx86_64 -q -v $IPVERSION -i $QCQMI -z 0 -m $MTUUPDATE -u $IPASSIGN -f ./$CONFIGFILE > output.txt & 
    ppid=$!
    echo $ppid > ppid.txt
    # store ip version
    echo $IPVERSION > ipversion.txt
    counter=1
    while [ $counter -le 30 ]
    do
        pgrep -P $ppid > pids.txt
        sleep 1
        ((counter++))
    done
elif [ "$1" == "stop" ]; then
    if grep -q ',' ipversion.txt; then
        ipver1=`cat ipversion.txt | cut -d "," -f1`
        ipver2=`cat ipversion.txt | cut -d "," -f2`
        ipver=`cat ipversion.txt | cut -d "," -f$2`
        if [ "$ipver1" == 7 ] || [ "$ipver2" == 7 ]  
        then
            default=1
            position=$2
            if [ "$2" == 2 ]; then
                if [ "$ipver1" == 7 ]; then
                    default=2
                    position=$(($2 + 1))
                fi
            fi
            if [ "$ipver" == 7 ]; then 
                index=$(($2 + $default))
                pid=`sed -n "$index p" pids.txt`
                rm /tmp/$pid.txt
            fi
            pid=`sed -n "$position p" pids.txt` 
            rm /tmp/$pid.txt
        else
            pid=`sed -n "$2 p" pids.txt` 
            rm /tmp/$pid.txt
        fi
    else
        ipver=`cat ipversion.txt`
        if [ "$ipver" == 7 ]; then
            default=1
            index=$(($2 + $default))
            pid=`sed -n "$index p" pids.txt`
            rm /tmp/$pid.txt
        fi
        pid=`sed -n "$2 p" pids.txt`
        rm /tmp/$pid.txt 
    fi
elif [ "$1" == "clean" ]; then
    ls pids.txt 1>/dev/null 2>&1
    if [ "$?" -eq 0 ]; then
        file="./pids.txt"
        while read -r LINE; do
            pid=$LINE
            if [ -f "/tmp/$pid.txt" ]; then
            rm /tmp/$pid.txt
            fi
        done < $file
    fi
    rm pids.txt
    rm ppid.txt
    rm ipversion.txt
    if [ -f "output.txt" ]; then
        rm output.txt
    fi
else
	printusage
fi
