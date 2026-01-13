Xephyr -br -ac -noreset -screen 1024x768 :1
sleep 1
DISPLAY=:1 ./a.out
sleep 1
