echo Commit Name:
read name
git add .
git commit -m "$name"
git push -u origin master
