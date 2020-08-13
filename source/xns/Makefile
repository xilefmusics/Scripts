xns: xns.c ../include/soclib.h
	cc -o xns xns.c

clean:
	rm -f *.o xns

install: xns
	cp xns ../../bin

run: xns
	./xns

service:
	echo "[Unit]" > /etc/systemd/system/xns.service
	echo "Description=a simple dns proxy server" >> /etc/systemd/system/xns.service
	echo "After=network.target" >> /etc/systemd/system/xns.service
	echo "StartLimitIntervalSec=0" >> /etc/systemd/system/xns.service
	echo "" >> /etc/systemd/system/xns.service
	echo "[Service]" >> /etc/systemd/system/xns.service
	echo "Type=simple" >> /etc/systemd/system/xns.service
	echo "Restart=always" >> /etc/systemd/system/xns.service
	echo "RestartSec=1" >> /etc/systemd/system/xns.service
	echo "ExecStart=$$PWD/../../bin/xns" >> /etc/systemd/system/xns.service
	echo "" >> /etc/systemd/system/xns.service
	echo "[Install]" >> /etc/systemd/system/xns.service
	echo "WantedBy=multi-user.target" >> /etc/systemd/system/xns.service
	systemctl daemon-reload

service_del: rm -r /etc/systemd/system/xns.service

start:
	systemctl start xns

stop:
	systemctl stop xns

reload:
	systemctl reload xns

status:
	systemctl status xns

enable:
	systemctl enable xns

disable:
	systemctl disable xns

nm_add:
	echo "[global-dns-domain-*]" > /etc/NetworkManager/NetworkManager.conf
	echo "servers=127.0.0.1" >> /etc/NetworkManager/NetworkManager.conf
