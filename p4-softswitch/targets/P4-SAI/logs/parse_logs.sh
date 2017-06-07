rm *.svg
rm log.txt filtered_log.txt
rm *.html

echo Melding logs
python scripts/log_aggregation.py

echo Extracting packets flows
grep -E ': hit|: miss|Processing packet received|Action entry is ' log.txt > filtered_log.txt

echo Creating svg files
python scripts/animate_flow.py #-i filterd_log.txt -o visio_log -vis 

echo Creating html PTF index
#python scripts/create_html.py

echo Presenting svg with firefox
#firefox ptf.html