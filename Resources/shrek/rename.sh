i=0
for file in *.gif; do
	mv "$file" "$i.png"
	i=$((i+1))
done
