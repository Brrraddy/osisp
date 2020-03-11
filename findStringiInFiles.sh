echo -n "Directory path: "
read directoryPath
echo -n "Substring to find: "
read substr
echo -n "Min file size: "
read sizeMin
echo -n "Max file size: "
read sizeMax



filesList=$(find $directoryPath -size +$sizeMin'c' -size -$sizeMax'c' -type f -exec grep -rl $substr {} \;)
numberOfFiles=0
wholeSize=0
curFilePath=""
#echo $filesList
for i in ${filesList[*]}
do
	curFilePath=$i
	numberOfFiles=$((numberOfFiles+1))
	curFileSize=$(stat -c %s "$curFilePath")
	wholeSize=$((wholeSize + $curFileSize))
	echo $curFilePath
d
echo "Number of files found: " $numberOfFiles
echo "Whole size: " $wholeSize
