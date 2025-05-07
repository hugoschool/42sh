mapfile -t new < tester/test_history.txt
mapfile -t old < tester/test_history.txt.old

return_value=1
for i in "${!new[@]}"; do
    num=$(echo ${new[$i]} | cut -d ':' -f 1)
    ret_new=$(echo ${new[$i]} | cut -d ':' -f 2)
    ret_old=$(echo ${old[$i]} | cut -d ':' -f 2)
    if [ "$ret_new" == "KO" -a "$ret_old" == "OK" ]
    then
        echo -e "::error::\033[1m$num\033[0m has failed"
        return_value=2
    fi
    if [ "$ret_new" == "OK" -a "$ret_old" == "KO" ]
    then
        echo "Test \033[1m$num\033[0m has now passed." >> $GITHUB_OUTPUT
    fi
done
if [ "$return_value" -eq "2" ]
then
    exit 1
fi
