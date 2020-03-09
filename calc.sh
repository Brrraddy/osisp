function checkvalue { 
while true
do
read -p "enter a value: " value
if [[ $value =~ ^[0-9]+$ ]] 
then
	b=$value
	return 1
fi
done
}

function checkoperation {
while true
do
read -p "enter an operation: " op
if [[ $op =~ ^[[+xd]|(-)|(s)|(ss)]$ ]]
	then
	a=$op
	return 1
fi

done
}

function checkother { 
while true
do
read -p "enter a value: " value
if [[ $value =~ ^[0-9]+$ ]] 
then
	c=$value;
	return 1
fi
done
}

checkoperation
case $a in
"+")
	checkvalue
	checkother
	echo $(( $b + $c ))
	;;
"-")
	checkvalue
	checkother
	echo $(( $b - $c ))
	;;
"x")
	checkvalue
	checkother
	echo $(( $b * $c ))
	;;
"d")
	checkvalue
	checkother
	d=$(echo "scale=2; $b / $c" | bc)
	echo $d
	;;
"s")
	checkvalue
	checkother
	sudo s=$(echo "scale=2; $b ^  $c" | bc)
	echo $s
	;;
"ss")
	checkvalue
	ss=$(echo "scale=2; sqrt($b)" | bc)
	echo $ss
	;;
esac
