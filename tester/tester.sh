#!/bin/bash

MYSHELL="./../42sh"
REFER="/bin/tcsh -f"
TRAPSIG=0
HISTORY_FILE="test_history.txt"

GREEN="\033[1;32m"
RED="\033[1;31m"
BLUE="\033[1;36m"
ORANGE="\033[1;38;5;208m"
RESET="\033[0m"

CAT=`which cat`
GREP=`which grep`
SED=`which sed`
RM=`which rm`
TR=`which tr`
HEAD=`which head`
TAIL=`which tail`
WC=`which wc`
CHMOD=`which chmod`
EXPR=`which expr`
MKDIR=`which mkdir`
CP=`which cp`
TOUCH=`which touch`

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
IMPROVED_TESTS=0
REGRESSED_TESTS=0

for i in `env | grep BASH_FUNC_ | cut -d= -f1`; do
    f=`echo $i | sed s/BASH_FUNC_//g | sed s/%%//g`
    unset -f $f
done

if [ ! -f "$HISTORY_FILE" ]; then
    $TOUCH "$HISTORY_FILE"
fi

get_previous_state() {
    test_id=$1
    previous_state=`$GREP "^$test_id:" "$HISTORY_FILE" | $CUT -d':' -f2`
    if [ -z "$previous_state" ]; then
        echo "UNKNOWN"
    else
        echo $previous_state
    fi
}

update_history() {
    test_id=$1
    new_state=$2
    
    $GREP -v "^$test_id:" "$HISTORY_FILE" > "$HISTORY_FILE.tmp"
    echo "$test_id:$new_state" >> "$HISTORY_FILE.tmp"
    $MV "$HISTORY_FILE.tmp" "$HISTORY_FILE"
}

disp_test()
{
  id=$1
  $CAT tests | $GREP -A1000 "\[$id\]" | $GREP -B1000 "^\[$id-END\]" | $GREP -v "^\[.*\]"
}

run_script()
{
  SC="$1"
  echo "$SC" > /tmp/.tmp.$$
  . /tmp/.tmp.$$
  $RM -f /tmp/.tmp.$$
}

prepare_test()
{
  local testfn="/tmp/.tester.$$"
  local runnerfn="/tmp/.runner.$$"
  local refoutfn="/tmp/.refer.$$"
  local shoutfn="/tmp/.shell.$$"

  WRAPPER="$runnerfn"

  echo "#!/bin/bash" > $runnerfn
  echo "$SETUP" >> $runnerfn
  echo "/bin/bash -c '"$testfn" | "$MYSHELL" ; echo Shell exit with code \$?' > "$shoutfn" 2>&1" >> $runnerfn
  echo "$CLEAN" >> $runnerfn
  echo "$SETUP" >> $runnerfn
  echo "$TCSHUPDATE" >> $runnerfn
  echo "/bin/bash -c '"$testfn" | "$REFER" ; echo Shell exit with code \$?' > "$refoutfn" 2>&1" >> $runnerfn
  echo "$CLEAN" >> $runnerfn

  echo "#!/bin/bash" > $testfn
  echo "$TESTS" | $TR "²" "\n" >> $testfn

  chmod 755 $testfn
  chmod 755 $runnerfn
}

load_test()
{
  id=$1
  debug=$2
  SETUP=`disp_test "$id" | $GREP "SETUP=" | $SED s/'SETUP='// | $SED s/'"'//g`
  CLEAN=`disp_test "$id" | $GREP "CLEAN=" | $SED s/'CLEAN='// | $SED s/'"'//g`
  NAME=`disp_test "$id" | $GREP "NAME=" | $SED s/'NAME='// | $SED s/'"'//g`
  TCSHUPDATE=`disp_test "$id" | $GREP "TCSHUPDATE=" | $SED s/'TCSHUPDATE='// | $SED s/'"'//g`
  TESTS=`disp_test "$id" | $GREP -v "SETUP=" | $GREP -v "CLEAN=" | $GREP -v "NAME=" | $GREP -v "TCSHUPDATE=" | $GREP -v "TESTS=" | $TR "\n" "²" | $SED s/"²$"//`
  prepare_test
  $WRAPPER
  nb=`$CAT /tmp/.refer.$$ | $GREP -v '^_=' | $GREP -v '^\[1\]' | $WC -l`
  i=1
  ok=1
  while [ $i -le $nb ]
  do
    l=`$CAT /tmp/.refer.$$ | $GREP -v '^_=' | $GREP -v '^\[1\]' | $HEAD -$i | $TAIL -1`
    a=`$CAT /tmp/.shell.$$ | $GREP -v '^_=' | $GREP -v '^\[1\]' | $GREP -- "$l$" | $WC -l`
    if [ $a -eq 0 ]
    then
      ok=0
    fi
    i=`$EXPR $i + 1`
  done

  TOTAL_TESTS=$((TOTAL_TESTS+1))

  previous_state=`$GREP "^$id:" "$HISTORY_FILE" | cut -d':' -f2`
  
  if [ $ok -eq 1 ]
  then
    PASSED_TESTS=$((PASSED_TESTS+1))
    current_state="OK"
    
    if [ "$previous_state" = "KO" ]; then
      IMPROVED_TESTS=$((IMPROVED_TESTS+1))
      status_color=$BLUE
    else
      status_color=$GREEN
    fi
    
    if [ $debug -ge 1 ]
    then
      echo -e "Test $id ($NAME) : ${status_color}OK${RESET}"
      if [ $debug -eq 2 ]
      then
        echo "Output $MYSHELL :"
        $CAT -e /tmp/.shell.$$
        echo "" 
        echo "Output $REFER :"
        $CAT -e /tmp/.refer.$$
        echo ""
      fi
    else
      echo -e "${status_color}OK${RESET}"
    fi
  else
    FAILED_TESTS=$((FAILED_TESTS+1))
    current_state="KO"
    
    if [ "$previous_state" = "OK" ]; then
      REGRESSED_TESTS=$((REGRESSED_TESTS+1))
      status_color=$ORANGE
    else
      status_color=$RED
    fi
    
    if [ $debug -ge 1 ]
    then
      echo -e "Test $id ($NAME) : ${status_color}KO${RESET} - /tmp/test.$$/$id/" 
      $MKDIR -p /tmp/test.$$/$id 2>/dev/null
      $CP /tmp/.shell.$$ /tmp/test.$$/$id/mysh.out
      $CP /tmp/.refer.$$ /tmp/test.$$/$id/tcsh.out
    else
      echo -e "${status_color}KO${RESET}"
    fi
  fi
  
  $GREP -v "^$id:" "$HISTORY_FILE" > "/tmp/.history.tmp.$$"
  echo "$id:$current_state" >> "/tmp/.history.tmp.$$"
  $CP "/tmp/.history.tmp.$$" "$HISTORY_FILE"
  $RM -f "/tmp/.history.tmp.$$"
}

print_stats() {
  echo "===== STATISTIQUES DES TESTS ====="
  echo "Total des tests exécutés: $TOTAL_TESTS"
  echo -e "Tests réussis: ${GREEN}$PASSED_TESTS${RESET}"
  echo -e "Tests échoués: ${RED}$FAILED_TESTS${RESET}"
  echo -e "Tests améliorés (KO → OK): ${BLUE}$IMPROVED_TESTS${RESET}"
  echo -e "Tests dégradés (OK → KO): ${ORANGE}$REGRESSED_TESTS${RESET}"
  
  if [ $TOTAL_TESTS -gt 0 ]; then
    PASS_PERCENTAGE=$((PASSED_TESTS*100/TOTAL_TESTS))
    echo "Taux de réussite: $PASS_PERCENTAGE%"
  fi
  echo "=================================="
}

if [ $TRAPSIG -eq 1 ]
then
  for sig in `trap -l`
  do
    echo "$sig" | grep "^SIG" >/dev/null 2>&1
    if [ $? -eq 0 ]
    then
      trap "echo Received signal $sig !" $sig
    fi
  done
fi

if [ ! -f tests ]
then
  echo "No tests file. Please read README.ME" >&2
  exit 1
fi

if [ ! -f $MYSHELL ]
then
  echo "$MYSHELL not found" >&2
  exit 1
fi

if [ $# -eq 2 ]
then
  echo "Debug mode" >&2
  echo "Shell : $MYSHELL" >&2
  echo "Reference : $REFER" >&2
  echo ""
fi

if [ $# -eq 0 ]
then
  for lst in `cat tests | grep "^\[.*\]$" | grep -vi end | sed s/'\['// | sed s/'\]'//`
  do
    path_backup=$PATH
    load_test $lst 1
    export PATH=$path_backup
  done
  print_stats
else
  if [ $# -eq 1 ]
  then
    load_test $1 0
    print_stats
  else
    if [ "X$1" = "X-d" ]
    then
      load_test $2 2
      print_stats
    else
      load_test $1 2
      print_stats
    fi
  fi
fi
