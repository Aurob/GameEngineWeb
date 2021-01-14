i=0
for file in *.gif; do
	mv "$file" "$i.gif"
	i=$((i+1))
done
