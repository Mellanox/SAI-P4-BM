#
# Regular cron jobs for the p4-sai-adapter package
#
0 4	* * *	root	[ -x /usr/bin/p4-sai-adapter_maintenance ] && /usr/bin/p4-sai-adapter_maintenance
